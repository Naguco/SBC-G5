void mqttSetup() {
  client.setServer("iot.etsisi.upm.es", 1883); //1883
}

void mqttReconnect() {
  int counter = 0;
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection ....");
    if (client.connect("MACETERO ETSIAAB 1", "PL6gQwBTY30HXmOXEbiu", NULL)) {
      counter = 0;
      Serial.println("Connected to MQTT Broker");
      client.subscribe(publishTopic);
    } else {
      int i;
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println("try again in 5 second");
      for (i = 0; i < 5; i++) {
        apagarLEDs();
        delay(500);
        encenderErrorMQTTAzul();
        delay(500);
      }
      counter++;
      if(counter == 5) {
        ESP.restart();
      }
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
