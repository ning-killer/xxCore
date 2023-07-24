//
// Created by xiong on 2022/1/12.
//

#ifndef EMX_EnvInner_HPP
#define EMX_EnvInner_HPP
#include "core/EmxTypeDef.hpp"

//ParamServer Msg ID
#define PARAM_SERVER_MSG_ID 0x00010000

#define PARAM_SERVER_MOD 0x1000

namespace Emx {
    enum class ParamServerMethodE : uint32_t {
        Set = 0,
        Get,
        GetDefault,
        Reset,
        ResetToFactory,
    };
}

#endif //EMX_EnvInner_HPP

