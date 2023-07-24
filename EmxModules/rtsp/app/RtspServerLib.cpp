/*
* @Author: xiong
* @Date: 2023/6/1
*/

#include "EmxRtspServer.hpp"
#include "RtspServerLib.hpp"

using namespace Emx;


RtspServerLib::RtspServerLib() {
    m_server = new EmxRtspServer;
}

RtspServerLib::~RtspServerLib() {
    delete m_server;
}

void RtspServerLib::Create(bool newThread) {
    m_server->Create(newThread);
}

void RtspServerLib::Destroy() {
    m_server->Destroy();
}