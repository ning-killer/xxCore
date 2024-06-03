/**
 * @file   DemoAenc.cpp
 * @author hening
 * @brief  媒体服务audio编码单元测试
 * @date   2023-08-30
 */

#include "EmxCore.hpp"
#include "EmxMedia.hpp"
#include <iostream>
#include <fstream>

using namespace Emx;

int main(int argc, char *argv[]) {
    if (argc < 2) {
        emxloge("argument error: [volume]\n");
        return -1;
    }
    int volume = std::stoi(argv[1]);

    int num;
    if (MediaClientAenc::GetChnNum(num) != ErrCodeE::Success
        || num < 1) {
        emxloge("aenc GetChnNum failed, num(%d).\n", num);
        return -1;
    }

    MediaClientAenc aencClient(0);
    MediaAenc::Param param;
    if (aencClient.GetParam(param) != ErrCodeE::Success) {
        emxloge("aenc GetParam failed.\n");
        return -1;
    }
    emxlogd("aenc param: codec[%d];bitRate[%d];sampleRate[%d];volume[%d];bitWidth[%d];mute[%d]\n"
            , param.codec, param.bitRate, param.sampleRate
            , param.volume, param.bitWidth, param.mute);

    // 静音
    param.volume = volume;

    emxlogd("aenc switch volume[%d].\n", param.volume);
    if (aencClient.SetParam(param) != ErrCodeE::Success) {
        emxloge("aenc GetParam failed.\n");
        return -1;
    }
    return 0;
}