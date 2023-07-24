/*
* @Author: xiong
* @Date: 2023/1/12
*/

#ifndef EMX_OnvifInner_HPP
#define EMX_OnvifInner_HPP

#include <cstdint>

#define EMX_ONVIF_MSG_ID 0x00090000

namespace Emx {

    enum class OnvifModuleE : uint8_t {
        Onvif,
    };

    enum class OnvifMethodE : uint8_t {
        GetParam,
        SetParam
    };

}
#endif //EMX_OnvifInner_HPP
