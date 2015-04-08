#!/usr/bin/python

import tweepy
import os

# custom files
import ftp_images

def weatherStation(rain, temperature):
	# image ftp processing
	ftp_images.imageControl() 

	# get oldest image in twitImages directory
	source = "/home/pi/EECE-281-Project-2/RaspberryPi/Python/images/twitImages"
	os.chdir(source)
	list = sorted(os.listdir(source), key=os.path.getmtime)	
	twitImage = source + "/" + list[0]

	# post weather report to twitter
	consumer_key = "9MugNn6CKoaPxF7Cah7xMVpbD"
	consumer_secret = "2JQeOIKLUEQvHgEoziTPeDiHhgkrrhTez9jjrw2bZ2eaMMWwAj"
	access_token = "3029136165-2j9Qpr14i0LfyvDGJXFsCEEfRO918FnIi2iqfLC"
	access_token_secret = "aBKmJuHxwVZwSPCo44bSV5s8xWRlhA2P7HpjPelRLmeUz"

	auth = tweepy.OAuthHandler(consumer_key, consumer_secret)
	auth.set_access_token(access_token, access_token_secret)

	api = tweepy.API(auth)

	# generate weather data report
	status = "Rise and shine, Thunderbirds! "
	if (rain is "1"):
		status += "It's a rainy day at UBC, "
	else:
		status += "It's a sunny day at UBC, "

	status += "with a temperature of " + temperature + " degrees Celsius."
	print(status+"\n")
	# post weather report to twitter
	print("Post to Twitter \n")
	print(twitImage)
	api.update_with_media(twitImage, status=status)
	# delete image posted to twitter from local directory
	os.remove(twitImage)	

#weatherStation("0","24")
