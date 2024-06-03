#!/usr/bin/python3
import json
import os
import sys
import utils

# paramCodec.py file/dir
# paramCodec.py可以对源文件进行加/解密操作
# 加解密后的文件会回写到相同的文件里
# 原始文件可以是单个文件，也可以是一个目录，如果是目录则递归遍历整个目录
# 进行加/解密操作


if len(sys.argv) != 3:
    print("usage: paramCodec enc/dec path/to/the/file/or/dir")
    exit(-1)

cmd = sys.argv[1]
path = sys.argv[2]

if cmd == "enc":
    if os.path.isdir(path):
        aes = utils.AesDir(utils.ParamKey.key, utils.ParamKey.iv)
        aes.encode(path, path)
    else:
        aes = utils.Aes(utils.ParamKey.key, utils.ParamKey.iv)
        aes.encode_file(path, path)
elif cmd == "dec":
    if os.path.isdir(path):
        aes = utils.AesDir(utils.ParamKey.key, utils.ParamKey.iv)
        aes.decode(path, path)
    else:
        aes = utils.Aes(utils.ParamKey.key, utils.ParamKey.iv)
        aes.decode_file(path, path)
else:
    print("usage: paramCodec enc/dec path/to/the/file/or/dir")
