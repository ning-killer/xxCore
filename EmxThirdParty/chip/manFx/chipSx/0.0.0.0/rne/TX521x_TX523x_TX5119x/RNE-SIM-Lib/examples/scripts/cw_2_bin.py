#!/usr/bin/env python
import sys
import struct
import os

RBuffer_CFG = 0
RBuffer_Weight = 1
DBuffer_CFG = 2
DBuffer_Weight = 3
RBuffer_MWeight = 4
RBuffer_WWeight = 5
Buffer_Type_Number = 6

def writebin(file_name, buffer_type, buffer_size, buffer):
    if buffer_type == RBuffer_CFG:
        file_name += "_r.cfg"
    elif buffer_type == RBuffer_Weight:
        file_name += "_r.weight"
    elif buffer_type == DBuffer_CFG:
        file_name += "_d.cfg"
    elif buffer_type == DBuffer_Weight:
        file_name += "_d.weight"
    elif buffer_type == RBuffer_MWeight:
        file_name += "_r.mweight"
    elif buffer_type == RBuffer_WWeight:
        file_name += "_r.wweight"
    else:
        print("[INFO]:There is a error type!")
    with open(file_name, 'wb') as f_out:
        f_out.write(buffer)

def cw2bin(argc, argv):
    cw_file = argv[1]
    file_stats = os.stat(cw_file)
    print("[INFO]:cw: File %s in Bytes is %d" %(cw_file, file_stats.st_size))
    with open(cw_file, 'rb') as f_in:
        context = f_in.read(4)
        count = struct.unpack('I',context)
        if count[0] == 0 :
            print("[ERROR]: count is zero!")
            return -1
        for i in range(count[0]):
            context = f_in.read(4)
            buffer_type = struct.unpack('i',context)
            context = f_in.read(4)
            buffer_size = struct.unpack('i',context)
            context = f_in.read(buffer_size[0])
            writebin(argv[2], buffer_type[0], buffer_size[0], context)
    return 0

def get_buffer_type(file_name):
    if "_r.cfg" in file_name:
        return RBuffer_CFG
    elif "_r.weight" in file_name:
        return RBuffer_Weight
    elif "_d.cfg" in file_name:
        return DBuffer_CFG
    elif "_d.weight" in file_name:
        return DBuffer_Weight
    elif "_r.mweight" in file_name:
        return DBuffer_MWeight
    elif "_r.wweight" in file_name:
        return RBuffer_WWeight
    else:
        return Buffer_Type_Number


def bin2cw(argc, argv):
    count = argc - 3
    buffer_count = struct.pack('I',count)
    with open(argv[2], 'wb') as f_out:
        f_out.write(buffer_count)
        for i in range(argc):
            if i <= 2:
                continue
            bin_file = argv[i]
            file_stats = os.stat(bin_file)
            print("[INFO]:bin: File %s in Bytes is %d" %(bin_file, file_stats.st_size))
            with open(bin_file, 'rb') as f_in:
                context = f_in.read(file_stats.st_size)
                f_out.write(struct.pack('I', get_buffer_type(bin_file)))
                f_out.write(struct.pack('I', file_stats.st_size))
                f_out.write(context)
    return 0

if __name__ == '__main__':

    if len(sys.argv) < 3:
        print("[INFO]:cw_2_bin.py cw_file bin_filename")
        print("[INFO]:cw_2_bin.py -r cw_file bin_list")
        sys.exit(1)

    if len(sys.argv) >= 3 and sys.argv[1] == '-r':
        if bin2cw(len(sys.argv), sys.argv) == 0:
            print("[INFO]:convert done")
    else:
        if cw2bin(len(sys.argv), sys.argv) == 0:
            print("[INFO]:convert done")

