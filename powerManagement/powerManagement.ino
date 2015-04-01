#include <Wire.h>
#include <Arduino.h>
#include "RealTimeClockDS1307.h"
#include "MAX1704.h"

#define RELAY_PIN 5
#define TEMPERATURE_PIN A1
#define RAIN_PIN A2

MAX1704 fuelGauge;

float temperature;
float batteryLevel;
int day, month, year, hours, minutes, seconds;
int isRaining;

void setup() {
  Wire.begin();
  Serial.begin(9600);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(TEMPERATURE_PIN, INPUT);
  pinMode(RAIN_PIN, INPUT);
  RTC.start();
  fuelGauge.reset();
  fuelGauge.quickStart();
  fuelGauge.showConfig();
  delay(1000);
}

void loop() {
  // Report Temperature
  temperature = analogRead(TEMPERATURE_PIN);
  temperature = temperature * 0.48828125;
  Serial.print("Temperature: ");
  Serial.println(temperature);

  // Report if Raining
  isRaining = analogRead(RAIN_PIN);
  Serial.print("Rain Value: ");
  Serial.println(isRaining);

  // Report Time and update if serial monitor has data
  if (Serial.available())
    setTime();
  refreshTime();
  Serial.print("Date & Time: ");
  Serial.print(day);
  Serial.print('/');
  Serial.print(month);
  Serial.print('/');
  Serial.print(year);
  Serial.print(" -- ");
  Serial.print(hours);
  Serial.print(':');
  Serial.print(minutes);
  Serial.print(':');
  Serial.println(seconds);

  // Report Battery Level
  batteryLevel = fuelGauge.stateOfCharge();
  Serial.print("Battery: ");
  Serial.println(batteryLevel);

  // Change Relay
  if ((seconds >= 0) && (seconds <= 2)) {
    digitalWrite(RELAY_PIN, HIGH);
    Serial.println("Relay ON");
  } else if ((seconds >= 30) && (seconds <= 32)) {
    digitalWrite(RELAY_PIN, LOW);
    Serial.println("Relay OFF");
  }
  
  // Delay and do it all over again
  delay(3000);
  Serial.println("---------");
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

void setTime() {
  if (!Serial.available()) {
    return;
  }
  char command = Serial.read();
  int in, in2;
  switch (command)
  {
    case 'H':
    case 'h':
      in = SerialReadPosInt();
      RTC.setHours(in);
      RTC.setClock();
      Serial.print("Setting hours to ");
      Serial.println(in);
      break;
    case 'I':
    case 'i':
      in = SerialReadPosInt();
      RTC.setMinutes(in);
      RTC.setClock();
      Serial.print("Setting minutes to ");
      Serial.println(in);
      break;
    case 'S':
    case 's':
      in = SerialReadPosInt();
      RTC.setSeconds(in);
      RTC.setClock();
      Serial.print("Setting seconds to ");
      Serial.println(in);
      break;
    case 'Y':
    case 'y':
      in = SerialReadPosInt();
      RTC.setYear(in);
      RTC.setClock();
      Serial.print("Setting year to ");
      Serial.println(in);
      break;
    case 'M':
    case 'm':
      in = SerialReadPosInt();
      RTC.setMonth(in);
      RTC.setClock();
      Serial.print("Setting month to ");
      Serial.println(in);
      break;
    case 'D':
    case 'd':
      in = SerialReadPosInt();
      RTC.setDate(in);
      RTC.setClock();
      Serial.print("Setting date to ");
      Serial.println(in);
      break;
    case 'W':
      Serial.print("Day of week is ");
      Serial.println((int) RTC.getDayOfWeek());
      break;
    case 'z':
      RTC.start();
      Serial.println("Clock oscillator started.");
      break;
    case 'Z':
      RTC.stop();
      Serial.println("Clock oscillator stopped.");
      break;
    default:
      Serial.println("Unknown command. Try these:");
      Serial.println(" h## - set Hours d## - set Date");
      Serial.println(" i## - set mInutes m## - set Month");
      Serial.println(" s## - set Seconds y## - set Year");
      Serial.println();
      Serial.println(" z - start clock Z - stop clock");
      Serial.println();
  }
}

//read in numeric characters until something else
//or no more data is available on serial.
int SerialReadPosInt() {
  int i = 0;
  boolean done = false;
  while (Serial.available() && !done)
  {
    char c = Serial.read();
    if (c >= '0' && c <= '9')
    {
      i = i * 10 + (c - '0');
    }
    else
    {
      done = true;
    }
  }
  return i;
}
