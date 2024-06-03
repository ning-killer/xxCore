#!/bin/bash

function build_all()
{
	if [ ${TARGET_CHIP} == "tx536" ]; then
		cp tx5368/moudle/chip_driver.ko ${TARGET_OUT_DIR}/rootfs/usr/lib/modules/ -fp
    elif [ ${TARGET_CHIP} == "tx5119dv200" ]; then
        cp TX521x_TX523x_TX5119x/moudle/rne_driver.ko ${TARGET_OUT_DIR}/rootfs/usr/lib/modules/ -fp
	elif [ ${TARGET_CHIP} == "tx5112cv201" ]; then
		echo "copy ${TARGET_CHIP} rne_driver.ko success"
        cp TX5112x201_TX5239x201-Lib-uclibc/module/rne_driver.ko ${TARGET_OUT_DIR}/rootfs/usr/lib/modules/ -fp
	else
		echo "copy TX521x_TX523x_TX5119x rne_driver.ko success"
		cp TX521x_TX523x_TX5119x/moudle/rne_driver.ko ${TARGET_OUT_DIR}/rootfs/usr/lib/modules/ -fp
	fi
	echo "chip type is ${TARGET_CHIP}"
}

function clean()
{
	echo "do nothing"
}

if [ $1 == "all" ] ; then
        build_all
else
        clean
fi
