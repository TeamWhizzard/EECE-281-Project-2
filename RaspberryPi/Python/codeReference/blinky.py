#!/usr/bin/python

import time
import RPi.GPIO as GPIO # Import GPIO library

GPIO.setmode(GPIO.BCM) # T-cobbler setup in BCM mode
GPIO.setup(5, GPIO.IN, pull_up_down=GPIO.PUD_UP) # Setup GPIO Pin 5 OUT

while (True): #GPIO.output(5,True) ## Turn on GPIO pin 5
	print (GPIO.input(5))
	time.sleep(1)
