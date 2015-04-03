/*
*------------------------------------------------------------------------------------------
* Libraries / _____ / Defined Values / Global Variables
*------------------------------------------------------------------------------------------
*/
#include <Wire.h>
#include "RTClib.h"
#include "MAX1704.h"

#include <avr/sleep.h>
#include <avr/wdt.h>

RTC_DS1307 RTC;
MAX1704 fuelGauge;

#define DEMO_MODE 1
#define RELAY_PIN 5
#define CLOCK_POWER 9
#define BATTERY_THRESHOLD 20

uint16_t lastRainVal = 1023;
uint8_t sunriseHour = 6;
uint8_t sunriseMinute = 30;
uint8_t sunsetHour = 20;
uint8_t sunsetMinute = 00;

/*
*------------------------------------------------------------------------------------------
* Watchdog Timer
*------------------------------------------------------------------------------------------
*/
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

/*
*------------------------------------------------------------------------------------------
* ADC Conversion
*------------------------------------------------------------------------------------------
*/
void adc_init()
{
  // AREF = AVcc
  ADMUX = (1 << REFS0);

  // ADC Enable and prescaler of 128
  // 16000000/128 = 125000
  ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint16_t adc_read(uint8_t ch)
{
  // select the corresponding channel 0~7
  // ANDing with �7? will always keep the value
  // of �ch� between 0 and 7
  ch &= 0b00000111;  // AND operation with 7
  ADMUX = (ADMUX & 0xF8) | ch; // clears the bottom 3 bits before ORing

  // start single convertion
  // write �1? to ADSC
  ADCSRA |= (1 << ADSC);

  // wait for conversion to complete
  // ADSC becomes �0? again
  // till then, run loop continuously
  while (ADCSRA & (1 << ADSC));

  return (ADC);
}

/*
*------------------------------------------------------------------------------------------
* Sleep Control Function
*------------------------------------------------------------------------------------------
*/
void sleep_cycle(DateTime now, int h, int s) {
  now.hour();

  for (int i = 0; i < 8; i++)
    myWatchdogEnable ();
}

/*
*------------------------------------------------------------------------------------------
* Sensor Functions
*------------------------------------------------------------------------------------------
*/
// Report battery level
float getBatteryLevel() {
  float batteryLevel = fuelGauge.stateOfCharge();

  Serial.print("Battery: ");
  Serial.println(batteryLevel);

  return batteryLevel;
}

// Report temperature
float getTemperature() {
  adc_read(2); // move the ADC to the LM35
  delay(10); // delay to allow the reading to settle
  uint16_t temperatureRaw = adc_read(2); // take the actual temperature reading we use
  float temperature =  temperatureRaw * 0.48828125;

  Serial.print("Temperature: ");
  Serial.println(temperature);

  delay(100); // allow the ADC to settle
  return temperature;
}

// Report if raining
bool checkIsRaining() {
  adc_read(3); // move the ADC to the rain sensor
  delay(10); // delay to allow the reading to settle
  uint16_t newRainVal = adc_read(3);

  Serial.print("Rain Value: ");
  Serial.println(newRainVal);

  if (newRainVal < 900 && newRainVal < lastRainVal + 300) {
    lastRainVal = newRainVal;
    return true;
  }
  else {
    lastRainVal = newRainVal;
    return false;
  }
}

void printTime() {
  DateTime now = RTC.now();
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(' ');
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();
  delay(1000);
}

/*
*------------------------------------------------------------------------------------------
* Setup Function
*------------------------------------------------------------------------------------------
*/
void setup()
{
  RTC.begin();  // activate clock (doesn't do much)
  Serial.begin(9600);
  adc_init();
  pinMode(RELAY_PIN, OUTPUT);
  fuelGauge.reset();
  fuelGauge.quickStart();
  fuelGauge.showConfig();
  delay(1000);
}  // end of setup

/*
*------------------------------------------------------------------------------------------
* Main Loop Function
*------------------------------------------------------------------------------------------
*/
void loop()
{

  // power up clock chip
  //  digitalWrite (CLOCK_POWER, HIGH);
  //  pinMode (CLOCK_POWER, OUTPUT);

  // activate I2C
  Wire.begin();

  // find the time
  DateTime now = RTC.now();

  // time now available in now.hour(), now.minute() etc.

  // finished with clock
  //  pinMode (CLOCK_POWER, INPUT);
  //  digitalWrite (CLOCK_POWER, LOW);

  // turn off I2C
  TWCR &= ~(bit(TWEN) | bit(TWIE) | bit(TWEA));

  // turn off I2C pull-ups
  digitalWrite (A4, LOW);
  digitalWrite (A5, LOW);

  // Do something every odd minute
  if (now.minute() % 2) {
    digitalWrite(5, HIGH);
  } else {
    digitalWrite(5, LOW);
  }

  printTime();
  if (DEMO_MODE) {
    float batteryLevel = getBatteryLevel();
    if (batteryLevel >= BATTERY_THRESHOLD) {
      if (now.minute() % 2) {
        
        digitalWrite(5, HIGH);
        float temperature = getTemperature();
        
        if (checkIsRaining() == false) {
          Serial.println("I'm awake");
        }
        else
          Serial.println("Time for bed");
          
      }
      else {
        Serial.println("Time for bed");
      }
    }

    else {
      Serial.println("Time for bed");
    }
  }

  /*Code for future development to run birdhouse in full outdoor mode

    else {
      // Check if weekday
      if (now.dayOfWeek() <= 5) {
        // Check if daytime
        if ((now.hour() > sunriseHour && now.hour() < sunsetHour) || ((now.hour() == sunriseHour && now.minute() >= sunriseMinute) || (now.hour() == sunsetHour && now.minute() <= sunsetMinute)))

        else {

        }

        // it is night time so sleep a hella lot

      }
    }
  */
}
