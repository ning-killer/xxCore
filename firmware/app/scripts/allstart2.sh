#!/bin/sh
EmxCoreServer -b
sleep 1
EmxMediaServer -b
sleep 1
EmxModulesServer -b
sleep 1
EapilMain -b
sleep 1
EmxAgeingServer -b
sleep 1
EmxFctToolsV1 -b
