/**
 * @file   DemoServer.cpp
 * @author hening
 * @brief  媒体服务系统启停控制单元测试
 * @date   2023-08-30
 */

#include "EmxCore.hpp"
#include "EmxMedia.hpp"
#include <iostream>
#include <fstream>

using namespace Emx;

int main(int argc, char *argv[]) {
    if (argc < 2) {
        emxloge("argument error: [DemoServer false(停止) | true(启动)]\n");
        return -1;
    }
    int flag = std::stoi(argv[1]);
    if (flag) {
        if (MediaClientServer::CreateMedia() == ErrCodeE::Success) {
            emxlogd("server CreateMedia success!\n");
        } else {
            emxloge("server CreateMedia failed\n");
        }
    } else {
        if (MediaClientServer::DestroyMedia() == ErrCodeE::Success) {
            emxlogd("server DestroyMedia success!\n");
        } else {
            emxloge("server DestroyMedia failed!\n");
        }
    }
    return 0;
}