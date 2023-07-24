/*
* @Author: xiong
* @Date: 2023/6/2
*/

#include "WebServer.hpp"
#include "WebServerLib.hpp"

using namespace Emx;

WebServerLib::WebServerLib() {
    m_server = new WebServer;
}

WebServerLib::~WebServerLib() {
    delete m_server;
}

void WebServerLib::Create(bool newThread) {
    m_server->Create(newThread);
}

void WebServerLib::Destroy() {
    m_server->Destroy();
}
