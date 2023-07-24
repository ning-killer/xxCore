//
// Created by xiong on 2020/11/7.
//
#include "NetClient.hpp"
#include "DetectionService.hpp"
#include "DevInfo.hpp"
#include "Sn.hpp"

using namespace Emx;

void DetectionService::Create(Json::Value &json) {
    auto &detection = json["detection"];
    m_ena = detection["ena"].asBool();
    if (!m_ena)
        return;
    if (m_started)
        return;
    m_port = detection["port"].asInt();
    m_bufferSize = detection["bufferSizeKB"].asInt() * 1024;
    m_buffer = (char *) malloc(m_bufferSize);
    m_loop.Init("DetectionService", [this]() {
        m_udp.Destroy();
    });
    m_udp.Create(m_loop);
    sockaddr_in addr = {};
    uv_ip4_addr("0.0.0.0", m_port, &addr);
    m_udp.Bind((sockaddr *) &addr, 0);
    m_udp.SetBroadcast(true);
    m_udp.StartRecv(m_buffer, m_bufferSize,
                    std::bind(&DetectionService::OnRecv, this, ph_1, ph_2, ph_3, ph_4));
    m_loop.Start();
    m_started = true;
}

void DetectionService::Destroy() {
    if (!m_ena)
        return;
    if (!m_started)
        return;
    m_loop.StopAndDeInit();
    free(m_buffer);
    m_started = false;
}

void DetectionService::OnRecv(ErrCodeE e, const char *data, int32_t size,
                              const struct sockaddr *addr) {
    if (e != ErrCodeE::Success)
        return;
    char ip[16];
    uv_ip4_name((sockaddr_in *) addr, ip, 16);
    //fill terminal '\0' to json string
    if (size < m_bufferSize)
        m_buffer[size] = 0;
    auto header = (Header *) m_buffer;
    header->cmd = (CmdE) ntohl((uint32_t) header->cmd);
    header->size = ntohl(header->size);
    if (header->size + sizeof(Header) >= (uint32_t) m_bufferSize) {
        emxloge("recv boardcast cmd %#x from %s, size = %u error %u\n", header->cmd, ip, ntohl(header->size));
    }

    if (header->cmd == CmdE::Req) {
        emxlogi("recv boardcast cmd %#x from %s\n", header->cmd, ip);
        OnReq(header);
    }
}

void DetectionService::OnReq(Header *header) {
    char *data = (char *) header + sizeof(Header);
    Json::Value jsonReq;
    if (EasyJson::Parse(data, header->size, jsonReq) != ErrCodeE::Success) {
        emxloge("parse %s failed\n", data);
        return;
    }

    printf("#%s#\n", data);
    if (!jsonReq.isMember("session")) {
        emxloge("cannot find session in #%s#\n", data + sizeof(Header));
        return;
    }

    Json::Value jsonResp;

    //session
    uint32_t session = jsonReq["session"].asUInt();
    jsonResp["session"] = session;
    DevInfo devInfo;
    devInfo.Load();
    //dev info

    jsonResp["dev_type"] = devInfo.GetOuter()->type;
    jsonResp["firmware"] = devInfo.GetOuter()->version;
    jsonResp["hardware"] = devInfo.GetHardware()->version;
    jsonResp["dev_id"] = devInfo.GetInner()->type;
    jsonResp["dev_ver"] = devInfo.GetInner()->version;

    //sn
    Sn sn;
    sn.Load();
    jsonResp["sn"] = sn.Get();

    //net
    Net::Addr addr = {};
    Net::DevE dev;
    NetMainDev::Get(dev);
    NetClient mainDev(dev);
    Net::GetLocalAddr(mainDev.GetInterface(), addr);
    jsonResp["ip"] = addr.ip4;
    jsonResp["ipv6"] = addr.ip6;
    jsonResp["mac"] = addr.mac;

    NetClientWifi::Info wifiInfo = {};
    if(NetMainDev::IsSupported(Net::DevE::Wifi)){
        NetClientWifi wifi;
        wifi.GetInfo(wifiInfo);
    }
    jsonResp["wifi_ssid"] = wifiInfo.ssid;
    jsonResp["wifi_quality"] = wifiInfo.signalLevel;

    std::string respStr;
    EasyJson::Serialize(jsonResp,respStr);
    uint32_t respSize = respStr.size();
    if (respSize >= m_bufferSize - sizeof(Header)) {
        emxloge("resp[%s] size(%d) larger than maximum(%d)\n", respStr.data(), respSize,
               m_bufferSize - sizeof(Header));
        return;
    }
    header->cmd = (CmdE) htonl((uint32_t) CmdE::Resp);
    header->size = htonl(respSize);
    memcpy(m_buffer + sizeof(Header), respStr.data(), respSize);
    sockaddr_in addrPeer = {};
    memset(&addrPeer, 0, sizeof(struct sockaddr_in));
    addrPeer.sin_family = AF_INET;
    addrPeer.sin_addr.s_addr = htonl(INADDR_BROADCAST);
    addrPeer.sin_port = htons(m_port);
    m_udp.Send((uint8_t *) m_buffer,
               (int) respSize + (int) sizeof(Header),
               (sockaddr *) &addrPeer, sizeof(addrPeer));
}
