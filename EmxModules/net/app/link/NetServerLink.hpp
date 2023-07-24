//
// Created by xiong on 2022/5/27.
//

#ifndef EMX_NetServerLink_HPP
#define EMX_NetServerLink_HPP

#include "NetServerLinkPhy.hpp"
#include "NetServerLinkLan.hpp"
#include "NetServerLinkInternet.hpp"

namespace Emx {
    class NetServerLink {
    public:
        NetServerLink(const char *interface, NetServerResource &res, NetServerLinkPhy *phy) :
                m_interface(interface),
                m_res(res),
                m_phy(phy),
                m_lan(interface, res),
                m_internet(interface, res), m_link({}) {}

        virtual ~NetServerLink() {}

        using OnLinkChanged = std::function<void(Net::LinkStat &link)>;

        void Create(OnLinkChanged cb);

        void Destroy();

        void GetLinkStat(Net::LinkStat &link) { link = m_link; }

    protected:
        void OnPhy(bool connected, const char *msg);

        void OnLan(bool con4, bool con6);

        void OnInternet(bool con4, bool con6);

    protected:
        const char *m_interface;
        NetServerResource &m_res;
        NetServerLinkPhy *m_phy;
        NetServerLinkLan m_lan;
        NetServerLinkInternet m_internet;
        OnLinkChanged m_cb;
        Net::LinkStat m_link;
    };
}
#endif //EMX_NetServerLink_HPP
