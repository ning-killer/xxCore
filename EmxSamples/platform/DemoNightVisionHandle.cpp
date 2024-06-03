#include "EmxCore.hpp"
#include "NightVision.hpp"
#include "NightVisionClient.hpp"

using namespace Emx;

int main(int argc, char *argv[]) {
    NightVisionClient nightVision;
    NightVision::Param param = {};
    param.manual = true;
    while (true) {
        param.manualMode = NightVision::ModeE::IrNightVision;
        nightVision.SetParamTemp(param);
        sleep(3);
        param.manualMode = NightVision::ModeE::Day;
        nightVision.SetParamTemp(param);
        sleep(3);
    }
    return 0;
}