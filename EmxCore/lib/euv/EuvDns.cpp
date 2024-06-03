//
// Created by xiong on 2021/6/29.
//
#include <cstring>
#include <utility>
#include "core/euv/EuvLoop.hpp"
#include "core/euv/EuvDns.hpp"
#include "core/EmxLog.hpp"

using namespace Emx;

ErrCodeE EuvDns::Create(EuvLoop *loop) {
    if (m_resolver)
        return ErrCodeE::Success;
    m_loop = loop;
    m_resolver = new uv_getaddrinfo_t;
    m_resolver->data = &m_data;
    emxlogt("%s create resolver %p\n", loop->GetName(), m_resolver);
    return ErrCodeE::Success;
}

void EuvDns::Destroy() {
    if (!m_resolver)
        return;
    if (!m_isRunning) {
        emxlogt("destroy resolver %p\n", m_resolver);
        delete m_resolver;
        m_resolver = nullptr;
        return;
    }
    m_resolver->cb = OnDestroy;
    //work is running on thread pool

    emxlogt("try cancel\n");
    if (uv_cancel((uv_req_t *) m_resolver) != 0) {
        while (m_resolver->work_req.work)usleep(10000);
        m_isRunning = false;
        emxlogt("cancel done\n");
    }
    if (m_cb)
        m_cb(ErrCodeE::Destroyed, nullptr, m_data.arg);
    m_resolver = nullptr;
}

ErrCodeE EuvDns::Run(const char *server, OnResolved cb, void *arg, EuvDns::TypeE type) {
    addrinfo hints = {};
    if (type == TypeE::Ipv4)
        hints.ai_family = AF_INET;
    else if (type == TypeE::Ipv6)
        hints.ai_family = AF_INET6;
    else
        hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = 0;
    m_data.obj = this;
    m_data.arg = arg;
    m_cb = std::move(cb);
    strncpy(m_server, server, EMX_MAX_PATH_SIZE - 1);
    int ret = uv_getaddrinfo(m_loop->GetLoop(), m_resolver,
                             [](uv_getaddrinfo_t *resolver, int status, struct addrinfo *res) {
                                 auto data = (EuvDns::Data *) resolver->data;
                                 auto obj = (EuvDns *) data->obj;
                                 emxlogt("EuvDns status=%d\n",status);
                                 if (status < 0) {
                                     obj->m_cb(ErrCodeE::Failure, res, data->arg);
                                 } else {
                                     obj->m_cb(ErrCodeE::Success, res, data->arg);
                                     uv_freeaddrinfo(res);
                                 }
                             }, server, nullptr, &hints);
    if (ret) {
        emxloge("failed:%s\n", uv_strerror(ret));
        return ErrCodeE::Failure;
    }
    m_isRunning = true;
    emxlogt("dns running\n");
    return ErrCodeE::Success;
}

void EuvDns::OnDestroy(uv_getaddrinfo_t *resolver, int status, struct addrinfo *res) {
    emxlogt("destroy resolver %p\n", resolver);
    delete resolver;
}

