#!/usr/bin/python
import subprocess

# loops until WiFi dongle connects to UBC secure
# runs script to send IP address to Pushbullet
def runProcess(exe):
	while(True):
		p = subprocess.Popen(exe, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
		line = p.stdout.read()

		print line
		# breaks if pushbullet message has sent succesfully
		if("active" in line):
			break
	
runProcess("/home/pi/EECE-281-Project-2/RaspberryPi/Python/ip_pushbullet_sub.sh")

