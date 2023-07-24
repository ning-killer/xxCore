#!/bin/sh
eth0=`ifconfig -a | grep eth0`
if [[ "$eth0" != "" ]];then
    ifconfig eth0 up
    udhcpc -b -t 1 -T 1 -A 3 -i eth0 -s /root/firmware/app/configs/net/udhcpc.script -R
fi

# wlan0=`ifconfig -a | grep wlan0`
# if [[ "$wlan0" != "" ]];then
#     ifconfig wlan0 up
#     wpa_supplicant -B -Dnl80211 -i wlan0 -c ${FIRMWARE}app/configs/net/wifi.conf
#     # udhcpc -b -t 1 -T 2 -A 3 -i wlan0 -s ${FIRMWARE}/app/configs/net/udhcpc.script -R
# fi

####################wifi start up(test)####################
# wpa_cli -i wlan0 add_network
# wpa_cli -i wlan0 set_network 0 ssid '"EAPIL_RD_2"'
# wpa_cli -i wlan0 set_network 0 psk '"panorama001@eapil"'
# wpa_cli -i wlan0 select_network 0