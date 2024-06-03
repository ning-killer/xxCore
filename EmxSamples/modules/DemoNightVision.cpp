/**
 * @file   DemoNightVision.cpp
 * @author hening
 * @brief  区域业务测试
 * @date   2023-10-09 控制isp环境模式切换
 */

#include "EmxCore.hpp"
#include "NightVision.hpp"
#include "NightVisionClient.hpp"

using namespace Emx;

int main(int argc, char *argv[]) {
    if (argc < 2) {
        emxloge("argument error: [ai 0(白天) | 1(夜晚) | 2(全彩) | 3(自动全彩) | 4(自动夜晚)]\n");
        return -1;
    }
    NightVisionClient nightVision;
    NightVision::Param param = {};
    int status = std::stoi(argv[1]);
    if (status == 0) {  
        param.manual = true;
        param.manualMode = NightVision::ModeE::Day;
    } else if (status == 1) {
        param.manual = true;
        param.manualMode = NightVision::ModeE::IrNightVision;
    } else if (status == 2) {
        param.manual = true;   
        param.manualMode = NightVision::ModeE::ColorNightVision;
    } else if (status == 3) {
        param.manual = false;
        param.autoMode = NightVision::AutoModeE::ColorNightVision;
    } else if (status == 4) {
        param.manual = false;
        param.autoMode = NightVision::AutoModeE::IrNightVision;
    }
    nightVision.SetParamTemp(param);
    return 0;
}