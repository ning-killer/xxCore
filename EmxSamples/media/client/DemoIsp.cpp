/**
 * @file   DemoIsp.cpp
 * @author hening
 * @brief  媒体服务图像Isp单元测试
 * @date   2023-08-30
 */

#include "EmxCore.hpp"
#include "EmxMedia.hpp"
#include <iostream>
#include <fstream>

using namespace Emx;

void RunMediaClientISP() {
    int num;
    if (MediaClientISP::GetChnNum(num) != ErrCodeE::Success
        || num < 1) {
        emxloge("isp GetChnNum failed, num(%d).\n", num);
        return;
    }
    MediaClientISP ispClient(0);
    MediaISP::Param param;
    if (ispClient.GetParam(param) != ErrCodeE::Success) {
        emxloge("isp GetParam failed.\n");
        return;
    }
    emxlogd("Isp expMode[%d], autoMaxExpGain[%d], wbMode[%d], manualExpGain[%d].\n"
        , (int)param.expMode, (int)param.autoMaxExpGain
        , (int)param.wbMode, (int)param.manualExpGain);
    
    // 验证更改参数
    // if (param.autoMaxExpGain != MediaISP::ExpGainE::X16) {
    //     param.autoMaxExpGain = MediaISP::ExpGainE::X16;
    // } else {
    //     param.autoMaxExpGain = MediaISP::ExpGainE::X1;
    // }
    
    if (ispClient.SetParam(param) != ErrCodeE::Success) {
        emxloge("isp SetParam failed.\n");
        return;
    }
    if (ispClient.GetParam(param) != ErrCodeE::Success) {
        emxloge("isp GetParam failed.\n");
        return;
    }
    emxlogd("Isp expMode[%d], autoMaxExpGain[%d], wbMode[%d], manualExpGain[%d].\n"
        , (int)param.expMode, (int)param.autoMaxExpGain
        , (int)param.wbMode, (int)param.manualExpGain);
    
    MediaISP::RunModeE runMode;
    if (ispClient.GetRunMode(runMode) != ErrCodeE::Success) {
        emxloge("isp GetRunMode failed.\n");
        return;
    }
    emxlogd("Isp current runMode[%d]\n", runMode);
    if (runMode == MediaISP::RunModeE::Day) {
        //切换夜晚模式，rtsp看图像效果
        runMode = MediaISP::RunModeE::NightWithLight;
    } else {
        //切换白天模式，rtsp看图像效果
        runMode = MediaISP::RunModeE::Day;
    }

    emxlogd("Isp switch runMode[%d]\n", runMode);
    if (ispClient.SetRunMode(runMode) != ErrCodeE::Success) {
        emxloge("SetRunMode failed.\n");
        return;
    }
}

void RunMediaClientISPInfo() {
    MediaClientISPInfo isp;
    EuvLoop loop;
    char buffer[1024];
    loop.Init("DemoIsp", []() { });
    isp.Start(loop, buffer, sizeof(buffer), [](MediaISP::Info &info) {
        // emxlogd("IspInfo: iso[%d];redGain[%d];blueGain[%d].\n", info.iso, info.redGain, info.blueGain);
        emxlogd("IspInfo: iso[%d];r[%d];b[%d];b+r[%d];b-r[%d];luma[%d]\n"
                        , info.iso
                        , info.redGain
                        , info.blueGain
                        , info.blueGain + info.redGain
                        , info.blueGain - info.redGain
                        , info.luma);
    });
    emxlogd("DemoIsp loop start.\n");
    loop.Start(false);
}

int main(int argc, char *argv[]) {
    // RunMediaClientISP();
    RunMediaClientISPInfo();
    return 0;
}