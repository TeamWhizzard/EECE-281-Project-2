#! /usr/bin/python3

# install picamera library into python version
# see tutorial for other features
# http://www.raspberrypi.org/documentation/usage/camera/python/README.md
# uploading to twitter
# http://raspi.tv/2014/taking-and-tweeting-a-photo-with-raspberry-pi-part-4-pi-twitter-app-series
import picamera
import time

# with statements are good
# and calls close() implicitly
# with picamera.PiCamera() as camera:
#    camera.start_preview()
#    time.sleep(10)
#    camera.stop_preview()

# TODO adjust settings based on current light ? (from light sensor)

# create instance of camera class
camera = picamera.PiCamera();

# set camera resolution - automatically sets to display
# size, else default is 1280x720
camera.resolution(1280, 720)

# take a picture
# also an option to capture to stream..
camera.capture('image.jpg')

# preview camera on screen - overlays python session
camera.start_preview()
time.sleep(5) # timer in seconds
camera.stop_preview() # restores display

camera.close() # clean up camera resources