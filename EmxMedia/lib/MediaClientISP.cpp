//
// Created by xiong on 2021/6/1.
//


#include "json/json.h"
#include "EmxCore.hpp"
#include "media/MediaClientISP.hpp"
#include "media/MediaClientParam.hpp"
#include "MediaInner.hpp"

using namespace Emx;

ErrCodeE MediaClientISP::SetParam(const MediaISP::Param &param) {
    MediaClientParam p(m_chn, (uint32_t) MediaModE::ISP);
    return p.SetParam(&param, sizeof(param));
}

ErrCodeE MediaClientISP::GetParam(MediaISP::Param &param) {
    MediaClientParam p(m_chn, (uint32_t) MediaModE::ISP);
    return p.GetParam(&param, sizeof(param));
}

ErrCodeE MediaClientISP::GetParamRange(Json::Value &range) {
    MediaClientParam p(m_chn, (uint32_t) MediaModE::ISP);
    return p.GetParamRange(range);
}

ErrCodeE MediaClientISP::GetChnNum(int &num) {
    MediaClientParam p(0, (uint32_t) MediaModE::ISP);
    return p.GetChnNum(num);
}

ErrCodeE MediaClientISP::SetRunMode(MediaISP::RunModeE mode) {
    int32_t chn = m_chn;
    char data[sizeof(chn) + sizeof(MediaISP::RunModeE)];
    memcpy(data, &chn, sizeof(chn));
    memcpy(data + sizeof(chn), &mode, sizeof(MediaISP::RunModeE));
    ErrCodeE e = MsgRpcReqSync::Request(MEDIA_SERVER_MSG_ID, (uint32_t) MediaModE::ISP,
                                        (uint32_t) MediaIspMethodE::SetRunMode,
                                        data, sizeof(data));
    if (e != ErrCodeE::Success) {
        emxloge("failed\n");
        return e;
    }
    return e;
}

ErrCodeE MediaClientISP::GetRunMode(MediaISP::RunModeE &mode) {
    std::string rep;
    int32_t chn = 0;
    char dataReq[sizeof(chn)];
    memcpy(dataReq, &chn, sizeof(chn));
    ErrCodeE e = MsgRpcReqSync::Request(MEDIA_SERVER_MSG_ID,
                                        (uint32_t) MediaModE::ISP,
                                        (uint32_t) MediaIspMethodE::GetRunMode,
                                        dataReq, (int) sizeof(dataReq),
                                        &(rep));
    if (e != ErrCodeE::Success) {
        emxloge("failed\n");
        return e;
    }
    memcpy(&mode, rep.data(), sizeof(mode));
    return e;
}

// ErrCodeE MediaClientISP::SetGrey(bool grey) {
//     int32_t chn = m_chn;
//     char data[sizeof(chn) + sizeof(MediaISP::RunModeE)];
//     memcpy(data, &chn, sizeof(chn));
//     memcpy(data + sizeof(chn), &grey, sizeof(grey));
//     ErrCodeE e = MsgRpcReqSync::Request(MEDIA_SERVER_MSG_ID, (uint32_t) MediaMod::ISP,
//                                         (uint32_t) MediaIspMethod::SetGrey,
//                                         data, sizeof(data));
//     if (e != ErrCodeE::SUCCESS) {
//         emxloge("failed\n");
//         return e;
//     }
//     return e;
// }

ErrCodeE MediaClientISPInfo::Start(EuvLoop &loop, char *buffer, int bufferSize,
                                   MediaClientISPInfo::OnGetInfoCallback cb) {
    if (m_started)
        return ErrCodeE::Success;
    m_cb = std::move(cb);
    MsgTopicSubscriberAsync::Config cfg = {};
    cfg.loop = &loop;
    cfg.sendByPack = true;
    cfg.maxSend = 1024;
    cfg.topicArray.emplace_back(MEDIA_TOPIC_ISP_INFO);
    cfg.buffer = buffer;
    cfg.size = bufferSize;
    cfg.OnConnected = [this]() {
        emxlogd("MediaClientISPInfo %s OnConnected\n", m_sub.GetSockName().c_str());
    };
    cfg.OnDisConnected = [this]() {
        emxlogd("MediaClientISPInfo %s OnDisConnected\n", m_sub.GetSockName().c_str());
    };
    cfg.OnRecvMsg = std::bind(&MediaClientISPInfo::OnMsgRecv, this, ph_1, ph_2, ph_3);
    m_sub.CreateMsgTopicSubscriberAsync(cfg);
    m_started = true;
    return ErrCodeE::Success;
}

void MediaClientISPInfo::Stop() {
    if (m_started) {
        m_sub.DestroyMsgTopicSubscriberAsync();
        m_started = false;
    }
}

void MediaClientISPInfo::OnMsgRecv(uint32_t topic, const char *data, int32_t size) {
    if (size != sizeof(MediaISP::Info))
        return;
    if (m_cb)
        m_cb(*(MediaISP::Info *) data);
}