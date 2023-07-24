//
// Created by xiong on 2022/6/9.
//

#include "core/utils/Socket.hpp"
#include "core/utils/Time.hpp"
#include "core/EmxLog.hpp"
#include <fcntl.h>

using namespace Emx;


ErrCodeE Socket::Bind(const struct sockaddr *addr, socklen_t addrSize) {
    if (bind(m_sock, addr, addrSize) != 0) {
        emxloge("failed:%d:%s\n", errno, strerror(errno));
        return ErrCodeE::NetworkError;
    }
    return ErrCodeE::Success;
}

ErrCodeE Socket::SetSendTimeOut(int32_t timeoutMs) {
    if (m_sendTimeout != timeoutMs) {
        timeval tv = {timeoutMs / 1000, (timeoutMs % 1000) * 1000};
        if (setsockopt(m_sock, SOL_SOCKET, SO_SNDTIMEO, (char *) &tv, sizeof(struct timeval))) {
            emxloge("getsockopt SO_SNDTIMEO failed:%d:%s\n", errno, strerror(errno));
            return ErrCodeE::NetworkError;
        }
        m_sendTimeout = timeoutMs;
    }
    return ErrCodeE::Success;
}

ErrCodeE Socket::SetRecvTimeOut(int32_t timeoutMs) {
    if (m_recvTimeout != timeoutMs) {
        timeval tv = {timeoutMs / 1000, (timeoutMs % 1000) * 1000};
        if (setsockopt(m_sock, SOL_SOCKET, SO_RCVTIMEO, (char *) &tv, sizeof(struct timeval))) {
            emxloge("getsockopt SO_SNDTIMEO failed:%d:%s\n", errno, strerror(errno));
            return ErrCodeE::NetworkError;
        }
        m_recvTimeout = timeoutMs;
    }
    return ErrCodeE::Success;
}

ErrCodeE SocketStream::Connect(const sockaddr *addr, int32_t addrSize, int32_t timeoutMs) {
    int flags = fcntl(m_sock, F_GETFL, 0);
    fcntl(m_sock, F_SETFL, flags | O_NONBLOCK);
    int ret = connect(m_sock, addr, addrSize);
    if (ret < 0) {
        /* if connect error */
        if (errno != EINPROGRESS) {
            emxloge("connecting failed:%d:%s\n", errno, strerror(errno));
            return ErrCodeE::NetworkError;
        } else {
            fd_set rset, wset;
            FD_ZERO(&rset);
            FD_SET(m_sock, &rset);
            wset = rset;
            timeval tv = {timeoutMs / 1000, (timeoutMs % 1000) * 1000};
            ret = select(m_sock + 1, &rset, &wset, NULL, &tv);
            if (ret <= 0) {
                emxloge("connect timeout!\n");
                return ErrCodeE::Timeout;
            }
            int error = 0;
            socklen_t len = sizeof(error);
            if (getsockopt(m_sock, SOL_SOCKET, SO_ERROR, &error, &len) || error) {
                emxloge("getsockopt SO_ERROR failed:%d:%s\n", errno, strerror(errno));
                return ErrCodeE::NetworkError;
            }
        }
    }
    fcntl(m_sock, F_SETFL, flags & ~O_NONBLOCK);
    return ErrCodeE::Success;
}


ErrCodeE SocketStream::Send(const char *buffer, int32_t size, int32_t timeoutMs) {
    if (m_sock < 0) {
        emxloge("socket is closed\n");
        return ErrCodeE::ResNotAvailable;
    }
    auto beginT = Time::GetMs64();
    int32_t remain = size;
    int32_t off = 0;
    while (timeoutMs > 0 && remain > 0) {
        if (SetSendTimeOut(timeoutMs) != ErrCodeE::Success)
            return ErrCodeE::NetworkError;
        int n = send(m_sock, buffer + off, remain, 0);
        if (n <= 0) {
            emxloge("send failed:%d:%s\n", errno, strerror(errno));
            return ErrCodeE::NetworkError;
        }
        remain -= n;
        off += n;
        timeoutMs -= (int) (Time::GetMs64() - beginT);
    }
    return remain == 0 ? ErrCodeE::Success : ErrCodeE::Timeout;
}

ErrCodeE SocketStream::Recv(char *buffer, int32_t size, int32_t timeoutMs) {
    if (m_sock < 0) {
        emxloge("socket is closed\n");
        return ErrCodeE::ResNotAvailable;
    }
    auto beginT = Time::GetMs64();
    int32_t remain = size;
    int32_t off = 0;
    while (timeoutMs > 0 && remain > 0) {
        if (SetRecvTimeOut(timeoutMs) != ErrCodeE::Success)
            return ErrCodeE::NetworkError;
        int n = recv(m_sock, buffer + off, remain, 0);
        if (n <= 0) {
            emxloge("recv failed:%d:%s\n", errno, strerror(errno));
            return ErrCodeE::NetworkError;
        }
        remain -= n;
        off += n;
        timeoutMs -= Time::GetMs64() - beginT;
    }
    return remain == 0 ? ErrCodeE::Success : ErrCodeE::Timeout;
}


ErrCodeE SocketUnStream::Open() {
    Close();
    m_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (m_sock <= 0) {
        emxloge("failed:%d:%s\n", errno, strerror(errno));
        return ErrCodeE::NetworkError;
    }
    return ErrCodeE::Success;
}

void SocketUnStream::MakeAddr(sockaddr_un &un, const char *path) {
    un.sun_family = AF_UNIX;
    sprintf(un.sun_path, "%s", path);
}

SocketUnStream::~SocketUnStream() {
    if (m_sock > 0) {
        sockaddr_un un = {};
        socklen_t len = sizeof(un);
        if (getsockname(m_sock, (sockaddr *) &un, &len) == 0) {
            unlink(un.sun_path);
        }
    }
}

void SocketUnDgram::MakeAddr(sockaddr_un &un, const char *path) {
    un.sun_family = AF_UNIX;
    sprintf(un.sun_path, "%s", path);
}

ErrCodeE SocketUnDgram::Open() {
    Close();
    m_sock = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (m_sock <= 0) {
        emxloge("failed:%d:%s\n", errno, strerror(errno));
        return ErrCodeE::NetworkError;
    }
    return ErrCodeE::Success;
}

SocketUnDgram::~SocketUnDgram() {
    if (m_sock > 0) {
        sockaddr_un un = {};
        socklen_t len = sizeof(un);
        if (getsockname(m_sock, (sockaddr *) &un, &len) == 0) {
            unlink(un.sun_path);
        }
    }
}

ErrCodeE
SocketDgram::Send(const char *buffer, int32_t size, const sockaddr *addr, socklen_t addrLen, int32_t timeoutMs) {
    if (m_sock < 0) {
        emxloge("socket is closed\n");
        return ErrCodeE::ResNotAvailable;
    }
    if (SetSendTimeOut(timeoutMs) != ErrCodeE::Success)
        return ErrCodeE::NetworkError;
    if (sendto(m_sock, buffer, size, 0, addr, addrLen) < 0) {
        emxloge("send failed:%d:%s\n", errno, strerror(errno));
        return ErrCodeE::NetworkError;
    }
    return ErrCodeE::Success;
}

ErrCodeE SocketDgram::Recv(char *buffer, int32_t size, sockaddr *addr, socklen_t *addrLen, int32_t timeoutMs) {
    if (m_sock < 0) {
        emxloge("socket is closed\n");
        return ErrCodeE::ResNotAvailable;
    }
    if (SetRecvTimeOut(timeoutMs) != ErrCodeE::Success)
        return ErrCodeE::NetworkError;
    int n = recvfrom(m_sock, buffer, size, 0, addr, addrLen);
    if (n <= 0) {
        emxloge("recv failed:%d:%s\n", errno, strerror(errno));
        return ErrCodeE::NetworkError;
    }
    return ErrCodeE::Success;
}
