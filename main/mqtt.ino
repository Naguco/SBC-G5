void mqttSetup() {
  client.setServer("demo.thingsboard.io", 1883);
}

void mqttReconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection ....");
    if (client.connect("FranPruebaSBC", "Fran", "123456789")) {
      Serial.println("Connected to MQTT Broker");
      client.subscribe(publishTopic);
    } else {
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
