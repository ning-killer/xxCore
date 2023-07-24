/*
* @Author: xiong
* @Date: 2023/2/8
*/

#ifndef EMX_EventInner_HPP
#define EMX_EventInner_HPP

#define EVENT_MSG_ID 0x000B0000
#define EVENT_RESP_ID EVENT_MSG_ID
#define EVENT_TOPIC_ID EVENT_MSG_ID

namespace Emx {
    enum class EventModuleE : uint32_t {
        Global,
        Motion,
    };
    enum class EventGlobalMethodE : uint32_t {
        ListGet,
        GlobalSet,
        GlobalGet,
    };
    enum class EventMotionMethodE : uint32_t {
        MotionSet,
        MotionGet,
    };
}
#endif //EMX_EventInner_HPP
