
const char* ssid = "Honor 10";
const char* password = "wificarlos";
/*
const char* ssid = "SBC";
const char* password = "sbc$2020";*/

void wifiSetup() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  delay(5000);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
 }

 
