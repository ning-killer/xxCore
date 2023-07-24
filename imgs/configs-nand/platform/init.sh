#!/bin/sh
PLATFORM=/root/platform
export PATH=${PATH}:${PLATFORM}
mount -t jffs2 /dev/mtdblock5 /root/configs
${PLATFORM}/sdcard_update
if [ -f /root/configs/updating ]; then
        echo "activate plan b"
	mount -t squashfs /dev/mtdblock4 /root/firmware
else
	mount -t squashfs /dev/mtdblock3 /root/firmware
fi
firmware_init.sh
