/**
 * @file   DemoVenc.cpp
 * @author hening
 * @brief  媒体服务video编码单元测试
 * @date   2023-08-30
 */

#include "EmxCore.hpp"
#include "EmxMedia.hpp"
#include <iostream>
#include <fstream>

using namespace Emx;

int main(int argc, char *argv[]) {
    int num;
    if (MediaClientVenc::GetChnNum(num) != ErrCodeE::Success
        || num < 1) {
        emxloge("venc GetChnNum failed, num(%d).\n", num);
        return -1;
    }

    // 主码流
    MediaClientVenc vencClient(0);
    MediaVenc::Param param;
    if (vencClient.GetParam(param) != ErrCodeE::Success) {
        emxloge("venc GetParam failed.\n");
        return -1;
    }
    emxlogd("venc param: codec[%d];width[%d];height[%d];fps[%d];bitRate[%d]\n"
            , param.codec, param.width, param.height, param.fps, param.bitRate);
    
    // 视频编码切换
    if (param.codec == VideoCodecE::H264) {
        param.codec = VideoCodecE::H265;
    } else {
        param.codec = VideoCodecE::H264;
    }

    // 分辨率切换
    if (param.width == 1920 && param.height == 1080) {
        param.width = 2560;
        param.height = 1440;
    } else {
        param.width = 1920;
        param.height = 1080;
    }
    param.bitRate = 285;
    
    emxlogd("changed width[%d];height[%d]\n", param.width, param.height);
    if (vencClient.SetParam(param) != ErrCodeE::Success) {
        emxloge("venc SetParam failed.\n");
        return -1;
    }
    return 0;
}