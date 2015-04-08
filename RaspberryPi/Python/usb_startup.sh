#!/bin/bash
#Code to start
echo 1 > /sys/devices/platform/bcm2708_usb/buspower;
sleep 2;
/etc/init.d/networking start
