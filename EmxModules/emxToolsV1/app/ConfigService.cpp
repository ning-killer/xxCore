//
// Created by xiong on 2020/11/7.
//
#include "ConfigService.hpp"

using namespace Emx;

void ConfigService::CreateConfigService(Json::Value &json) {
    auto &config = json["config"];
    m_ena = config["ena"].asBool();
    if (!m_ena)
        return;
    if (m_started)
        return;
    m_port = config["port"].asInt();
    m_bufferSize = config["bufferSizeKB"].asInt() * 1024;
    m_buffer = (char *) malloc(m_bufferSize);
    m_loop.Init("ConfigService", [this]() {
        Destroy();
    });
    Create(m_loop, "0.0.0.0",
           m_port,
           config["maxClientNum"].asInt(),
           m_buffer,
           m_bufferSize);
    m_loop.Start();
}

void ConfigService::DestroyConfigService() {
    if (!m_ena)
        return;
    if (!m_started)
        return;
    m_loop.StopAndDeInit();
    free(m_buffer);
    m_started = false;
}

EuvStmServerClientBase *ConfigService::OnAllocClient() {
    return new ConfigClient(this, m_loop);
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
    if (c->update) {
        ErrCodeE e = c->update->Push((uint8_t *) data, size);
        if (e == ErrCodeE::ReachTheEof) {
            UpdatePushResult(c,0,"update success");
        }
        if (e != ErrCodeE::Success) {
            UpdatePushResult(c,400,"update failed");
        }
        return;
    }
    c->data.append(data, size);
    while (c->data.size() >= c->waitSize) {
        if (c->waitHeader) {
            memcpy(&c->header, c->data.data(), sizeof(Config::Header));
            c->header.syncCmd = htonl(c->header.syncCmd);
            c->header.size = htonl(c->header.size);
            uint16_t sync = c->header.syncCmd >> 16;
            if (sync != Config::SYNC) {
                emxloge("client[%s] miss sync\n", c->ip);
                DisConnectClient(c);
                return;
            }
            if (0x5a0e2170 == c->header.syncCmd) {
                c->update = new UpdateClient;
                c->update->Create(1000);
                //skip header and 32 md5
                if (c->update->Push((uint8_t *) c->data.data() + c->waitSize + 32,
                                    (int32_t) c->data.size() - c->waitSize - 32) != ErrCodeE::Success) {
                    UpdatePushResult(c, 400, "update failed");
                }
                return;
            }
            c->waitSize = c->header.size + sizeof(Config::Header);
            c->waitHeader = false;
            //for update

            continue;
        }
        ProcCmd(c);
        memcpy((void *) c->data.data(), c->data.data() + c->waitSize, c->data.size() - c->waitSize);
        c->data.resize(c->data.size() - c->waitSize);
        c->waitSize = sizeof(Config::Header);
        c->waitHeader = true;
    }
}

void ConfigService::UpdatePushResult(ConfigClient *c, int code, const char *desc) {
    Json::Value json;
    json["code"] = code;
    json["desc"] = desc;
    std::string jsonString;
    EasyJson::Serialize(json, jsonString);
    Config::Header header = {};
    header.syncCmd = htonl(0x5a0e2170);
    header.size = htonl(jsonString.size());
    auto s = std::make_shared<std::string>((char *) &header, sizeof(header));
    s->append(jsonString.data(), jsonString.size());
    c->Send(s);
    c->update->Destroy();
    delete c->update;
    c->update = nullptr;
}

void ConfigService::ProcCmd(ConfigClient *c) {
//        for (uint32_t i = 0; i < c->data.size(); i++) {
//            printf("0x%02x ", *(c->data.data() + i));
//        }
//    printf("\n");
    uint16_t sync = c->header.syncCmd >> 16;
    uint16_t cmd = c->header.syncCmd & 0xffff;
    if (sync != Config::SYNC)
        return;
    std::string out;
    m_protoV1.Proc((EmxProtoV1::CmdE) cmd,
                   (char *) c->data.data() + sizeof(Config::Header),
                   c->header.size,
                   out);
    if (out.empty())
        return;
    Config::Header header = {};
    header.syncCmd = htonl(cmd + (Config::SYNC << 16));
    header.size = htonl(out.size());
    auto s = std::make_shared<std::string>((char *) &header, sizeof(header));
    s->append(out.data(), out.size());
    c->Send(s);
//        for (uint32_t i = 0; i < s->size(); i++) {
//            printf("0x%02x ", *(s->data() + i));
//        }
//        printf("\n");
}
