/*
* @Author: xiong
* @Date: 2022/11/30
*/

#include "NightVisionFactory.hpp"
#include "NightVisionServerNormal.hpp"
#include "NightVisionServerP42Like.hpp"

using namespace Emx;

NightVisionServerBase *NightVisionFactory::Make(const char *type) {
    if (strcmp(type, "normal") == 0) {
        return new NightVisionServerNormal;
    } else if (strcmp(type, "P42Like") == 0) {
        return new NightVisionServerP42Like;
    }
    emxlogc("unknown NightVisionServer type %s\n", type);
    return nullptr;
}
