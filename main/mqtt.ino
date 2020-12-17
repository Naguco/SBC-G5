void mqttSetup() {
  client.setServer("iot.etsisi.upm.es", 1883); //1883
}

void mqttReconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection ....");
    if (client.connect("ESP-32", "Li7Fp1RY5BH2sSG1jnsG", NULL)) {
      Serial.println("Connected to MQTT Broker");
      client.subscribe(publishTopic);
    } else {
      int i;
      for (i = 0; i < 10; i++) {
        apagarLEDs();
        delay(2000);
        encenderErrorMQTTAzul();
        delay(2000);
      }
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println("try again in 5 second");
      delay(5000);
    }
  }
}

void publishData(String sensor, int value) {
  String json = String("{\"name\":\"value\"}");
  char charBuf[100];
  json.replace("name", sensor);
  json.replace("value", String(value));
  Serial.println(json);
  json.toCharArray(charBuf,100);
  client.publish(publishTopic, charBuf);
}
