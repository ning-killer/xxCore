/*
* @Author: xiong
* @Date: 2023/1/16
*/

#ifndef EMX_RtspInner_HPP
#define EMX_RtspInner_HPP

#include <cstdint>

#define EMX_RTSP_MSG_ID 0x000A0000

namespace Emx {

    enum class RtspModuleE : uint8_t {
        Rtsp,
    };

    enum class RtspMethodE : uint8_t {
        GetParam,
        SetParam
    };

}
#endif //EMX_RtspInner_HPP
