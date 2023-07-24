//
// Created by xiong on 2022/6/8.
//

#include "NetServerLinkPhyWifi.hpp"

using namespace Emx;

void NetServerLinkPhyWifi::Create(NetServerLinkPhy::OnPhyChanged cb) {
    m_cb = std::move(cb);
    InitPhyStat();
}

void NetServerLinkPhyWifi::SetPhyLinked(bool connected, const char *msg) {
    if (m_connected != connected) {
        m_connected = connected;
        m_cb(m_connected, msg);
    }
}
