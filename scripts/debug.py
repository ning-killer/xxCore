#!/usr/bin/python3
import json
import os
import sys
import utils

# ./release [outer version]

topdir = os.getcwd() + "/.."

# 更新devInfo.json内部版本号和外部版本号
dev_info_file = topdir + "/firmware/app/configs/devInfo.json"
out_version = ''
if len(sys.argv) == 2:
    out_version = sys.argv[1]
utils.DevInfo.update_version(dev_info_file, out_version)
dev_info = utils.DevInfo.get(dev_info_file)

# build and write to firmware
os.environ["LANG"] = "C"
firmware_dir = topdir + "/firmware"
app_dir = firmware_dir + "/app"
media_dir = firmware_dir + "/media"
tmp = topdir + "/tmp_build"
os.system("mkdir -p " + tmp)
os.chdir(tmp)
utils.cmd_check("rm -rf *")
utils.cmd_check("cmake .. -DINSTALL_APP_DIR={} -DINSTALL_MEDIA_DIR={}".format(app_dir, media_dir))
utils.cmd_check("make -j6")
utils.cmd_check("make install")
utils.os.chdir(topdir)
os.system("rm -rf " + tmp)