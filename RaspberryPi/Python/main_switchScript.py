#!/usr/bin/python

import serial
import subprocess
import urllib2
import time
import RPi.GPIO as GPIO
import os
import datetime

# custom scripts
import main_imageMode
import main_weatherStation
import ip_pushbullet_startup

# bash scripts
usbOff = "/home/pi/EECE-281-Project-2/RaspberryPi/Python/usb_shutdown.sh"
usbOn = "/home/pi/EECE-281-Project-2/RaspberryPi/Python/usb_startup.sh"

# initialize interrupt pin
pinInterrupt = 6       # GPIO6
GPIO.setmode(GPIO.BCM) # map for T-cobbler
GPIO.setup(pinInterrupt, GPIO.OUT)

# send interrupt pulse to wake ATmega328p
GPIO.output(pinInterrupt, True)
GPIO.output(pinInterrupt, False)
print("Trigger Interrupt\n")

# open serial port
ser = serial.Serial('/dev/ttyAMA0', 115200) #, timeout=1)
ser.open() 
print("Serial port open\n")

# send interrupt confirmation message to ATmega328p via serial
ser.write("Hello Arduino!")
print("Interrupt confirmation sent")

while True: # read from serial port until data received

	# default values in case pi doesn't receive serial correctly
	rain = "0"
	battery = "100"
	temp = "20"

	try:
		greeting = ser.readline()
		rain = ser.readline().strip()
		battery = ser.readline().strip()
		temp = ser.readline().strip()
	except: 
		pass
	
	print("rain " + rain)
	print("battery " + battery)
	print("temp " + temp + "\n")
	
	if (rain == "1" or rain == "0"): # ATmega328p requested image mode
		if (rain in "0"): # take pictures if not raining
			print("Image mode \n")
		
			# turn off usb hub for image mode to conserve power
			#print("Shut off USB port\n")
			#off = subprocess.Popen(usbOff, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
			main_imageMode.imageMode()
			#print("Turn on USB port\n")
			#on = subprocess.Popen(usbOn, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)

		print("Weather mode \n") 
		while True: # confirm internet connection
			try:
				urllib2.urlopen("http://www.google.com").close()
			except urllib2.URLError:
				# wifi not connected
				time.sleep(0.5) # sleep time in seconds
			else: # wifi is connected
				print(datetime.datetime.now()
				ip_pushbullet_startup.sendPushbullet()
				main_weatherStation.weatherStation(rain, temp) # rain, temperature
				break

		ser.close()
		
		# send interrupt pulse to wake ATmega328p
		GPIO.output(pinInterrupt, True) # set high
		GPIO.output(pinInterrupt, False) # send start signal to arduino
		print("Trigger Interrupt\n")
		
		# Shut down RPi
		print("Shutting down")
		os.system("sudo shutdown -h now")
