#!/usr/bin/python3
# pip3 install pycryptodome
from Crypto.Cipher import AES
import os
import datetime
import json
import hashlib


def print_err(string):
    print("\033[1;31m" + string + "\033[0m")


def print_info(string):
    print("\033[1;35m" + string + "\033[0m")


class ParamKey:
    """用于给参数加解密的密钥"""
    key = "Eapil432Param786"
    iv = "Tea98781Key42658"


class RomKey:
    """用于给rom或flash加解密的密钥"""
    key = "Eapil27RomKey324"
    iv = "Tea551616Key1451"


def cmd_check(cmd_str):
    ret = os.system(cmd_str)
    if ret >> 8 != 0:
        print_err("error")
        exit()


class Aes:
    """用于AES加解密"""

    def __init__(self, key, iv):
        key_bytes = bytes(key, encoding='utf-8')
        iv_bytes = bytes(iv, encoding='utf-8')
        self.aes = AES.new(key_bytes, AES.MODE_CBC, iv_bytes)

    def decode_file(self, src, dst):
        """
        解密一个文件
        :param src: 待解密文件路径
        :param dst: 解密后文件存储路径
        :return: none
        """
        with open(src, "rb") as f:
            data = f.read()
            f.close()
            data = self.decode(data)
            with open(dst, "wb+") as f:
                f.write(data)
                f.close()

    def encode_file(self, src, dst):
        """
        加密一个文件
        :param src: 待加密文件路径
        :param dst: 加密后文件存储路径
        :return: none
        """
        with open(src, "rb") as f:
            data = f.read()
            f.close()
            data = self.encode(data)
            with open(dst, "wb+") as f:
                f.write(data)
                f.close()

    def encode(self, content):
        """
        加密bytes
        :param content: 待加密bytes
        :return: 加密后bytes
        """
        # 处理明文
        content_padding = self.pkcs7_padding(content)
        # 加密
        encode_bytes = self.aes.encrypt(bytes(content_padding))
        return encode_bytes

    def decode(self, content):
        """
        解密bytes
        :param content: 待解密bytes
        :return: 解密后bytes
        """
        # 解密
        aes_decode_bytes = self.aes.decrypt(content)
        # 去除填充内容
        result = self.pkcs7_unpadding(aes_decode_bytes)
        return result

    @staticmethod
    def pkcs7_padding(data):
        bs = AES.block_size  # 16
        length = len(data)
        padding = bs - length % bs
        return data + bytes([padding] * padding)

    @staticmethod
    def pkcs7_unpadding(data):
        length = len(data)
        unpadding = data[length - 1]
        return data[0:length - unpadding]


class AesDir:
    """将src目录下所有文件和目录加密/解密存储到dst目录下"""

    def __init__(self, key, iv):
        self.key = key
        self.iv = iv

    def encode(self, src, dst):
        self._codec(src, dst, True)

    def decode(self, src, dst):
        self._codec(src, dst, False)

    def _codec(self, src, dst, codec_flag):
        if src != dst:
            os.system("rm -r " + dst)
        self._proc_dir(src, dst, codec_flag)

    def _proc_dir(self, src_dir, dst_dir, codec_flag):
        # print("src={}".format(src_dir))
        # print("dst={}".format(dst_dir))
        # 如果目标目录不存在就创建一个
        os.system("mkdir -p {}".format(dst_dir))
        for e in os.listdir(src_dir):
            src_path = os.path.join(src_dir, e)
            dst_path = os.path.join(dst_dir, e)
            if os.path.isdir(src_path):
                # 如果是目录就递归扫描
                self._proc_dir(src_path, dst_path, codec_flag)
            else:
                # print("src={}".format(src_path))
                # print("dst={}".format(dst_path))
                # print("\n")
                # 如果是文件就加密/解密处理
                aes = Aes(self.key, self.iv)
                if codec_flag:
                    aes.encode_file(src_path, dst_path)
                else:
                    aes.decode_file(src_path, dst_path)


class DevInfo:
    """对于devInfo.json的一些操作"""

    @staticmethod
    def update_version(path, outer_version):
        """
        更新devInfo.json中的内部版本号
        :param path: devInfo.json文件路径
        :param outer_version: 需要修改的外部版本号，为空则不修改
        :return: 更新后的内部版本号
        """
        with open(path, 'r') as f:
            dev_info = json.load(f)
            f.close()
        main_ver, sub_ver, old_date, build_ver = dev_info["inner"]["version"].split(".")
        date = datetime.datetime.now().strftime('%Y%m%d')
        if old_date != date:
            build_ver = "0"
        inner_version = main_ver + '.' + sub_ver + '.' + date + '.' + str(int(build_ver) + 1)
        dev_info["inner"]["version"] = inner_version
        if len(outer_version) > 0:
            dev_info["outer"]["version"] = outer_version

        with open(path, 'w+') as f:
            json.dump(dev_info, f, indent=2, ensure_ascii=False)
            f.close()
        return inner_version

    @staticmethod
    def get(path):
        """
        获取内部版本号
        :param path: devInfo.json文件路径
        :return: 内部版本号
        """
        with open(path, 'r') as f:
            dev_info = json.load(f)
            f.close()
        return dev_info


class RomInfoJson:
    def __init__(self, inner_type, inner_version):
        self.info = json.loads("{}")
        self.info["type"] = inner_type
        self.info["version"] = inner_version
        self.img = json.loads("[]")
        self.script = json.loads("{}")

    def add_pre_script(self, size):
        self.script["preSize"] = size

    def add_post_script(self, size):
        self.script["postSize"] = size

    def add_image(self, size, name):
        img = json.loads("{}")
        img["size"] = size
        img["name"] = name
        self.img.append(img)

    def final(self):
        self.info["img"] = self.img
        if "preSize" in self.script or "postSize" in self.script:
            self.info["script"] = self.script
        return self.info


def get_md5_file(file):
    m = hashlib.md5()
    with open(file, 'rb') as f:
        content = f.read()
        m.update(content)
        return m.hexdigest()


def get_md5_content(content):
    m = hashlib.md5()
    m.update(content)
    return m.hexdigest()


# transform string to int such as (64K)->(65536)
def img_size_str2int(string):
    size = 0
    if string[-1] == 'K':
        size = int(string[0:-1]) * 1024
    elif string[-1] == 'M':
        size = int(string[0:-1]) * 1024 * 1024
    elif string[-1] == 'G':
        size = int(string[0:-1]) * 1024 * 1024 * 1024
    elif string == '-':
        size = 0
    else:
        size = int(string)
    return size


# img_info
# {
#     "boot": {
#         "partSize": 393216,
#         "file": "/the/path/to/fip.bin"
#     },
#     "env": {
#         "partSize": 131072
#     },
#     "kernel": {
#         "partSize": 2490368,
#         "file": "/the/path/to/kernel.bin"
#     },
#     "rootfs": {
#         "partSize": 1048576,
#         "file": "/the/path/to/rootfs.bin"
#     },
#     "media": {
#         "partSize": 5963776,
#         "file": "/the/path/to/media.bin"
#     },
#     "app": {
#         "partSize": 3145728,
#         "file": "/the/path/to/app.bin"
#     },
#     "data": {
#         "partSize": 458752
#     },
# }
def pack_prepare(imgs_dir):
    # 根据partition.json构建img_info，用于后续打包
    img_info = dict()
    with open(imgs_dir + "/partition.json", 'r') as f:
        partition = json.load(f)["partitionTable"]
        for part in partition:
            name = part["name"]
            # 过滤掉双系统分区中的另一个
            if '.' in name:
                name, __ = str(name).split('.')
                if name in img_info:
                    continue
            one_img = dict()
            one_img["partSize"] = img_size_str2int(part["size"])
            if "file" in part:
                one_img["file"] = imgs_dir + "/" + part["file"]
                if os.path.getsize(one_img["file"]) > one_img["partSize"]:
                    print_err("{} img size {} > part size {}".format(name, os.path.getsize(one_img["file"]),
                                                                     one_img["partSize"]))
                    exit(-1)
            img_info[name] = one_img
    return img_info


# def pack_prepare(topdir):
#     # 建立一个目录用于存放打包过程中的临时文件
#     pack_tmp_path = topdir + "/output/pack_tmp"
#     os.system("rm -rf " + pack_tmp_path)
#     os.system("mkdir -p " + pack_tmp_path)
#     # 明文img目录
#     pack_plain_path = pack_tmp_path + "/plain"
#     os.system("mkdir -p " + pack_plain_path)
#     # 加密img目录
#     pack_encrypt_path = pack_tmp_path + "/encrypt"
#     os.system("mkdir -p " + pack_encrypt_path)
#
#     imgs_dir = topdir + "/imgs"
#
#     # img_info中存放打包过程需要的所有关于img的信息
#     img_info = dict()
#     with open(imgs_dir + "/partition.json", 'r') as f:
#         partition = json.load(f)["partitionTable"]
#         for part in partition:
#             name = part["name"]
#             # 过滤掉双系统分区中的另一个
#             if '.' in name:
#                 name, __ = str(name).split('.')
#                 if name in img_info:
#                     continue
#             one_img = dict()
#             p = dict()
#             p["size"] = img_size_str2int(part["size"])
#             one_img["part"] = p
#             if "file" in part:
#                 file = dict()
#                 # plain img
#                 plain = dict()
#                 plain["path"] = pack_plain_path + "/" + part["file"]
#                 # copy img file from imgs to plain path
#                 os.system("cp {} {}".format(imgs_dir + "/" + part["file"], pack_plain_path))
#                 plain["size"] = os.path.getsize(plain["path"])
#                 if plain["size"] > p["size"]:
#                     print("{} plain size {} > partition size {}\n", name, plain["size"], p["size"])
#                 plain["md5"] = get_md5_file(plain["path"])
#                 file["plain"] = plain
#                 # encrypt img
#                 encrypt = dict()
#                 encrypt["path"] = pack_encrypt_path + "/" + part["file"]
#                 # encrypt img file from imgs to encrypt path
#                 aes = Aes(RomKey.key, RomKey.iv)
#                 aes.encode_file(imgs_dir + "/" + part["file"], encrypt["path"])
#                 encrypt["size"] = os.path.getsize(encrypt["path"])
#                 if encrypt["size"] > p["size"]:
#                     print("{} encrypt size {} > partition size {}\n", name, encrypt["size"], p["size"])
#                 encrypt["md5"] = get_md5_file(encrypt["path"])
#                 file["encrypt"] = encrypt
#                 one_img["file"] = file
#             img_info[name] = one_img
#     return img_info


def pack_raw_flash_img(topdir, img_info, out):
    # 拷贝分区信息到临时目录
    imgs_dir = topdir + "/imgs"
    with open(imgs_dir + "/partition.json", 'r') as partition_file:
        partition = json.load(partition_file)
        partition_file.close()
        flash_img_size = img_size_str2int(partition["flashImgSize"])
        writen_size = 0
        with open(out, "wb+") as flash:
            for part in partition["partitionTable"]:
                name = part["name"]
                # 双系统分区
                if '.' in name:
                    name, __ = str(name).split('.')
                one_img = img_info[name]
                part_size = one_img["partSize"]
                if "file" not in one_img:
                    flash.write(bytearray([255] * part_size))
                else:
                    with open(one_img["file"], "rb") as file:
                        flash.write(file.read())
                        flash.write(bytearray([255] * (part_size - os.path.getsize(one_img["file"]))))
                writen_size += part_size
            if writen_size > flash_img_size:
                print_err("flash_img_size:[{}] < writen_size:[{}]".format(flash_img_size, writen_size))
                exit(-1)
            flash.write(bytearray([255] * (flash_img_size - writen_size)))
            flash.close()


# rom file format
# total size是整个文件的长度，包含自身的4字节
# 其中第一个md5是文件整体加密后的md5
# rom header size是rom json header的大小，需要控制在64KB以下
# rom json header的内容见后面描述
# rom json header之后的内容是全部打包的明文固件数据
#  ________________________________________________
# |4Byte              rom size                     |
# |32Byte             md5 of all(encrypted) below  |
# |4Byte              rom header size(<64KB)       |
# |header sizeByte    rom json header              |
# |xByte              pre_script.sh                |
# |xByte              pre_script.sh                |
# |xByte              img0                         |
# |xByte              img1                         |
# |xByte              ....                         |
# |________________________________________________|

# rom json header
# script and pre/post are optional
# {
#     "type": "ZN_HY_M4C7S3W0P0",
#     "version": "2.0.20220826.2",
#     "script": {
#         "preSize": 0,
#         "postSize": 0
#     },
#     "img": [
#         {"name": "boot", "size": 0},
#         {"name": "kernel", "size": 0},
#         {"name": "firmware", "size": 0}
#     ]
# }
def pack_rom(dev_info, out, img_info, img_list, pre_script_file=None, post_script_file=None):
    write_file_list = []
    rom_info = RomInfoJson(dev_info["inner"]["type"], dev_info["inner"]["version"])

    # 如果存在前处理脚本，则添加到rom json header中
    if pre_script_file is not None:
        rom_info.add_pre_script(os.path.getsize(pre_script_file))
        write_file_list.append(pre_script_file)

    # 如果存在后处理脚本，则添加到rom json header中
    if post_script_file is not None:
        rom_info.add_post_script(os.path.getsize(post_script_file))
        write_file_list.append(post_script_file)

    # 将img添加到rom json header中
    for name in img_list:
        rom_info.add_image(os.path.getsize(img_info[name]["file"]), name)
        write_file_list.append(img_info[name]["file"])

    rom_info_json_string = json.dumps(rom_info.final())
    rom_info_json_size = len(rom_info_json_string)
    if rom_info_json_size > 64 * 1024:
        print_err("rom info size {} > 64KB\n".format(rom_info_json_size))
        exit(-1)

    rom = bytearray()
    rom.extend(rom_info_json_size.to_bytes(4, byteorder='big', signed=False))
    rom.extend(rom_info_json_string.encode(encoding="utf-8"))
    for file in write_file_list:
        with open(file, "rb") as f:
            rom.extend(f.read())
            f.close()
    aes = Aes(RomKey.key, RomKey.iv)
    rom_encrypted = aes.encode(rom)
    with open(out, 'wb+')as out_file:
        out_file.write(len(rom_encrypted).to_bytes(4, byteorder='big', signed=False))
        out_file.write(bytes(get_md5_content(rom_encrypted), encoding='utf-8'))
        out_file.write(rom_encrypted)
        out_file.close()


# def pack_rom(dev_info, out, img_info, img_list, pre_script_file=None, post_script_file=None):
#     write_file_list = []
#     rom_info = RomInfoJson(dev_info["inner"]["type"], dev_info["inner"]["version"])
#     if pre_script_file is not None:
#         dst_file = "/tmp/pre_script_file"
#         aes = Aes(RomKey.key, RomKey.iv)
#         aes.encode_file(pre_script_file, dst_file)
#         rom_info.add_pre_script(os.path.getsize(dst_file), get_md5_file(dst_file))
#         write_file_list.append(dst_file)
#
#     if post_script_file is not None:
#         dst_file = "/tmp/post_script_file"
#         aes = Aes(RomKey.key, RomKey.iv)
#         aes.encode_file(post_script_file, dst_file)
#         rom_info.add_post_script(os.path.getsize(dst_file), get_md5_file(dst_file))
#         write_file_list.append(dst_file)
#
#     for img in img_list:
#         rom_info.add_image(img_info[img]["file"]["encrypt"]["size"],
#                            img,
#                            img_info[img]["file"]["encrypt"]["md5"])
#         write_file_list.append(img_info[img]["file"]["encrypt"]["path"])
#
#     rom_info_json_string = json.dumps(rom_info.final())
#     # print(rom_info_json_string)
#     aes = Aes(RomKey.key, RomKey.iv)
#     rom_info_json_encrypted = aes.encode(bytes(rom_info_json_string, encoding='utf-8'))
#     rom_info_json_size = len(rom_info_json_encrypted)
#     if rom_info_json_size > 64 * 1024:
#         print("rom info size {} > 64KB\n".format(rom_info_json_size))
#         exit(-1)
#     # rom_info_json_encrypted_md5 = hashlib.md5()
#     # with open(file, 'rb') as f:
#     #     content = f.read()
#     #     m.update(content)
#     #     return m.hexdigest()
#
#     # print("total md5:{}".format(total_md5_string))
#     # print("total size = {}".format(str(hex(total_size))))
#     # print("total size = {}".format(total_size))
#     with open(out, 'wb+')as out_file:
#         # rom json header的md5
#         rom_info_json_encrypted_md5 = hashlib.md5()
#         rom_info_json_encrypted_md5.update(rom_info_json_encrypted)
#         out_file.write(bytes(rom_info_json_encrypted_md5.hexdigest(), encoding='utf-8'))
#         # 网络字节序
#         out_file.write(rom_info_json_size.to_bytes(4, byteorder='big', signed=False))
#         out_file.write(rom_info_json_encrypted)
#         for file in write_file_list:
#             with open(file, "rb") as f:
#                 out_file.write(f.read())
#                 f.close()
#         out_file.close()


def parse_rom_info(file):
    with open(file, "rb") as f:
        total_size = int.from_bytes(f.read(4), byteorder='big', signed=False)
        total_md5 = f.read(32)
        aes = Aes(RomKey.key, RomKey.iv)
        rom = aes.decode(f.read())
        rom_info_json_size = int.from_bytes(rom[0:4], byteorder='big', signed=False)
        print_info("rom info json size = {}".format(rom_info_json_size))
        rom_info_json = json.loads(rom[4:rom_info_json_size + 4])
        print_info("rom_info_json = {}".format(
            json.dumps(rom_info_json, indent=2, ensure_ascii=False)))
