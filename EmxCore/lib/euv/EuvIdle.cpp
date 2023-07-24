//
// Created by xiong on 2021/7/12.
//
#include "core/EmxLog.hpp"
#include "core/euv/EuvIdle.hpp"

#include <utility>

using namespace Emx;


ErrCodeE EuvIdle::Create(EuvLoop &loop, EuvIdle::Callback cb) {
    if (m_idle)
        return ErrCodeE::Success;
    m_idle = (uv_idle_t *) malloc(sizeof(uv_idle_t));
    int ret = uv_idle_init(loop.GetLoop(), m_idle);
    if (ret) {
        free(m_idle);
        m_idle = nullptr;
        emxloge("%s create idle failed:%s\n", loop.GetName(), uv_strerror(ret));
        return ErrCodeE::Failure;
    }
    m_idle->data = this;
    emxlogt("%s create idle %p\n", loop.GetName(), m_idle);
    m_cb = std::move(cb);
    m_idle->data = this;
    ret = uv_idle_start(m_idle, [](uv_idle_t *handle) {
        auto obj = (EuvIdle *) handle->data;
        obj->m_cb();
    });
    if (ret) {
        emxloge("idle failed:%s\n", uv_strerror(ret));
        free(m_idle);
        m_idle = nullptr;
        return ErrCodeE::Failure;
    }
    return ErrCodeE::Success;
}

void EuvIdle::Destroy() {
    if (m_idle) {
        if (!uv_is_closing((uv_handle_t *) m_idle)) {
            uv_close((uv_handle_t *) m_idle, OnDestroy);
        }
        m_idle = nullptr;
    }
}

void EuvIdle::OnDestroy(uv_handle_t *handle) {
    emxlogt("destroy idle %p\n", handle);
    free(handle);
}
