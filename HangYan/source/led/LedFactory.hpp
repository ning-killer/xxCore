//
// Created by xiong on 2022/7/5.
//

#ifndef EAPIL_LedFactory_HPP
#define EAPIL_LedFactory_HPP

#include "Led.hpp"

namespace Emx {

    class LedFactory {
    public:
        static Led *Make(EuvLoop &loop, Bsp::Led *led, const char *type);
    };

}

#endif //EAPIL_LedFactory_HPP
