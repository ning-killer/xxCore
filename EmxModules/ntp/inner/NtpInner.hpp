#ifndef EMX_NtpInner_HPP
#define EMX_NtpInner_HPP
#include <stdint.h>
#define NTP_SERVER_MSG_ID 0x000F0000
#define NTP_SERVER_RESP_ID NTP_SERVER_MSG_ID
#define NTP_SERVER_TOPIC_ID NTP_SERVER_MSG_ID

#define NTP_SERVER_DEFAULT_MODULE_ID 0x12340000


namespace Emx {
    enum class NtpModE : uint32_t {
        Default = 0,
    };

    enum class NtpDefaultMethodE : uint32_t {
        Update = 0,
        Butt,
    };
}// namespace Emx

#endif//EMX_NetInner_HPP
