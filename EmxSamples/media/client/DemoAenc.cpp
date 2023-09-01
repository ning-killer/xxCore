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
    
    // 编码切换
    if (param.codec ==  AudioCodecE::G711A) {
        param.codec = AudioCodecE::AAC;
    } else {
        param.codec = AudioCodecE::G711A;
    }

    // 静音
    // param.mute = true;

    emxlogd("aenc switch codec[%d].\n", param.codec);
    if (aencClient.SetParam(param) != ErrCodeE::Success) {
        emxloge("aenc GetParam failed.\n");
        return -1;
    }
    return 0;
}