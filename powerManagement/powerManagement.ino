#include <Wire.h>
#include "RealTimeClockDS1307.h"

int date[3];
int time[3];

void setup() {
	Serial.begin(9600);
	RTC.start();
}

void loop() {
	RTC.readClock();
	refreshTime();
	String dateString = date[0] + "/" + date[1] + "/" + date[2];
	Serial.println(dateString);
	delay(5000);
}

void refreshTime() {
	date[0] = RTC.getDate();
	date[1] = RTC.getMonth();
	date[2] = RTC.getYear();
	
	time[0] = RTC.getHours();
	time[1] = RTC.getMinutes()
	time[2] = RTC.getSeconds();
}