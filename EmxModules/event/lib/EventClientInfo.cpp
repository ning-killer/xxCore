/*
* @Author: xiong
* @Date: 2023/2/16
*/

#include "EventClientInfo.hpp"
#include "EventInner.hpp"
using namespace Emx;


ErrCodeE EventClientInfo::Start(EuvLoop &loop, char *buffer, int bufferSize,
                                   EventClientInfo::OnGetInfoCallback cb) {
    if (m_started)
        return ErrCodeE::Success;
    m_cb = std::move(cb);
    MsgTopicSubscriberAsync::Config cfg = {};
    cfg.loop = &loop;
    cfg.sendByPack = true;
    cfg.maxSend = 1024;
    cfg.topicArray.emplace_back(EVENT_TOPIC_ID);
    cfg.buffer = buffer;
    cfg.size = bufferSize;
    cfg.OnConnected = [this]() {
        emxlogd("EventClientInfo %s OnConnected\n", m_sub.GetSockName().c_str());
    };
    cfg.OnDisConnected = [this]() {
        emxlogd("EventClientInfo %s OnDisConnected\n", m_sub.GetSockName().c_str());
    };
    cfg.OnRecvMsg = std::bind(&EventClientInfo::OnMsgRecv, this, ph_1, ph_2, ph_3);
    m_sub.CreateMsgTopicSubscriberAsync(cfg);
    m_started = true;
    return ErrCodeE::Success;
}

void EventClientInfo::Stop() {
    if (m_started) {
        m_sub.DestroyMsgTopicSubscriberAsync();
        m_started = false;
    }
}

void EventClientInfo::OnMsgRecv(uint32_t topic, const char *data, int32_t size) {
    if (size != sizeof(Event::Info))
        return;
    if (m_cb)
        m_cb(*(Event::Info *) data);
}