from flask import Flask, jsonify, render_template, request
import json

app = Flask(__name__)

dispositivos = []
datos_json = ["Dispositivo", "Temperatura_Ambiental", "Humedad_Ambiental", "Nivel_Agua", "Humedad_Tierra", "Regando"]

def actualizar(disp, datos): #------------------------------------Hay que hacerlo
    global dispositivos
    global datos_json
    #print("bip bop bap, actualizando ...")
    for d in dispositivos:
        if d[0] == disp:
            #print(d[0] + "  " + str(d[1]))
            for i in datos_json[1:]:
                if datos[i] != 'NULL':
                    d[1][i] = datos[i]
                


#Comprueba que existe el elemento en la doble lista
def check(disp): 
    global dispositivos
    encontrado = False
    for d in dispositivos:
        if d[0] == disp:
            encontrado = True
            break

    return encontrado

@app.route('/', methods=['POST', 'GET'])
#Con el post aniadimos o actualizamos el dispositivo con los datos
#Con el get devolvemosuna lista con los nombres de todos los dispositivos separdos por una coma ','
def Datos():

    global dispositivos
    global datos_json

    if request.method == 'POST':
        data = request.get_json(force=True)
        
        data = json.dumps(data)

        #Tenemos que adapatar un poco los datos de thingsboard
        if ( data[0] == '"'):
            data = data[1:len(data)-1]
            data = data.replace("\\", "")
            
        data = json.loads(data)

        #Si el dispositivo se encuentra en la lista lo actualizamos
        if (check(data[datos_json[0]])):
            #Actualizamos valores
            actualizar(data[datos_json[0]], data)
            return "Actualizado"

        else:
            #Lo aniadimos a la lista de dispositivos         
            dispositivos.append([ data[datos_json[0]], data ])
            return "Aniadido"

    elif request.method == 'GET':
        #Devolvemos la lista con todos los dispositivos que podemos ver
        disp = ""

        for d in dispositivos:
            disp += d[0] + ","

        return disp

@app.route('/disp/<nombre>', methods=['GET'])
#Devuelve solo el dispositivo seleccionado
def get_dispositivo(nombre):
    global dispositivos
    d = "NULL"
    for i in dispositivos:
        if i[0] == nombre:
            d = i[1]

    return d


if __name__ == '__main__':
    app.run()

"""
ejemplo
{ "Dispositivo": "ESP-32", "Temperatura_Ambiental": 99,  "Humedad_Ambiental": 50, "Nivel_Agua": "NULL", "Humedad_Tierra": "NULL", "Regando": "NULL" }
"""

