#!/bin/sh
cnt=50
while [ $cnt -gt 0 ]
do
	wlan=`ps | grep udhcpc | grep wlan0`
	eth=`ps | grep udhcpc | grep eth0`
	if [[ -n "$wlan" && -n "$eth" ]]; then
		exit 0
	fi
	let "cnt--"
	usleep 100000
done
if [ -z "$wlan" ]; then
	echo "cannot find wlan0"
fi
if [ -z "$eth" ]; then
	echo "cannot find eth0"
fi
