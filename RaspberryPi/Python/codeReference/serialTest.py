#!/usr/bin/python

import serial

ser = serial.Serial('/dev/ttyAMA0', 9600, timeout=1)
ser.open()
	 
ser.write("testing")

try:
  while 1:
    response = ser.readline()
    print response
except KeyboardInterrupt:
  ser.close()
