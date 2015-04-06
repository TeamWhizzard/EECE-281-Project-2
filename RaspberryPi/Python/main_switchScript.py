#!/usr/bin/python

import serial
import subprocess
import urllib2
import time
import RPi.GPIO as GPIO

# custom scripts
import main_imageMode
import main_weatherStation
import ip_pushbullet_startup

# bash scripts
usbOff = "/home/pi/EECE-281-Project-2/RaspberryPi/Python/usb_shutdown.sh"
usbOn = "/home/pi/EECE-281-Project-2/RaspberryPi/Python/usb_startup.sh"

# initialize interrupt pin
pinInterrupt = 6
GPIO.setmode(GPIO.BCM) # map for T-cobbler
GPIO.setup(pinInterrupt, GPIO.OUT)

GPIO.output(pinInterrupt, True) # set high
time.sleep(2)
GPIO.output(pinInterrupt, False) # send start signal to arduino

print("Arduino signal sent")

# open serial port
ser = serial.Serial('/dev/ttyAMA0', 57600) #, timeout=1)
ser.open() 

while True: # read from serial port until command recieved
	input = ser.readline()
	print(input + "\n") # for testing
		
	data = input.split(",")
	if (data[0] is "1" or "0"): # Arduino requested image mode
		if (data[0] is "0"): # take pictures if not raining
			print("Image mode \n")
		
			# turn off usb hub for image mode to conserve power
			#off = subprocess.Popen(usbOff, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
			main_imageMode.imageMode()
			#on = subprocess.Popen(usbOn, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)

		print("Weather mode \n") 
		while True: # confirm internet connection
			try:
				urllib2.urlopen("http://www.google.com").close()
			except urllib2.URLError:
				# wifi not connected
				time.sleep(0.5) # sleep time in seconds
			else: # wifi is connected
				main_weatherStation.weatherStation(data[0], data[2]) # rain, temperature
				break

		GPIO.output(pinInterrupt, True) # set high
		time.sleep(0.5)
		GPIO.output(pinInterrupt, false) # send start signal to arduino
		
		os.system("sudo shutdown -h now")
