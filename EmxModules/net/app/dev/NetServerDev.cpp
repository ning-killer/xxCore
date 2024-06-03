//
// Created by xiong on 2021/3/9.
//
#include <netinet/in.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/if.h>
#include <csignal>
#include <cstring>
#include "EmxCore.hpp"
#include "NetServerResource.hpp"
#include "NetServerDev.hpp"
#include "NetInner.hpp"
#include "Mac.hpp"

using namespace Emx;


NetServerDev::NetServerDev(NetServerResource &res, Json::Value &devJsonParam, NetServerLinkPhy *phy) :
        m_res(res), m_devJsonParam(devJsonParam), m_link(nullptr) {
    memset(&m_linkMsg, 0, sizeof(m_linkMsg));
    m_linkMsg.dev = GetDevType(devJsonParam["name"].asCString());
    strncpy(m_interface, devJsonParam["interface"].asCString(), sizeof(m_interface));
    m_link = new NetServerLink(m_interface, res, phy);
}

NetServerDev::~NetServerDev() {
    delete m_link;
}

ErrCodeE NetServerDev::Create() {
    m_link->Create(std::bind(&NetServerDev::OnLinkChanged, this, ph_1));
    CreateDev();
    ConfigEna(m_devJsonParam["ena"].asBool());
    return ErrCodeE::Success;
}

void NetServerDev::Destroy() {
    DestroyDev();
    m_link->Destroy();
}

ErrCodeE NetServerDev::Proc(uint32_t method, const char *data, int32_t size, std::string &out) {
    switch ((NetClientMethodE) method) {
        case NetClientMethodE::GetInterface: {
            out.assign(GetInterface());
            return ErrCodeE::Success;
        }
        case NetClientMethodE::SetAddr: {
            Net::Addr addr = {};
            memcpy(&addr, data, sizeof(addr));
            return SetAddr(addr);
        }
        case NetClientMethodE::SetTempAddr: {
            Net::Addr addr = {};
            memcpy(&addr, data, sizeof(addr));
            return SetTempAddr(addr);
        }
        case NetClientMethodE::GetAddr: {
            Net::Addr addr = {};
            if (GetAddr(addr) != ErrCodeE::Success) {
                return ErrCodeE::Failure;
            }
            out.assign((char *) &addr, sizeof(addr));
            return ErrCodeE::Success;
        }
        case NetClientMethodE::SetEna: {
            bool ena;
            memcpy(&ena, data, sizeof(ena));
            return SetEna(ena);
        }
        case NetClientMethodE::GetEna: {
            bool ena;
            if (GetEna(ena) != ErrCodeE::Success) {
                return ErrCodeE::Failure;
            }
            out.assign((char *) &ena, sizeof(ena));
            return ErrCodeE::Success;
        }
        case NetClientMethodE::GetLinkStat: {
            LinkStat link = {};
            GetLinkStat(link);
            out.assign((char *) &link, sizeof(link));
            return ErrCodeE::Success;
        }
        case NetClientMethodE::ReLink: {
            return ReLink();
        }
        default:
            return ProcDev(method, data, size, out);
    }
}

ErrCodeE NetServerDev::SetAddr(Addr &addr) {
    bool isChanged = false;
    bool isMacChange = false;

    if (addr.mac[0]) {
        for (char &e : addr.mac) {
            if (e >= 'a' && e <= 'f') {
                e -= 32;
            } else if (e == '-') {
                e = ':';
            }
        }
        if (strcmp(addr.mac, m_devJsonParam["mac"].asCString()) != 0) {
            m_devJsonParam["mac"] = addr.mac;
            isMacChange = true;
        }
    }
    if (addr.dhcp != m_devJsonParam["dhcp"].asBool()) {
        m_devJsonParam["dhcp"] = addr.dhcp;
        isChanged = true;
    }
    if (!addr.dhcp) {
        if (addr.ip4[0] && strcmp(addr.ip4, m_devJsonParam["ip4"].asCString()) != 0) {
            m_devJsonParam["ip4"] = addr.ip4;
            isChanged = true;
        }
        if (addr.netmask4[0] && strcmp(addr.netmask4, m_devJsonParam["netmask4"].asCString()) != 0) {
            m_devJsonParam["netmask4"] = addr.netmask4;
            isChanged = true;
        }
        if (addr.gateway4[0] && strcmp(addr.gateway4, m_devJsonParam["gateway4"].asCString()) != 0) {
            m_devJsonParam["gateway4"] = addr.gateway4;
            isChanged = true;
        }
        if (addr.ip6[0] && strcmp(addr.ip6, m_devJsonParam["ip6"].asCString()) != 0) {
            m_devJsonParam["ip6"] = addr.ip6;
            isChanged = true;
        }
        if (addr.netmask6[0] && strcmp(addr.netmask6, m_devJsonParam["netmask6"].asCString()) != 0) {
            m_devJsonParam["netmask6"] = addr.netmask6;
            isChanged = true;
        }
        if (addr.gateway6[0] && strcmp(addr.gateway6, m_devJsonParam["gateway6"].asCString()) != 0) {
            m_devJsonParam["gateway6"] = addr.gateway6;
            isChanged = true;
        }
    }
    emxlogd("isChanged[%d],isMacChange[%d]\n", isChanged, isMacChange);
    if (isChanged || isMacChange) {
        if (isMacChange) {
            ConfigMac();
        }
        ConfigAddr();
        return m_res.saveParam();
    }
    return ErrCodeE::Success;
}

ErrCodeE NetServerDev::SetTempAddr(Addr &addr) {
    bool isChanged = false;
    bool isMacChange = false;

    if (addr.mac[0]) {
        for (char &e : addr.mac) {
            if (e >= 'a' && e <= 'f') {
                e -= 32;
            } else if (e == '-') {
                e = ':';
            }
        }
        if ((!m_devJsonParam["mac"].asString().empty()) && (strcmp(addr.mac, m_devJsonParam["mac"].asCString())) != 0) {
            isMacChange = true;
        }
    }
    if (addr.dhcp != m_devJsonParam["dhcp"].asBool()) {
        isChanged = true;
    }
    if (!addr.dhcp) {
        if (addr.ip4[0] && strcmp(addr.ip4, m_devJsonParam["ip4"].asCString()) != 0) {
            isChanged = true;
        }
        if (addr.netmask4[0] && strcmp(addr.netmask4, m_devJsonParam["netmask4"].asCString()) != 0) {
            isChanged = true;
        }
        if (addr.gateway4[0] && strcmp(addr.gateway4, m_devJsonParam["gateway4"].asCString()) != 0) {
            isChanged = true;
        }
        if (addr.ip6[0] && strcmp(addr.ip6, m_devJsonParam["ip6"].asCString()) != 0) {
            isChanged = true;
        }
        if (addr.netmask6[0] && strcmp(addr.netmask6, m_devJsonParam["netmask6"].asCString()) != 0) {
            isChanged = true;
        }
        if (addr.gateway6[0] && strcmp(addr.gateway6, m_devJsonParam["gateway6"].asCString()) != 0) {
            isChanged = true;
        }
    }
    emxlogd("SetTempAddr isChanged[%d],isMacChange[%d]\n", isChanged, isMacChange);
    if (isChanged || isMacChange) {
        if (isMacChange) {
            Cmd::Run("ifconfig %s down", m_interface);
            Cmd::Run("ifconfig %s hw ether %s", m_interface, addr.mac);
            Cmd::Run("ifconfig %s up", m_interface);
        }
        if (addr.dhcp) {
            DhcpReNew();
        } else {
            DhcpRelease();
            if (strlen(addr.ip4) > 0) {
                Cmd::Run("ifconfig %s %s", m_interface, addr.ip4);
            }
            if (strlen(addr.netmask4) > 0) {
                Cmd::Run("ifconfig %s netmask %s", m_interface, addr.netmask4);
            }
            if (strlen(addr.gateway4) > 0) {
                Cmd::Run("route del default gw 0.0.0.0 dev %s", m_interface);
                Cmd::Run("route add default gw %s dev %s", addr.gateway4, m_interface);
            }
            //todo modify
            if (strlen(addr.ip6) > 0 && strlen(addr.netmask6) > 0) {
                char maskStr[Net::Ipv6AddrSize] = {};
                strncpy(maskStr, addr.netmask6, Net::Ipv6AddrSize);
                int mask = 0;
                for (char c : maskStr) {
                    if (c == 'f')
                        mask += 4;
                }
                Cmd::Run("ifconfig %s %s/%d", m_interface, addr.ip6, mask);
            }
            if (strlen(addr.netmask6) > 0) {
                Cmd::Run("ifconfig %s netmask %s", m_interface, addr.netmask6);
            }
            if (strlen(addr.gateway6) > 0) {
                Cmd::Run("route -A inet6 del default gw ::/0 dev %s", m_interface);
                Cmd::Run("route -A inet6 add default gw %s dev %s", addr.gateway6, m_interface);
            }
        }
        DoDns();
    }
    return ErrCodeE::Success;
}

std::string NetServerDev::GetMac() {
    std::string mac = "";
    FILE* pipe = popen("ifconfig", "r");
    if (!pipe) {
        return mac;
    }
    char buffer[256] = { 0 };
    std::string result = "";
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }
    pclose(pipe);
    size_t pos = result.find("HWaddr");
    if (pos != std::string::npos) {
        mac = result.substr(pos + 6 + 1, 17);
    }
    return mac;
}

void NetServerDev::Ipv6Delete()
{
    Addr addr = {};
    NetServerDev::GetAddr(addr);

    emxlogi("ipv6 is %s\n", strlen(addr.ip6)>0?"null":addr.ip6);
    if (strlen(addr.ip6) > 0)
        Cmd::Run("ip addr del %s/64 dev %s", addr.ip6, m_interface);
}

ErrCodeE NetServerDev::GetAddr(Addr &addr) {
    memset(&addr, 0, sizeof(Addr));
    addr.dhcp = m_devJsonParam["dhcp"].asBool();
    Net::GetLocalAddr(GetInterface(), addr);
    if (strlen(addr.mac) == 0) {
        std::string s_mac = GetMac();
        if (s_mac.size() <= sizeof(addr.mac)) {
            memset(addr.mac, 0, sizeof(addr.mac));
            memcpy(addr.mac, s_mac.c_str(), s_mac.size());
        } 
    }
    /*
    Destination     Gateway         Genmask         Flags   MSS Window  irtt Iface
    default         192.168.31.1    0.0.0.0         UG        0 0          0 wlan0
    default         192.168.31.1    0.0.0.0         UG        0 0          0 eth0
    192.168.31.0    *               255.255.255.0   U         0 0          0 eth0
    192.168.31.0    *               255.255.255.0   U         0 0          0 wlan0
    */
    char tmp[1024];
    sprintf(tmp, "route -ne | grep UG | grep %s", m_interface);
    FILE *pp = popen(tmp, "r");
    if (pp) {
        memset(tmp, 0, 1024);
        fread(tmp, 1, 1024, pp);
        sscanf(tmp, "%*s %s", addr.gateway4);
        if (strlen(addr.gateway4) < 7) //0.0.0.0
            addr.gateway4[0] = 0;
        pclose(pp);
    } else {
        emxloge("popen route %s failed\n", m_interface);
    }
    //todo ipv6 default gateway
//    Kernel IPv6 routing table
//    Destination                      Next Hop       Flags Metric Ref    Use Iface
//    fe80::2e0:99ff:fe0c:26d1/128     ::             Un    0      1        0 lo
//    ff00::/8                         ::             U     256    1     1933 eth0
//    ::/0                             ::             U     256    0        0 eth0
//    ::/0                             ::             !n    -1     1        5 lo
//    sprintf(tmp, "route -A inet6 | grep U | grep %s", m_interface);
//    pp = popen(tmp, "r");
//    if (pp) {
//        memset(tmp, 0, 1024);
//        fread(tmp, 1, 1024, pp);
//        sscanf(tmp, "%*s %s", addr.gateway4);
//        if (strlen(addr.gateway4) < 7) //0.0.0.0
//            addr.gateway4[0] = 0;
//        pclose(pp);
//    } else {
//        emxloge("popen route %s failed\n", m_interface);
//    }
    return ErrCodeE::Success;
}

ErrCodeE NetServerDev::SetEna(bool ena) {
    if (m_devJsonParam["ena"].asBool() == ena) {
        return ErrCodeE::Success;
    }
    m_devJsonParam["ena"] = ena;
    ConfigEna(ena);
    m_res.saveParam();
    return ErrCodeE::Success;
}

ErrCodeE NetServerDev::GetEna(bool &ena) {
    ena = m_devJsonParam["ena"].asBool();
    return ErrCodeE::Success;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//int NetServerDev::GetDhcpPid() {
//    char tmp[EMX_MAX_PATH_SIZE] = {};
//    sprintf(tmp, "%s/udhcpc_%s.pid", EMX_NET_PID_DIR, m_interface);
//    FILE *fp = fopen(tmp, "r");
//    if (!fp) {
//        return 0;
//    }
//    fread(tmp, 1, EMX_MAX_PATH_SIZE, fp);
//    fclose(fp);
//    return (int) strtol(tmp, nullptr, 10);
//}

void NetServerDev::DhcpRelease() {
//    int pid = GetDhcpPid();
//    if (pid > 0)
//        kill(pid, SIGUSR2);
    Cmd::Run("ps | grep udhcpc |  grep %s | grep -v grep | awk '{print $1}' | xargs kill ", GetInterface());
    sleep(1);//杀掉udhcpc之后立刻通过ifconfig操作网卡会导致网卡操作失败，这里延时1秒再返回就正常了
}

void NetServerDev::DhcpReNew() {
//    int pid = GetDhcpPid();
//    if (pid > 0)
//        kill(pid, SIGUSR1);
    Cmd::Run("ps | grep udhcpc |  grep %s | grep -v grep | awk '{print $1}' | xargs kill -9", GetInterface());
    //用于NFS调试的时候如果udhcpc.script位于nfs目录会导致udhcpc不成功
    if (strstr(Misc::GetAppConfigsDir(), "/mnt/nfs")) {
        const char *nfsDebugNetDir = "/root/firmware/app/configs/net";
        emxlogw("nfs debug mode, udhcpc -s %s/udhcpc.script\n", nfsDebugNetDir);
        Cmd::System("udhcpc -b -t 5 -T 2 -A 5 -i %s -s %s/udhcpc.script -R&", GetInterface(), nfsDebugNetDir);
    } else {
        Cmd::System("udhcpc -b -t 5 -T 2 -A 5 -i %s -s %s/udhcpc.script -R&", GetInterface(), m_res.netConfigDir);
    }

}

ErrCodeE NetServerDev::ConfigAddr() {
    ConfigMac();
    if (m_devJsonParam["dhcp"].asBool()) {
        DhcpReNew();
    } else {
        DhcpRelease();
        if (!m_devJsonParam["ip4"].asString().empty()) {
            Cmd::Run("ifconfig %s %s", m_interface, m_devJsonParam["ip4"].asCString());
        }
        if (!m_devJsonParam["netmask4"].asString().empty()) {
            Cmd::Run("ifconfig %s netmask %s", m_interface, m_devJsonParam["netmask4"].asCString());
        }
        if (!m_devJsonParam["gateway4"].asString().empty()) {
            Cmd::Run("route del default gw 0.0.0.0 dev %s", m_interface);
            Cmd::Run("route add default gw %s dev %s", m_devJsonParam["gateway4"].asCString(), m_interface);
        }
        //todo modify
        if (!m_devJsonParam["ip6"].asString().empty() && !m_devJsonParam["netmask6"].asString().empty()) {
            char maskStr[Net::Ipv6AddrSize] = {};
            strncpy(maskStr, m_devJsonParam["netmask6"].asCString(), Net::Ipv6AddrSize);
            int mask = 0;
            for (char c : maskStr) {
                if (c == 'f')
                    mask += 4;
            }
            Cmd::Run("ifconfig %s %s/%d", m_interface, m_devJsonParam["ip6"].asCString(), mask);
        }
        if (!m_devJsonParam["netmask6"].asString().empty()) {
            Cmd::Run("ifconfig %s netmask %s", m_interface, m_devJsonParam["netmask4"].asCString());
        }
        if (!m_devJsonParam["gateway6"].asString().empty()) {
            Cmd::Run("route -A inet6 del default gw ::/0 dev %s", m_interface);
            Cmd::Run("route -A inet6 add default gw %s dev %s", m_devJsonParam["gateway6"].asCString(), m_interface);
        }

        //todo set ipv6 addr
    }
    DoDns();
    return ErrCodeE::Success;
}

ErrCodeE NetServerDev::ConfigMac() {
    emxlogd("ConfigMac\n");
    if (!m_devJsonParam["mac"].asString().empty()) {
        Addr addr = {};
        GetAddr(addr);
        emxlogd("addr mac[%s]; current mac[%s]\n", m_devJsonParam["mac"].asString().c_str(), addr.mac);
        if (m_devJsonParam["mac"].asString() != addr.mac) {
            Cmd::Run("ifconfig %s down", m_interface);
            Cmd::Run("ifconfig %s hw ether %s", m_interface, m_devJsonParam["mac"].asCString());
            Cmd::Run("ifconfig %s up", m_interface);
            emxlogd("netServer mac config end\n");
        }
    } else {
        Mac mac;
        if (mac.Load() != ErrCodeE::Success) {
            return ErrCodeE::Success;
        }
        std::string s_mac(mac.Get());
        if (s_mac == "") {
            return ErrCodeE::Success;
        }
        Addr addr = {};
        GetAddr(addr);
        emxlogd("addr mac[%s]; current mac[%s]\n", s_mac.c_str(), addr.mac);
        if (s_mac != std::string(addr.mac)) {
            Cmd::Run("ifconfig %s down", m_interface);
            Cmd::Run("ifconfig %s hw ether %s", m_interface, s_mac.c_str());
            Cmd::Run("ifconfig %s up", m_interface);
            emxlogd("mac config end\n");
        }
    }
    return ErrCodeE::Success;
}

ErrCodeE NetServerDev::ConfigEna(bool ena) {
    if (ena) {
        Cmd::Run("ifconfig %s up", GetInterface());
        ConfigAddr();
    } else {
        DhcpRelease();
        Cmd::Run("ifconfig %s down", GetInterface());
    }
    return ErrCodeE::Success;
}

void NetServerDev::OnLinkChanged(LinkStat &link) {
    if (!m_linkMsg.link.phy && link.phy)
        ConfigAddr();
    m_linkMsg.link = link;
    m_res.pub.Publish(NET_SERVER_TOPIC_LINK, (char *) &m_linkMsg, sizeof(m_linkMsg));
}

/////////////////////////////////////////////////////////////////////////////////////

ErrCodeE NetServerDev::ProcDev(uint32_t method, const char *data, int32_t size, std::string &out) {
    emxloge("%s not support method[%u]\n", m_devJsonParam["name"].asCString(), method);
    return ErrCodeE::OperationNotSupport;
}

void NetServerDev::DoDns() {
    // 仅主网卡配置dns
    if (m_devJsonParam["name"].asString() != m_res.param["main"].asString()) {
        return;
    }
    Cmd::Run("ps | grep udhcpc |  grep dns | grep -v grep | awk '{print $1}' | xargs kill -9");
    auto &dns = m_res.param["dns"];
    if (dns["manual"].asBool()) {
        std::string tmp;
        for (auto &elem : dns["server"])
            tmp.append("nameserver " + elem.asString() + "\n");
        for (auto &elem : dns["append"])
            tmp.append("nameserver " + elem.asString() + "\n");
        File::Write("/tmp/resolv.conf", tmp.data(), (int) tmp.size());
    } else {
        std::string tmp;
        for (auto &elem : dns["append"])
            tmp.append("nameserver " + elem.asString() + "\n");
        File::Write("/tmp/dnsAppend.conf", tmp.data(), (int) tmp.size());
        Cmd::System("udhcpc -b -t 5 -T 2 -A 5 -q -i %s -s %s/dns.script &",
                    m_devJsonParam["interface"].asCString(), m_res.netConfigDir);
    }
}

