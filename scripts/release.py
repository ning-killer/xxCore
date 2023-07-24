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
os.system("rm -f firmware/app/lib/*.a")
# encrypt plain param
encrypt_plain_param = utils.AesDir(utils.ParamKey.key, utils.ParamKey.iv)
encrypt_plain_param.encode(topdir + "/plainParam", app_dir + "/param")

imgs_dir = topdir + "/imgs"
# pack firmware.bin
firmware_bin = imgs_dir + "/firmware.bin"
os.system("rm " + firmware_bin)
utils.cmd_check("mksquashfs {} {} -b 64K -comp xz >> /dev/null".format(firmware_dir, firmware_bin))

# output dir
new_version = dev_info["outer"]["type"]
new_version += "_" + dev_info["inner"]["version"]
new_version += "_" + dev_info["outer"]["version"]
output_dir = topdir + "/output/" + new_version
os.system("mkdir -p " + output_dir)

# prepare to pack
img_info = utils.pack_prepare(imgs_dir)

# 打包用于烧片的裸flash文件
utils.pack_raw_flash_img(topdir, img_info, output_dir + "/" + new_version + "_RawFlash.bin")

# 打包用于常规升级的rom文件
dst_rom_file = output_dir + "/" + new_version + "_Rom.bin"

utils.pack_rom(dev_info,
               dst_rom_file,
               img_info,
               ["kernel", "rootfs", "firmware"],
               imgs_dir + "/pre_script.sh",
               imgs_dir + "/post_script.sh")

# 打印出pack后的文件信息
utils.parse_rom_info(dst_rom_file)
print("\033[1;35m" + "new version = " + new_version + "\033[0m")
