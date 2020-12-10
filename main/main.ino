#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <esp_wifi.h>
#include <esp_bt.h>

#define TOKEN "5Rdt2HPlKGz6bBQjc5Fc"
#define CLIENTID "d76a30a0-24ea-11eb-b0e1-d73cf2f8386f"
#define DHTPIN 32
#define DHTTYPE DHT22
#define versionActual 0
#define us_to_seconds 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  5        /* Time ESP32 will go to sleep (in minutes) */

enum estado{
  wifiOn,
  wifiOff,
  lowPowerMode
};

// Declaraciones variables globales.
//Variables del Wifi y Thinsboard
const char* serverName = "https://demo.thingsboard.io/api/v1/RLGdJssAKeG0PwnExDgr/telemetry";
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

// Declaraciones funciones globales.
void wifiSetup();
void otaSetup();
void mqttSetup();
void mqttReconnect();
void publishData(String sensor, int value);
void initDHT();
float leerHumedad();
int readMoisture();

void setup() {
    Serial.begin(115200);
    
  if(esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_TIMER){
    Serial.println("Reseting System");
    Serial.flush(); 
    ESP.restart();
  }
  
  Serial.println("Booting");
  //wifiSetup();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  delay(1000);
  //otaSetup();
  //delay(5000);
  //mqttSetup();
  initDHT();
}

void loop() {
  switch(state){
    case wifiOff: 
      Serial.print("Estado sin wifi: Leemos los sensores.\n");
      
      //Parte reservada para leer sensores 
      temperatura = leerTemperatura();
      Serial.print("Temperatura:");Serial.println(temperatura);
      humedad = leerHumedad();
      Serial.print("Humedad Ambiental:");Serial.println(humedad);
      humedadTierra = readMoisture();
      Serial.print("Humedad de la Tierra:");Serial.print(humedadTierra);Serial.println("%");;
      
      state = wifiOn;
      break;    
    case wifiOn: 
      Serial.print("Estado con wifi: Activamos el wifi, actualizamos los actuadores y mandamos los datos a thingsboard.\n");
      state = lowPowerMode;

      //Parte reservada para dar o quitar el agua
      
      //Encendemos el wifi y mandamos los datos a Thingsboard
      wifiSetup();  

      //Parte reservada para enviar datos 
      break;
    case lowPowerMode: 
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
