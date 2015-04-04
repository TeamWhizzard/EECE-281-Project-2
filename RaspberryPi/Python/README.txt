This document describes the functions of each file in the Raspberry Pi directory.

ip_pushbullet_startup.py - runs an infinate loop on startup waiting for WiFi connection. Once connected, sends the IP address of the RPi in a PushBullet message by calling ip_pushbullet_sub.sh.

ip_pushbullet_sub.sh - bash script sends IP address of RPi via PushBullet for SSH.

main_switchScript.py - runs on startup. Recieves weather data from Arduino and uploads the report to twitter. It also takes camera snapshots if it is not raining.

main_imageMode.py - image mode runs until message is recieved from the Arduino for the system to shut off. Script waits for the bird sensor to be triggered and takes an images with the camera, storing it locally in /images/cameraImages/.

main_weatherStation.py - connects to WiFi and calls ftp_images.py to sort camera images. The weather data from Arduino is then formatted into a report and posted to twitter.

ftp_images.py - synch's RPI camera images with Birdhouse FTP server, allowing for manual sorting.

usb_shutdown.sh - script to disable USB for power conservation.

usb_startup.sh - script to enable USB for wifi connection.

--------------------
/images/ directories:

/images/cameraImages/ - stores local images from past 24 hours before they are uploaded to Imgur.

/images/twitImages/ - stores sorted images from Imgur to be used in twitter updates.

--------------------
/codeReferences/ directory:

- outdated or test scripts used for code reference.
