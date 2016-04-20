#!/usr/bin/env python
#-*- coding: utf-8 -*-
import os
import picamera

camera = picamera.PiCamera()
camera.vflip = True
camera.brightness = 70

camera.capture('cctv.jpg')
os.system('tg-send.sh ID "출입자가 있습니다." `pwd`/cctv.jpg')
