/*
*------------------------------------------------------------------------------------------
* Libraries / Objects / Defined Values / Global Variables
*------------------------------------------------------------------------------------------
*/
#include <Wire.h>
#include <SoftwareSerial.h>
#include "RTClib.h"
#include "MAX1704.h"

#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/io.h>
#include <avr/interrupt.h>

RTC_DS1307 RTC;
MAX1704 fuelGauge;
SoftwareSerial mySerial(12, 13); // RX, TX

#define DEMO_MODE 1
#define RPI_INTERRUPT_PIN 1
#define PI_POWER_PIN 5
#define CLOCK_POWER 9
#define BATTERY_THRESHOLD 20
#define SLEEP_FOR_30 4
#define HIBERNATE 2700 // sleep

uint16_t lastRainVal = 1023;
float batteryLevel;
float temperature;
bool rainStatus;
volatile bool piDelay;

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
void sleep_cycle(int delayVal) {
  for (int i = 0; i < delayVal; i++)
    myWatchdogEnable ();
}

void startPi() {
  digitalWrite(PI_POWER_PIN, HIGH); // wake up raspberry pi
  //setupInterrupt();
  //piDelay = true;
  //napTime();
}

void waitForPi() {
  //setupInterrupt();
  //  piDelay = true;
  //  napTime();
  sleep_cycle(8);
  rpiAtmegaDataTransfer(); // talking to raspberry pi
  Serial.println("Can you hear me?");
  mySerial.println("Can you hear me?");
}

void shutdownPi() {
  napTime();
  sleep_cycle(3);
  digitalWrite(PI_POWER_PIN, LOW); // cut off power to raspberry pi
}

void napTime() {
  while (piDelay)
    sleep_cycle(1);
}

/*
*------------------------------------------------------------------------------------------
* Sensor Functions
*------------------------------------------------------------------------------------------
*/
//Refreshes all the values from each sensor
void refreshSensors() {
  getBatteryLevel();
  getTemperature();
  rainStatus = checkForRain();
}

//Prints updated sensor data to the serial monitor via bluetooth
void printSensorInfo() {
  // activate I2C
  Wire.begin();
  refreshSensors();
  printTime();

  mySerial.print("Battery: ");
  mySerial.println(batteryLevel);

  mySerial.print("Temperature: ");
  mySerial.println(temperature);

  if (rainStatus == false)
    mySerial.println("Clear Skies");
  else
    mySerial.println("Rain");

  mySerial.println("");

  // turn off I2C
  TWCR &= ~(bit(TWEN) | bit(TWIE) | bit(TWEA));
  // turn off I2C pull-ups
  digitalWrite (A4, LOW);
  digitalWrite (A5, LOW);
}

// Report battery level
void getBatteryLevel() {
  batteryLevel = fuelGauge.stateOfCharge();
}

// Report temperature
void getTemperature() {
  adc_read(2); // move the ADC to the LM35
  delay(10); // delay to allow the reading to settle
  uint16_t temperatureRaw = adc_read(2); // take the actual temperature reading we use
  temperature =  temperatureRaw * 0.48828125;
  delay(100); // allow the ADC to settle
}

// Report if raining
bool checkForRain() {
  adc_read(3); // move the ADC to the rain sensor
  delay(10); // delay to allow the reading to settle
  uint16_t newRainVal = adc_read(3);

  if (newRainVal < 900 && newRainVal < lastRainVal + 200) {
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
  mySerial.print(now.year(), DEC);
  mySerial.print('/');
  mySerial.print(now.month(), DEC);
  mySerial.print('/');
  mySerial.print(now.day(), DEC);
  mySerial.print(' ');
  mySerial.print(now.hour(), DEC);
  mySerial.print(':');
  mySerial.print(now.minute(), DEC);
  mySerial.print(':');
  mySerial.print(now.second(), DEC);
  mySerial.println();
  delay(1000);
}

String stringCreate() {
  refreshSensors();
  String rain = String(rainStatus);
  String batt = String(batteryLevel);
  String temp = String(temperature);

  String message = rain + "," + batt + "," + temp;
  return message;
}

void rpiAtmegaDataTransfer() {
  String sensorInfo = stringCreate();
  Serial.println(sensorInfo); // sends updated sensor readings to the raspberry pi
  mySerial.println(sensorInfo); // prints the sensor message that is sent to the raspberry pi to the serial monitor
}

/*
*------------------------------------------------------------------------------------------
* Interrupt Functions
*------------------------------------------------------------------------------------------
*/
void rpiInterrupt() {
  piDelay = false;
}

//void setupInterrupt() {
//  DDRB  = 0b11111111;   // All outputs
//  DDRC  = 0b01111111;   // All outputs (Although we will just use PC0 and PC1)
//  DDRD  = 0b11111011;   // set PD2 to input
//  PORTD = 0b00000100;   // set PD2 to high
//
//  EIMSK |= (1 << INT0);     // Turns on INT0
//  EICRA |= (1 << ISC01);    // set INT0 to trigger on ANY logic change
//
//  sei();                    // turn on interrupts
//}
//
//ISR (SIG_INTERRUPT0) {
//  piDelay = false;
//}

/*
*------------------------------------------------------------------------------------------
* Setup Function
*------------------------------------------------------------------------------------------
*/
void setup()
{
  RTC.begin();  // activate clock (doesn't do much)
  Serial.begin(9600); // raspberry pi and atmega328p communication
  mySerial.begin(9600); // bluetooth communication
  attachInterrupt(RPI_INTERRUPT_PIN, rpiInterrupt, CHANGE);
  adc_init();
  pinMode(PI_POWER_PIN, OUTPUT);
  fuelGauge.reset();
  fuelGauge.quickStart();
  fuelGauge.showConfig();
  delay(10000); // delay for bluetooth pairing
}  // end of setup

/*
*------------------------------------------------------------------------------------------
* Main Loop Function
*------------------------------------------------------------------------------------------
*/
void loop()
{
  if (DEMO_MODE) {
    printSensorInfo();

    if (batteryLevel >= BATTERY_THRESHOLD) {
      mySerial.println("Starting up raspberry pi");
      delay(100);
      startPi();
      rpiAtmegaDataTransfer(); // talking to raspberry pi
      mySerial.println("Waiting for raspberry pi");
      delay(100);
      waitForPi();
      mySerial.println("Shutting down raspberry pi");
      delay(100);
      shutdownPi();
    }
    else {
      sleep_cycle(HIBERNATE); // battery low, atmega328p is put to sleep for 6 hours then battery level is checked again
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
