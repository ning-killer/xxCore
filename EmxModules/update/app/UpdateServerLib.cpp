/*
* @Author: xiong
* @Date: 2023/6/2
*/

#include "UpdateServer.hpp"
#include "UpdateServerLib.hpp"

using namespace Emx;

UpdateServerLib::UpdateServerLib() {
    m_server = new UpdateServer;
}

UpdateServerLib::~UpdateServerLib() {
    delete m_server;
}

void UpdateServerLib::Create(bool newThread) {
    m_server->Create(newThread);
}

void UpdateServerLib::Destroy() {
    m_server->Destroy();
}
