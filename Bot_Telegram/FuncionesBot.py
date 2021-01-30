from bs4 import BeautifulSoup
import urllib3
import json
from multiprocessing import Process, Queue
import time

from telegram import (
    Poll,
    ParseMode,
    KeyboardButton,
    KeyboardButtonPollType,
    ReplyKeyboardMarkup,
    ReplyKeyboardRemove,
    InlineKeyboardButton,
    InlineKeyboardMarkup,
    Update,
)
from telegram.ext import (
    Updater,
    CommandHandler,
    CallbackQueryHandler,
    PollAnswerHandler,
    PollHandler,
    MessageHandler,
    Filters,
    CallbackContext,
)

"""
Futuros cambios:

-> La url_base por la definitiva donde corra la API
-> El Json de Thingsboard, a ver como se mandan al final los datos
-> La funcion asincrona, cada cuanto tiempo la ejecutamos
-> La medida del aviso del ultrasonidos cuando cambien el deposito
-> Aniadiar otro aviso basandose en la medida del agua, en plan, si no ha cambiado en 24 horas o 3 dias, es que o no esta regando o no esta conectada
-> Avisos por temperaturas muy altas o muy bjas, etc

"""
url_base = "http://franheredia.pythonanywhere.com/"

#Ejemplo de POST a la API para aniadir o actualizar una maceta: 
# { "Dispositivo": "ESP-335", "Temperatura_Ambiental": 99,  "Humedad_Ambiental": 50, "Nivel_Agua": "NULL", "Humedad_Tierra": "NULL", "Regando": "NULL" }

#Ejemplo de POST a Thingsboard para actualizar un dispositivo:
# {"temperatura":90, "humedad": 15, "agua": 12, "humedad_tierra": 70, "regando": false }

datos_json = ["Dispositivo", "Temperatura_Ambiental", "Humedad_Ambiental", "Nivel_Agua", "Humedad_Tierra", "Regando"]

Bot_TOKEN = "1493895560:AAFxGrhYxOLy5oI9um4RSPmDHUQj6_i1Wls"

def get_url(url):
    http = urllib3.PoolManager()
    content = http.request('GET', url)
    soup = BeautifulSoup(content.data, features="html.parser")
    return soup

#Devuelve el json del dispositivo con los datos mas actualizados
def get_dispositivo(nombre):
    url = url_base + "disp/" +nombre
    disp = get_url(url)
    disp = json.loads(str(disp))
    return disp

#Devuelve una lista con los nombres de todos los dispositivos registrados
def get_dispositivos():
    data = get_url(url_base)
    disps = str(data).split(",")
    disps = disps[:len(disps)-1]
    return disps

#Devuelve un texto bonito dado el json de la maceta
def MacetaToString(jsontxt):
    res = ""
    for i in datos_json:
        d = jsontxt[i]
        res += i + ":  " + str(d) + "\n"

    return res
    
# Define a few command handlers. These usually take the two arguments update and
# context. Error handlers also receive the raised TelegramError object in error.
def start(update: Update, context: CallbackContext) -> None:
    """Send a message when the command /start is issued."""
    txt = "Bienvenid@, me encargare de darte los datos de la "
    txt += "maceta que me digas siempre que quieras, "
    txt += "usa el comando /maceta y escoge una.\n"
    txt += "Para mas comandos /help ."
    
    update.message.reply_text(txt)


def help_command(update: Update, context: CallbackContext) -> None:
    """Send a message when the command /help is issued."""
    txt = "Tenemos varios comandos, para ver los datos de una sola maceta /maceta , "
    txt += "para ver los datos de todas las macetas a la vez /macetas .\n"
    txt += "Suscribete a las alertas de las macetas, "
    txt += "usa /activar_avisos , o si quieres dejar de recibirlos /desactivar_avisos ."

    update.message.reply_text(txt)


def Escoger_Dispositivo(update: Update, context: CallbackContext) -> None:
    
    keyboard = []
    
    cont = 1
    pos = 0

    disp = get_dispositivos()

    for c in disp:
        if (cont == 1):
            keyboard.append([])
        
        keyboard[pos].append(InlineKeyboardButton(c, callback_data= c))
        cont += 1

        if (cont == 4):
            cont = 1
            pos += 1

    #keyboard.append([InlineKeyboardButton("Finalize", callback_data= "Finalize")])

    reply_markup = InlineKeyboardMarkup(keyboard)

    update.message.reply_text('Que maceta te interesa?', reply_markup=reply_markup)


def Respuesta_Seleccion(update: Update, context: CallbackContext) -> None:
    query = update.callback_query

    query.answer()

    d = get_dispositivo(query.data)

    m = MacetaToString(d)
    query.edit_message_text(m)


def TodosLosDispositivos(update: Update, context: CallbackContext) -> None:
    
    disps = get_dispositivos()
    
    for disp in disps:
        d = get_dispositivo(disp)
        m = MacetaToString(d)
        update.message.reply_text(m)


#Funciones para los avisos, incluye funcion asyncrona
Suscritos = Queue(1) 
l = []
Suscritos.put(l)


def Telegram_Web_API(chat, txt):
    url = "https://api.telegram.org/bot"+ Bot_TOKEN +"/sendMessage"

    encoded_body = json.dumps({
        "chat_id": chat,
        "text": txt,
    })
    http = urllib3.PoolManager()
    r = http.request('POST', url,
                headers={'Content-Type': 'application/json'},
                body=encoded_body)

#Esta es la funcion asincrona que cada cierto tiempo se encarga de mandar los avisos a los usuarios suscritos
def avisoRetardo(Suscritos: Queue):
    while True:
        
        #Obtenemos la lista de chats que quieren que les mandemos las alertas
        lista_suscritos = Suscritos.get()
        Suscritos.put(lista_suscritos)
        
        #Obtenemos los dispositivos
        dispositivos = get_dispositivos()

        for d in dispositivos:
            #Vamos revisando cada maceta
            maceta = get_dispositivo(d)
            txt = ""
#----------------------------------------------------------------------------AVISOS-----------------------------------------------------------------------------
            #Aviso 1: Deposito vacio
            if (maceta[datos_json[3]] != "NULL"):

                if int(maceta[datos_json[3]]) >= 9 : #9 cm es lo que mide el sensor ultrasonidos en nuestro deposito
                    txt += "La maceta "  + maceta[datos_json[0]] + " necesita que le rellenen el deposito.\n"


            #Aviso 2: Temperatura ambiental superior a 35 grados
            if (maceta[datos_json[1]] != "NULL"):

                if int(maceta[datos_json[1]]) >= 35 : #grados del sensor
                    txt += "La maceta "  + maceta[datos_json[0]] + " se encuentra a una temperatura igual o superior a 35 grados.\n"

            #Aviso 3: Temperatura ambiental inferior a 0 grados
            if (maceta[datos_json[1]] != "NULL"):

                if int(maceta[datos_json[1]]) >= 9 : #grados del sensor
                    txt += "La maceta "  + maceta[datos_json[0]] + " se encuentra a una temperatura igual o inferior a 0 grados.\n"

#---------------------------------------------------------------------FIN_AVISOS--------------------------------------------------------------------------------
            #Por ultimo mandamos el aviso de la maceta actual
            if txt != "":
                for c in lista_suscritos:
                    Telegram_Web_API(c,txt)

        #Dormimos hasta la siguiente revision de los datos
        time.sleep(10) # 10 s

#Activamos el proceso concurrente
p = Process(target=avisoRetardo, args=(Suscritos,))
p.start()

#Aqui se activan o desactivan los avisos
def Suscribirse (update: Update, context: CallbackContext) -> None:
    global Suscritos
    
    l = Suscritos.get()
    l.append(update.effective_chat.id)
    l = list(dict.fromkeys(l))
    Suscritos.put(l)

    txt = "Avisos activados correctamente. \n"
    txt += "Te avisaremos de cuando un deposito este a punto "
    txt += "de acabarse."

    update.message.reply_text(txt)


def Desuscribirse (update: Update, context: CallbackContext) -> None:
    global Suscritos

    l = Suscritos.get()

    l.remove(update.effective_chat.id)
    Suscritos.put(l)

    update.message.reply_text("Ya no te llegaran mas avisos.")