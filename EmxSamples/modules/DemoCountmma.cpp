/**
 * @file   DemoCountmma.cpp
 * @author hening
 * @brief  区域业务测试
 * @date   2023-10-17 统计ss mma资源
 */

#include "EmxCore.hpp"
#include <sys/statvfs.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <cstdlib>

using namespace Emx;

struct MMAInfo {
    int iToalMemory;//芯片总内存大小，单位KB
    int iFreeMemory;//芯片剩余内存，单位KB
    int iUsedMemory;//芯片使用内存，单位KB
};

//sigmatar:
// cat /proc/mi_modules/mi_sys_mma/mma_heap_name0 
// heap_info:
//         heap_name     pa_start       length         avail
//    mma_heap_name0    40000000     1f000000      1b238000
bool GetChipMMAInfoV1(MMAInfo &info) {
    bool ret = false;
    std::ifstream file("/proc/mi_modules/mi_sys_mma/mma_heap_name0");
    if (!file.is_open()) {
        return ret;
    }
    std::string line;
    while (std::getline(file, line)) {
        if (line.find("heap_name") != std::string::npos) {
            std::istringstream iss(line);
            std::string heap_name;
            unsigned long long pa_start, length, avail;
            iss >> heap_name >> std::hex >> pa_start >> std::hex >> length >> std::hex >> avail;
            info.iToalMemory = length / (1024 * 1024);
            info.iFreeMemory = avail / (1024 * 1024);
            info.iUsedMemory = info.iToalMemory  - info.iFreeMemory;
            ret = true;
        }
    }
    file.close();
    return ret;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        emxloge("argument error: [统计间隔]\n");
        return -1;
    }
    int time = std::stoi(argv[1]);
    MMAInfo info = { 0 };
    while(true) {
        GetChipMMAInfoV1(info);
        emxlogd("mma total[%d], used[%d], free[%d]\n",info.iToalMemory, info.iUsedMemory, info.iFreeMemory);
        usleep(time * 100 * 1000);
    }
    return 0;
}