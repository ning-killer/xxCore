//
// Created by xiong on 2020/10/23.
//
#include "core/EmxLog.hpp"
#include "core/euv/EuvTimer.hpp"

#include <utility>

using namespace Emx;

ErrCodeE EuvTimer::Create(EuvLoop &loop) {
    if(m_timer)
        return ErrCodeE::Success;
    m_timer = (uv_timer_t *) malloc(sizeof(uv_timer_t));
    int ret = uv_timer_init(loop.GetLoop(), m_timer);
    if (ret) {
        free(m_timer);
        m_timer = nullptr;
        emxloge("%s create timer failed:%s\n", loop.GetName(), uv_strerror(ret));
        return ErrCodeE::Failure;
    }
    m_timer->data = this;
    emxlogt("%s create timer %p\n", loop.GetName(), m_timer);
    return ErrCodeE::Success;
}

void EuvTimer::Destroy() {
    if (m_timer) {
        if (!uv_is_closing((uv_handle_t *) m_timer)) {
            uv_close((uv_handle_t *) m_timer, OnDestroy);
        }
        m_timer = nullptr;
    }
}

void EuvTimer::OnDestroy(uv_handle_t *handle) {
    emxlogt("destroy timer %p\n", handle);
    free(handle);
}

ErrCodeE EuvTimer::Start(uint64_t timeout, uint64_t repeat, EuvTimer::Callback cb) {
    m_cb = std::move(cb);
    if (0 != uv_timer_start(m_timer,
                            [](uv_timer_t *handle) {
                                auto obj = (EuvTimer *) handle->data;
                                if (obj->m_cb)
                                    obj->m_cb();
                            },
                            timeout, repeat)) {
        emxloge("timer start failed\n");
        return ErrCodeE::Failure;
    }
    return ErrCodeE::Success;
}
