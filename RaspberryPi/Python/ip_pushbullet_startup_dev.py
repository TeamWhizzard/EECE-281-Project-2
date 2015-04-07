#!/usr/bin/python
import subprocess
import re

# loops until WiFi dongle connects to UBC secure
# runs script to send IP address to Pushbullet
def sendPushbullet():
	subScript = "/home/pi/EECE-281-Project-2/RaspberryPi/Python/ip_pushbullet_sub.sh"
	while(True):
		p = subprocess.Popen(subScript, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
		line = p.stdout.read()

		print line
		# breaks if pushbullet message has sent succesfully
		# regex to search for what constitutes an IP Address 
		# if the message has an ip address, internet connection works
		if(re.search("[\d]{1,3}\.[\d]{1,3}\.[\d]{1,3}\.[\d]{1,3}", line)):
			break
	
sendPushbullet()
#206.87.112.248

