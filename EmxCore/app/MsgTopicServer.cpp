//
// Created by xiong on 2020/10/23.
//
#include "core/EmxLog.hpp"
#include "MsgTopicServer.hpp"
#include "MsgInner.hpp"

using namespace Emx;

void MsgTopicServer::CreateMsgTopicServer() {
    if (access(MSG_DOMAIN_DIR, 0)) {
        mkdir(MSG_DOMAIN_DIR, 0777);
    }
    m_loop.Init("MsgTopicServer", [this]() {
        Destroy();
    });

    m_buffer = (char *) malloc(BufferSize);
    Create(m_loop, TOPIC_SERVER_DOMAIN_PATH, 0, 512, m_buffer, BufferSize);
    m_loop.Start();
}

void MsgTopicServer::OnClientDisConnect(EuvStmServerClientBase *client) {
    auto c = (MsgTopicClient *) client;
    m_clientList.remove(c);
    for (auto &it : c->topicList) {
        m_clientAppend2Topic[it].remove(c);
    }
    if (c->type == MsgTopic::ClientTypeE::Pub) {
        emxlogt("Publisher[%#x] disconnected\n", c->clientId);
    } else {
        emxlogt("Subscriber[%#x] disconnected\n", c->clientId);
    }
    delete c;
}

void MsgTopicServer::OnClientConnect(EuvStmServerClientBase *client) {
    char name[EMX_MAX_PATH_SIZE] = {};
    size_t len = EMX_MAX_PATH_SIZE;
    auto c = (MsgTopicClient *) client;
    c->clientId = c->GetStmInf()->GetFd();
    c->GetStmInf()->GetPeerName(name, len);
    if (strncmp(name + strlen(TOPIC_CLIENT_DOMAIN_PREFIX), "SUB", 3) == 0) {
        c->type = MsgTopic::ClientTypeE::Sub;
        int32_t sndBuf = 2048;
        if (setsockopt(c->GetStmInf()->GetFd(), SOL_SOCKET, SO_SNDBUF, &sndBuf, sizeof(int)) == -1)
            printf("SO_SNDBUF error %d:%s\n", errno, strerror(errno));
        emxlogt("Subscriber[%#x] connected\n", c->clientId);
    } else {
        c->type = MsgTopic::ClientTypeE::Pub;
        emxlogt("Publisher[%#x] connected\n", c->clientId);
    }
    m_clientList.push_back((MsgTopicClient *) client);
}

void MsgTopicServer::OnRecv(EuvStmServerClientBase *client, char *data, int32_t size) {
    auto c = (MsgTopicClient *) client;
    c->data.append(data, size);
    while (c->data.size() >= c->waitSize) {
        if (c->waitHeader) {
            memcpy(&c->header, c->data.data(), sizeof(MsgTopic::Header));
            if (c->header.sync != MSG_SYNC_ID) {
                emxloge("client[%#x] miss sync\n", c->clientId);
                DisConnectClient(c);
                return;
            }
            if (c->type == MsgTopic::ClientTypeE::Pub && c->header.msgType != MsgTopic::MsgTypeE::Transfer) {
                emxloge("publisher[%#x] recv illegal msg type[%d]\n", c->clientId, c->header.msgType);
                DisConnectClient(c);
                return;
            }
            if (c->type == MsgTopic::ClientTypeE::Sub && c->header.msgType != MsgTopic::MsgTypeE::Register) {
                emxloge("subscriber[%#x] recv illegal msg type[%d]\n", c->clientId, c->header.msgType);
                DisConnectClient(c);
                return;
            }
            c->waitSize = c->header.size + sizeof(MsgTopic::Header);
            c->waitHeader = false;
            continue;
        }

        if (c->type == MsgTopic::ClientTypeE::Sub) {
            if (c->topicList.empty()) {
                MsgTopic::SubRegister reg = {};
                memcpy(&reg, c->data.data(), sizeof(reg));
                for (auto e : reg.topicArray) {
                    if (e == 0)
                        break;
                    c->topicList.push_back(e);
                    m_clientAppend2Topic[e].push_back(c);
                    emxlogt("client %#x sub %#x\n", c->clientId, e);
                }
                c->SetMaxSendBuffer(reg.sendByPack, reg.maxSend);
            }
        } else if (c->type == MsgTopic::ClientTypeE::Pub) {
            MsgTopic::Transfer transfer = {};
            memcpy(&transfer, c->data.data(), sizeof(transfer));
            //just for debug
            auto s = std::make_shared<std::string>(c->data.data(), c->waitSize);
            if (s && m_clientAppend2Topic.count(transfer.topic)) {
                ClientList &clientList = m_clientAppend2Topic[transfer.topic];
                for (auto &it : clientList) {
                    it->Send(s);
                }
            }
        }
        memcpy((void *) c->data.data(), c->data.data() + c->waitSize, c->data.size() - c->waitSize);
        c->data.resize(c->data.size() - c->waitSize);
        c->waitSize = sizeof(MsgTopic::Header);
        c->waitHeader = true;
    }
}
