//
// Created by xiong on 2021/12/9.
//

#include <cstdlib>
#include "core/EmxLog.hpp"
#include "core/euv/EuvStmServerBase.hpp"

using namespace Emx;

ErrCodeE EuvStmServerHostBase::Create(EuvLoop &loop, const char *ip, uint16_t port, int maxClientNum,
                                      char *buffer, int32_t size) {
    m_buffer = buffer;
    m_size = size;
    m_loop = &loop;
    m_stm->Create(loop);
    m_stm->Bind(ip, port);
    m_stm->GetStm()->data = this;
    int res = uv_listen(m_stm->GetStm(), maxClientNum, [](uv_stream_t *stream, int status) {
        if (status == -1) {
            emxlogc("EuvStmServerHostBase listen client error\n");
            return;
        }
        auto host = (EuvStmServerHostBase *) stream->data;
        auto client = host->OnAllocClient();
        if (!client) {
            emxloge("create client failed\n");
            return;
        }
        EuvStreamInf *clientStmInf = client->GetStmInf();
        clientStmInf->Create(*host->m_loop);
        clientStmInf->GetStm()->data = client;
        int res = uv_accept(host->m_stm->GetStm(), clientStmInf->GetStm());
        if (0 != res) {
            emxloge("uv_accept failed res = %d : %s\n", res, uv_strerror(res));
            clientStmInf->Destroy();
            host->OnClientDisConnect(client);
            return;
        }
        res = uv_read_start(clientStmInf->GetStm(),
                            [](uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
                                auto host = (EuvStmServerHostBase *) ((EuvStmServerClientBase *) handle->data)->GetHost();
                                buf->base = host->m_buffer;
                                buf->len = host->m_size;
                            },
                            [](uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf) {
                                auto client = (EuvStmServerClientBase *) stream->data;
                                auto host = (EuvStmServerHostBase *) client->GetHost();
                                if (nread > 0) {
                                    host->OnRecv(client, buf->base, nread);
                                } else if (nread < 0) {
                                    client->GetStmInf()->Destroy();
                                    host->OnClientDisConnect(client);
                                    host->m_clientList.remove(client);
                                }
                            });
        if (0 != res) {
            emxloge("uv_read_start failed res = %d : %s\n", res, uv_strerror(res));
            clientStmInf->Destroy();
            return;
        }
        host->m_clientList.push_back(client);
        host->OnClientConnect(client);
    });
    if (0 != res) {
        emxloge("streamServer listen failed res = %d : %s\n", res, uv_strerror(res));
        return ErrCodeE::Failure;
    }
    return ErrCodeE::Success;
}

void EuvStmServerHostBase::Destroy() {
    for (auto &e : m_clientList) {
        e->GetStmInf()->Destroy();
        OnClientDisConnect(e);
    }
    m_clientList.clear();
    m_stm->Destroy();
}

void EuvStmServerHostBase::DisConnectClient(EuvStmServerClientBase *client) {
    client->GetStmInf()->Destroy();
    ((EuvStmServerHostBase *) client->GetHost())->m_clientList.remove(client);
    OnClientDisConnect(client);
}

ErrCodeE EuvStmServerClientBase::Send(std::shared_ptr<std::string> &data) {
    if (!uv_is_active((uv_handle_t *) GetStmInf()->GetStm())) {
        return ErrCodeE::ResNotAvailable;
    }
    if (BufferOverflow()) {
        emxlogd("%p drop send data\n", this);
        return ErrCodeE::BufferOverflow;
    }
    auto writeReq = new(std::nothrow) WriteReq;
    if (!writeReq) {
        emxloge("malloc failed\n");
        return ErrCodeE::MallocFailed;
    }
    writeReq->obj = this;
    writeReq->data = data;
    writeReq->buf.base = (char *) data->data();
    writeReq->buf.len = data->size();
    writeReq->req.data = writeReq;

    int ret = uv_write(&writeReq->req, GetStmInf()->GetStm(), &writeReq->buf, 1,
                       [](uv_write_t *req, int status) {
                           if (status < 0) {
                               emxlogd("%p stream write error %s\n", req->handle, uv_strerror(status));
                           }
                           auto writeReq = (WriteReq *) req->data;
                           //do not notice the 'obj' by callback if the handle is closing or closed
                           //because the 'obj' may be  already deleted
                           if (!uv_is_closing((uv_handle_t *) (req->handle))) {
                               auto obj = writeReq->obj;
                               obj->SendDec(writeReq->buf.len);
                               obj->OnSendDone(status == 0 ? ErrCodeE::Success : ErrCodeE::Failure,
                                               writeReq->buf.base, writeReq->buf.len);
                           }
                           delete writeReq;
                       });
    if (ret) {
        emxloge("failed:%s\n", uv_strerror(ret));
        delete writeReq;
        return ErrCodeE::Failure;
    }
    SendInc(data->size());
    return ErrCodeE::Success;
}
