//
// Created by xiong on 2022/5/25.
//

#ifndef EMX_NetInner_HPP
#define EMX_NetInner_HPP

#define NET_SERVER_MSG_ID 0x00040000
#define NET_SERVER_RESP_ID NET_SERVER_MSG_ID
#define NET_SERVER_TOPIC_ID NET_SERVER_MSG_ID
#define NET_SERVER_TOPIC_LINK (NET_SERVER_TOPIC_ID + 0)

#define NET_SERVER_DNS_MODULE_ID 0x12340000


namespace Emx {
    enum class NetMainMethodE : uint32_t {
        Set = 0,
        Get,
        IsSupported,
    };

    enum class NetClientMethodE : uint32_t {
        GetInterface = 0,
        SetAddr,
        SetTempAddr,
        GetAddr,
        SetEna,
        GetEna,
        GetLinkStat,
        ReLink,
        Butt,
    };

    enum class NetClientWifiMethodE : uint32_t {
        SwitchToMode = (uint32_t) NetClientMethodE::Butt,
        SetSTA,
        SetAP,
        GetAPList,
        GetInfo,
    };

    enum class NetClientDnsMethodE : uint32_t {
        GetParam,
        SetParam,
    };
}


#endif //EMX_NetInner_HPP

