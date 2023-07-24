/*
* @Author: xiong
* @Date: 2023/6/1
*/

#include "NetServer.hpp"
#include "NetServerLib.hpp"

using namespace Emx;

NetServerLib::NetServerLib() {
    m_server = new NetServer;
}

NetServerLib::~NetServerLib() {
    delete m_server;
}

void NetServerLib::Create(OnCreated cb, bool newThread) {
    m_server->Create(std::move(cb), newThread);
}

void NetServerLib::Destroy() {
    m_server->Destroy();
}
