//
// Created by xiong on 2020/11/7.
//
#include "NetClient.hpp"
#include "BroadcastService.hpp"
#include "DevInfo.hpp"
#include "Sn.hpp"
#include "Mac.hpp"

using namespace Emx;

void BroadcastService::Create(Resource *res, Json::Value &json) {
    m_res = res;
    m_portHost = json["portHost"].asInt();
    m_portPeer = json["portPeer"].asInt();
    m_udp.Create(m_res->loop);
    sockaddr_in addr = {};
    uv_ip4_addr("0.0.0.0", m_portHost, &addr);
    m_udp.Bind((sockaddr *) &addr, 0);
    m_udp.SetBroadcast(true);
    m_udp.StartRecv(m_res->buffer, m_res->bufferSize,
                    std::bind(&BroadcastService::OnRecv, this, ph_1, ph_2, ph_3, ph_4));
}

void BroadcastService::Destroy() {
    m_udp.Destroy();
}

void BroadcastService::OnRecv(ErrCodeE e, const char *data, int32_t size,
                              const struct sockaddr *addr) {
    if (e != ErrCodeE::Success)
        return;
    char ip[16];
    uv_ip4_name((sockaddr_in *) addr, ip, 16);
    auto header = (Header *) m_res->buffer;
    auto magic = header->magic;
    if (magic[0] != 'E' || magic[1] != 'P' || magic[2] != 'B' || magic[3] != 'P') {
        emxloge("client[%s] magic miss match [%c,%c,%c,%c]\n",
               ip, magic[0], magic[1], magic[2], magic[3]);
        return;
    }
    header->size = ntohl(header->size);
    if (header->size + sizeof(Header) >= (uint32_t) m_res->bufferSize) {
        emxloge("recv boardcast from %s, size = %u error %u\n", ip, ntohl(header->size));
    }
    emxlogi("size:%d\n", size);
    emxlogi("header->size:%d\n", header->size);
    emxlogi("recv boardcast from %s\n", ip);
    char *jsonData = (char *) header + sizeof(Header);
    emxlogd("req[%.*s]\n", header->size, jsonData);
    Json::Value jsonReq;
    if (EasyJson::Parse(jsonData, (int) header->size, jsonReq) != ErrCodeE::Success) {
        emxloge("parse %s failed\n", jsonData);
        return;
    }
    if (!jsonReq.isMember("session")) {
        emxloge("cannot find session\n");
        return;
    }
    if (!jsonReq.isMember("cmd")) {
        emxloge("cannot find cmd\n");
        return;
    }
    Json::Value jsonResp;
    jsonResp["cmd"] = jsonReq["cmd"];
    jsonResp["session"] = jsonReq["session"];
    auto &in = jsonReq["data"];
    auto &out = jsonResp["data"];
    switch ((CmdE) jsonReq["cmd"].asUInt()) {
        case CmdE::Detection:
            Detection(in, out);
            break;
        default:
            emxloge("unsupported cmd %u\n", jsonReq["cmd"].asUInt());
            return;
    }
    std::string respStr;
    EasyJson::Serialize(jsonResp, respStr);
    uint32_t respSize = respStr.size();
    if (respSize >= m_res->bufferSize - sizeof(Header)) {
        emxloge("resp[%s] size(%d) larger than maximum(%d)\n", respStr.data(), respSize,
               m_res->bufferSize - sizeof(Header));
        return;
    }
    emxlogd("resp[%.*s]\n", respSize, respStr.data());
    header->size = htonl(respSize);
    memcpy(m_res->buffer + sizeof(Header), respStr.data(), respSize);
    sockaddr_in addrPeer = {};
    memset(&addrPeer, 0, sizeof(struct sockaddr_in));
    addrPeer.sin_family = AF_INET;
    addrPeer.sin_addr.s_addr = htonl(INADDR_BROADCAST);
    addrPeer.sin_port = htons(m_portPeer);
    m_udp.Send((uint8_t *) m_res->buffer,
               (int) respSize + (int) sizeof(Header),
               (sockaddr *) &addrPeer, sizeof(addrPeer));
}

void BroadcastService::Detection(Json::Value &in, Json::Value &out) {
    Sn sn;
    if (sn.Load() != ErrCodeE::Success)
        return;
    out["sn"] = sn.Get();

    Mac mac;
    if (mac.Load() != ErrCodeE::Success)
        return;
    out["mac"] = mac.Get();

    DevInfo devInfo;
    if (devInfo.Load() != ErrCodeE::Success)
        return;
    out["inner"]["type"] = devInfo.GetInner()->type;
    out["inner"]["version"] = devInfo.GetInner()->version;
    out["outer"]["type"] = devInfo.GetOuter()->type;
    out["outer"]["version"] = devInfo.GetOuter()->version;
    out["hardware"]["type"] = devInfo.GetHardware()->type;
    out["hardware"]["version"] = devInfo.GetHardware()->version;

    auto &outNet = out["net"];
    Net::DevE dev;
    if (NetMainDev::Get(dev) != ErrCodeE::Success)
        return;
    NetClient net(dev);
    outNet["type"] = Net::GetDevName(dev);

    Net::Addr addr = {};
    if (net.GetAddr(addr) != ErrCodeE::Success)
        return;
    outNet["dhcp"] = addr.dhcp;
    outNet["ip4"] = addr.ip4;
    outNet["netmask4"] = addr.netmask4;
    outNet["gateway4"] = addr.gateway4;
    outNet["ip6"] = addr.ip6;
    outNet["netmask6"] = addr.netmask6;
    outNet["gateway6"] = addr.gateway6;
    outNet["mac"] = addr.mac;
}
