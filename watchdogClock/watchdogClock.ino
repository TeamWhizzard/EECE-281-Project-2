#include <Wire.h>
#include "RTClib.h"

#include <avr/sleep.h>
#include <avr/wdt.h>

RTC_DS1307 RTC;

#define DEMO_MODE 1
#define CLOCK_POWER 9

// watchdog interrupt
ISR (WDT_vect)
{
	wdt_disable();  // disable watchdog
}

void myWatchdogEnable()
{
	// clear various "reset" flags
	MCUSR = 0;
	// allow changes, disable reset
	WDTCSR = bit (WDCE) | bit (WDE);
	// set interrupt mode and an interval
	WDTCSR = bit (WDIE) | bit (WDP3) | bit (WDP0);    // set WDIE, and 8 seconds delay
	wdt_reset();  // pat the dog

	// disable ADC
	ADCSRA = 0;

	// ready to sleep
	set_sleep_mode (SLEEP_MODE_PWR_DOWN);
	sleep_enable();

	// turn off brown-out enable in software
	MCUCR = bit (BODS) | bit (BODSE);
	MCUCR = bit (BODS);
	sleep_cpu ();

	// cancel sleep as a precaution
	sleep_disable();
}

void setup()
{
	RTC.begin();  // activate clock (doesn't do much)
}  // end of setup

void loop()
{

	// power up clock chip
	digitalWrite (CLOCK_POWER, HIGH);
	pinMode (CLOCK_POWER, OUTPUT);

	// activate I2C
	Wire.begin();

	// find the time
	DateTime now = RTC.now();

	// time now available in now.hour(), now.minute() etc.

	// finished with clock
	pinMode (CLOCK_POWER, INPUT);
	digitalWrite (CLOCK_POWER, LOW);

	// turn off I2C
	TWCR &= ~(bit(TWEN) | bit(TWIE) | bit(TWEA));

	// turn off I2C pull-ups
	digitalWrite (A4, LOW);
	digitalWrite (A5, LOW);

	// Do something every odd minute
	if (now.second() < 30) {
		digitalWrite(5, HIGH);
		} else {
		digitalWrite(5, LOW);
	}
	
	uint8_t sunrise
	uint8_t sunset
	now.
	if (DEMO_MODE) {
		

		} else {
		// Check if weekday
		if (now.dayOfWeek() <= 5) {
			// Check if daytime
			if ((now.hour() > sunriseHour && now.hour() < sunsetHour) || ((now.hour() == sunriseHour && now.minute() >= sunriseMinute) || (now.hour() == sunsetHour && now.minute() <= sunsetMinute)))

			else {}

			// it is night time so sleep a hella lot
			
		}
	}
}

void sleep_cycle(DateTime now, int h, int s) {
	now.hour();
	
	for (int i = 0; i < 8; i++)
	myWatchdogEnable ();
}