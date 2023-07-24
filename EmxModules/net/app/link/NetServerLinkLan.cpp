//
// Created by xiong on 2022/6/6.
//

#include "NetServerLinkLan.hpp"

using namespace Emx;


void NetServerLinkLan::Create(OnLanChanged cb) {
    m_cb = std::move(cb);
    m_timer.Create(m_res.loop);
    m_timer.Start(0, 1000, [this]() { OnCheck(); });
}

void NetServerLinkLan::Destroy() {
    m_timer.Destroy();
}

void NetServerLinkLan::OnCheck() {
    Net::Addr addr = {};
    Net::GetLocalAddr(m_interface, addr);
    bool lan4 = addr.ip4[0] != 0;
    bool lan6 = addr.ip6[0] != 0;
    // if connection status changed
    if (m_con4 != lan4 || m_con6 != lan6) {
        m_con4 = lan4;
        m_con6 = lan6;
        if (m_con4 || m_con6)
            emxlogi("%s ip4[%s],ip6[%s]\n", m_interface, addr.ip4, addr.ip6);
        m_cb(m_con4, m_con6);
    }
}