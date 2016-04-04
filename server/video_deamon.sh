#!/bin/bash

while true:
	wait_shutter
	
	#turn off LED
	killall videosend
	
	sleep 2
	
	#turn on LED
	videosend &
