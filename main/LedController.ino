void encenderErrorWifiRojo() {
  digitalWrite(pinRED, HIGH);
  digitalWrite(pinGREEN, LOW);
  digitalWrite(pinBLUE, LOW);
}

void encenderErrorMQTTAzul(){
  digitalWrite(pinRED, LOW);
  digitalWrite(pinGREEN, LOW);
  digitalWrite(pinBLUE, HIGH); 
}

void encenderValidoVerde(){
  digitalWrite(pinRED, LOW);
  digitalWrite(pinGREEN, HIGH);
  digitalWrite(pinBLUE, LOW);
}

void encenderActualizandoAmarillo(){
  digitalWrite(pinRED, HIGH);
  digitalWrite(pinGREEN, HIGH);
  digitalWrite(pinBLUE, LOW);
}

void encenderErrorDepositoMorado(){
  digitalWrite(pinRED, HIGH);
  digitalWrite(pinGREEN, LOW);
  digitalWrite(pinBLUE, HIGH);
}

void apagarLEDs() {
  digitalWrite(pinRED, LOW);
  digitalWrite(pinGREEN, LOW);
  digitalWrite(pinBLUE, LOW);
}
