void initDHT() {
  dht.begin();
}

float leerHumedad() {
  return dht.readHumidity();
}

float leerTemperatura() {
  return dht.readTemperature();
}
