#!/usr/bin/python

import time
import RPi.GPIO as GPIO
import picamera
from datetime import datetime

pinBirdSensor = 5 # GPIO pin

GPIO.setmode(GPIO.BCM) # T-cobbler setup in BCM mode
GPIO.setup(pinBirdSensor, GPIO.IN, pull_up_down=GPIO.PUD_UP) # Setup GPIO Pin 5 IN

camera = picamera.PiCamera() # create instance of camera class
camera.exposure_settings = 'auto' # sets to auto-adjust exposure settings

# TODO run loops until shutdown command recieved from Arduino - then perform shutdown command so things dont break
while (True): #GPIO.output(5,True) ## Turn on GPIO pin 5
  if (GPIO.input(pinBirdSensor) == 1):
    i = datetime.now()               #take time and date for filename
    now = i.strftime('%Y%m%d-%H%M%S')
    photo_name = "images/cameraImages/" + now + '.jpg'
    camera.capture(photo_name)

# if not using infinite loop, call camera.close() at end of usage to conserve resources
