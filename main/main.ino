// Librerías empleadas
#include <WiFi.h>  creo que no sirve para nada porque no da error
#include <WiFiUdp.h>
#include <esp_wifi.h>
#include <esp_bt.h>
#include <PubSubClient.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <DHT.h> //needed
#include "soc/rtc_cntl_reg.h"
#include "soc/rtc.h"
#include "driver/rtc_io.h"
#include <ESPmDNS.h>
#include <EEPROM.h>
#include <WebServer.h>
#include <Preferences.h>

// Definiciones estáticas
#define TOKEN "5Rdt2HPlKGz6bBQjc5Fc"
#define CLIENTID "d76a30a0-24ea-11eb-b0e1-d73cf2f8386f"
#define DHTPIN 32
#define DHTTYPE DHT22
#define versionActual 1
#define us_to_seconds 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  60        /* Time ESP32 will go to sleep (in minutes) */
#define LED 18
#define EEPROM_SIZE 100
enum estado {
  wifiOn,
  wifiOff,
  irrigate,
  lowPowerMode,
  toExcel
};

// Declaraciones variables globales.
// Variables del Wifi y Thinsboard
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
float temperatura = 0;
float humedad = 0;
float humedadTierra = 0;
float distancia = 0;
float Deposito = 0;
bool depositoBajo = false;

// Nums LEDS Pins
int pinRED = 12;
int pinGREEN = 14;
int pinBLUE = 27;

// Setup for first time
int isSetedUp = 0;
int hayWifi = 0;

//Wifi AP settings;
IPAddress local_ip(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

//Servidor http
WebServer server(80);
String header;

//Memoria estática
Preferences preferences;


// Declaraciones funciones.
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
void irrigateState();
void wifiOnState();
void wifiOffState();
void lowPowerModeState();
void setupPinsLED();
void setupInternalEEPROM();
void wifiInit();
void setupAPServer();
void listenNewPostMethod();
void handleConnectionRoot();
void toExcelState();
void writeSSIDEEPROM(String ssid);
void writePASSWORDEEPROM(String password);
void readPASSWORDEEPROM();
void readSSIDEEPROM();

void setup() {
  Serial.begin(115200);
  setupInternalEEPROM();
  if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_TIMER) {
    Serial.println("Reseting System");
    Serial.flush();
    ESP.restart();
  }
  setupPinsLED();
  Serial.println("Booting");
  wifiSetup();
  //otaSetup();
  //initDHT();
  //WiFi.disconnect(true);
  //WiFi.mode(WIFI_OFF);
  //btStop();
  // esp_wifi_stop(); -- revienta aquí
  //esp_bt_controller_disable();
}

void loop() {
  if (hayWifi == 0) {
    server.handleClient();
  } else {
    switch (state) {
      case wifiOff:
        wifiOffState();
        break;
      case wifiOn:
        wifiOnState();
        break;
      case irrigate:
        irrigateState();
        break;
      case lowPowerMode:
        lowPowerModeState();
        break;
      case toExcel:
        toExcelState();
        break;
    }
  }
}
