#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

void wifiSetup();
void otaSetup();

void setup() {
  Serial.begin(115200);
  Serial.println("Booting");
  wifiSetup();
  otaSetup();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  ArduinoOTA.handle();
}
