//
// Created by xiong on 2022/5/12.
//

#include "json/json.h"
#include "EmxCore.hpp"
#include "media/MediaClientAi.hpp"
#include "media/MediaClientParam.hpp"
#include "MediaInner.hpp"

using namespace Emx;

ErrCodeE MediaClientAi::SetParam(const MediaClientAi::Param &param) {
    MediaClientParam p(m_chn, (uint32_t) MediaModE::Ai);
    return p.SetParam(&param, sizeof(param));
}

ErrCodeE MediaClientAi::GetParam(MediaClientAi::Param &param) {
    MediaClientParam p(m_chn, (uint32_t) MediaModE::Ai);
    return p.GetParam(&param, sizeof(param));
}

ErrCodeE MediaClientAi::GetParamRange(Json::Value &range) {
    MediaClientParam p(m_chn, (uint32_t) MediaModE::Ai);
    return p.GetParamRange(range);
}

ErrCodeE MediaClientAi::GetChnNum(int &num) {
    MediaClientParam p(0, (uint32_t) MediaModE::Ai);
    return p.GetChnNum(num);
}

void MediaClientAiData::OnMsgRecv(uint32_t topic, const char *data, int32_t size) {
    MediaAi::AiInfoHeader info = {};
    memcpy(&info, data, size);
    if (m_cbInfo) {
        m_cbInfo(info);
    }
    if (m_cbData) {
        if(info.size){
            std::string frameData;
            ErrCodeE e = m_ring.Pop(info.off, info.size, info.timestampMs, frameData);
            if (e == ErrCodeE::Success) {
                info.data = (uint8_t *) frameData.data();
                m_cbData(e, info);
            }
        }else{
            m_cbData(ErrCodeE::Success, info);
        }

    }
}

ErrCodeE MediaClientAiData::GetDataByInfo(MediaAi::AiInfoHeader &data) {
    ErrCodeE e = m_ring.Pop(data.off, data.size, data.timestampMs, m_data);
    if (e == ErrCodeE::Success) {
        data.data = (uint8_t *) m_data.data();
    }
    return e;
}

ErrCodeE MediaClientAiDataAsync::Start(MediaClientAiData::OnGetDataCallback cb,
                                       MediaClientAiData::OnGetInfoCallback cbInfo) {
    m_cbData = std::move(cb);
    m_cbInfo = std::move(cbInfo);
    MsgTopicSubscriberAsync::Config cfg = {};
    cfg.loop = &m_loop;
    cfg.sendByPack = true;
    cfg.maxSend = 1024;
    cfg.topicArray.emplace_back(MEDIA_TOPIC_AI);
    cfg.buffer = m_buffer;
    cfg.size = 256;
    cfg.OnConnected = [this]() {
        emxlogd("MediaClientAiDataAsync %s OnConnected\n", m_sub.GetSockName().c_str());
    };
    cfg.OnDisConnected = [this]() {
        emxlogd("MediaClientAiDataAsync %s OnDisConnected\n", m_sub.GetSockName().c_str());
    };
    cfg.OnRecvMsg = std::bind(&MediaClientAiDataAsync::OnMsgRecv, this, ph_1, ph_2, ph_3);
    m_sub.CreateMsgTopicSubscriberAsync(cfg);
    m_ring.Create(MEDIA_RING_BUFFER_KEY);
    return ErrCodeE::Success;
}

void MediaClientAiDataAsync::Stop() {
    m_sub.DestroyMsgTopicSubscriberAsync();
    m_ring.Destroy();
}

ErrCodeE MediaClientAiDataSync::Open(int32_t timeoutMs) {
    MsgTopicSubscriberSync::Config cfg = {};
    cfg.sendByPack = true;
    cfg.maxSend = 1024;
    cfg.topicArray.emplace_back(MEDIA_TOPIC_AI);
    ErrCodeE e = m_sub.Connect(cfg, timeoutMs);
    if (e != ErrCodeE::Success) {
        return e;
    }
    m_ring.Create(MEDIA_RING_BUFFER_KEY);
    emxlogd("MediaClientAiDataSync %s Opened\n", m_sub.GetSockName());
    return ErrCodeE::Success;
}

ErrCodeE MediaClientAiDataSync::GetData(MediaAi::AiInfoHeader &data, int32_t timeoutMs) {
    uint32_t topic;
    ErrCodeE e = m_sub.GetMsg(topic, m_data, timeoutMs);
    if (e != ErrCodeE::Success) {
        return e;
    }
    memcpy(&data, m_data.data(), m_data.size());
    e = m_ring.Pop(data.off, data.size, data.timestampMs, m_data);
    if (e != ErrCodeE::Success) {
        return e;
    }
    data.data = (uint8_t *) m_data.data();
    return ErrCodeE::Success;
}

ErrCodeE MediaClientAiDataSync::GetInfo(MediaAi::AiInfoHeader &data, int32_t timeoutMs) {
    uint32_t topic;
    ErrCodeE e = m_sub.GetMsg(topic, m_data, timeoutMs);
    if (e != ErrCodeE::Success) {
        return e;
    }
    memcpy(&data, m_data.data(), m_data.size());
    return ErrCodeE::Success;
}


void MediaClientAiDataSync::Close() {
    m_sub.DisConnect();
    m_ring.Destroy();
    emxlogd("MediaClientAiDataSync %s Closed\n", m_sub.GetSockName());
}
