//
// Created by xiong on 2020/10/23.
//

#include <algorithm>
#include "core/EmxLog.hpp"
#include "MsgRpcServer.hpp"
#include "MsgInner.hpp"

using namespace Emx;

void MsgRpcServer::CreateMsgRpcServer() {
    if (access(MSG_DOMAIN_DIR, 0)) {
        mkdir(MSG_DOMAIN_DIR, 0777);
    }
    m_loop.Init("MsgRpcServer", [this]() {
        Destroy();
    });
    m_buffer = (char *) malloc(BufferSize);
    Create(m_loop, RPC_SERVER_DOMAIN_PATH, 0, 512, m_buffer, BufferSize);
    m_loop.Start();
}

void MsgRpcServer::OnClientDisConnect(EuvStmServerClientBase *client) {
    auto c = (MsgRpcClient *) client;
    auto &map = GetMapByType(c->type);
    map.erase(c->clientId);
    emxlogt("%s[%#x][%p] disconnected\n", c->GetTypeName(), c->clientId, c);
    delete c;
}

void MsgRpcServer::OnClientConnect(EuvStmServerClientBase *client) {
    char name[256] = {};
    size_t len = 256;
    client->GetStmInf()->GetPeerName(name, len);
    auto c = (MsgRpcClient *) client;
    //clientId[/tmp/emx/domain/msg/RpcC_REQ_432]
    //clientId[/tmp/emx/domain/msg/RpcC_RESP_12345678]
    if (strncmp(name + strlen(RPC_CLIENT_DOMAIN_PREFIX), "REQ", 3) == 0) {
        //make the fd as the client's id
        c->clientId = c->GetStmInf()->GetFd();
        c->type = MsgRpc::ClientTypeE::Req;
    } else {
        c->clientId = strtoul(name + strlen(RPC_RESP_DOMAIN_PREFIX), nullptr, 16);
        c->type = MsgRpc::ClientTypeE::Resp;
    }

    auto &map = GetMapByType(c->type);
    if (map.count(c->clientId)) {
        emxlogt("same client id %#x, deleted\n", c->GetTypeName(), c->clientId, c);
        DisConnectClient(c);
    }
    map[c->clientId] = c;
    emxlogt("%s[%#x][%p] connected\n", c->GetTypeName(), c->clientId, c);
}

void MsgRpcServer::OnRecv(EuvStmServerClientBase *client, char *data, int32_t size) {
    auto c = (MsgRpcClient *) client;
    c->data.append(data, size);

    while (c->data.size() >= c->waitSize) {
        if (c->waitHeader) {
            memcpy(&c->header, c->data.data(), sizeof(MsgRpc::Header));
            if (c->header.sync != MSG_SYNC_ID) {
                emxloge("%s[%#x] miss sync\n", c->GetTypeName(), c->clientId);
                DisConnectClient(c);
                return;
            }
            if (c->header.size != 0) {
                c->waitSize = c->header.size + sizeof(MsgRpc::Header);
                c->waitHeader = false;
                continue;
            }
        }

        if (c->type == MsgRpc::ClientTypeE::Req) {
            c->header.reqClientId = c->clientId;
            auto header = (MsgRpc::Header *) c->data.data();
            header->reqClientId = c->clientId;
        }
        emxlogt("type[%s]ReqId[%#x]RespId[%#x]session[%u]module[%#x]method[%u]code[%d]size[%u]\n",
               c->GetTypeName(),
               c->header.reqClientId, c->header.respClientId, c->header.sessionId,
               c->header.module, c->header.method, c->header.code, c->header.size);
        SendRpc(c);
        memcpy((void *) c->data.data(), c->data.data() + c->waitSize, c->data.size() - c->waitSize);
        c->data.resize(c->data.size() - c->waitSize);
        c->waitSize = sizeof(MsgRpc::Header);
        c->waitHeader = true;
    }
}


void MsgRpcServer::SendRpc(MsgRpcClient *client) {
    if (client->type == MsgRpc::ClientTypeE::Req) {
        if (m_respMap.count(client->header.respClientId)) {
            auto s = std::make_shared<std::string>(client->data.data(), client->waitSize);
            m_respMap[client->header.respClientId]->Send(s);
        } else {
            emxloge("not found %#x\n", client->header.respClientId);
            client->header.size = 0;
            client->header.code = ErrCodeE::ResNotExist;
            auto s = std::make_shared<std::string>((const char *) &client->header, sizeof(client->header));
            client->Send(s);
        }
    } else {
        if (m_reqMap.count(client->header.reqClientId)) {
            auto s = std::make_shared<std::string>(client->data.data(), client->waitSize);
            m_reqMap[client->header.reqClientId]->Send(s);
        }
    }
}
