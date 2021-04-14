String ssid;
String password;

void wifiSetup() {
  int numIntentos = 7;
  WiFi.mode(WIFI_STA);
  readPASSWORDEEPROM();
  readSSIDEEPROM();
  Serial.print(ssid.c_str());
  WiFi.begin(ssid.c_str(), password.c_str());
  delay(5000);
  while ((WiFi.waitForConnectResult() != WL_CONNECTED) && (numIntentos > 0)) {
    Serial.println("WiFi Connection Failed! Retrying...");
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
    // ESP.restart();
    numIntentos--;
  }
  if (numIntentos == 0) {
    ssid = "";
    password = "";
    setupAPServer();
  } else {
    Serial.println("Ready");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    isSetedUp = 1;
    hayWifi = 1;
  }
}

void wifiInit() {
  int numIntentos = 5;
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), password.c_str());
  delay(5000);
  while ((WiFi.waitForConnectResult() != WL_CONNECTED) && (numIntentos > 0)) {
    Serial.println("WiFi Connection Failed! Retrying...");
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
    numIntentos--;
  }
  if (numIntentos != 0) {
    hayWifi = 1;
  }
}

void setupAPServer() {
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  delay(1000);
  Serial.println("Entro al modo access point");
  WiFi.softAP("esp-32", "12345678");
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(1000);
  server.on("/setupWifi", HTTP_POST, handlePOSTWifiConfig);
  server.on("/", HTTP_GET, handleConnectionRoot);
  Serial.println("Empezamos WebServer");
  server.begin();
}

void listenNewPostMethod() {
  server.handleClient();
}

void handleConnectionRoot(){
  String toSend = ""; 
  Serial.println("Vino un GET!");
  server.send(200, "text/html", "<form action=\"/setupWifi\" method=\"post\"> <label for=\"ssid\">ssid:</label><input type=\"text\" id=\"ssid\" name=\"ssid\"><br><br><label for=\"password\">password:</label><input type=\"text\" id=\"password\" name=\"password\"><br><br><input type=\"submit\" value=\"Submit\"></form>");
}

void handlePOSTWifiConfig() {
  Serial.println("Vino un POST!");
  Serial.println(server.arg(String("ssid")));
  Serial.println(server.arg(String("password")));
  writeSSIDEEPROM(String(server.arg(String("ssid"))));
  writePASSWORDEEPROM(String(server.arg(String("password"))));
  server.send(200, "text/html", "Guardado! Me voy a reiniciar para conectarme al nuevo wifi, por favor, espera.");
  delay(1000);
  Serial.println("Me voy a reiniciar!");
  ESP.restart();
}

void writeSSIDEEPROM(String ssid) {
   preferences.putString("ssid", ssid); 
}

void writePASSWORDEEPROM(String password) {
  preferences.putString("password", password);
}

void readPASSWORDEEPROM() {
  String pwdDef = preferences.getString("password", "");
  password = pwdDef;
}

void readSSIDEEPROM() {
  String ssidDef = preferences.getString("ssid", "");
  ssid = ssidDef;
}
