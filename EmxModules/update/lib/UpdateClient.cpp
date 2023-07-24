/*
 * @Author: xiong
 * @Date: 2022/9/6
*/

#include "UpdateClient.hpp"
#include "UpdateInner.hpp"

using namespace Emx;

ErrCodeE UpdateClient::Create(uint32_t session) {
    if (m_created) {
        emxloge("recreate\n");
        return ErrCodeE::OperationNotSupport;
    }
    m_session = session;
    m_seq = 0;
    sockaddr_un un = {};
    m_buffer = (uint8_t *) malloc(EMX_UPDATE_UDP_BUFFER_SIZE);
    if (!m_buffer) {
        emxloge("malloc failed\n");
        goto EXIT;
    }
    SocketUnDgram::MakeAddr(m_addr, EMX_DOMAIN_UPDATE_SERVER);
    if (m_sock.Open() != ErrCodeE::Success) {
        emxloge("open sock failed\n");
        goto EXIT;
    }

    if (m_sock.SetSendTimeOut(EMX_UPDATE_TIMEOUT) != ErrCodeE::Success ||
        m_sock.SetRecvTimeOut(EMX_UPDATE_TIMEOUT) != ErrCodeE::Success) {
        emxloge("set send/recv timeout failed\n");
        goto EXIT;
    }
    un.sun_family = AF_UNIX;
    sprintf(un.sun_path, "/tmp/UpdateC%d", session);
    unlink(un.sun_path);
    if (m_sock.Bind((sockaddr *) &un, sizeof(un)) != ErrCodeE::Success) {
        goto EXIT;
    }
    m_created = true;
    return ErrCodeE::Success;
    EXIT:
    if (m_buffer)
        free(m_buffer);
    m_sock.Close();
    return ErrCodeE::Failure;
}

ErrCodeE UpdateClient::Push(const uint8_t *data, int32_t size) {
    if (!m_created) {
        emxloge("not created\n");
        return ErrCodeE::ResNotAvailable;
    }
    ErrCodeE e = ErrCodeE::Success;
    UpdatePackHeader header = {};
    header.sync = EMX_UPDATE_PACK_SYNC;
    header.session = m_session;
    int maxSendSize = EMX_UPDATE_UDP_BUFFER_SIZE - sizeof(header);
    int off = 0;
    emxlogt("push total size %d\n", size);
    while (size > 0) {
        int actSize = size > maxSendSize ? maxSendSize : size;
        header.seq = m_seq++;
        memcpy(m_buffer, &header, sizeof(header));
        memcpy(m_buffer + sizeof(header), data + off, actSize);
        emxlogt("push split pack size %d\n", actSize);
        if (m_sock.Send((const char *) (m_buffer),
                        actSize + (int32_t) sizeof(header),
                        (sockaddr *) &m_addr, sizeof(m_addr),
                        EMX_UPDATE_TIMEOUT) != ErrCodeE::Success) {
            emxloge("send error\n");
            return ErrCodeE::NetworkError;
        }
        off += actSize;
        size -= actSize;
        sockaddr_un addr = {};
        socklen_t len = sizeof(addr);
        if (m_sock.Recv((char *) &e, sizeof(e), (sockaddr *) &addr, &len, EMX_UPDATE_TIMEOUT) != ErrCodeE::Success) {
            emxloge("recv error\n");
            return ErrCodeE::NetworkError;
        }
        if (e != ErrCodeE::Success && e != ErrCodeE::ReachTheEof) {
            emxloge("recv error msg\n");
            return e;
        }
    }
    return e;
}


void UpdateClient::Destroy() {
    if (m_created) {
        free(m_buffer);
        m_sock.Close();
        m_created = false;
    }
}


ErrCodeE UpdateEvent::Create(EuvLoop *loop, char *buffer, int size, UpdateEvent::EventCallBack cb) {
    m_cb = std::move(cb);
    MsgTopicSubscriberAsync::Config cfg = {};
    cfg.loop = loop;
    cfg.sendByPack = true;
    cfg.maxSend = 64;
    cfg.topicArray.emplace_back(UPDATE_SERVER_MSG_ID);
    cfg.buffer = buffer;
    cfg.size = size;
    cfg.OnRecvMsg = std::bind(&UpdateEvent::OnMsgRecv, this, ph_1, ph_2, ph_3);
    cfg.OnConnected = nullptr;
    m_sub.CreateMsgTopicSubscriberAsync(cfg);
    return ErrCodeE::Success;
}

void UpdateEvent::Destroy() {
    m_sub.DestroyMsgTopicSubscriberAsync();
}


void UpdateEvent::OnMsgRecv(uint32_t topic, const char *data, int32_t size) {
    if (topic == UPDATE_SERVER_MSG_ID) {
        Event event = {};
        if (size != sizeof(event)) {
            emxloge("data size error need %d actual %d\n", sizeof(event), size);
            return;
        }
        memcpy(&event, data, sizeof(event));
        m_cb(event);
    }
}


