#!/bin/sh

echo "into appinit"

####################pinmux####################
pinmux.sh

####################ulimit####################
ulimit -s unlimited
ulimit -l unlimited
ulimit -p unlimited

####################create dir####################
mkdir -p /var/run/hostapd
mkdir -p /var/lib/misc
touch /var/lib/misc/udhcpd.leases
mkdir /tmp/net
mkdir /tmp/webserver

####################mount sdcard####################
sdmnt.sh
####################network####################
telnetd
####################insert module####################
app_insertko.sh
media_insertko.sh
net.sh
####################run deamon####################
cp ${EMX_APP_CONFIGS_DIR}/zoneinfo/Etc/GMT-8 /tmp/localtime
allstart2.sh
# daemon.sh&

echo "leave appinit"
