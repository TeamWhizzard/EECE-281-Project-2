#!/usr/bin/python

import os
import shutil
from ftplib import FTP

cameraImages = "/home/pi/EECE-281-Project-2/RaspberryPi/Python/images/cameraImages"
twitImages = "/home/pi/EECE-281-Project-2/RaspberryPi/Python/images/twitImages"

def imageControl():
	ftp = FTP('ubc.dyndns.org') # connect to host
	ftp.login('teamWhizzard','awesomeWhiz1') # authentication

	ftp.cwd('Birdhouse') # navigate to birdhouse directory

	# compare current twitImages to server images delete sorted images

	piTwitList =  os.listdir(twitImages) # returns list files 
	ftpList = ftp.nlst() # list of images in FTP

	for image in piTwitList:
		# if image has been removed from ftp server, remove from pi
		if image not in ftpList:
			os.remove(twitImages + "/" + image)
	
	for image in os.listdir(cameraImages):
		fullImage = cameraImages + "/" + image

		# upload camera images to ftp folder
		file = open(fullImage,'rb')
		ftp.storbinary('STOR ' + image, file)
		file.close()
	
		# move rpi cameraImages to twitImages
        	print image
        	shutil.move(fullImage, twitImages)