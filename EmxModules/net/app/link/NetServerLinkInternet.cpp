//
// Created by xiong on 2022/6/6.
//
#include "NetServerLinkInternet.hpp"

#include <linux/if.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <netinet/in.h>
#include <netinet/ip6.h>
#include <netinet/ip_icmp.h>
#include <netinet/icmp6.h>

using namespace Emx;

#define PACKET_SIZE 64

// ICMP报文结构
struct icmp_packet {
    struct icmphdr header;
    char data[PACKET_SIZE - sizeof(struct icmphdr)];
};

struct ip_icmp_packet {
    struct ip ip;
    icmp_packet icmp;
};

// ICMPv6 报文结构
struct icmpv6_packet {
    struct icmp6_hdr header;
    char data[PACKET_SIZE - sizeof(struct icmp6_hdr)];
};

struct ipv6_icmpv6_packet {
    struct ip6_hdr ip;
    icmpv6_packet icmp;
};

static bool GetGateWay(const std::string &netDev, char *gateway) {
    char tmp[1024] = {};
    sprintf(tmp, "route -ne | grep UG | grep %s", netDev.c_str());
    FILE *pp = popen(tmp, "r");
    if (pp) {
        memset(tmp, 0, 1024);
        fread(tmp, 1, 1024, pp);
        sscanf(tmp, "%*s %s", gateway);
        if (strlen(gateway) < 7)  // 0.0.0.0
            gateway[0] = 0;
        pclose(pp);
        return true;
    }
    return false;
}

void NetServerLinkInternetConnect::Create(OnConnectionChanged cb) {
    m_cb = std::move(cb);
    auto &addr = m_res.param["internetCheck"]["addr"];
    for (auto &e : addr) {
        Domain domain = {};
        strncpy(domain.name, e["addr"].asCString(), sizeof(domain.name));
        domain.port = e["port"].asInt();
        m_domainArray.push_back(domain);
    }
    m_timer.Create(m_res.loop);
    m_dns.Create(&m_res.loop);
    Start();
}

void NetServerLinkInternetConnect::Destroy() {
    // m_tcp.Destroy();
    m_dns.Destroy();
    m_timer.Destroy();
    if (m_sock >= 0) {
        m_poll.Stop();
        close(m_sock);
        m_sock = -1;
    }
}

void NetServerLinkInternetConnect::Start() {
    GetGateWay(m_interface, m_gateway);
    m_domainIdx = 0;
    RunDns();
}

void NetServerLinkInternetConnect::ReStart() {
    uint32_t interval = m_res.param["internetCheck"]["intervalS"].asInt() * 1000;
    m_timer.Start(interval, 0,
                  [this]() { Start(); });
    emxlogt("after %ums to restart\n", interval);
}

void NetServerLinkInternetConnect::RunDns() {
    m_tcpIdx = 0;
    m_id++;// 更新ping id
    m_pingSend = false;
    for (uint32_t i = 0; i < m_domainArray.size(); i++) {
        m_domainArray[i].addrArray.clear();
    }
    
    m_dns.Run(m_domainArray[m_domainIdx].name,
              std::bind(&NetServerLinkInternetConnect::OnResolve, this, ph_1,
                        ph_2, ph_3));
    m_timer.Start(15 * 1000, 0, [this]() {
        emxlogd("%s parse domain %s timeout\n", GetProtoName(),
                m_domainArray[m_domainIdx].name);
        m_cb(false);
    });

    if (m_sock >= 0) {
        emxlogt("socket:%d close\n", m_sock);
        m_poll.Stop();
        close(m_sock);
        m_sock = -1;
    }
    m_sock = SocketCreate();
    m_poll.InitSock(m_res.loop, m_sock);
    m_poll.Start(std::bind(&NetServerLinkInternetConnect::RecvPong, this), nullptr, nullptr);

    emxlogt("socket:%d, %s parsing domain %s!\n", m_sock, GetProtoName(),
            m_domainArray[m_domainIdx].name);
}

// 计算校验和
uint16_t NetServerLinkInternetConnect::CheckSum(uint16_t *buffer, int length) {
    unsigned long sum = 0;
    for (sum = 0; length > 1; length -= 2) { sum += *buffer++; }
    if (length == 1) { sum += *(uint8_t *) buffer; }
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    return (uint16_t) (~sum);
}

void NetServerLinkInternetConnect::OnResolve(ErrCodeE e, struct addrinfo *addr,
                                             void *arg) {
    m_timer.Stop();
    emxlogt("%s got domain %s\n", GetProtoName(),
            m_domainArray[m_domainIdx].name);
    std::vector<Domain::Addr> tmpAddrArray = {};
    if (e == ErrCodeE::Success) {
        emxlogt("success\n");
        for (auto p = addr; p; p = p->ai_next) {
            // 只获取和当前family一致的地址
            if (m_family != p->ai_family) {
                continue;
            }
            Domain::Addr a = {};
            if (p->ai_family == AF_INET) {
                uv_ip4_name(((sockaddr_in *)p->ai_addr), a.ip, sizeof(a.ip));
            } else if (p->ai_family == AF_INET6) {
                uv_ip6_name(((sockaddr_in6 *)p->ai_addr), a.ip, sizeof(a.ip));
            } else {
                continue;
            }
            tmpAddrArray.push_back(a);
            emxlogt("%s domain %s:%d got ip [%s]\n", GetProtoName(),
                    m_domainArray[m_domainIdx].name,
                    m_domainArray[m_domainIdx].port, a.ip);
        }
    } else {
        emxlogt("failed\n");
    }

    // 成功获取到ip后才会更新IP列表
    if (!tmpAddrArray.empty()) {
        m_domainArray[m_domainIdx].addrArray = tmpAddrArray;
    }

    if (m_domainArray[m_domainIdx].addrArray.empty()) {  // 最终ip列表还是空的
        if (++m_domainIdx <
            (int)m_domainArray.size()) {  // 还有可以解析的则解析下一个domain
            RunDns();
        } else {  // 没有可解析的domain就回调连接失败，并延时开始下次检测
            m_cb(false);
            ReStart();
        }
    } else {  // ip列表中有IP地址(可能是新的，也可能是上次成功解析留下来的，主要用来预防dns偶发性解析失败)，尝试连接ip
        ConnectTcp();
    }
}

void NetServerLinkInternetConnect::ConnectTcp() {
    auto ip = m_domainArray[m_domainIdx].addrArray[m_tcpIdx].ip;

    // 小米路由器在无外网的情况下，获取到的地址为其网关，跳过这种ip
    if (strcmp(m_gateway, ip) == 0) {
        emxlogd("skip gateway ip %s\n", ip);
        OnConnectResult(false);
        return;
    }

    // 这里设置一个5s超时，超时后认为连接失败尝试连接下一个
    m_timer.Start(5 * 1000, 0, [this]() {
        emxlogt("ping %s timeout\n",
                m_domainArray[m_domainIdx].addrArray[m_tcpIdx].ip);
        OnConnectResult(false);
    });

    emxlogt("try ping [%s]\n", ip);
    auto timeMs = (uint32_t) Time::GetMonoClockMS();
    SendPingPack(m_sock, ip, m_id, 0, timeMs);
}

void NetServerLinkInternetConnect::OnConnectResult(bool connected) {
    emxlogt("ping domain[%d]:%s ip[%d]:%s addrArray:%d %s\n", m_domainIdx, m_domainArray[m_domainIdx].name,
            m_tcpIdx, m_domainArray[m_domainIdx].addrArray[m_tcpIdx].ip, (int)m_domainArray[m_domainIdx].addrArray.size(),
            connected ? "success" : "failed");
    // m_tcp.Destroy();
    m_pingSend = false;
    m_timer.Stop();
    if (connected) {
        m_cb(true);
        ReStart();
    } else if (++m_tcpIdx < (int)m_domainArray[m_domainIdx].addrArray.size()) {
        ConnectTcp();
    } else if (++m_domainIdx < (int)m_domainArray.size()) {
        RunDns();
    } else {
        m_cb(false);
        ReStart();
    }

}

int NetServerLinkInternetConnect::SocketCreate() {
    if (m_family == AF_INET6) {
        return socket(AF_INET6, SOCK_RAW, IPPROTO_ICMPV6);
    }else {
        return socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    }
}

void NetServerLinkInternetConnect::SendPingPack(int sock, const char *ip, uint16_t id, uint16_t sequence, uint32_t timeMs) {
    if (m_family == AF_INET6) { 
        emxlogt("ping6 %s, id:%d, sequence:%d\n", ip, id, sequence);
        sockaddr_in6 addr = {};
        uv_ip6_addr(ip, 0, &addr);
        icmpv6_packet packet = {};
        packet.header.icmp6_type = ICMP6_ECHO_REQUEST;
        packet.header.icmp6_code = 0;
        packet.header.icmp6_cksum = 0;
        packet.header.icmp6_id = id;
        packet.header.icmp6_seq = sequence;
        memcpy(packet.data, &timeMs, sizeof(timeMs));
        packet.header.icmp6_cksum = CheckSum((unsigned short *) &packet, sizeof(packet));
        sendto(sock, &packet, sizeof(packet), MSG_DONTWAIT, (struct sockaddr *) &addr, sizeof(addr));
    }else {
        emxlogt("ping4 %s, id:%d, sequence:%d\n", ip, id, sequence);
        sockaddr_in addr = {};
        uv_ip4_addr(ip, 0, &addr);
        icmp_packet packet = {};
        packet.header.type = ICMP_ECHO;
        packet.header.code = 0;
        packet.header.checksum = 0;
        packet.header.un.echo.id = id;
        packet.header.un.echo.sequence = sequence;
        memcpy(packet.data, &timeMs, sizeof(timeMs));
        packet.header.checksum = CheckSum((unsigned short *) &packet, sizeof(packet));
        sendto(sock, &packet, sizeof(packet), MSG_DONTWAIT, (struct sockaddr *) &addr, sizeof(addr));
    }
    m_pingSend = true;
}

bool NetServerLinkInternetConnect::RecvPongPack(int sock, uint16_t &id, uint16_t &sequence, uint32_t &timeMs, char *ip) {
    if (m_family == AF_INET6) { 
        ipv6_icmpv6_packet packet = {};
        sockaddr_in6 addr = {};
        socklen_t len = sizeof(addr);
        auto n = recvfrom(sock, &packet, sizeof(packet), 0, (struct sockaddr *) &addr, &len);
        if (n == -1 || n != sizeof(packet)) { 
            emxlogt("pong6 ip:%s, id:%d, sequence:%d n:%d, sizeof(packet):%d, failed\n", ip, id, sequence, n, sizeof(packet));
            return false; 
        }
        id = packet.icmp.header.icmp6_id;
        sequence = packet.icmp.header.icmp6_seq;
        memcpy(&timeMs, packet.icmp.data, sizeof(uint64_t));
        uv_ip6_name(&addr, ip, Net::MaxAddrSize);
        emxlogt("pong6 ip:%s, id:%d, sequence:%d\n", ip, id, sequence);
        return true;
    }else {
        ip_icmp_packet packet = {};
        sockaddr_in addr = {};
        socklen_t len = sizeof(addr);
        auto n = recvfrom(sock, &packet, sizeof(packet), 0, (struct sockaddr *) &addr, &len);
        if (n == -1 || n != sizeof(packet)) { 
            emxlogt("pong4 ip:%s, id:%d, sequence:%d n:%d, sizeof(packet):%d, failed\n", ip, id, sequence, n, sizeof(packet));
            return false; 
        }
        id = packet.icmp.header.un.echo.id;
        sequence = packet.icmp.header.un.echo.sequence;
        memcpy(&timeMs, packet.icmp.data, sizeof(uint64_t));
        uv_ip4_name(&addr, ip, Net::MaxAddrSize);
        emxlogt("pong4 ip:%s, id:%d, sequence:%d\n", ip, id, sequence);
        return true;
    }
}

void NetServerLinkInternetConnect::RecvPong() {
    uint16_t id;
    uint16_t sequence;
    uint32_t timeMs;
    char ip[Net::MaxAddrSize];
    auto c = RecvPongPack(m_sock, id, sequence, timeMs, ip) && id == m_id;
    if (c) {
        OnConnectResult(c);
    }
}

void NetServerLinkInternet::Create(
    NetServerLinkInternet::OnInternetChanged cb) {
    m_cb = std::move(cb);
    m_connect4last = false;
    m_connect4.Create(
        std::bind(&NetServerLinkInternet::OnConnect4Done, this, ph_1));
    m_connect6last = false;
    m_connect6.Create(
        std::bind(&NetServerLinkInternet::OnConnect6Done, this, ph_1));
}

void NetServerLinkInternet::Destroy() {
    m_connect4.Destroy();
    m_connect6.Destroy();
}

void NetServerLinkInternet::OnConnect4Done(bool connected) {
    emxlogt("last=%d, current=%d\n", m_connect4last, connected);
    // 如果连接状态发生了改变
    if (connected != m_connect4last) {
        m_connect4last = connected;
        m_cb(m_connect4last, m_connect6last);
    }
}

void NetServerLinkInternet::OnConnect6Done(bool connected) {
    emxlogt("last=%d, current=%d\n", m_connect6last, connected);
    // 如果连接状态发生了改变
    if (connected != m_connect6last) {
        m_connect6last = connected;
        m_cb(m_connect4last, m_connect6last);
    }
}
