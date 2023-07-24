//
// Created by xiong on 2022/5/25.
//

#include "Net.hpp"
#include <regex.h>

#define NET_DEV_WIFI "wifi"
#define NET_DEV_ETH "eth"
#define NET_DEV_MOBILE "mobile"
using namespace Emx;

const char *Net::GetDevName(DevE dev) {
    switch (dev) {
        case DevE::Eth:
            return NET_DEV_ETH;
        case DevE::Wifi:
            return NET_DEV_WIFI;
        case DevE::Mobile:
            return NET_DEV_MOBILE;
        default:
            return "none";
    }
}

Net::DevE Net::GetDevType(const char *name) {
    if (strcmp(name, NET_DEV_ETH) == 0)
        return DevE::Eth;
    else if (strcmp(name, NET_DEV_WIFI) == 0)
        return DevE::Wifi;
    else if (strcmp(name, NET_DEV_MOBILE) == 0)
        return DevE::Mobile;
    else
        return DevE::None;
}

bool Net::IsIpv4Addr(const char *addr) {
    bool is_ipv4 = false;
    regex_t reg = {};
    const size_t nmatch = 1;
    regmatch_t pmatch[1] = {};
    const char *pattern = "^((25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9]).){3}(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])$";

    if (addr) {
        regcomp(&reg, pattern, REG_EXTENDED);
        if ((0 == regexec(&reg, addr, nmatch, pmatch, 0))) {
            is_ipv4 = true;
        }
        regfree(&reg);
    }
    return is_ipv4;
}

bool Net::IsIpv6Addr(const char *addr) {
    bool is_ipv6 = false;
    regex_t reg = {};
    regmatch_t pmatch[9] = {};
    const char *pattern = "^\\s*((([0-9A-Fa-f]{1,4}:){7}(([0-9A-Fa-f]{1,4})|:))|(([0-9A-Fa-f]{1,4}:){6}\
       (:|((25[0-5]|2[0-4]\\d|[01]?\\d{1,2})(\\.(25[0-5]|2[0-4]\\d|[01]?\\d{1,2})){3})|\
       (:[0-9A-Fa-f]{1,4})))|(([0-9A-Fa-f]{1,4}:){5}((:((25[0-5]|2[0-4]\\d|[01]?\\d{1,2})\
       (\\.(25[0-5]|2[0-4]\\d|[01]?\\d{1,2})){3})?)|((:[0-9A-Fa-f]{1,4}){1,2})))|\
       (([0-9A-Fa-f]{1,4}:){4}(:[0-9A-Fa-f]{1,4}){0,1}((:((25[0-5]|2[0-4]\\d|[01]?\\d{1,2})\
       (\\.(25[0-5]|2[0-4]\\d|[01]?\\d{1,2})){3})?)|((:[0-9A-Fa-f]{1,4}){1,2})))|\
       (([0-9A-Fa-f]{1,4}:){3}(:[0-9A-Fa-f]{1,4}){0,2}((:((25[0-5]|2[0-4]\\d|[01]?\\d{1,2})\
       (\\.(25[0-5]|2[0-4]\\d|[01]?\\d{1,2})){3})?)|((:[0-9A-Fa-f]{1,4}){1,2})))|\
       (([0-9A-Fa-f]{1,4}:){2}(:[0-9A-Fa-f]{1,4}){0,3}((:((25[0-5]|2[0-4]\\d|[01]?\\d{1,2})\
       (\\.(25[0-5]|2[0-4]\\d|[01]?\\d{1,2})){3})?)|((:[0-9A-Fa-f]{1,4}){1,2})))|\
       (([0-9A-Fa-f]{1,4}:)(:[0-9A-Fa-f]{1,4}){0,4}((:((25[0-5]|2[0-4]\\d|[01]?\\d{1,2})\
       (\\.(25[0-5]|2[0-4]\\d|[01]?\\d{1,2})){3})?)|((:[0-9A-Fa-f]{1,4}){1,2})))|\
       (:(:[0-9A-Fa-f]{1,4}){0,5}((:((25[0-5]|2[0-4]\\d|[01]?\\d{1,2})\
       (\\.(25[0-5]|2[0-4]\\d|[01]?\\d{1,2})){3})?)|((:[0-9A-Fa-f]{1,4}){1,2})))|\
       (((25[0-5]|2[0-4]\\d|[01]?\\d{1,2})(\\.(25[0-5]|2[0-4]\\d|[01]?\\d{1,2})){3})))(%.+)?\\s*$";

    if (addr) {
        if (regcomp(&reg, pattern, REG_EXTENDED | REG_NEWLINE) == 0) {
            if (regexec(&reg, addr, sizeof(pmatch) / sizeof(pmatch[0]), pmatch, 0) == 0) {
                is_ipv6 = true;
            }
        }
        regfree(&reg);
    }
    return is_ipv6;
}

void Net::GetLocalAddr(const char *interface, Net::Addr &addr) {
    auto tmp = addr.dhcp;
    memset(&addr, 0, sizeof(addr));
    addr.dhcp = tmp;
    uv_interface_address_t *info;
    int count;

    uv_interface_addresses(&info, &count);
    int i = count;

    while (i--) {
        uv_interface_address_t iface = info[i];
        if (strcmp(iface.name, interface) != 0)
            continue;
        if (iface.address.address4.sin_family == AF_INET) {
            uv_ip4_name(&iface.address.address4, addr.ip4, Ipv4AddrSize);
            uv_ip4_name(&iface.netmask.netmask4, addr.netmask4, Ipv4AddrSize);

        } else if (iface.address.address4.sin_family == AF_INET6) {
            uv_ip6_name(&iface.address.address6, addr.ip6, Ipv6AddrSize);
            //drop the scope:link ipv6 addr
            if (strncmp(addr.ip6, "fe80", 4) == 0) {
                memset(addr.ip6, 0, Ipv6AddrSize);
            } else {
                uv_ip6_name(&iface.netmask.netmask6, addr.netmask6, Ipv6AddrSize);
            }
        }
        snprintf(addr.mac, MacAddrSize, "%02X:%02X:%02X:%02X:%02X:%02X",
                 iface.phys_addr[0], iface.phys_addr[1], iface.phys_addr[2],
                 iface.phys_addr[3], iface.phys_addr[4], iface.phys_addr[5]);
    }
    uv_free_interface_addresses(info, count);
}

//void NetDns::Param::Serialize(std::string &out) const{
//    out.append(manual, sizeof(manual));
//    int num = (int) serverList.size();
//    out.append((char *) &num, sizeof(num));
//    for (auto &e : serverList) {
//        out.append((char *) &e, sizeof(e));
//    }
//    num = (int) appendList.size();
//    out.append((char *) &num, sizeof(num));
//    for (auto &e : appendList) {
//        out.append((char *) &e, sizeof(e));
//    }
//}
//
//void NetDns::Param::Parse(const char *data, int size) {
//    int off = 0;
//    memcpy(&manual, data + off, sizeof(manual));
//    off += sizeof(manual);
//    int num = 0;
//    memcpy(&num, data + off, sizeof(num));
//    off += sizeof(num);
//    serverList.clear();
//    for (int i = 0; i < num; i++) {
//        Addr item = {};
//        memcpy(&item, data + off, sizeof(item));
//        off += sizeof(item);
//        serverList.emplace_back(item);
//    }
//    memcpy(&num, data + off, sizeof(num));
//    off += sizeof(num);
//    appendList.clear();
//    for (int i = 0; i < num; i++) {
//        Addr item = {};
//        memcpy(&item, data + off, sizeof(item));
//        off += sizeof(item);
//        appendList.emplace_back(item);
//    }
//}

void NetDns::Param::Serialize(Json::Value &json) const{
    json["manual"] = manual;
    json["server"].resize(0);
    for (auto &e:server) {
        json["server"].append(e.ip);
    }
    json["append"].resize(0);
    for (auto &e:append) {
        json["append"].append(e.ip);
    }
}

void NetDns::Param::Parse(const Json::Value &json) {
    manual = json["manual"].asBool();
    server.clear();
    for (auto &e:json["server"]) {
        Addr addr = {};
        strncpy(addr.ip, e.asCString(), sizeof(addr.ip));
        server.emplace_back(addr);
    }
    append.clear();
    for (auto &e:json["append"]) {
        Addr addr = {};
        strncpy(addr.ip, e.asCString(), sizeof(addr.ip));
        append.emplace_back(addr);
    }
}
