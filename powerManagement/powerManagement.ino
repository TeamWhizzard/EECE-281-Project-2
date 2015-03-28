#include <Wire.h>
#include "RealTimeClockDS1307.h"
#include "WString.h"

#define RELAY_PIN 3

int day, month, year, hours, minutes, seconds;

void setup() {
	Serial.begin(9600);
	RTC.start();
	pinMode(RELAY_PIN, OUTPUT);
}

void loop() {
	refreshTime();
	if (seconds == 15)
	digitalWrite(3,HIGH);
	else if (seconds == 30)
	digitalWrite(3,LOW);
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