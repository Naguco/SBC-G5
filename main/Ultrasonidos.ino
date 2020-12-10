// defines pins numbers
const int Pin = 2;
// defines variables
long duration;
int distance;

void setupUltrasonidos() {
}

int readUltrasonics() {
// Clears the trigPin
pinMode(Pin, OUTPUT); // Sets the Pin as an Output
digitalWrite(Pin, LOW);
delay(1000);

// Sets the trigPin on HIGH state for 10 micro seconds
digitalWrite(Pin, HIGH);
delay(1000);
digitalWrite(Pin, LOW);

// Reads the echoPin, returns the sound wave travel time in microseconds
pinMode(Pin, INPUT); // Sets the Pin as an Input
duration = pulseIn(Pin, HIGH);

// Calculating the distance
distance= duration*0.034/2;

return distance;
}
