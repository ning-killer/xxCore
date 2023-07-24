//
// Created by xiong on 2020/10/23.
//
#include "core/EmxLog.hpp"
#include "core/euv/EuvAsync.hpp"

#include <utility>

using namespace Emx;

ErrCodeE EuvAsync::Create(EuvLoop &loop, Callback cb) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_async)
        return ErrCodeE::Success;
    m_cb = std::move(cb);
    m_async = (uv_async_t *) malloc(sizeof(uv_async_t));
    int ret = uv_async_init(loop.GetLoop(), m_async,
                            [](uv_async_t *async) {
                                auto obj = (EuvAsync *) async->data;
                                if (obj->m_cb)
                                    obj->m_cb();
                            });
    if (ret) {
        free(m_async);
        m_async = nullptr;
        emxloge("%s create async failed:%s\n", loop.GetName(), uv_strerror(ret));
        return ErrCodeE::Failure;
    }
    m_async->data = this;
    emxlogt("%s create async %p\n", loop.GetName(), m_async);
    return ErrCodeE::Success;
}

void EuvAsync::Destroy() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_async) {
        if (!uv_is_closing((uv_handle_t *) m_async)) {
            uv_close((uv_handle_t *) m_async, OnDestroy);
        }
        m_async = nullptr;
    }
}

void EuvAsync::OnDestroy(uv_handle_t *handle) {
    emxlogt("destroy async %p\n", handle);
    free(handle);
}

ErrCodeE EuvAsync::Send() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_async) {
        int ret = uv_async_send(m_async);
        if (ret) {
            emxloge("%p failed:%s\n", m_async, uv_strerror(ret));
            return ErrCodeE::Failure;
        }
    }
    return ErrCodeE::Success;
}
