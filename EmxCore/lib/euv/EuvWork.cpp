//
// Created by xiong on 2021/4/8.
//
#include "core/EmxLog.hpp"
#include "core/euv/EuvWork.hpp"

using namespace Emx;

ErrCodeE EuvWork::Create(EuvLoop *loop,
                         void *arg,
                         RunWorkCallBack runCb,
                         AfterWorkCallBack afterCb) {
    if (m_work)
        return ErrCodeE::Success;
    m_loop = loop;
    m_work = new uv_work_t;
    m_work->data = this;
    m_arg = arg;
    m_runCb = std::move(runCb);
    m_afterCb = std::move(afterCb);
    emxlogt("%s create work %p\n", loop->GetName(), m_work);
    return ErrCodeE::Success;
}

void EuvWork::Destroy() {
    if (!m_work)
        return;
    if (!m_isWorking) {
        emxlogt("destroy work %p\n", m_work);
        delete m_work;
        m_work = nullptr;
        return;
    }
    m_work->after_work_cb = OnDestroy;
    //work is running on thread pool
    if (uv_cancel((uv_req_t *) m_work) != 0) {
        while (m_work->work_req.work)usleep(10000);
        m_isWorking = false;
    }
    if (m_afterCb)
        m_afterCb(ErrCodeE::Destroyed, m_arg);
    m_work = nullptr;
}

ErrCodeE EuvWork::Run(void *arg, RunWorkCallBack runCb, AfterWorkCallBack afterCb) {
    if (m_isWorking) {
        emxloge("work is working!\n");
        return ErrCodeE::ResInUsing;
    }
    if (arg)
        m_arg = arg;
    if (runCb)
        m_runCb = std::move(runCb);
    if (afterCb)
        m_afterCb = std::move(afterCb);
    int ret = uv_queue_work(m_loop->GetLoop(), m_work,
                            [](uv_work_t *req) {
                                auto obj = (EuvWork *) req->data;
                                obj->m_runCb(obj->m_arg);
                            },
                            [](uv_work_t *req, int status) {
                                auto obj = (EuvWork *) req->data;
                                obj->m_isWorking = false;
                                if (status == UV_ECANCELED) {
                                    obj->m_afterCb(ErrCodeE::Canceled, obj->m_arg);
                                } else {
                                    obj->m_afterCb(0 == status ? ErrCodeE::Success : ErrCodeE::Failure, obj->m_arg);
                                }
                            });
    if (ret) {
        emxloge("failed:%s\n", uv_strerror(ret));
        return ErrCodeE::Failure;
    }
    m_isWorking = true;
    return ErrCodeE::Success;
}

void EuvWork::OnDestroy(uv_work_t *req, int status) {
    emxlogt("destroy work %p\n", req);
    delete req;
}
