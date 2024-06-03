//
// Created by xiong on 2020/11/6.
//

#include <unistd.h>
#include "core/EmxLog.hpp"
#include "core/euv/EuvTcp.hpp"

using namespace Emx;

ErrCodeE EuvTcp::Create(EuvLoop &loop) {
    if (m_tcp)
        return ErrCodeE::Success;
    m_tcp = (uv_tcp_t *) malloc(sizeof(uv_tcp_t));
    int ret = uv_tcp_init(loop.GetLoop(), m_tcp);
    if (ret) {
        free(m_tcp);
        m_tcp = nullptr;
        emxloge("%s create tcp failed:%s\n", loop.GetName(), uv_strerror(ret));
        return ErrCodeE::Failure;
    }
    emxlogt("%s create tcp %p\n", loop.GetName(), m_tcp);
    return ErrCodeE::Success;
}


void EuvTcp::OnDestroy(uv_handle_t *handle) {
    emxlogt("destroy tcp %p\n", handle);
    free(handle);
}

ErrCodeE EuvTcp::Bind(const char *ip, uint16_t port) {
    sockaddr_in addr4 = {};
    sockaddr_in6 addr6 = {};
    int res = -1;
    if (uv_ip4_addr(ip, port, &addr4) == 0) {
        res = uv_tcp_bind(m_tcp, (sockaddr *) &addr4, 0);
    } else if (uv_ip6_addr(ip, port, &addr6) == 0) {
        res = uv_tcp_bind(m_tcp, (sockaddr *) &addr6, 0);
    } else {
        emxloge("illegal ip %s\n", ip);
        return ErrCodeE::Failure;
    }
    if (0 != res) {
        emxloge("tcp bind %s failed\n", ip);
        return ErrCodeE::Failure;
    }
    return ErrCodeE::Success;
}

ErrCodeE EuvTcp::Open(int fd) {
    int err = uv_tcp_open(m_tcp, fd);
    if (err != 0) {
        emxloge("open fd %d failed with %s\n", uv_err_name(err));
        return ErrCodeE::Failure;
    }
    return ErrCodeE::Success;
}

ErrCodeE EuvTcp::Connect(uv_connect_t *req, const char *serverIp, uint16_t serverPort, uv_connect_cb cb) {
    sockaddr_in addr4 = {};
    sockaddr_in6 addr6 = {};
    if (uv_ip4_addr(serverIp, serverPort, &addr4) == 0) {
        uv_tcp_connect(req, m_tcp, (sockaddr *) &addr4, cb);
    } else if (uv_ip6_addr(serverIp, serverPort, &addr6) == 0) {
        uv_tcp_connect(req, m_tcp, (sockaddr *) &addr6, cb);
    } else {
        emxloge("illegal serverIp %s\n", serverIp);
        return ErrCodeE::Failure;
    }
//    UV_HANDLE_READABLE                    = 0x00004000,
//    UV_HANDLE_WRITABLE                    = 0x00008000,
    m_tcp->flags |= 0x00004000 | 0x00008000;
    return ErrCodeE::Success;
}

ErrCodeE EuvTcp::GetSockName(void *name, size_t &len) {
    if (0 != uv_tcp_getsockname(m_tcp, (sockaddr *) name, (int *) &len)) {
        emxloge("uv_tcp_getsockname failed\n");
        return ErrCodeE::Failure;
    }
    return ErrCodeE::Success;
}

ErrCodeE EuvTcp::GetPeerName(void *name, size_t &len) {
    int res = uv_tcp_getpeername(m_tcp, (sockaddr *) name, (int *) &len);
    if (0 != res) {
        emxloge("uv_tcp_getpeername failed\n");
        return ErrCodeE::Failure;
    }
    return ErrCodeE::Success;
}

void EuvTcp::DoDestroy() {
    if (m_tcp) {
        if (!uv_is_closing((uv_handle_t *) m_tcp)) {
            uv_close((uv_handle_t *) m_tcp, OnDestroy);
        }
        m_tcp = nullptr;
    }
}
