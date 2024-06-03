//
// Created by xiong on 2022/7/5.
//

#include <cstring>
#include "LedFactory.hpp"
#include "LedSingle.hpp"
#include "LedDouble.hpp"
#include "LedRgb.hpp"

using namespace Emx;

Led *LedFactory::Make(EuvLoop &loop, Bsp::Led *led, const char *type) {
    if (strcmp(type, "single") == 0)
        return new LedSingle(loop, led);
    else if (strcmp(type, "double") == 0)
        return new LedDouble(loop, led);
    else if (strcmp(type, "rgb") == 0)
        return new LedRgb(loop, led);
    else
        return nullptr;
}
