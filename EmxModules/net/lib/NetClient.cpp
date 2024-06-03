//
// Created by xiong on 2021/3/11.
//
#include "EmxCore.hpp"
#include "NetClient.hpp"
#include "NetInner.hpp"
#include <utility>

using namespace Emx;

void NetClientEvent::Create(EuvLoop &loop, char *buffer, int size,
                            Net::LinkCallback cbLink, OnConnected cbConnected) {
    if (m_inited)
        return;
    m_cbLink = std::move(cbLink);
    MsgTopicSubscriberAsync::Config cfg = {};
    cfg.loop = &loop;
    cfg.sendByPack = true;
    cfg.maxSend = 64;
    cfg.topicArray.emplace_back(NET_SERVER_TOPIC_LINK);
    cfg.buffer = buffer;
    cfg.size = size;
    cfg.OnConnected = std::move(cbConnected);
    cfg.OnDisConnected = []() { emxlogd("NetClientEvent OnDisConnected\n"); };
    cfg.OnRecvMsg = std::bind(&NetClientEvent::OnMsgRecv, this, ph_1, ph_2, ph_3);
    m_sub.CreateMsgTopicSubscriberAsync(cfg);
    m_inited = true;
}

void NetClientEvent::Destroy() {
    if (m_inited) {
        m_inited = false;
        m_sub.DestroyMsgTopicSubscriberAsync();
    }
}

void NetClientEvent::OnMsgRecv(uint32_t topic, const char *data, int32_t size) {
    Net::LinkMsg msg = {};
    memcpy(&msg, data, sizeof(msg));
    m_cbLink(msg);
}

ErrCodeE NetMainDev::Set(Net::DevE dev) {
    std::string resp;
    ErrCodeE e = MsgRpcReqSync::Request(NET_SERVER_RESP_ID,
                                        (uint32_t) Net::DevE::None,
                                        (uint32_t) NetMainMethodE::Set,
                                        (char *) &dev, sizeof(dev), &resp);
    if (e != ErrCodeE::Success) {
        emxloge("failed:%d\n", e);
    }
    return e;
}

ErrCodeE NetMainDev::Get(Net::DevE &dev) {
    std::string resp;
    ErrCodeE e = MsgRpcReqSync::Request(NET_SERVER_RESP_ID,
                                        (uint32_t) Net::DevE::None,
                                        (uint32_t) NetMainMethodE::Get,
                                        nullptr, 0, &resp);
    if (e != ErrCodeE::Success) {
        emxloge("failed:%d\n", e);
        return e;
    }
    memcpy(&dev, resp.data(), sizeof(dev));
    return ErrCodeE::Success;
}

bool NetMainDev::IsSupported(Net::DevE dev) {
    return MsgRpcReqSync::Request(NET_SERVER_RESP_ID,
                                  (uint32_t) Net::DevE::None,
                                  (uint32_t) NetMainMethodE::IsSupported,
                                  (char *) &dev, sizeof(dev), nullptr) == ErrCodeE::Success;

}

const char *NetClient::GetInterface() {
    std::string resp;
    ErrCodeE e = MsgRpcReqSync::Request(NET_SERVER_RESP_ID,
                                        (uint32_t) m_dev,
                                        (uint32_t) NetClientMethodE::GetInterface,
                                        nullptr, 0, &resp);
    if (e != ErrCodeE::Success) {
        emxloge("failed:%d\n", e);
        return m_interface;
    }
    memset(m_interface, 0, InterfaceSize);
    strncpy(m_interface, resp.data(), InterfaceSize);
    return m_interface;
}

ErrCodeE NetClient::SetAddr(Net::Addr &addr) {
    std::string resp;
    ErrCodeE e = MsgRpcReqSync::Request(NET_SERVER_RESP_ID,
                                        (uint32_t) m_dev,
                                        (uint32_t) NetClientMethodE::SetAddr,
                                        (char *) &addr, sizeof(addr), &resp);
    if (e != ErrCodeE::Success) {
        emxloge("failed:%d\n", e);
    }
    return e;
}

ErrCodeE NetClient::SetTempAddr(Net::Addr &addr) {
    std::string resp;
    ErrCodeE e = MsgRpcReqSync::Request(NET_SERVER_RESP_ID,
                                        (uint32_t) m_dev,
                                        (uint32_t) NetClientMethodE::SetTempAddr,
                                        (char *) &addr, sizeof(addr), &resp);
    if (e != ErrCodeE::Success) {
        emxloge("failed:%d\n", e);
    }
    return e;
}

ErrCodeE NetClient::GetAddr(Net::Addr &addr) {
    std::string resp;
    ErrCodeE e = MsgRpcReqSync::Request(NET_SERVER_RESP_ID,
                                        (uint32_t) m_dev,
                                        (uint32_t) NetClientMethodE::GetAddr,
                                        nullptr, 0, &resp);
    if (e != ErrCodeE::Success) {
        emxloge("failed:%d\n", e);
    } else {
        memcpy(&addr, resp.data(), sizeof(addr));
    }
    return e;
}

ErrCodeE NetClient::SetEna(bool ena) {
    std::string resp;
    ErrCodeE e = MsgRpcReqSync::Request(NET_SERVER_RESP_ID,
                                        (uint32_t) m_dev,
                                        (uint32_t) NetClientMethodE::SetEna,
                                        (char *) &ena, sizeof(ena), &resp);
    if (e != ErrCodeE::Success) {
        emxloge("failed:%d\n", e);
    }
    return e;
}

ErrCodeE NetClient::GetEna(bool &ena) {
    std::string resp;
    ErrCodeE e = MsgRpcReqSync::Request(NET_SERVER_RESP_ID,
                                        (uint32_t) m_dev,
                                        (uint32_t) NetClientMethodE::GetEna,
                                        nullptr, 0, &resp);
    if (e != ErrCodeE::Success) {
        emxloge("failed:%d\n", e);
    } else {
        memcpy(&ena, resp.data(), sizeof(ena));
    }
    return e;
}

ErrCodeE NetClient::GetLinkStat(LinkStat &link) {
    std::string resp;
    ErrCodeE e = MsgRpcReqSync::Request(NET_SERVER_RESP_ID,
                                        (uint32_t) m_dev,
                                        (uint32_t) NetClientMethodE::GetLinkStat,
                                        nullptr, 0, &resp);
    if (e != ErrCodeE::Success) {
        emxloge("failed:%d\n", e);
    } else {
        memcpy(&link, resp.data(), sizeof(link));
    }
    return e;
}

ErrCodeE NetClient::ReLink() {
    std::string resp;
    ErrCodeE e = MsgRpcReqSync::Request(NET_SERVER_RESP_ID,
                                        (uint32_t) m_dev,
                                        (uint32_t) NetClientMethodE::ReLink,
                                        nullptr, 0);
    if (e != ErrCodeE::Success) {
        emxloge("failed:%d\n", e);
    }
    return e;
}

ErrCodeE NetClientWifi::SwitchToMode(Wifi::ModeE mode) {
    std::string resp;
    ErrCodeE e = MsgRpcReqSync::Request(NET_SERVER_RESP_ID,
                                        (uint32_t) m_dev,
                                        (uint32_t) NetClientWifiMethodE::SwitchToMode,
                                        (char *) &mode, sizeof(mode), &resp);
    if (e != ErrCodeE::Success) {
        emxloge("failed:%d\n", e);
    }
    return e;
}

ErrCodeE NetClientWifi::SetSTA(const char *ssid, const char *psk) {
    std::string resp;
    char data[MaxSsidSize + MaxPskSize];
    int off = 0;
    strncpy(data + off, ssid, MaxSsidSize);
    off += MaxSsidSize;
    strncpy(data + off, psk, MaxPskSize);
    off += MaxPskSize;
    ErrCodeE e = MsgRpcReqSync::Request(NET_SERVER_RESP_ID,
                                        (uint32_t) m_dev,
                                        (uint32_t) NetClientWifiMethodE::SetSTA,
                                        data,
                                        sizeof(data),
                                        &resp);
    if (e != ErrCodeE::Success) {
        emxloge("failed:%d\n", e);
    }
    return e;
}

ErrCodeE NetClientWifi::SetAP(const char *ssid, const char *psk) {
    std::string resp;
    char data[MaxSsidSize + MaxPskSize];
    int off = 0;
    strncpy(data + off, ssid, MaxSsidSize);
    off += MaxSsidSize;
    strncpy(data + off, psk, MaxPskSize);
    off += MaxPskSize;
    ErrCodeE e = MsgRpcReqSync::Request(NET_SERVER_RESP_ID,
                                        (uint32_t) m_dev,
                                        (uint32_t) NetClientWifiMethodE::SetAP,
                                        data,
                                        sizeof(data),
                                        &resp);
    if (e != ErrCodeE::Success) {
        emxloge("failed:%d\n", e);
    }
    return e;
}

ErrCodeE NetClientWifi::GetAPList(Wifi::ApInfoListPtr &list) {
    std::string resp;
    ErrCodeE e = MsgRpcReqSync::Request(NET_SERVER_RESP_ID,
                                        (uint32_t) m_dev,
                                        (uint32_t) NetClientWifiMethodE::GetAPList,
                                        nullptr, 0, &resp);
    if (e != ErrCodeE::Success) {
        emxloge("failed:%d\n", e);
    } else {
        ApInfo info = {};
        for (int i = 0; i < (int) resp.size(); i += sizeof(ApInfo)) {
            memcpy(&info, resp.data() + i, sizeof(ApInfo));
            list->push_back(info);
        }
    }
    return e;
}

ErrCodeE NetClientWifi::GetInfo(Wifi::Info &info) {
    std::string resp;
    ErrCodeE e = MsgRpcReqSync::Request(NET_SERVER_RESP_ID,
                                        (uint32_t) m_dev,
                                        (uint32_t) NetClientWifiMethodE::GetInfo,
                                        nullptr, 0, &resp);
    if (e != ErrCodeE::Success) {
        emxloge("failed:%d\n", e);
    } else {
        memcpy(&info, resp.data(), sizeof(info));
    }
    return e;
}

ErrCodeE NetClientDns::GetParam(NetDns::Param &param) {
    std::string resp;
    ErrCodeE e = MsgRpcReqSync::Request(NET_SERVER_RESP_ID,
                                        (uint32_t) NET_SERVER_DNS_MODULE_ID,
                                        (uint32_t) NetClientDnsMethodE::GetParam,
                                        nullptr, 0, &resp);
    if (e != ErrCodeE::Success) {
        emxloge("failed:%d\n", e);
    } else {
        Json::Value json;
        EasyJson::Parse(resp.data(), (int) resp.size(), json);
        param.Parse(json);
    }
    return e;
}

ErrCodeE NetClientDns::SetParam(const NetDns::Param &param) {
    std::string req;
    std::string resp;
    Json::Value json;
    param.Serialize(json);
    EasyJson::Serialize(json, req);
    ErrCodeE e = MsgRpcReqSync::Request(NET_SERVER_RESP_ID,
                                        (uint32_t) NET_SERVER_DNS_MODULE_ID,
                                        (uint32_t) NetClientDnsMethodE::SetParam,
                                        req.data(),
                                        (int) req.size(),
                                        &resp);
    if (e != ErrCodeE::Success) {
        emxloge("failed:%d\n", e);
    }
    return e;
}
