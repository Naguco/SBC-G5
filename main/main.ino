#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>

#define TOKEN "5Rdt2HPlKGz6bBQjc5Fc"
#define CLIENTID "d76a30a0-24ea-11eb-b0e1-d73cf2f8386f"
#define DHTPIN 32
#define DHTTYPE DHT22
#define versionActual 0

// Declaraciones variables globales.
const char* serverName = "https://demo.thingsboard.io/api/v1/RLGdJssAKeG0PwnExDgr/telemetry";
const char publishTopic[] = "v1/devices/me/telemetry";
long lastData = 0;
WiFiClient espClient;
PubSubClient client(espClient);
char data[] = "Hola";
DHT dht(DHTPIN, DHTTYPE);
char checkedVersion = 0;

// Declaraciones funciones globales.
void wifiSetup();
void otaSetup();
void mqttSetup();
void mqttReconnect();
void publishData(String sensor, int value);
void initDHT();
float leerHumedad();
float llerTemperatura();
int readMoisture();

void setup() {
  Serial.begin(9600);
  Serial.println("Booting");
  wifiSetup();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  delay(5000);
  otaSetup();
  delay(5000);
  mqttSetup();
  initDHT();
}

void loop() {
  ArduinoOTA.handle();
  if (!client.connected()){
    mqttReconnect();
  }
  client.loop();
  publishData("Moisture",readMoisture());
  publishData("Temperatura", (int) leerTemperatura());
  publishData("Humedad", (int) leerHumedad());
  delay(5000);
}
