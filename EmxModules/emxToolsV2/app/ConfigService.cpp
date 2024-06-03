//
// Created by xiong on 2020/11/7.
//
#include "ConfigService.hpp"

using namespace Emx;

void ConfigService::CreateConfigService(Resource *res, Json::Value &json) {
    m_res = res;
    m_port = json["port"].asInt();
    Create(m_res->loop, "0.0.0.0",
           m_port,
           json["maxClientNum"].asInt(),
           m_res->buffer,
           m_res->bufferSize);
}

void ConfigService::DestroyConfigService() {
    Destroy();
}

EuvStmServerClientBase *ConfigService::OnAllocClient() {
    return new ConfigClient(this, m_res->loop);
}

void ConfigService::OnClientConnect(EuvStmServerClientBase *client) {
    sockaddr_in addr = {};
    size_t len = sizeof(addr);
    client->GetStmInf()->GetPeerName((sockaddr *) &addr, len);
    auto c = (ConfigClient *) client;
    uv_ip4_name(&addr, c->ip, 16);
    emxlogd("client[%s] connected\n", c->ip);
    m_clientList.push_back((ConfigClient *) client);
}

void ConfigService::OnClientDisConnect(EuvStmServerClientBase *client) {
    auto c = (ConfigClient *) client;
    m_clientList.remove(c);
    emxlogd("client[%s] disconnected\n", c->ip);
    delete c;
}

void ConfigService::OnRecv(EuvStmServerClientBase *client, char *data, int32_t size) {
    auto c = (ConfigClient *) client;
    c->data.append(data, size);
    while (c->data.size() >= c->waitSize) {
        if (c->waitHeader) {

            memcpy(&c->header.magic, c->data.data(), sizeof(ConfigHeader::magic));
            memcpy(&c->header.size, c->data.data() + sizeof(ConfigHeader::magic), sizeof(ConfigHeader::size));
            c->header.size = ntohl(c->header.size);
            auto magic = c->header.magic;
            if (magic[0] != 'E' || magic[1] != 'P' || magic[2] != 'T' || magic[3] != 'P') {
                emxloge("client[%s] magic miss match [%c,%c,%c,%c]\n",
                       c->ip, magic[0], magic[1], magic[2], magic[3]);
                DisConnectClient(c);
                return;
            }
            c->waitSize = c->header.size + ConfigHeader::HeaderSize;
            c->waitHeader = false;
            continue;
        }
        ProcCmd(c);
        memcpy((void *) c->data.data(), c->data.data() + c->waitSize, c->data.size() - c->waitSize);
        c->data.resize(c->data.size() - c->waitSize);
        c->waitSize = ConfigHeader::HeaderSize;
        c->waitHeader = true;
    }
}

void ConfigService::ProcCmd(ConfigClient *c) {
    std::string out;
    emxlogd("Req[%.*s]\n", (int32_t) c->header.size, (char *) c->data.data() + ConfigHeader::HeaderSize);
    m_proto.Proc((char *) c->data.data() + ConfigHeader::HeaderSize,
                 (int32_t) c->header.size, out);
    if (out.empty())
        return;
    ConfigHeader header = {};
    header.magic[0] = 'E';
    header.magic[1] = 'P';
    header.magic[2] = 'T';
    header.magic[3] = 'P';
    header.size = htonl(out.size());
    auto s = std::make_shared<std::string>((char *) &header, sizeof(header));
    s->append(out.data(), out.size());
    c->Send(s);
    emxlogd("Resp[%s]\n", out.data());
}
