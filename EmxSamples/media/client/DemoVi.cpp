/**
 * @file   DemoVi.cpp
 * @author hening
 * @brief  媒体服务Vi单元测试
 * @date   2023-08-30
 */

#include "EmxCore.hpp"
#include "EmxMedia.hpp"
#include <iostream>
#include <fstream>

using namespace Emx;

int main(int argc, char *argv[]) {
    int num;
    if (MediaClientVi::GetChnNum(num) != ErrCodeE::Success
        || num < 1) {
        emxloge("vi GetChnNum failed, num(%d).\n", num);
        return -1;
    }
    
    // 绑定主码流验证
    MediaClientVi viClient(0);
    MediaVi::Param viParam;
    if (viClient.GetParam(viParam) != ErrCodeE::Success) {
        emxloge("vi GetParam is failed\n");
        return -1;
    }

    emxlogd("vi param: flip[%d];mirror[%d]\n", viParam.flip, viParam.mirror);
    
    // 镜像翻转切换
    if (viParam.flip) {
        viParam.flip = false;
    } else {
        viParam.flip = true;
    }
    if (viParam.mirror) {
        viParam.mirror = false;
    } else {
        viParam.mirror = true;
    }

    emxlogd("aenc switch flip[%d];mirror[%d]\n", viParam.flip, viParam.mirror);
    if (viClient.SetParam(viParam) != ErrCodeE::Success) {
        emxloge("vi GetParam failed.\n");
        return -1;
    }
    return 0;
}