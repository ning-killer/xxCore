/*
* @Author: xiong
* @Date: 2022/11/30
*/

#ifndef EMX_NightVisionFactory_HPP
#define EMX_NightVisionFactory_HPP

#include "NightVisionServerBase.hpp"

namespace Emx {

    class NightVisionFactory {
    public:
        static NightVisionServerBase *Make(const char *type);
    };

}

#endif //EMX_NightVisionFactory_HPP
