#!/bin/bash

sudo rmmod usbtest 2>/dev/null
sudo modprobe usbtest pattern=1
