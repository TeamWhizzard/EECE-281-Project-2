This document describes the functions of the files in this directory.

ip_pushbullet_startup.py - runs infinate loop on startup. Once connected to WiFi sends the IP address of the RPi in a PushBullet message for SSH by calling ip_pushbullet_sub.sh.

ip_pushbullet_sub.sh - bash script sends IP address of RPi via PushBullet for SSH.

main_switchScript.py - runs on startup. Recieves date/time from Arduino and runs camera or weather mode depending on the time of day.

main_imageMode.py - image mode runs until messge is recieved from the Arduino for the system to shut off. Script waits for a trip on the bird sensor and takes an images with the camera, storing it locally in /images/cameraImages/

main_weatherStation.py - connects to WiFi and uploads local pictures from the previous day in /images/cameraImages/ to Imgur. Script then downloads sorted images from Imgur into /images/twitImages/. The weather data from arduino is then formatted into a report and posted to twitter.

usb_startup.sh - script to enable USB hub for wifi connection

usb_shutdown.sh - script to disable USB for wifi connection

--------------------
/images/ directories:

/images/cameraImages/ - stores local images from past 24 hours before they are uploaded to Imgur.

/images/twitImages/ - stores sorted images from Imgur to be used in twitter updates.
