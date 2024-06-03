//
// Created by xiong on 2020/10/29.
//
#include "core/EmxLog.hpp"
#include "core/euv/EuvPoll.hpp"

using namespace Emx;

ErrCodeE EuvPoll::Init(EuvLoop &loop, int fd) {
    int ret = uv_poll_init(loop.GetLoop(), &m_poll, fd);
    if (ret) {
        emxloge("failed:%s\n", uv_strerror(ret));
        return ErrCodeE::Failure;
    }
    return ErrCodeE::Success;
}

ErrCodeE EuvPoll::InitSock(EuvLoop &loop, int fd) {
    int ret = uv_poll_init_socket(loop.GetLoop(), &m_poll, fd);
    if (ret) {
        emxloge("failed:%s\n", uv_strerror(ret));
        return ErrCodeE::Failure;
    }
    return ErrCodeE::Success;
}

ErrCodeE EuvPoll::Start(EuvPoll::Callback OnRead, EuvPoll::Callback OnWrite, EuvPoll::Callback OnDisconnect) {
    m_cbRead = OnRead;
    m_cbWrite = OnWrite;
    m_cbDisconnect = OnDisconnect;
    uint32_t events = 0;
    if (m_cbRead)
        events |= UV_READABLE;
    if (m_cbWrite)
        events |= UV_WRITABLE;
    if (m_cbDisconnect)
        events |= UV_DISCONNECT;
    m_poll.data = this;
    int ret = uv_poll_start(&m_poll, events,
                            [](uv_poll_t *handle, int status, int events) {
                                if (status != 0) {
                                    emxlogw("status = %d err = %s\n", status, uv_strerror(status));
                                }
                                auto obj = (EuvPoll *) handle->data;
                                if (events & UV_READABLE && obj->m_cbRead)
                                    obj->m_cbRead();
                                if (events & UV_WRITABLE && obj->m_cbWrite)
                                    obj->m_cbWrite();
                                if (events & UV_DISCONNECT && obj->m_cbDisconnect)
                                    obj->m_cbDisconnect();
                            }
    );
    if (ret) {
        emxloge("failed:%s\n", uv_strerror(ret));
        return ErrCodeE::Failure;
    }
    return ErrCodeE::Success;
}
