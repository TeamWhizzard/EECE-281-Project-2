#!/usr/bin/python

import picamera
import time
import tweepy
from subprocess import call
from datetime import datetime

camera = picamera.PiCamera() # create instance of camera class

i = datetime.now()               #take time and date for filename  
now = i.strftime('%Y%m%d-%H%M%S')  
photo_name = now + '.jpg'  

camera.capture(photo_name)

consumer_key = "9MugNn6CKoaPxF7Cah7xMVpbD"
consumer_secret = "2JQeOIKLUEQvHgEoziTPeDiHhgkrrhTez9jjrw2bZ2eaMMWwAj"
access_token = "3029136165-2j9Qpr14i0LfyvDGJXFsCEEfRO918FnIi2iqfLC"
access_token_secret = "aBKmJuHxwVZwSPCo44bSV5s8xWRlhA2P7HpjPelRLmeUz"

auth = tweepy.OAuthHandler(consumer_key, consumer_secret)  
auth.set_access_token(access_token, access_token_secret)

api = tweepy.API(auth) 

# Send the tweet with photo  
photo_path = '/home/pi/EECE-281-Project-2/RaspberryPi/Python/codeReference/' + photo_name  
status = "Greetings, from Team Whizzard's birds!"  
api.update_with_media(photo_path, status=status) 

camera.close()
