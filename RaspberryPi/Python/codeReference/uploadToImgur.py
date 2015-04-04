#!/usr/bin/python
# https://github.com/Imgur/imgurpython

import os, sys, re
from imgurpython import ImgurClient

newImagesAlbum = 'ogvkH'
path = "/home/pi/EECE-281-Project-2/RaspberryPi/Python/images/cameraImages"
	
# Imgur authentication
client_id = '7d93686adde0c8d'
client_secret = '9bbd8aa21c99a00ec2f5319ea2271f614f29fb33'
access_token = '98fc8cd909ba029bd210cf0a7c76c5609f51278a'
refresh_token = 'f02874b84943949c43c927cf30f8eebbe299494b'
	# note that library will automatically generate a new access_token, 
	# which expires after one hour
	
client = ImgurClient(client_id, client_secret, access_token, refresh_token)
	
	#credentials = client.authorize('2b01d6edad', 'pin')
	#client.set_user_auth(credentials['access_token'], credentials['refresh_token'])
	#print (credentials['access_token'] + "\n")
	#print (credentials['refresh_token'] + "\n") 
	
config = {
	'album': newImagesAlbum, # album ID or None
	'name': 'Test',
	'title': 'Test',
	'description': 'Whizzards are cool'
	 }

def upload():
	dirs = os.listdir(path)
	for image in dirs:
		print (image)
		if re.search(".jpg$", image):
			config.name = image
			config.title = image
			link = client.upload_from_path(path + "/" + image, config=config, anon=False)
			os.remove(path + "/" + image)
			print("Image was posted! Go check your images you sexy beast!")
			print("You can find it here: {0}".format(link['link']))

			

def getAlbum():
	test=client.get_account_image_ids("teamwhizzard")
	for images in test:
		print images

getAlbum()
