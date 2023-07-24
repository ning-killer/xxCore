//
// Created by xiong on 2022/5/27.
//
#include "NetServerLink.hpp"

using namespace Emx;

void NetServerLink::Create(NetServerLink::OnLinkChanged cb) {
    m_cb = std::move(cb);
    m_phy->Create(std::bind(&NetServerLink::OnPhy, this, ph_1, ph_2));
    m_lan.Create(std::bind(&NetServerLink::OnLan, this, ph_1, ph_2));
    m_internet.Create(std::bind(&NetServerLink::OnInternet, this, ph_1, ph_2));
}

void NetServerLink::Destroy() {
    m_internet.Destroy();
    m_lan.Destroy();
    m_phy->Destroy();
}

void NetServerLink::OnPhy(bool connected, const char *msg) {
    m_link.phy = connected;
    memset(m_link.msg, 0, sizeof(m_link.msg));
    if (msg)
        strncpy(m_link.msg, msg, sizeof(m_link.msg));
    m_cb(m_link);
}

void NetServerLink::OnLan(bool con4, bool con6) {
    m_link.lan4 = con4;
    m_link.lan6 = con6;
    m_cb(m_link);
}

void NetServerLink::OnInternet(bool con4, bool con6) {
    m_link.internet4 = con4;
    m_link.internet6 = con6;
    m_cb(m_link);
}
