/*
* @Author: xiong
* @Date: 2023/5/31
*/

#include <utility>
#include "SDCardServer.hpp"
#include "SDCardServerLib.hpp"

using namespace Emx;

SDCardServerLib::SDCardServerLib() {
    m_server = new SDCardServer;
}

SDCardServerLib::~SDCardServerLib() {
    delete m_server;
}

void SDCardServerLib::Create(SDCardServerLib::OnCreated cb, bool newThread) {
    m_server->Create(std::move(cb), newThread);
}

void SDCardServerLib::Destroy() {
    m_server->Destroy();
}

