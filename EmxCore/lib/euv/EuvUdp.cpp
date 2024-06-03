//
// Created by xiong on 2020/11/7.
//
#include "core/EmxLog.hpp"
#include "core/euv/EuvUdp.hpp"

#include <utility>

using namespace Emx;

ErrCodeE EuvUdp::Create(EuvLoop &loop) {
    if (m_udp)
        return ErrCodeE::Success;
    m_udp = (uv_udp_t *) malloc(sizeof(uv_udp_t));
    int ret = uv_udp_init(loop.GetLoop(), m_udp);
    if (ret) {
        free(m_udp);
        m_udp = nullptr;
        emxloge("%s create udp failed:%s\n", loop.GetName(), uv_strerror(ret));
        return ErrCodeE::Failure;
    }
    m_udp->data = this;
    emxlogt("%s create udp %p\n", loop.GetName(), m_udp);
    return ErrCodeE::Success;
}

void EuvUdp::Destroy() {
    if (m_udp) {
        if (!uv_is_closing((uv_handle_t *) m_udp)) {
            uv_close((uv_handle_t *) m_udp, OnDestroy);
        }
        m_udp = nullptr;
    }
}

void EuvUdp::OnDestroy(uv_handle_t *handle) {
    emxlogt("destroy udp %p\n", handle);
    free(handle);
}

ErrCodeE EuvUdp::OpenExistSock(uv_os_sock_t sock) {
    int ret = uv_udp_open(m_udp, sock);
    if (ret) {
        emxloge(" %p failed:%s\n", m_udp, uv_strerror(ret));
        return ErrCodeE::Failure;
    }
    return ErrCodeE::Success;
}

ErrCodeE EuvUdp::Bind(const struct sockaddr *addr, unsigned int flags) {
    int ret = uv_udp_bind(m_udp, addr, flags);
    if (ret) {
        emxloge("%p failed:%s\n", m_udp, uv_strerror(ret));
        return ErrCodeE::Failure;
    }
    return ErrCodeE::Success;
}

ErrCodeE EuvUdp::Bind(const char *ip, int port, unsigned int flags) {
    struct sockaddr_in addr = {0};
    uv_ip4_addr(ip, port, &addr);
    return Bind((struct sockaddr *) &addr, flags);
}

ErrCodeE EuvUdp::SetMembership(const char *multicastAddr, const char *interfaceAddr, EuvMembership membership) {
    int ret = uv_udp_set_membership(m_udp, multicastAddr, interfaceAddr, (uv_membership) membership);
    if (ret) {
        emxloge("%p failed:%s\n", m_udp, uv_strerror(ret));
        return ErrCodeE::Failure;
    }
    return ErrCodeE::Success;
}

ErrCodeE EuvUdp::SetMulticastLoop(bool ena) {
    int on = ena;
    int ret = uv_udp_set_multicast_loop(m_udp, on);
    if (ret) {
        emxloge("%p failed:%s\n", m_udp, uv_strerror(ret));
        return ErrCodeE::Failure;
    }
    return ErrCodeE::Success;
}

ErrCodeE EuvUdp::SetMulticastTTL(int ttl) {
    int ret = uv_udp_set_multicast_ttl(m_udp, ttl);
    if (ret) {
        emxloge("%p failed:%s\n", m_udp, uv_strerror(ret));
        return ErrCodeE::Failure;
    }
    return ErrCodeE::Success;
}

ErrCodeE EuvUdp::SetMulticastInterface(const char *interfaceAddr) {
    int ret = uv_udp_set_multicast_interface(m_udp, interfaceAddr);
    if (ret) {
        emxloge("%p failed:%s\n", m_udp, uv_strerror(ret));
        return ErrCodeE::Failure;
    }
    return ErrCodeE::Success;
}

ErrCodeE EuvUdp::SetBroadcast(bool ena) {
    int on = ena;
    int ret = uv_udp_set_broadcast(m_udp, on);
    if (ret) {
        emxloge("%p failed:%s\n", m_udp, uv_strerror(ret));
        return ErrCodeE::Failure;
    }
    return ErrCodeE::Success;
}

ErrCodeE EuvUdp::StartRecv(char *buffer, int32_t size, RecvCallback cbRecv) {
    m_cbRecv = std::move(cbRecv);
    m_buffer = buffer;
    m_size = size;
    int ret = uv_udp_recv_start(m_udp,
                                [](uv_handle_t *handle,
                                   size_t suggested_size,
                                   uv_buf_t *buf) {
                                    auto obj = (EuvUdp *) handle->data;
                                    buf->base = obj->m_buffer;
                                    buf->len = obj->m_size;
                                },
                                [](uv_udp_t *handle,
                                   ssize_t nread,
                                   const uv_buf_t *buf,
                                   const struct sockaddr *addr,
                                   unsigned flags) {
                                    auto obj = (EuvUdp *) handle->data;
                                    if (nread == 0)
                                        return;
                                    if (obj->m_cbRecv) {
                                        // nread: Number of bytes that have been received.
                                        // 0 if there is no more data to read.
                                        // Note that 0 may also mean that an empty datagram was received
                                        // (in this case addr is not NULL).
                                        // < 0 if a transmission error was detected;
                                        // if using recvmmsg(2) no more chunks will be received and
                                        // the buffer can be freed safely.
                                        ErrCodeE e;
                                        if (nread < 0) {
                                            e = ErrCodeE::Failure;
                                            emxlogw("recv error:%s\n", uv_strerror(nread));
                                        }else{
                                            e = ErrCodeE::Success;
                                        }
                                        obj->m_cbRecv(e, buf->base, nread, addr);
                                    }
                                });
    if (ret) {
        emxloge("%p failed:%s\n", m_udp, uv_strerror(ret));
        return ErrCodeE::Failure;
    }
    return ErrCodeE::Success;
}

ErrCodeE EuvUdp::GetPeerName(struct sockaddr *name, int *namelen) {
    int ret = uv_udp_getpeername(m_udp, name, namelen);
    if (ret) {
        emxloge("%p failed:%s\n", m_udp, uv_strerror(ret));
        return ErrCodeE::Failure;
    }
    return ErrCodeE::Success;
}

ErrCodeE EuvUdp::GetSockName(struct sockaddr *name, int *namelen) {
    int ret = uv_udp_getsockname(m_udp, name, namelen);
    if (ret) {
        emxloge("%p failed:%s\n", m_udp, uv_strerror(ret));
        return ErrCodeE::Failure;
    }
    return ErrCodeE::Success;
}


