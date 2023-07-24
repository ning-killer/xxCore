/*
* @Author: xiong
* @Date: 2023/6/1
*/

#include "NtpClientLib.hpp"
#include "NtpClient.hpp"

using namespace Emx;

NtpClientLib::NtpClientLib() {
    m_client = new NtpClient;
}

NtpClientLib::~NtpClientLib() {
    delete m_client;
}

void NtpClientLib::Create(bool newThread) {
    m_client->Create(newThread);
}

void NtpClientLib::Destroy() {
    m_client->Destroy();
}

