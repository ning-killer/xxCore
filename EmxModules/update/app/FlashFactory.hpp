/*
 * @Author: xiong
 * @Date: 2022/9/5
*/

#ifndef EMX_FlashFactory_HPP
#define EMX_FlashFactory_HPP

#include "FlashInf.hpp"

namespace Emx {
    class FlashFactory {
    public:
        static FlashInf *Make(const char *type);
    };
};

#endif //EMX_FlashFactory_HPP
