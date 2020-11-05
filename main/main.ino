#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <HTTPClient.h>

const char* serverName = "https://demo.thingsboard.io/api/v1/RLGdJssAKeG0PwnExDgr/telemetry";

void wifiSetup();
void otaSetup();
void sendDataExample();

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
  sendDataExample();
  delay(1000);
}
