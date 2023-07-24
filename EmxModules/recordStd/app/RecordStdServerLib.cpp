/*
* @Author: xiong
* @Date: 2023/6/1
*/

#include "RecordStdServer.hpp"
#include "RecordStdServerLib.hpp"

using namespace Emx;


RecordStdServerLib::RecordStdServerLib() {
    m_server = new RecordStdServer;
}

RecordStdServerLib::~RecordStdServerLib() {
    delete m_server;
}

void RecordStdServerLib::Create(bool newThread) {
    m_server->Create(newThread);
}

void RecordStdServerLib::Destroy() {
    m_server->Destroy();
}
