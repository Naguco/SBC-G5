/*
const char* ssid = "Honor 10";
const char* password = "wificarlos";
*/
const char* ssid = "PisitoChill";
const char* password = "3r35_T0nt0";

void wifiSetup() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  delay(5000);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Connection Failed! Rebooting...");
    encenderErrorWifiRojo();
    delay(5000);
    int i;
    for (i = 0; i < 5; i++) {
      apagarLEDs();
      delay(500);
      encenderErrorWifiRojo();
      delay(500);
    }
    apagarLEDs();
    ESP.restart();
  }
 }

 
