/*
* @Author: xiong
* @Date: 2022/10/12
*/

#include "NightVisionClient.hpp"
#include "NightVisionInner.hpp"

using namespace Emx;

ErrCodeE NightVisionClient::SetParam(const NightVision::Param &param) {
    std::string rep;
    ErrCodeE e = MsgRpcReqSync::Request(NIGHT_VISION_RESP_ID,
                                        NIGHT_VISION_MODULE,
                                        (uint32_t) NightVisionMethodE::SetParam,
                                        (char *) &param, (int) sizeof(param),
                                        &(rep), 2000);
    if (e != ErrCodeE::Success) {
        emxloge("failed\n");
        return e;
    }
    return e;
}

ErrCodeE NightVisionClient::GetParam(NightVision::Param &param) {
    std::string rep;
    ErrCodeE e = MsgRpcReqSync::Request(NIGHT_VISION_RESP_ID,
                                        NIGHT_VISION_MODULE,
                                        (uint32_t) NightVisionMethodE::GetParam,
                                        nullptr, 0,
                                        &(rep), 2000);
    if (e != ErrCodeE::Success) {
        emxloge("failed\n");
        return e;
    }
    memcpy(&param, rep.data(), sizeof(param));
    return e;
}

ErrCodeE NightVisionClient::SetParamTemp(const NightVision::Param &param) {
    std::string rep;
    ErrCodeE e = MsgRpcReqSync::Request(NIGHT_VISION_RESP_ID,
                                        NIGHT_VISION_MODULE,
                                        (uint32_t) NightVisionMethodE::SetParamTemp,
                                        (char *) &param, (int) sizeof(param),
                                        &(rep), 2000);
    if (e != ErrCodeE::Success) {
        emxloge("failed\n");
        return e;
    }
    return e;
}

ErrCodeE NightVisionClient::GetParamTemp(NightVision::Param &param) {
    std::string rep;
    ErrCodeE e = MsgRpcReqSync::Request(NIGHT_VISION_RESP_ID,
                                        NIGHT_VISION_MODULE,
                                        (uint32_t) NightVisionMethodE::GetParamTemp,
                                        nullptr, 0,
                                        &(rep), 2000);
    if (e != ErrCodeE::Success) {
        emxloge("failed\n");
        return e;
    }
    memcpy(&param, rep.data(), sizeof(param));
    return e;
}

ErrCodeE NightVisionClient::GetStatus(NightVision::Status &status) {
    std::string rep;
    ErrCodeE e = MsgRpcReqSync::Request(NIGHT_VISION_RESP_ID,
                                        NIGHT_VISION_MODULE,
                                        (uint32_t) NightVisionMethodE::GetStatus,
                                        nullptr, 0,
                                        &(rep), 2000);
    if (e != ErrCodeE::Success) {
        emxloge("failed\n");
        return e;
    }
    memcpy(&status, rep.data(), sizeof(status));
    return e;
}

ErrCodeE NightVisionClientInfo::Start(EuvLoop &loop, char *buffer, int bufferSize, OnGetInfoCallback cb) {
    if (m_started)
        return ErrCodeE::Success;
    m_cb = std::move(cb);
    MsgTopicSubscriberAsync::Config cfg = {};
    cfg.loop = &loop;
    cfg.sendByPack = true;
    cfg.maxSend = 1024;
    cfg.topicArray.emplace_back(NightVisionTopicId);
    cfg.buffer = buffer;
    cfg.size = bufferSize;
    cfg.OnConnected = [this]() {
        emxlogd("NightVisionClientInfo %s OnConnected\n", m_sub.GetSockName().c_str());
        NightVisionClient night;
        NightVision::Status status = {};
        night.GetStatus(status);
        m_cb(status);
    };
    cfg.OnDisConnected = [this]() {
        emxlogd("NightVisionClientInfo %s OnDisConnected\n", m_sub.GetSockName().c_str());
    };
    cfg.OnRecvMsg = std::bind(&NightVisionClientInfo::OnMsgRecv, this, ph_1, ph_2, ph_3);
    m_sub.CreateMsgTopicSubscriberAsync(cfg);
    m_started = true;
    return ErrCodeE::Success;
}

void NightVisionClientInfo::Stop() {
    if (m_started) {
        m_sub.DestroyMsgTopicSubscriberAsync();
        m_started = false;
    }
}

void NightVisionClientInfo::OnMsgRecv(uint32_t topic, const char *data, int32_t size) {
    if (size != sizeof(NightVision::Status))
        return;
    if (m_cb)
        m_cb(*(NightVision::Status *) data);
}
