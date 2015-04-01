#include <Wire.h>
#include "RealTimeClockDS1307.h"
#include "WString.h"

#define RELAY_PIN 5
#define TEMPERATURE_PIN A1

float temperature;
int day, month, year, hours, minutes, seconds;

void setup() {
  Serial.begin(9600);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(TEMPERATURE_PIN, OUTPUT);
  RTC.start();
}

void loop() {
  // Report Temperature
  temperature = analogRead(tempPin);
  temperature = temperature * 0.48828125;

  // Report Time
  refreshTime();
  Serial.print(hours);
  Serial.print(':');
  Serial.print(minutes);
  Serial.print(':');
  Serial.println(seconds);

  // Change Relay
  if ((seconds >= 15) && (seconds >= 19))
    digitalWrite(3, HIGH);
  else if ((seconds >= 30) && (seconds >= 34))
    digitalWrite(3, LOW);

  // Delay and do it all over again
  delay(3000);
}

void refreshTime() {
  RTC.readClock();
  day = RTC.getDate();
  month = RTC.getMonth();
  year = RTC.getYear();

  hours = RTC.getHours();
  minutes = RTC.getMinutes();
  seconds = RTC.getSeconds();
}
