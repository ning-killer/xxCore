#!/bin/sh

AppList="EmxCoreServer EmxMediaServer EapilMain EmxFctToolsV1 EmxAgeingServer"
echo "-----------------------#MEM#-----------------------" > collection
cat /proc/meminfo >> collection
for app in $AppList
do
    echo "##-----------------------#$app#-----------------------" >> collection
    cat /proc/`ps | grep $app | grep -v grep | awk '{print $1}'`/smaps >> collection
done
