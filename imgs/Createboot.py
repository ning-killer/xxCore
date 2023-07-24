#!/usr/bin/python3
# 获取生成nand-flash boot.bin(4*sv_cv182x_2k_pg.bin+fip.bin)
import os
flash_img_size = 2621440
with open("./boot.bin", "wb+") as flash:
    with open("./sv_cv182x_2k_pg.bin", "rb") as file:
        flash.write(file.read())
        file.close()
    with open("./sv_cv182x_2k_pg.bin", "rb") as file:
        flash.write(file.read())
        file.close()
    with open("./sv_cv182x_2k_pg.bin", "rb") as file:
        flash.write(file.read())
        file.close()
    with open("./sv_cv182x_2k_pg.bin", "rb") as file:
        flash.write(file.read())
        file.close()
    with open("./fip.bin", "rb") as fip:
        flash.write(fip.read())
    size = flash_img_size - 4*os.path.getsize('./sv_cv182x_2k_pg.bin') - os.path.getsize('./fip.bin')
    print(size)
    flash.write(bytearray([255] * size))
    flash.close()