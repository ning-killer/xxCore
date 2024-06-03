#include "EmxBsp.hpp"

using namespace Emx;

int main(int argc, char *argv[]) {
    if (argc < 3) {
        emxloge("argument error: [chn] [enable]\n");
        return -1;
    }
    int chn = std::stoi(argv[1]);
    bool enable = std::stoi(argv[2]) == 0 ? false : true;
    Bsp bsp;
    emxlogd("ircut control chn[%d]; enable[%d]\n", chn, enable);
    bsp.irCut->SwitchToNight(enable, chn);
    return 0;
}