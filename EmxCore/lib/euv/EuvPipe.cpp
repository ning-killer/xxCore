//
// Created by xiong on 2020/10/23.
//
#include <unistd.h>
#include "core/EmxLog.hpp"
#include "core/euv/EuvPipe.hpp"

using namespace Emx;

ErrCodeE EuvPipe::Create(EuvLoop &loop) {
    if (m_pipe)
        return ErrCodeE::Success;
    m_pipe = (uv_pipe_t *) malloc(sizeof(uv_pipe_t));
    int ret = uv_pipe_init(loop.GetLoop(), m_pipe, 0);
    if (ret) {
        free(m_pipe);
        m_pipe = nullptr;
        emxloge("%s create pipe failed:%s\n", loop.GetName(), uv_strerror(ret));
        return ErrCodeE::Failure;
    }
    emxlogt("%s create pipe %p\n", loop.GetName(), m_pipe);
    return ErrCodeE::Success;
}

void EuvPipe::OnDestroy(uv_handle_t *handle) {
    emxlogt("destroy pipe %p\n", handle);
//    char name[256] = {};
//    size_t len = 256;
//    int ret = uv_pipe_getsockname((uv_pipe_t *) handle, (char *) name, &len);
//    if (ret) {
//        emxloge("failed:%s\n", uv_strerror(ret));
//    } else {
//        unlink(name);
//        printf("unlink %s\n", name);
//    }
    free(handle);

}

ErrCodeE EuvPipe::Bind(const char *ip, uint16_t port) {
    unlink(ip);
    int ret = uv_pipe_bind(m_pipe, ip);
    if (ret) {
        emxloge("%s failed:%s\n", ip, uv_strerror(ret));
        return ErrCodeE::Failure;
    }
    return ErrCodeE::Success;
}

ErrCodeE EuvPipe::Connect(uv_connect_t *req, const char *serverIp, uint16_t serverPort, uv_connect_cb cb) {
    uv_pipe_connect(req, m_pipe, serverIp, cb);
//    UV_HANDLE_READABLE                    = 0x00004000,
//    UV_HANDLE_WRITABLE                    = 0x00008000,
//depend uv version
    m_pipe->flags |= 0x00004000 | 0x00008000;
    return ErrCodeE::Success;
}

ErrCodeE EuvPipe::GetSockName(void *name, size_t &len) {
    int ret = uv_pipe_getsockname(m_pipe, (char *) name, &len);
    if (ret) {
        emxloge("failed:%s\n", uv_strerror(ret));
        return ErrCodeE::Failure;
    }
    return ErrCodeE::Success;
}

ErrCodeE EuvPipe::GetPeerName(void *name, size_t &len) {
    int ret = uv_pipe_getpeername(m_pipe, (char *) name, &len);
    if (ret) {
        emxloge("failed:%s\n", uv_strerror(ret));
        return ErrCodeE::Failure;
    }
    return ErrCodeE::Success;
}

void EuvPipe::DoDestroy() {
    if (m_pipe) {
        if (!uv_is_closing((uv_handle_t *) m_pipe)) {
            uv_close((uv_handle_t *) m_pipe, OnDestroy);
        }
        m_pipe = nullptr;
    }
}

