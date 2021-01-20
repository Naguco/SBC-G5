void otaSetup() {
  HTTPClient httpClient;
  httpClient.begin("http://esp32ota.herokuapp.com/latestVersion");
  httpClient.GET();
  String payload = httpClient.getString();
  int versionEnNube = payload.toInt();
  if (versionEnNube > versionActual) {
      Serial.println("Actualización en marcha!");
      encenderActualizandoAmarillo();
      delay(5000);
      t_httpUpdate_return ret = httpUpdate.update(espClient, "http://esp32ota.herokuapp.com/latestFileVersion.bin");
      switch (ret) {
        case HTTP_UPDATE_FAILED:
          Serial.printf("HTTP_UPDATE_FAILED Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
          break;
  
        case HTTP_UPDATE_NO_UPDATES:
          Serial.println("HTTP_UPDATE_NO_UPDATES");
          break;
  
        case HTTP_UPDATE_OK:
          Serial.println("HTTP_UPDATE_OK");
          break;
      } 
    } else {
      Serial.println("No tengo que actualizar!");
    }
    Serial.printf("La version actual es la: %d \n", versionActual);
}
