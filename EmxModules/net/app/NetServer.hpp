//
// Created by xiong on 2021/3/11.
//

#ifndef EMX_NETSERVERNET_HPP
#define EMX_NETSERVERNET_HPP

#include "Net.hpp"
#include "NetServerResource.hpp"
#include "NetServerWifi.hpp"
#include "NetServerEth.hpp"
#include "NetServerMobile.hpp"

namespace Emx {
    class NetServer {
    public:
        NetServer() : m_quit(false) {
            m_res.buffer = nullptr;
        }

        virtual ~NetServer() {}

        using OnCreated = std::function<void()>;

        void Create(OnCreated cb, bool newThread);

        void Destroy();

    private:
        ErrCodeE OnRecvReq(uint32_t module, uint32_t method,
                           const char *data, int32_t size, std::string &out);

        ErrCodeE Proc(uint32_t method, const char *data, int32_t size, std::string &out);

        ErrCodeE SaveParam();

    private:
        bool m_quit;
        NetServerResource m_res;
        std::map<Net::DevE, NetServerDev *> m_mapDev;
        MsgRpcRep m_resp;
        bool m_pubConnected;
        bool m_respConnected;
        EuvTimer m_launcher;
        EuvAsync m_test;
        OnCreated m_cb;
    };
}
#endif //EMX_NETSERVERNET_HPP
