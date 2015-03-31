
void setup() {
// initialize serial communication at 9600 bits per second:
Serial.begin(9600);
}


void loop() {
// read the input on analog pin 0:
int sensorValue = analogRead(A0); 
Serial.println(sensorValue);
delay(100);
} 

//when dry, the serial monitor prints 892. As it gets wet, the value decreases
