#!/usr/bin/python
import subprocess
import re

''' Note: dev version of ip_pushbullet_startup.python
calls sendPushbullet() function when script is run'''

# loops until WiFi dongle connects to UBC secure
# runs script to send IP address to Pushbullet
def sendPushbullet():
	# PushBullet script name
	subScript = "/home/pi/EECE-281-Project-2/RaspberryPi/Python/ip_pushbullet_sub.sh"
	while(True): # loops until WiFi dongle connects to UBC secure
		# runs script to send IP address to PushBullet
		p = subprocess.Popen(subScript, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
		line = p.stdout.read() # output message from PushBullet bash script

		print line
		# breaks loop if PushBullet message has sent successfully
		# regex to search for what constitutes an IP Address 
		# if the message has an IP address, internet connection works
		if(re.search("[\d]{1,3}\.[\d]{1,3}\.[\d]{1,3}\.[\d]{1,3}", line)):
			break
	
sendPushbullet()
