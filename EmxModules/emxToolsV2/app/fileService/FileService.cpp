/*
* @Author: xiong
* @Date: 2023/2/14
*/

#include "FileService.hpp"
#include "FileClient.hpp"

using namespace Emx;

void FileService::CreateFileService(Resource *res, Json::Value &json) {
    m_res = res;
    m_port = json["port"].asInt();
    Create(m_res->loop, "0.0.0.0",
           m_port,
           json["maxClientNum"].asInt(),
           m_res->buffer,
           m_res->bufferSize);
}

void FileService::DestroyFileService() {
    Destroy();
}

EuvStmServerClientBase *FileService::OnAllocClient() {
    return new FileClient(this, m_res);
}

void FileService::OnClientConnect(EuvStmServerClientBase *client) {
    sockaddr_in addr = {};
    size_t len = sizeof(addr);
    client->GetStmInf()->GetPeerName((sockaddr *) &addr, len);
    auto c = (FileClient *) client;
    char ip[16];
    uv_ip4_name(&addr, ip, 16);
    c->SetClientIp(ip);
    emxlogd("client[%s] connected\n", ip);
    m_clientList.push_back((FileClient *) client);
}

void FileService::OnClientDisConnect(EuvStmServerClientBase *client) {
    auto c = (FileClient *) client;
    m_clientList.remove(c);
    emxlogd("client[%s] disconnected\n", c->GetClientIp());
    delete c;
}

void FileService::OnRecv(EuvStmServerClientBase *client, char *data, int32_t size) {
    ((FileClient *) client)->OnRecvData(data, size);
}
