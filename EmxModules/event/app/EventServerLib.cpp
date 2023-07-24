/*
* @Author: xiong
* @Date: 2023/6/1
*/

#include "EventServer.hpp"
#include "EventServerLib.hpp"

using namespace Emx;

EventServerLib::EventServerLib() {
    m_server = new EventServer;
}

EventServerLib::~EventServerLib() {
    delete m_server;
}

void EventServerLib::Create(bool newThread) {
    m_server->Create(newThread);
}

void EventServerLib::Destroy() {
    m_server->Destroy();
}
