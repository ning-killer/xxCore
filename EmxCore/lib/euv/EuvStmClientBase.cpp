//
// Created by xiong on 2020/10/23.
//
#include <cstdlib>
#include "core/EmxLog.hpp"
#include "core/euv/EuvStmClientBase.hpp"

using namespace Emx;

ErrCodeE EuvStmClientBase::Create(EuvLoop &loop, char *buffer, int32_t size,
                                  const char *serverIp, uint16_t serverPort,
                                  const char *localIp, uint16_t localPort) {
    m_stm->Create(loop);
    if (localIp) {
        m_stm->Bind(localIp, localPort);
    }
    m_stm->GetStm()->data = this;
//    uv_stream_set_blocking(GetStm(), blocking);
    auto connect = new uv_connect_t;
    connect->data = this;
    m_buffer = buffer;
    m_size = size;
    m_isConnected = false;
    ErrCodeE e = m_stm->Connect(connect, serverIp, serverPort,
                                [](uv_connect_t *connect, int status) {
                                    auto client = (EuvStmClientBase *) connect->data;
                                    auto handle = (uv_handle_t *) connect->handle;
                                    delete connect;
                                    if (uv_is_closing(handle))
                                        return;
                                    if (status != 0) {
                                        client->OnDisConnect();
                                    } else {
                                        if (client->ResumeRecv() == ErrCodeE::Success) {
                                            client->m_isConnected = true;
                                            client->OnConnect();
                                        } else {
                                            client->m_isConnected = false;
                                            client->OnDisConnect();
                                        }
                                    }
                                });
    if (ErrCodeE::Success != e) {
        delete connect;
        emxloge("connect failed\n");
        return ErrCodeE::Failure;
    }

    return ErrCodeE::Success;
}

void EuvStmClientBase::Destroy() {
    if (m_stm)
        m_stm->Destroy();
}


ErrCodeE EuvStmClientBase::Send(std::shared_ptr<std::string> &data) {
    if (!m_isConnected) {
        return ErrCodeE::ResNotAvailable;
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

    int err = uv_write(&writeReq->req, m_stm->GetStm(), &writeReq->buf, 1,
                       [](uv_write_t *req, int status) {
                           if (status < 0) {
                               emxlogd("%p stream write error %s\n", req->handle, uv_strerror(status));
                           }
                           auto writeReq = (WriteReq *) req->data;
                           //do not notice the 'obj' by callback if the handle is closing or closed
                           //because the 'obj' may be  already deleted
                           if (!uv_is_closing((uv_handle_t *) (req->handle))) {
                               auto obj = writeReq->obj;
                               obj->OnSendDone(status == 0 ? ErrCodeE::Success : ErrCodeE::Failure,
                                               writeReq->buf.base, writeReq->buf.len);
                           }
                           delete writeReq;
                       });
    if (err) {
        emxloge("write failed:%d[%s]\n", err, uv_strerror(err));
        delete writeReq;
        return ErrCodeE::Failure;
    }
    return ErrCodeE::Success;
}

ErrCodeE EuvStmClientBase::ResumeRecv() {
    int res = uv_read_start(m_stm->GetStm(),
                            [](uv_handle_t *handle, size_t suggested_size,
                               uv_buf_t *buf) {
                                auto client = (EuvStmClientBase *) handle->data;
                                buf->base = client->m_buffer;
                                buf->len = client->m_size;
                            },
                            [](uv_stream_t *stream, ssize_t nread,
                               const uv_buf_t *buf) {
                                auto client = (EuvStmClientBase *) stream->data;
                                if (nread > 0) {
                                    client->OnRecv(buf->base, nread);
                                } else {
                                    client->m_isConnected = false;
                                    client->OnDisConnect();
                                }
                            });
    if (0 != res) {
        emxloge("%p failed res = %d : %s\n",m_stm->GetStm(), res, uv_strerror(res));
        return ErrCodeE::Failure;
    }
    return ErrCodeE::Success;
}

void EuvStmClientBase::PauseRecv() {
    uv_read_stop(m_stm->GetStm());
}




