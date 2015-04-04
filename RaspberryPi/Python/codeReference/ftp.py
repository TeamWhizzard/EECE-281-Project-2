#!/usr/bin/python

import os

source = "/home/pi/EECE-281-Project-2/RaspberryPi/Python/images/twitImages"
os.chdir(source)
list = sorted(os.listdir(source), key=os.path.getmtime)
for image in list:
	print image
