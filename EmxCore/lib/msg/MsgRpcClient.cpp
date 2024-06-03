//
// Created by xiong on 2020/10/23.
//
#include <atomic>
#include "core/EmxLog.hpp"
#include "core/msg/MsgRpc.hpp"
#include "MsgInner.hpp"
#include "core/EmxUtils.hpp"

using namespace Emx;

static std::atomic<uint32_t> g_rpcReqId;
static uint32_t g_pid = getpid();

ErrCodeE MsgRpcReqSync::Request(uint32_t dstClientId,
                                uint32_t module, uint32_t method,
                                const char *req, int32_t size,
                                std::string *resp, int32_t timeoutMs) {


#define UPDATE_TIME() if ((timeoutMs = timeoutPoint - Time::GetMs64()) < 0) {\
                        emxloge("timeout\n");\
                        return ErrCodeE::Timeout;\
                    }


    auto timeoutPoint = Time::GetMs64() + timeoutMs;
    MsgRpc::Header header = {};
    header.sync = MSG_SYNC_ID;
    header.respClientId = dstClientId;
    header.size = size;
    header.module = module;
    header.method = method;

    SocketUnStream sock;
    ErrCodeE e = sock.Open();
    if (e != ErrCodeE::Success) {
        return e;
    }
    sockaddr_un un = {};
    char name[EMX_MAX_PATH_SIZE];
    snprintf(name, EMX_MAX_PATH_SIZE, "%s%u_%u_%u", RPC_REQ_DOMAIN_PREFIX, Time::GetS(), g_pid, g_rpcReqId++);
    SocketUnStream::MakeAddr(un, name);
    unlink(name);
    e = sock.Bind((sockaddr *) &un, sizeof(un));
    if (e != ErrCodeE::Success) {
        return e;
    }
    SocketUnStream::MakeAddr(un, RPC_SERVER_DOMAIN_PATH);
    e = sock.Connect((sockaddr *) &un, sizeof(un), timeoutMs);
    if (e != ErrCodeE::Success) {
        return e;
    }
    UPDATE_TIME();
    e = sock.Send((const char *) &header, sizeof(header), timeoutMs);
    if (e != ErrCodeE::Success) {
        return e;
    }
    UPDATE_TIME();
    if (req && size) {
        e = sock.Send((const char *) req, size, timeoutMs);
        if (e != ErrCodeE::Success) {
            return e;
        }
        UPDATE_TIME();
    }
    e = sock.Recv((char *) &header, sizeof(MsgRpc::Header), timeoutMs);
    if (e != ErrCodeE::Success) {
        return e;
    }
    if (header.sync != MSG_SYNC_ID) {
        emxloge("sync error\n");
        return ErrCodeE::ParseFailed;
    }
    UPDATE_TIME();
    if (resp) {
        resp->resize(header.size);
        e = sock.Recv((char *) resp->data(), resp->size(), timeoutMs);
        if (e != ErrCodeE::Success) {
            return e;
        }
    }
    sock.Close();
    unlink(name);
    return header.code;
}


ErrCodeE MsgRpcReqAsync::CreateMsgRpcReqAsync(MsgRpcReqAsync::Config &cfg) {
    m_cbOnConnected = cfg.OnConnected;
    m_cbOnDisConnected = cfg.OnDisConnected;
    m_cbOnRecvResp = cfg.OnRecvResp;
    char name[EMX_MAX_PATH_SIZE];
    snprintf(name, EMX_MAX_PATH_SIZE, "%s%u_%u_%u", RPC_REQ_DOMAIN_PREFIX, Time::GetS(), g_pid, g_rpcReqId++);
    return Create(*cfg.loop, cfg.buffer, cfg.size, RPC_SERVER_DOMAIN_PATH, 0, name);
}

ErrCodeE MsgRpcReqAsync::Request(uint32_t dstClientId, uint32_t module, uint32_t method,
                                 const char *req, int32_t size) {
    MsgRpc::Header header = {};
    header.sync = MSG_SYNC_ID;
    header.size = size;
    header.respClientId = dstClientId;
    header.module = module;
    header.method = method;
    auto s = std::make_shared<std::string>();
    s->assign((char *) &header, sizeof(header));
    s->append(req, size);
    return Send(s);
}

void MsgRpcReqAsync::OnRecv(const char *data, int32_t size) {
    m_data.append(data, size);
    while (m_data.size() >= m_waitSize) {
        if (m_waitHeader) {
            memcpy(&m_header, m_data.data(), sizeof(MsgRpc::Header));
            if (m_header.sync != MSG_SYNC_ID) {
                emxloge("recv miss sync\n");
                Destroy();
                return;
            }
            if (m_header.size != 0) {
                m_waitSize = m_header.size + sizeof(MsgRpc::Header);
                m_waitHeader = false;
                continue;
            }
        }
        if (m_cbOnRecvResp) {
            m_cbOnRecvResp(m_header.code, m_header.respClientId, m_header.module, m_header.method,
                           m_data.data() + sizeof(MsgRpc::Header),
                           m_waitSize - sizeof(MsgRpc::Header));
        }
        memcpy((void *) m_data.data(), m_data.data() + m_waitSize, m_data.size() - m_waitSize);
        m_data.resize(m_data.size() - m_waitSize);
        m_waitSize = sizeof(MsgRpc::Header);
        m_waitHeader = true;
    }
}


ErrCodeE MsgRpcRep::CreateMsgRpcRep(MsgRpcRep::Config &cfg) {
    m_cbConnected = cfg.OnConnected;
    m_cbDisConnected = cfg.OnDisConnected;
    m_cbRecvReq = cfg.OnRecvReq;
    m_clientId = cfg.clientId;
    char name[EMX_MAX_PATH_SIZE];
    snprintf(name, EMX_MAX_PATH_SIZE, "%s%#x", RPC_RESP_DOMAIN_PREFIX, m_clientId);
    emxlogd("create resp %#x\n", m_clientId);
    return Create(*cfg.loop, cfg.buffer, cfg.size, RPC_SERVER_DOMAIN_PATH, 0, name);
}

void MsgRpcRep::OnRecv(const char *data, int32_t size) {
    m_data.append(data, size);
    while (m_data.size() >= m_waitSize) {
        if (m_waitHeader) {
            memcpy(&m_header, m_data.data(), sizeof(MsgRpc::Header));
            if (m_header.sync != MSG_SYNC_ID) {
                emxloge("client[%s] recv miss sync\n", m_clientId);
                Destroy();
                return;
            }
            if (m_header.size != 0) {
                m_waitSize = m_header.size + sizeof(MsgRpc::Header);
                m_waitHeader = false;
                continue;
            }
        }
        if (m_cbRecvReq) {
            std::string resp;
            m_header.code = m_cbRecvReq(m_header.module, m_header.method,
                                        m_data.data() + sizeof(MsgRpc::Header),
                                        m_waitSize - sizeof(MsgRpc::Header),
                                        resp);
            m_header.size = resp.size();
            auto s = std::make_shared<std::string>();
            s->assign((char *) &m_header, sizeof(m_header));
            s->append(resp);
            Send(s);
        }
        memcpy((void *) m_data.data(), m_data.data() + m_waitSize, m_data.size() - m_waitSize);
        m_data.resize(m_data.size() - m_waitSize);
        m_waitSize = sizeof(MsgRpc::Header);
        m_waitHeader = true;
    }
}
