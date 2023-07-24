//
// Created by xiong on 2022/4/15.
//

#ifndef EMX_ToolsInner_HPP
#define EMX_ToolsInner_HPP

#include "EmxCore.hpp"

#define EMX_TOOLS_MSG_ID 0x00030000

namespace Emx {
    enum class EmxToolsModuleE : uint8_t {
        Tools,
    };

    enum class EmxToolsMethodE : uint8_t {
        Start,
        Stop
    };
}


#endif //EMX_ToolsInner_HPP

