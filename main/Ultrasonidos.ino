// defines pins numbers
const int Pin = 2;
// defines variables
long duration;
int distance;

int readDistance() {
    pinMode(Pin, OUTPUT);
    digitalWrite(Pin, LOW);
    delayMicroseconds(2);
    digitalWrite(Pin, HIGH);
    delayMicroseconds(5);
    digitalWrite(Pin,LOW);
    pinMode(Pin,INPUT);
    duration = pulseIn(Pin,HIGH);
    
  return duration/29.0/2.0;
}
