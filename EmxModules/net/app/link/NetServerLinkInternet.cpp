//
// Created by xiong on 2022/6/6.
//
#include <netinet/in.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/if.h>
#include "NetServerLinkInternet.hpp"

using namespace Emx;


void NetServerLinkInternet::Create(NetServerLinkInternet::OnInternetChanged cb) {
    m_cb = std::move(cb);
    m_timer.Create(m_res.loop);
    m_con4.socket = []() { return socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); };
    m_con6.socket = []() { return socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP); };
    Run();
}

void NetServerLinkInternet::Destroy() {
    m_timer.Destroy();
    m_dns.Destroy();
    m_con4.tcp.Destroy();
    m_con6.tcp.Destroy();
}

void NetServerLinkInternet::Run() {
    m_dnsIdx = 0;
    m_domainArray.clear();
    m_con4.Reset();
    m_con6.Reset();

    auto &addr = m_res.param["internetCheck"]["addr"];
    for (auto &e : addr) {
        Domain domain = {};
        strncpy(domain.name, e["addr"].asCString(), sizeof(domain.name));
        domain.port = e["port"].asInt();
        m_domainArray.push_back(domain);
    }
    if (m_dnsIdx < (int) m_domainArray.size()) {
        m_dns.Create(&m_res.loop);
        m_dns.Run(m_domainArray[m_dnsIdx].name,
                  std::bind(&NetServerLinkInternet::OnResolve, this, ph_1, ph_2, ph_3));
    } else {
        m_timer.Start(m_res.param["internetCheck"]["intervalS"].asInt() * 1000,
                      0, [this]() { Run(); });
    }
}

void NetServerLinkInternet::OnResolve(ErrCodeE e, struct addrinfo *addr, void *arg) {
    for (auto p = addr; p; p = p->ai_next) {
        InetCon::Addr a = {};
        a.port = m_domainArray[m_dnsIdx].port;
        if (p->ai_family == AF_INET) {
            uv_ip4_name(((sockaddr_in *) p->ai_addr), a.ip, sizeof(a.ip));
            m_con4.addrArray.push_back(a);
        } else if (p->ai_family == AF_INET6) {
            uv_ip6_name(((sockaddr_in6 *) p->ai_addr), a.ip, sizeof(a.ip));
            m_con6.addrArray.push_back(a);
        } else {
            continue;
        }
    }
    m_dnsIdx++;
    if (m_dnsIdx < (int) m_domainArray.size()) {
        m_dns.Run(m_domainArray[m_dnsIdx].name,
                  std::bind(&NetServerLinkInternet::OnResolve, this, ph_1, ph_2, ph_3));
    } else {
        if (m_con4.tcpIdx < (int) m_con4.addrArray.size())
            ConnectTcp(m_con4);
        if (m_con6.tcpIdx < (int) m_con6.addrArray.size())
            ConnectTcp(m_con6);
    }
}

void NetServerLinkInternet::ConnectTcp(InetCon &con) {
    con.tcp.Create(m_res.loop);
    int sock = con.socket();
    if (sock <= 0) {
        emxloge("%s socket error %d:%s\n", m_interface, errno, strerror(errno));
        OnConnectResult(con, false);
        return;
    }
    struct ifreq ifr = {};
    strcpy(ifr.ifr_name, m_interface);
    if (setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE, (char *) &ifr, sizeof(ifr)) < 0) {
        emxloge("%s SO_BINDTODEVICE error %d:%s\n", m_interface, errno, strerror(errno));
        close(sock);
        OnConnectResult(con, false);
        return;
    }
    con.tcp.Open(sock);
    con.tcp.GetStm()->data = this;
    auto connect = new uv_connect_t;
    connect->data = &con;
    auto &addr = con.addrArray[con.tcpIdx];
    ErrCodeE e = con.tcp.Connect(connect, addr.ip, addr.port,
                                 [](uv_connect_t *conn, int status) {
                                     auto handle = (uv_handle_t *) conn->handle;
                                     auto con = (InetCon *) conn->data;
                                     delete conn;
                                     if (uv_is_closing(handle)) {
                                         return;
                                     }
                                     auto internet = (NetServerLinkInternet *) handle->data;
                                     internet->OnConnectResult(*con, status == 0);
                                 });
    if (ErrCodeE::Success != e) {
        emxloge("connect failed\n");
        delete connect;
        OnConnectResult(con, false);
    }

}

void NetServerLinkInternet::OnConnectResult(InetCon &con, bool connected) {
    con.tcp.Destroy();
    con.tcpIdx++;
    // if connected, then skip the same protocol(IPV4/IPV6)
    if (connected) {
        con.tcpIdx = (int) con.addrArray.size();
        con.conTmp = true;
    }

    if (!con.connected && con.conTmp) {
        con.connected = con.conTmp;
        m_cb(m_con4.connected, m_con6.connected);
    }

    if (con.connected && !con.conTmp && con.tcpIdx >= (int) con.addrArray.size()) {
        con.connected = con.conTmp;
        m_cb(m_con4.connected, m_con6.connected);
    }

    if (con.tcpIdx < (int) con.addrArray.size()) {
        ConnectTcp(con);
    } else if (m_con4.tcpIdx >= (int) m_con4.addrArray.size() &&
               m_con6.tcpIdx >= (int) m_con6.addrArray.size()) {
        m_timer.Start(m_res.param["internetCheck"]["intervalS"].asInt() * 1000,
                      0, [this]() { Run(); });
    }
}
