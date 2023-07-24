//
// Created by xiong on 2022/6/24.
//

#include "ThreadInvoke.hpp"

using namespace Emx;


ThreadInvoke::Req::Req(const char *respId) {
    SocketUnDgram::MakeAddr(m_addr, respId);
    if (m_sock.Open() != ErrCodeE::Success) {
        emxloge("open sock failed\n");
        return;
    }
    if (m_sock.SetSendTimeOut(5000) != ErrCodeE::Success ||
        m_sock.SetRecvTimeOut(5000) != ErrCodeE::Success) {
        emxloge("set send/recv timeout failed\n");
        return;
    }
    sockaddr_un un = {};
    un.sun_family = AF_UNIX;
    sprintf(un.sun_path, "%sC", respId);
    unlink(un.sun_path);
    if (m_sock.Bind((sockaddr *) &un, sizeof(un)) != ErrCodeE::Success) {
        emxloge("bind failed\n");
    }
}

ThreadInvoke::Req::~Req() {
    m_sock.Close();
}

ErrCodeE ThreadInvoke::Req::Send(ThreadInvoke::Packet &packet) {
    sem_t sem;
    sem_init(&sem, 0, 0);
    packet.sem = &sem;
    if (m_sock.Send((const char *) (&packet),
                    sizeof(Packet),
                    (sockaddr *) &m_addr, sizeof(m_addr),
                    5000) != ErrCodeE::Success) {
        emxloge("send error\n");
        sem_destroy(&sem);
        return ErrCodeE::Failure;
    }
    sem_wait(&sem);
    sem_destroy(&sem);
    return ErrCodeE::Success;
}

void ThreadInvoke::Resp::Start(const char *respId, ThreadInvoke::Resp::Callback cb) {
    if (m_started)
        return;
    sockaddr_un addr = {};
    memset(&addr, 0, sizeof(sockaddr_un));
    addr.sun_family = AF_UNIX;
    sprintf(addr.sun_path, "%s", respId);
    m_sock = socket(AF_UNIX, SOCK_DGRAM, 0);
    unlink(addr.sun_path);
    if (bind(m_sock, (struct sockaddr *) &addr, sizeof(sockaddr_un)) < 0) {
        emxloge("bind socket failed with %s\n", strerror(errno));
        close(m_sock);
        m_sock = -1;
        return;
    }
    m_cb = std::move(cb);
    m_udp.Create(m_loop);
    m_udp.OpenExistSock(m_sock);
    m_udp.StartRecv(m_buffer, sizeof(m_buffer),
                    [this](ErrCodeE e, const char *data, int32_t size,
                           const struct sockaddr *addr) {
                        Packet packet = {};
                        memcpy(&packet, data, sizeof(Packet));
                        m_cb(packet);
                        sem_post(packet.sem);
                    });
    m_started = true;
}

void ThreadInvoke::Resp::Stop() {
    if (!m_started)
        return;
    m_udp.Destroy();
    m_started = false;
}