//
// Created by xiong on 2022/6/9.
//

#ifndef EMX_Socket_HPP
#define EMX_Socket_HPP

#include "core/EmxTypeDef.hpp"
#include <sys/socket.h>
#include <sys/un.h>

namespace Emx {

    //!socket操作接口
    class Socket {
    public:
        Socket() {
            m_sock = -1;
            m_sendTimeout = -1;
            m_recvTimeout = -1;
        }

        virtual ~Socket() {
            Close();
        }

        //!打开socket
        virtual ErrCodeE Open() = 0;

        //!关闭socket
        void Close() {
            if (m_sock > 0) {
                close(m_sock);
                m_sock = -1;
            }
        }

        /*!
         * @brief 绑定地址
         * @param addr [in]需要绑定的嗯地址
         * @param addrSize [in]地址空间大小
         * @return ErrCodeE
         */
        ErrCodeE Bind(const struct sockaddr *addr, socklen_t addrSize);

        //!设置发送超时时间
        ErrCodeE SetSendTimeOut(int32_t timeoutMs);

        //!设置接收超时时间
        ErrCodeE SetRecvTimeOut(int32_t timeoutMs);

        int m_sock;
        int m_sendTimeout;
        int m_recvTimeout;
    };

    //!面向连接的socket操作接口
    class SocketStream : public Socket {
    public:
        SocketStream() : Socket() {}

        virtual ~SocketStream() {};

        ErrCodeE Connect(const sockaddr *addr, int32_t addrSize, int32_t timeoutMs);

        ErrCodeE Send(const char *buffer, int32_t size, int32_t timeoutMs);

        ErrCodeE Recv(char *buffer, int32_t size, int32_t timeoutMs);
    };

    //!面相数据包的socket操作接口
    class SocketDgram : public Socket {
    public:
        SocketDgram() : Socket() {}

        virtual ~SocketDgram() {};

        ErrCodeE Send(const char *buffer, int32_t size, const sockaddr *addr, socklen_t addrLen, int32_t timeoutMs);

        ErrCodeE Recv(char *buffer, int32_t size, sockaddr *addr, socklen_t *addrLen, int32_t timeoutMs);
    };

    //!面向连接的AF_UNIX操作接口
    class SocketUnStream : public SocketStream {
    public:
        SocketUnStream() : SocketStream() {}

        virtual ~SocketUnStream();

        ErrCodeE Open() override;

        static void MakeAddr(sockaddr_un &un, const char *path);
    };

    //!面相数据包的AF_UNIX操作接口
    class SocketUnDgram : public SocketDgram {
    public:
        SocketUnDgram() : SocketDgram() {}

        virtual ~SocketUnDgram();

        ErrCodeE Open() override;

        static void MakeAddr(sockaddr_un &un, const char *path);
    };
}

#endif //EMX_Socket_HPP
