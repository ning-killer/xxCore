#!/bin/sh

# 更新firmware
rm -rf SystemWareHouse/TX5239_T8V1R030C100B001/prebuilts/rootfs_32/root/firmware
cp -r firmware SystemWareHouse/TX5239_T8V1R030C100B001/prebuilts/rootfs_32/root/

# 编译rootfs
cd ../../../SystemWareHouse/TX5239_T8V1R030C100B001/build/
./clean.sh
./pack.sh

# 拷贝rootfs到应用工程
pwd
cd -
pwd
cp ../../../SystemWareHouse/TX5239_T8V1R030C100B001/out/rootfs.ubi imgs/rootfs.bin -v
# cp ../../../SystemWareHouse/TX5239_T8V1R030C100B001/out/fip_tx5239dv200_yitong.bin imgs/boot.bin -v
# cp ../../../SystemWareHouse/TX5239_T8V1R030C100B001/out/ts-tx5239dv200-yitong.itb imgs/kernel.bin -v