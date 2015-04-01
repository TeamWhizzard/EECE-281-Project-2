#!/usr/bin/python

# receives time via GPIO from arduino
# based on time decides which main script to run

# if other time
	# turn off usb hub
	# run image script

# if weather report time
	# turn on usb hub and wait for wifi connection
	# run weather station script

# if some other command
	# run ip_pushbullet script for SSH