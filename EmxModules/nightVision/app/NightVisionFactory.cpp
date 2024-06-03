/*
* @Author: xiong
* @Date: 2022/11/30
*/

#include "NightVisionFactory.hpp"
#include "NightVisionServerNormal.hpp"
#include "NightVisionServerP42Like.hpp"
#include "NightVisionServerMMC233Z.hpp"
#include "NightVisionServerQ600J.hpp"

using namespace Emx;

NightVisionServerBase *NightVisionFactory::Make(const char *type) {
    if (strcmp(type, "normal") == 0) {
        return new NightVisionServerNormal;
    } else if (strcmp(type, "P42Like") == 0) {
        return new NightVisionServerP42Like;
    } else if (strcmp(type, "MMC233Z") == 0) {
        return new NightVisionServerMMC233Z;
    } else if (strcmp(type, "Q600J") == 0) {
        return new NightVisionServerQ600J;
    }
    emxlogc("unknown NightVisionServer type %s\n", type);
    return nullptr;
}
