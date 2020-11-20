const int AirValue = 2432;   //you need to replace this value with Value_1
const int WaterValue = 1;  //you need to replace this value with Value_2
int soilMoistureValue = 0;
int soilmoisturepercent = 0;



int readMoisture() {
  soilMoistureValue = analogRead(34);  //put Sensor insert into soil
  Serial.println(soilMoistureValue);
  soilmoisturepercent = map(soilMoistureValue, AirValue, WaterValue, 0, 100);

  if (soilmoisturepercent >= 100) {
    Serial.println("100 %");
  }
  else if (soilmoisturepercent <= 0) {
    Serial.println("0 %");
  }
  else if (soilmoisturepercent > 0 && soilmoisturepercent < 100) {
    Serial.print(soilmoisturepercent);
    Serial.println("%");
  }

  return soilmoisturepercent;
}
