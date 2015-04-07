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

RTC_DS1307 RTC;
MAX1704 fuelGauge;

// Set software serial pins for bluetooth connection (mainly used in demo mode)
SoftwareSerial mySerial(12, 13); // RX, TX

// Set demo mode on and off with values 1 and 0 respectively
#define DEMO_MODE 1

// Defined pins
#define RPI_INTERRUPT 1
#define PI_POWER_PIN 5

#define BATTERY_THRESHOLD 10//20 // battery threshold set to 20% to prevent battery from getting fully depleted

// Standard sleep cycles
#define RPI_STARTUP_DELAY 2 // the initial state of rpi gpio pins is unstable, this delay sleeps through the fluctuations
#define DEMO_RESTART_CYCLE 2 // charge time between photo shoot sessions in demo mode, equivalent to 16 seconds
#define RECHARGE_CYCLE 337 // charge time between photo shoot sessions, equivalent to 45 minutes
#define HIBERNATE 2700 // charge time for when battery is below 20%, equivalent to 6 hours
#define NIGHT_TIME 4050 // sleep mode for night time, equivalent to 9 hours (ie. from 8pm to 5am)
#define WEEKEND_MODE 21600 // charge time for the weekend, equivalent to 48 hours

uint16_t lastRainVal = 1023; // set a "last" rain value to compare to, originally max level of dryness
float batteryLevel;
float temperature;
bool rainStatus; // status of the rain, 1 if raining, 0 if not raining
volatile int piDelay; // value for exiting infinite loop in naptTime() using an interrupt from the raspberry pi

// defined sunrise and sunset values, 5am for sunrise and 8pm for sunset
uint8_t sunriseHour = 5;
uint8_t sunriseMinute = 0;
uint8_t sunsetHour = 20;
uint8_t sunsetMinute = 0;

/*
*------------------------------------------------------------------------------------------
* Watchdog Timer Functions
*    - ISR (WDT_vect)
*    - myWatchdogEnable()
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
* ADC Conversion Functions
*    - adc_init()
*    - adc_read(uint8_t ch)
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
*    - napTime()
*    - shutdownPi()
*    - sleep_cycle(int delayVal)
*    - startPi()
*    - waitForPi()
*------------------------------------------------------------------------------------------
*/
// turns on raspberry pi and waits for a message back when it is fully awake
void startPi() {
  digitalWrite(PI_POWER_PIN, HIGH); // wake up raspberry pi
  sleep_cycle(RPI_STARTUP_DELAY);
  EIFR |= 0x01; // clears the interrupt flag
  mySerial.println("Interrupt flag cleared!");
  piDelay = 1;
  napTime(); // sleep until raspberry pi sends interrupt
}

// waits for raspberry pi to finish so that it can shut it down
void waitForPi() {
  piDelay = 1;
  napTime(); // sleep until raspberry pi sends interrupt
}

// waits 24 seconds for raspberry pi to prepared to be shutdown
void shutdownPi() {
  sleep_cycle(3);
  digitalWrite(PI_POWER_PIN, LOW); // cut off power to raspberry pi
}

// sleep until raspberry pi sends interrupt
void napTime() {
  while (piDelay)
    sleep_cycle(1); // sleep for 8 seconds until interrupt changes value of piDelay to 0
}

// enables the watchdog sleep cycle, delayVal=1 is equivalent to 8 seconds of sleep
void sleep_cycle(int delayVal) {
  for (int i = 0; i < delayVal; i++)
    myWatchdogEnable ();
}

/*
*------------------------------------------------------------------------------------------
* Sensor Functions
*    - checkForRain()
*    - getBatteryLevel()
*    - getTemperature()
*    - printSensorInfo()
*    - printTime()
*    - refreshSensors()
*    - rpiAtmegaDataTransfer()
*    - stringCreate()
*------------------------------------------------------------------------------------------
*/
// refreshes all the values from each sensor
void refreshSensors() {
  // activate I2C
  Wire.begin();

  // refresh all sensors
  getBatteryLevel();
  getTemperature();
  rainStatus = checkForRain();

  // turn off I2C
  TWCR &= ~(bit(TWEN) | bit(TWIE) | bit(TWEA));
  // turn off I2C pull-ups
  digitalWrite (A4, LOW);
  digitalWrite (A5, LOW);
}

// prints updated sensor data to the serial monitor via bluetooth
void printSensorInfo() {
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
}

// report battery level
void getBatteryLevel() {
  batteryLevel = fuelGauge.stateOfCharge();
}

// report temperature
void getTemperature() {
  adc_read(2); // move the ADC to the LM35
  delay(100); // delay to allow the reading to settle
  uint16_t temperatureRaw = adc_read(2); // take the actual temperature reading we use
  temperature =  temperatureRaw * 0.48828125;
}

// report if raining
bool checkForRain() {
  adc_read(3); // move the ADC to the rain sensor
  delay(10); // delay to allow the reading to settle
  uint16_t newRainVal = adc_read(3);

  /* checks for reasonable amount of rain, and also determines if there has been enough of a change in resistance
   * between new and last readings to see if it has stopped raining (ie. if it stops it should theoretically dry
   * by a difference of around 200)
   */
  if (newRainVal < 900 && newRainVal < lastRainVal + 200) {
    lastRainVal = newRainVal;
    return true;
  }
  else {
    lastRainVal = newRainVal;
    return false;
  }
}

// prints the current date and time to the serial monitor via bluetooth
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
}

// creates a single string out of all the sensor data to be sent over to the raspberry pi
String stringCreate() {
  refreshSensors();
  String rain = String(rainStatus);
  String batt = String(batteryLevel);
  String temp = String(temperature);

  String message = rain + "," + batt + "," + temp; // combines all three strings into one
  return message;
}

/* prints the string with sensor information to be read by the raspberry pi and also to be
 * seen on the serial monitor in demo mode
 */
void rpiAtmegaDataTransfer() {
  //String sensorInfo = stringCreate();
 //delay(5000);
  
  refreshSensors();
  

  // serial greeting
  String greeting = "Hello Pi!";
  Serial.println(greeting);
  mySerial.println(greeting);

  //print rain
  Serial.println(rainStatus);
  mySerial.println(rainStatus);
  
  // print battery
  Serial.println(batteryLevel);
  mySerial.println(batteryLevel);

  // print temperature
  Serial.println(temperature);
  mySerial.println(temperature);
  
  //Serial.println(sensorInfo); // sends updated sensor readings to the raspberry pi
  //mySerial.println(sensorInfo); // prints the sensor message that is sent to the raspberry pi to the serial monitor
}

/*
*------------------------------------------------------------------------------------------
* Interrupt Functions
*    - rpiInterrupt()
*------------------------------------------------------------------------------------------
*/
// interrupt function that changes value of piDelay in order to exite the infinite loop in napTime()
void rpiInterrupt() {
  sleep_disable();
  piDelay = 0;
  mySerial.println("ISR Toggled");
}

/*
*------------------------------------------------------------------------------------------
* Setup Function
*------------------------------------------------------------------------------------------
*/
void setup()
{
  RTC.begin();  // activate clock (doesn't do much)
  Serial.begin(115200); // raspberry pi and atmega328p communication
  mySerial.begin(9600); // bluetooth communication
  adc_init(); // used to help read analog pins on the atmega328p
  pinMode(PI_POWER_PIN, OUTPUT); // set the pin that turns the latch relay on and off to control the state of the raspberry pi
  attachInterrupt(RPI_INTERRUPT, rpiInterrupt, FALLING); // sets interrupt pin to activate on falling edge
  
  // configures fuel gauge to be ready for use
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
  // code for demo mode
  if (DEMO_MODE) {
    printSensorInfo();

    // checks to see if battery level is above threshold before turning on the raspberry pi
    if (batteryLevel >= BATTERY_THRESHOLD) {
      mySerial.println("Starting up raspberry pi");
      startPi();
      rpiAtmegaDataTransfer(); // talks to raspberry pi
      mySerial.println("Waiting for raspberry pi");
      waitForPi();
      mySerial.println("Shutting down raspberry pi");
      shutdownPi();
      mySerial.println("Restarting cycle");
      sleep_cycle(DEMO_RESTART_CYCLE); // enter sleep mode for 16 seconds between photo shoot sessions
    }
    // sleeps the atmega328p if battery level is below threshold
    else {
      sleep_cycle(HIBERNATE); // battery low, atmega328p is put to sleep for 6 hours then battery level is checked again
    }
  }

  // code to run birdhouse in full outdoor mode
  else {
    refreshSensors();
    DateTime now = RTC.now();

    if (batteryLevel >= BATTERY_THRESHOLD) {
      // check if weekday
      if (now.dayOfWeek() <= 5) {
        // check if daytime
        if ((now.hour() > sunriseHour && now.hour() < sunsetHour) || ((now.hour() == sunriseHour && now.minute() >= sunriseMinute) || (now.hour() == sunsetHour && now.minute() <= sunsetMinute))) {
          startPi();
          rpiAtmegaDataTransfer(); // talking to raspberry pi
          waitForPi();
          shutdownPi();
          sleep_cycle(RECHARGE_CYCLE); // enter sleep mode for 45 minutes to recharge battery between photo shoot sessions
        }
        // sleep for 9 hours if night time
        else {
          sleep_cycle(NIGHT_TIME); // enter sleep mode for the night time
        }
      }
      // sleep for 48 hours if weekend
      else {
        sleep_cycle(WEEKEND_MODE); // enter sleep mode for the weekend to allow time for charging
      }
    }
    // sleeps the atmega328p if battery level is below threshold
    else {
      sleep_cycle(HIBERNATE); // battery low, atmega328p is put to sleep for 6 hours then battery level is checked again
    }
  }
}
