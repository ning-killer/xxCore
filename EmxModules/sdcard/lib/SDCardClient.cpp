//
// Created by xiong on 2021/6/1.
//
#include "SDCardClient.hpp"
#include "SDCardInner.hpp"

using namespace Emx;

int SDCardClient::GetChnNum() {
    int chn = 0;
    std::string resp;
    ErrCodeE e = MsgRpcReqSync::Request(SDCARD_RESP_ID, SDCARD_MODULE, (uint32_t) SDCardMethodE::GetChnNum,
                                        (char *) &chn, sizeof(chn), &resp);
    if (e != ErrCodeE::Success) {
        emxloge("failed:%d\n", e);
        return -1;
    }
    int chnNum;
    memcpy(&chnNum, resp.data(), sizeof(chnNum));
    return chnNum;
}

ErrCodeE SDCardClient::Mount() {
    ErrCodeE e = MsgRpcReqSync::Request(SDCARD_RESP_ID, SDCARD_MODULE, (uint32_t) SDCardMethodE::Mount,
                                        (char *) &m_chn, sizeof(m_chn));
    if (e != ErrCodeE::Success) {
        emxloge("failed:%d\n", e);
    }
    return e;
}

ErrCodeE SDCardClient::UnMount() {
    ErrCodeE e = MsgRpcReqSync::Request(SDCARD_RESP_ID, SDCARD_MODULE, (uint32_t) SDCardMethodE::UnMount,
                                        (char *) &m_chn, sizeof(m_chn));
    if (e != ErrCodeE::Success) {
        emxloge("failed:%d\n", e);
    }
    return e;
}

ErrCodeE SDCardClient::Format() {
    ErrCodeE e = MsgRpcReqSync::Request(SDCARD_RESP_ID, SDCARD_MODULE, (uint32_t) SDCardMethodE::Format,
                                        (char *) &m_chn, sizeof(m_chn));
    if (e != ErrCodeE::Success) {
        emxloge("failed:%d\n", e);
    }
    return e;
}

ErrCodeE SDCardClient::GetInfo(SDCard::Info &info) {
    std::string resp;
    ErrCodeE e = MsgRpcReqSync::Request(SDCARD_RESP_ID, SDCARD_MODULE, (uint32_t) SDCardMethodE::GetInfo,
                                        (char *) &m_chn, sizeof(m_chn), &resp);
    if (e != ErrCodeE::Success) {
        emxloge("failed:%d\n", e);
    } else {
        memcpy(&info, resp.data(), sizeof(info));
    }
    return e;
}

ErrCodeE SDCardClient::Check() {
    ErrCodeE e = MsgRpcReqSync::Request(SDCARD_RESP_ID, SDCARD_MODULE, (uint32_t) SDCardMethodE::Check,
                                        (char *) &m_chn, sizeof(m_chn));
    if (e != ErrCodeE::Success) {
        emxloge("failed:%d\n", e);
    }
    return e;
}

void SDCardClientEvent::Create() {
    MsgTopicSubscriberAsync::Config cfg = {};
    cfg.loop = &m_loop;
    cfg.sendByPack = true;
    cfg.maxSend = 64;
    cfg.topicArray.emplace_back(SDCARD_TOPIC_ID);
    cfg.buffer = m_buffer;
    cfg.size = m_size;
    cfg.OnRecvMsg = std::bind(&SDCardClientEvent::OnMsgRecv, this, ph_1, ph_2, ph_3);
    cfg.OnConnected = [this]() {
        int num = SDCardClient::GetChnNum();
        for (int i = 0; i < num; i++) {
            SDCardClient sdcard(i);
            SDCard::Info info = {};
            if (sdcard.GetInfo(info) == ErrCodeE::Success) {
                m_cb(i, info);
            }
        }
    };
    m_sub.CreateMsgTopicSubscriberAsync(cfg);
}

void SDCardClientEvent::Destroy() {
    m_sub.DestroyMsgTopicSubscriberAsync();
}

void SDCardClientEvent::OnMsgRecv(uint32_t topic, const char *data, int32_t size) {
    int chn;
    memcpy(&chn, data, sizeof(chn));

    SDCard::Info info = {};
    memcpy(&info, data + sizeof(chn), sizeof(info));

    m_cb(chn, info);
}
