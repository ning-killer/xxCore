//
// Created by xiong on 2022/6/6.
//
#include <netinet/in.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/if.h>
#include "NetServerLinkPhyEth.hpp"

using namespace Emx;

void NetServerLinkPhyEth::Create(NetServerLinkPhyEth::OnPhyChanged cb) {
    m_cb = std::move(cb);
    sockaddr_nl addr = {};
    m_sock = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (m_sock <= 0) {
        emxloge("socket error %d:%s\n", errno, strerror(errno));
        return;
    }
    addr.nl_family = AF_NETLINK;
    addr.nl_groups = RTNLGRP_LINK;
    if (bind(m_sock, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
        emxloge("bind error %d:%s\n", errno, strerror(errno));
        close(m_sock);
        m_sock = -1;
        return;
    }
    m_poll.Init(m_res.loop, m_sock);
    m_poll.Start([this]() {
        int n = read(m_sock, m_res.buffer, m_res.bufferSize);
        if (n > 0) {
            OnCheck(m_res.buffer, n);
        } else {
            emxlogw("monitor:n=%d\n", n);
        }
    }, nullptr, []() {
        emxlogd("cmd OnDisconnect\n");
    });
    InitPhyStat();
}

void NetServerLinkPhyEth::Destroy() {
    m_poll.Stop();
    close(m_sock);
    m_sock = -1;
}

void NetServerLinkPhyEth::OnCheck(char *buffer, int size) {
    struct nlmsghdr *nh;
    struct ifinfomsg *if_info;
    struct rtattr *attr;
    for (nh = (struct nlmsghdr *) buffer; NLMSG_OK(nh, size); nh = NLMSG_NEXT(nh, size)) {
        if (nh->nlmsg_type != RTM_NEWLINK)
            continue;
        if_info = (ifinfomsg *) NLMSG_DATA(nh);
        bool connected = (if_info->ifi_flags & IFF_LOWER_UP);
        attr = (struct rtattr *) (((char *) nh) + NLMSG_SPACE(sizeof(*if_info)));
        int len = nh->nlmsg_len - NLMSG_SPACE(sizeof(*if_info));
        for (; RTA_OK(attr, len); attr = RTA_NEXT(attr, len)) {
            if (attr->rta_type == IFLA_IFNAME &&
                strcmp((char *) RTA_DATA(attr), m_interface) == 0) {
                emxlogi("%u:%s %s\n", if_info->ifi_index, m_interface, connected ? "up" : "down");
                if (m_connected != connected) {
                    m_connected = connected;
                    m_cb(m_connected, nullptr);
                }
                break;
            }
        }
    }
}
