#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <esp_wifi.h>
#include <esp_bt.h>

#define TOKEN "5Rdt2HPlKGz6bBQjc5Fc"
#define CLIENTID "d76a30a0-24ea-11eb-b0e1-d73cf2f8386f"
#define DHTPIN 32
#define DHTTYPE DHT22
#define us_to_seconds 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  0.5        /* Time ESP32 will go to sleep (in minutes) */

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
//Variable de estado principal
estado state = wifiOn;

//Variables globales para compartir datos:


// Declaraciones funciones globales.
void wifiSetup();
void otaSetup();
void mqttSetup();
void mqttReconnect();
void publishData(String sensor, int value);
void initDHT();
float leerHumedad();
int readMoisture();
void setupUltrasonidos();

void setup() {
  Serial.begin(115200);
  Serial.println("Booting");
  //wifiSetup();
  //otaSetup();
  //mqttSetup();
  initDHT();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  switch(state){
    case wifiOff: 
      Serial.print("Estado sin wifi: Leemos los sensores.\n");
      WiFi.disconnect(true);
      WiFi.mode(WIFI_OFF);
      state = wifiOn;
      btStop();
      esp_wifi_stop();
      esp_bt_controller_disable();

      //Parte reservada para leer sensores 
          
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
      Serial.print("Estado low power: Nos vamos a dormir\n");
      state = wifiOff;
      esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP *60 * us_to_seconds);
      Serial.flush(); 
      esp_deep_sleep_start();
      break;   
  }


  /*
  //publishData("Moisture",readMoisture());
  publishData("Temperatura", (int) leerTemperatura());
  publishData("Humedad", (int) leerHumedad());
  int  distancia=readUltrasonics();
  Serial.print("Distancia:");
  Serial.println(distancia);
  */
}
