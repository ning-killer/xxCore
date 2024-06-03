//
// Created by xiong on 2020/10/23.
//
#include <atomic>
#include <unistd.h>
#include "core/EmxUtils.hpp"
#include "core/EmxLog.hpp"
#include "core/msg/MsgTopic.hpp"
#include "MsgInner.hpp"

using namespace Emx;

static std::atomic<uint32_t> g_topicId;
static uint32_t g_pid = getpid();

ErrCodeE MsgTopicPublisherAsync::CreateMsgTopicPublisherAsync(const Config &cfg) {
    m_cbConnected = cfg.OnConnected;
    m_cbDisConnected = cfg.OnDisConnected;
    char name[EMX_MAX_PATH_SIZE];
    snprintf(name, EMX_MAX_PATH_SIZE, "%s%u_%u_%u", TOPIC_PUB_DOMAIN_PREFIX, Time::GetS(), g_pid, g_topicId++);
    return Create(*cfg.loop, nullptr, 0, TOPIC_SERVER_DOMAIN_PATH, 0, name);
}

ErrCodeE MsgTopicPublisherAsync::Publish(uint32_t topic, const char *data, int32_t size) {
    if (!m_connected) {
        return ErrCodeE::ResNotAvailable;
    }
    auto s = std::make_shared<std::string>();
    MsgTopic::Transfer transfer = {};
    transfer.header.sync = MSG_SYNC_ID;
    transfer.header.msgType = MsgTopic::MsgTypeE::Transfer;
    transfer.header.size = size + sizeof(topic);
    transfer.topic = topic;
    s->assign((char *) &transfer, sizeof(transfer));
    s->append(data, size);
    return Send(s);
}

ErrCodeE MsgTopicPublisherSync::Publish(uint32_t topic, const char *data, int32_t size, int32_t timeoutMs) {
    auto timeoutPoint = Time::GetMs64() + timeoutMs;
    SocketUnStream sock;
    ErrCodeE e = sock.Open();
    if (e != ErrCodeE::Success) {
        return e;
    }
    sockaddr_un un = {};
    char name[EMX_MAX_PATH_SIZE];
    snprintf(name, EMX_MAX_PATH_SIZE, "%s%u_%u_%u", TOPIC_PUB_DOMAIN_PREFIX, Time::GetS(), g_pid, g_topicId++);
    SocketUnStream::MakeAddr(un, name);
    unlink(name);
    e = sock.Bind((sockaddr *) &un, sizeof(un));
    if (e != ErrCodeE::Success) {
        return e;
    }
    SocketUnStream::MakeAddr(un, TOPIC_SERVER_DOMAIN_PATH);
    e = sock.Connect((sockaddr *) &un, sizeof(un), timeoutMs);
    if (e != ErrCodeE::Success) {
        return e;
    }
    if ((timeoutMs = timeoutPoint - Time::GetMs64()) < 0) {
        emxloge("timeout\n");
        return ErrCodeE::Timeout;
    }
    std::string s;
    MsgTopic::Transfer transfer = {};
    transfer.header.sync = MSG_SYNC_ID;
    transfer.header.msgType = MsgTopic::MsgTypeE::Transfer;
    transfer.header.size = size + sizeof(topic);
    transfer.topic = topic;
    s.assign((char *) &transfer, sizeof(transfer));
    s.append(data, size);
    e = sock.Send(s.data(), s.size(), timeoutMs);
    sock.Close();
    unlink(name);
    return e;
}

ErrCodeE MsgTopicSubscriberAsync::CreateMsgTopicSubscriberAsync(const Config &cfg) {
    m_cbConnected = cfg.OnConnected;
    m_cbDisConnected = cfg.OnDisConnected;
    m_cbRecvMsg = cfg.OnRecvMsg;
    MsgTopic::SubRegister reg = {};
    reg.header.sync = MSG_SYNC_ID;
    reg.header.msgType = MsgTopic::MsgTypeE::Register;
    reg.header.size = sizeof(MsgTopic::SubRegister) - sizeof(MsgTopic::Header);
    reg.sendByPack = cfg.sendByPack;
    reg.maxSend = cfg.maxSend;
    for (int i = 0; i < MsgTopic::MaxTopicNum && i < (int) cfg.topicArray.size(); i++)
        reg.topicArray[i] = cfg.topicArray[i];
    m_reg = std::make_shared<std::string>();
    m_reg->resize(sizeof(MsgTopic::SubRegister));
    memcpy((char *) m_reg->data(), (char *) &reg, sizeof(MsgTopic::SubRegister));
    char name[EMX_MAX_PATH_SIZE];
    snprintf(name, EMX_MAX_PATH_SIZE, "%s%u_%u_%u", TOPIC_SUB_DOMAIN_PREFIX, Time::GetS(), g_pid, g_topicId++);
    return Create(*cfg.loop, cfg.buffer, cfg.size, TOPIC_SERVER_DOMAIN_PATH, 0, name);
}

void MsgTopicSubscriberAsync::OnConnect() {
    Send(m_reg);
    m_reg.reset();
    if (m_cbConnected)m_cbConnected();
}

void MsgTopicSubscriberAsync::OnRecv(const char *data, int32_t size) {
    m_data.append(data, size);
    while (m_data.size() >= m_waitSize) {
        if (m_waitHeader) {
            memcpy(&m_header, m_data.data(), sizeof(MsgTopic::Header));
            if (m_header.sync != MSG_SYNC_ID) {
                emxlogc("miss sync\n");
                Destroy();
                return;
            }
            m_waitSize = m_header.size + sizeof(MsgTopic::Header);
            m_waitHeader = false;
            continue;
        }
        MsgTopic::Transfer transfer = {};
        memcpy(&transfer, m_data.data(), sizeof(MsgTopic::Transfer));
        if (m_cbRecvMsg)
            m_cbRecvMsg(transfer.topic, m_data.data() + sizeof(MsgTopic::Transfer),
                        m_waitSize - sizeof(MsgTopic::Transfer));
        memcpy((void *) m_data.data(), m_data.data() + m_waitSize, m_data.size() - m_waitSize);
        m_data.resize(m_data.size() - m_waitSize);
        m_waitSize = sizeof(MsgTopic::Header);
        m_waitHeader = true;
    }
}


ErrCodeE MsgTopicSubscriberSync::Connect(const MsgTopicSubscriberSync::Config &cfg, int32_t timeoutMs) {
    auto timeoutPoint = Time::GetMs64() + timeoutMs;
    ErrCodeE e = m_sock.Open();
    if (e != ErrCodeE::Success) {
        return e;
    }
    sockaddr_un un = {};
    snprintf(m_path, EMX_MAX_PATH_SIZE, "%s%u_%u_%u", TOPIC_SUB_DOMAIN_PREFIX, Time::GetS(), g_pid, g_topicId++);

    SocketUnStream::MakeAddr(un, m_path);
    unlink(m_path);
    e = m_sock.Bind((sockaddr *) &un, sizeof(un));
    if (e != ErrCodeE::Success) {
        return e;
    }
    SocketUnStream::MakeAddr(un, TOPIC_SERVER_DOMAIN_PATH);
    e = m_sock.Connect((sockaddr *) &un, sizeof(un), timeoutMs);
    if (e != ErrCodeE::Success) {
        return e;
    }
    if ((timeoutMs = timeoutPoint - Time::GetMs64()) < 0) {
        emxloge("timeout\n");
        return ErrCodeE::Timeout;
    }
    MsgTopic::SubRegister reg = {};
    reg.header.sync = MSG_SYNC_ID;
    reg.header.msgType = MsgTopic::MsgTypeE::Register;
    reg.header.size = sizeof(MsgTopic::SubRegister) - sizeof(MsgTopic::Header);
    reg.sendByPack = cfg.sendByPack;
    reg.maxSend = cfg.maxSend;
    for (int i = 0; i < MsgTopic::MaxTopicNum && i < (int) cfg.topicArray.size(); i++)
        reg.topicArray[i] = cfg.topicArray[i];
    return m_sock.Send((const char *) &reg, sizeof(reg), timeoutMs);
}

ErrCodeE MsgTopicSubscriberSync::GetMsg(uint32_t &topic, std::string &data, int32_t timeoutMs) {
    auto timeoutPoint = Time::GetMs64() + timeoutMs;
    MsgTopic::Transfer transfer = {};
    ErrCodeE e = m_sock.Recv((char *) &transfer, sizeof(transfer), timeoutMs);
    if (e != ErrCodeE::Success) {
        return e;
    }
    if (transfer.header.sync != MSG_SYNC_ID) {
        emxloge("miss sync\n");
        return ErrCodeE::ParseFailed;
    }
    if ((timeoutMs = timeoutPoint - Time::GetMs64()) < 0) {
        emxloge("timeout\n");
        return ErrCodeE::Timeout;
    }
    topic = transfer.topic;
    data.resize(transfer.header.size - sizeof(transfer.topic));
    e = m_sock.Recv((char *) data.data(), data.size(), timeoutMs);
    return e;
}

void MsgTopicSubscriberSync::DisConnect() {
    m_sock.Close();
    unlink(m_path);
}
