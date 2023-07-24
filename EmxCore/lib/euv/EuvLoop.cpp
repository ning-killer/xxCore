//
// Created by xiong on 2020/10/23.
//
#include <sys/prctl.h>
#include "core/EmxLog.hpp"
#include "core/euv/EuvLoop.hpp"

using namespace Emx;

EuvLoop::EuvLoop() : m_quit(false) {
    m_loop = nullptr;
    memset(&m_thread, 0, sizeof(uv_thread_t));
    memset(&m_async, 0, sizeof(uv_async_t));
}

ErrCodeE EuvLoop::Init(const char *name, Callback onQuit, ModeE mode) {
    m_cb = onQuit;
    m_async.data = this;
    m_mode = mode;
    m_name.assign(name);
    int ret;
    if (mode == Dynamic) {
        m_loop = new uv_loop_t();
        ret = uv_loop_init(m_loop);
        if (ret) {
            emxloge("%s failed:%s\n", m_name.c_str(), uv_strerror(ret));
            return ErrCodeE::Failure;
        }
    } else {
        m_loop = uv_default_loop();
    }
    ret = uv_async_init(m_loop, &m_async,
                        [](uv_async_t *async) {
                            uv_close((uv_handle_t *) async, nullptr);
                            ((EuvLoop *) async->data)->m_cb();
                        });
    if (ret) {
        emxloge("%s async init failed:%s\n", m_name.c_str(), uv_strerror(ret));
        Close();
        return ErrCodeE::Failure;
    }
    return ErrCodeE::Success;
}

ErrCodeE EuvLoop::Start(bool newThread) {
    if (newThread) {
        int err = uv_thread_create(&m_thread, [](void *arg) {
            auto obj = (EuvLoop *) arg;
            prctl(PR_SET_NAME, obj->m_name.c_str());
            emxlogd("Loop [%s] Run\n", obj->GetName());
            uv_run(obj->GetLoop(), UV_RUN_DEFAULT);
            obj->Close();
            emxlogd("loop [%s] Quit\n", obj->GetName());
        }, this);
        if (err) {
            emxloge("%s failed : %s\n", m_name.c_str(), uv_strerror(err));
            return ErrCodeE::Failure;
        }
    } else {
        uv_run(GetLoop(), UV_RUN_DEFAULT);
        Close();
    }
    return ErrCodeE::Success;
}


void EuvLoop::Stop() {
    m_quit = true;
    int err = uv_async_send(&m_async);
    if (err) {
        emxloge("%s async send failed : %s\n", m_name.c_str(), uv_strerror(err));
    }
}

void EuvLoop::DeInit() {
    uv_thread_join(&m_thread);
    m_quit = false;
}

void EuvLoop::Close() {
    uv_loop_close(m_loop);
    if (m_mode == Dynamic) {
        delete m_loop;
    }
}