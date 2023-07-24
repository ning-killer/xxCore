//
// Created by xiong on 2022/6/6.
//

#ifndef EMX_NetServerLinkPhy_HPP
#define EMX_NetServerLinkPhy_HPP

#include "Net.hpp"
#include "NetServerResource.hpp"

namespace Emx {

    class NetServerLinkPhy {
    public:
        NetServerLinkPhy(const char *interface, NetServerResource &res) :
                m_interface(interface),
                m_res(res) {}

        virtual ~NetServerLinkPhy() {}

        using OnPhyChanged = std::function<void(bool connected, const char *msg)>;

        virtual void Create(OnPhyChanged cb) = 0;

        virtual void Destroy() = 0;

    protected:
        void InitPhyStat() {
            bool connected = false;
            char temp[1024] = {};
            sprintf(temp, "ifconfig %s | grep RUNNING", m_interface);
            FILE *pp = popen(temp, "r");
            if (pp) {
                memset(temp, 0, 1024);
                while (fgets(temp, 1024, pp)) {
                    if (strstr(temp, "RUNNING")) {
                        connected = true;
                        break;
                    }
                }
                pclose(pp);
            }
            m_connected = connected;
            m_cb(m_connected, nullptr);
        }

    protected:
        const char *m_interface;
        NetServerResource &m_res;
        OnPhyChanged m_cb;
        bool m_connected;
    };

}

#endif //EMX_NetServerLinkPhy_HPP
