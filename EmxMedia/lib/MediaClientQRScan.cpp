//
// Created by xiong on 2021/6/25.
//
#include "media/MediaTypedef.hpp"
#include "media/MediaClientQRScan.hpp"
#include <utility>
#include "MediaInner.hpp"
#include "EmxCore.hpp"

using namespace Emx;

ErrCodeE MediaClientQRScan::Start() {
    ErrCodeE e = MsgRpcReqSync::Request(MEDIA_SERVER_MSG_ID,
                                        (uint32_t) MediaModE::QrScan,
                                        (uint32_t) MediaQrScanMethodE::Start);
    if (e != ErrCodeE::Success) {
        emxloge("failed\n");
        return e;
    }
    return e;
}

ErrCodeE MediaClientQRScan::Stop() {
    ErrCodeE e = MsgRpcReqSync::Request(MEDIA_SERVER_MSG_ID,
                                        (uint32_t) MediaModE::QrScan,
                                        (uint32_t) MediaQrScanMethodE::Stop);
    if (e != ErrCodeE::Success) {
        emxloge("failed\n");
        return e;
    }
    return e;
}

ErrCodeE MediaClientQRScan::StartQR(EuvLoop &loop, char *buffer, int size,
                                    MediaClientQRScan::OnGetResult cbRes) {
    if (m_started)
        return ErrCodeE::ResInUsing;
    ErrCodeE e = Start();
    if (e != ErrCodeE::Success)
        return e;
    m_cb = std::move(cbRes);
    MsgTopicSubscriberAsync::Config cfg = {};
    cfg.loop = &loop;
    cfg.sendByPack = true;
    cfg.maxSend = -1;
    cfg.topicArray.emplace_back(MEDIA_TOPIC_QR_SCAN);
    cfg.buffer = buffer;
    cfg.size = size;
    cfg.OnConnected = nullptr;
    cfg.OnDisConnected = nullptr;
    cfg.OnRecvMsg = [this](uint32_t topic, const char *data, int32_t size) {
        if (m_cb) {
            Json::Value json;
            if (EasyJson::Parse(data, json) == ErrCodeE::Success)
                m_cb(json["chn"].asInt(), json["content"].asCString());
        }
    };
    m_sub.CreateMsgTopicSubscriberAsync(cfg);
    m_started = true;
    return ErrCodeE::Success;
}

ErrCodeE MediaClientQRScan::StopQR() {
    if (!m_started)
        return ErrCodeE::ResNotExist;
    m_sub.DestroyMsgTopicSubscriberAsync();
    Stop();
    m_started = false;
    return ErrCodeE::Success;
}
