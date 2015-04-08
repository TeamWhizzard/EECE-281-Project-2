#!/usr/bin/python

import time
import RPi.GPIO as GPIO
import picamera
from datetime import datetime
import os

def imageMode():
	# constant and variable declarations
	pinBirdSensor = 5 # GPIO pin
	count = 0 # picture count initialization for demo mode

	GPIO.setmode(GPIO.BCM) # T-cobbler setup in BCM mode
	GPIO.setup(pinBirdSensor, GPIO.IN, pull_up_down=GPIO.PUD_UP) # Setup GPIO Pin 5 IN

	camera = picamera.PiCamera() # create instance of camera class
	camera.exposure_settings = 'auto' # sets to auto-adjust exposure settings

	while (count < 5):
  		if (GPIO.input(pinBirdSensor) == 1): # sensor has been triggered
			print("Image Triggered")
			# create file name with current date and time    
			i = datetime.now()

    		now = i.strftime('%Y%m%d-%H%M%S')
    		photo_name = "images/cameraImages/" + now + '.jpg'
			
			# take photo and increase photo count
			camera.capture(photo_name)
			count += 1

	camera.close() # conserve resources

#imageMode()
