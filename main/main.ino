 #include <WiFi.h>
#include <WiFiUdp.h>
#include <esp_wifi.h>

#include <esp_bt.h>

#include <PubSubClient.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>

#include <DHT.h>

#include "soc/rtc_cntl_reg.h"
#include "soc/rtc.h"
#include "driver/rtc_io.h"
#include <ESPmDNS.h>


#define TOKEN "5Rdt2HPlKGz6bBQjc5Fc"
#define CLIENTID "d76a30a0-24ea-11eb-b0e1-d73cf2f8386f"
#define DHTPIN 32
#define DHTTYPE DHT22
#define versionActual 1
#define us_to_seconds 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  10        /* Time ESP32 will go to sleep (in minutes) */
#define LED 18
enum estado{
  wifiOn,
  wifiOff,
  irrigate,
  lowPowerMode
};

// Declaraciones variables globales.
//Variables del Wifi y Thinsboard
const char publishTopic[] = "v1/devices/me/telemetry";
long lastData = 0;
WiFiClient espClient;
PubSubClient client(espClient);
char data[] = "Hola";
//Variables del DHT
DHT dht(DHTPIN, DHTTYPE);
char checkedVersion = 0;

//Variable de estado principal
estado state = wifiOff;

//Variables globales para compartir datos:
float temperatura=0;
float humedad=0;
float humedadTierra=0;
float distancia=0;
float Deposito=0;

// Nums LEDS Pins
int pinRED = 12;
int pinGREEN = 14;
int pinBLUE = 27;


// Declaraciones funciones globales.
void wifiSetup();
void otaSetup();
void mqttSetup();
void mqttReconnect();
void publishData(String sensor, int value);
void initDHT();
float leerHumedad();
int readMoisture();
float leerTemperatura();
int readDistance();
void encenderErrorWifiRojo();
void encenderErrorMQTTAzul();
void encenderValidoVerde();
void encenderActualizandoAmarillo();
void encenderErrorDepositoMorado();
void apagarLEDs();

void setup() {
    Serial.begin(115200);
    
  if(esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_TIMER){
    Serial.println("Reseting System");
    Serial.flush(); 
    ESP.restart();
  }
  pinMode(pinRED, OUTPUT);
  pinMode(pinGREEN, OUTPUT);
  pinMode(pinBLUE, OUTPUT);
  Serial.println("Booting");
  wifiSetup();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  delay(1000);
  otaSetup();
  delay(5000);
  initDHT();
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);      
  btStop();
  esp_wifi_stop();
  esp_bt_controller_disable();
}

void loop() {
  switch(state){
    case wifiOff: 
      encenderValidoVerde();
      Serial.println("--------------------------------------");
      Serial.print("Estado sin wifi: Leemos los sensores y actualizamos los actuadores.\n");
      
      //Parte reservada para leer sensores 
      temperatura = leerTemperatura();
      Serial.print("Temperatura:");Serial.println(temperatura);
      humedad = leerHumedad();
      Serial.print("Humedad Ambiental:");Serial.println(humedad);
      humedadTierra = readMoisture();
      Serial.print("Humedad de la Tierra:");Serial.print(humedadTierra);Serial.println("%");
      distancia= readDistance();
      Serial.println(distancia);
      Deposito=100 - (distancia/10)*100 ;
      Serial.print("Nivel Agua:");Serial.print(Deposito); Serial.println("%");

      //Decidimos si regamos o no
      if(humedadTierra <= 20){
        Serial.println("El nivel de Humedad de la tierra es menor al 20%. Es necesario regar.");
        state= irrigate;
      }
      else{
        Serial.println("El nivel de Humedad de la tierra es mayor al 20%. No es necesario regar.");
        state = wifiOn;
      }
        

      //Activamos el codigo de error del deposito si es necesario
      if(Deposito <= 30){
       int i;
       Serial.println("Deposito con poca agua");
       for (i = 0; i < 10; i++) {
          apagarLEDs();
          delay(1000);
          encenderErrorDepositoMorado();
          delay(1000);
          ESP.restart();
      }
      }
       
        
      break;    
    case wifiOn:
      encenderValidoVerde();
      Serial.println("--------------------------------------");
      Serial.print("Estado con wifi: Activamos el wifi y mandamos los datos a thingsboard.\n");
      
      //Encendemos el wifi y mandamos los datos a Thingsboard
      wifiSetup();
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
      mqttSetup();
      mqttReconnect();
      delay(1000);

      //Parte reservada para enviar datos 
      publishData("temperatura_ambiental",temperatura);
      delay(500);
      publishData("humedad_ambiental",humedad);
      delay(500);
      publishData("humedad_tierra",humedadTierra);
      delay(500);
      publishData("nivel_agua",Deposito);
      delay(500);
      //-----------------------------------------------------------------------------me falta el estado de regando, que es true o false---------POR HACER
      //publishData("regando", True);
      //delay(500);

            
      state = lowPowerMode;
      break;
     case irrigate:
     encenderValidoVerde();
      {Serial.println("--------------------------------------");
      Serial.print("Estado Irrigate: Abrimos el riego, nos dormimos 10 min \n y volvemos a comprobar si hay que regar\n");      

      while(humedadTierra<=35){
        Serial.print("Estado Irrigate: \n");      

        pinMode(GPIO_NUM_18, OUTPUT);
        digitalWrite(18,HIGH);
        /*rtc_gpio_init(GPIO_NUM_18); 
        rtc_gpio_set_direction(GPIO_NUM_18, RTC_GPIO_MODE_OUTPUT_ONLY);
        rtc_gpio_set_level(GPIO_NUM_18,1);
        esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);*/
    
        esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * us_to_seconds);
        Serial.flush(); 
        Serial.println("Seguimos regando");
        esp_light_sleep_start();

        humedadTierra = readMoisture();
        Serial.print("Humedad de la Tierra:");Serial.print(humedadTierra);Serial.println("%");
      }
      
      state = wifiOn;
      break;
      
    case lowPowerMode: 
      encenderValidoVerde();
      Serial.println("--------------------------------------");
      Serial.print("Estado low power: Apagamos todo y nos vamos a dormir\n");
      WiFi.disconnect(true);
      WiFi.mode(WIFI_OFF);      
      btStop();
      esp_wifi_stop();
      esp_bt_controller_disable();
      state = wifiOff;   

      
      esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * us_to_seconds);
      Serial.flush(); 
      esp_deep_sleep_start();
      break;   
  }
}
}
