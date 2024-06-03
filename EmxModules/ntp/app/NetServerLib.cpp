/*
* @Author: xiong
* @Date: 2023/6/1
*/

#include "NtpServer.hpp"
#include "NtpServerLib.hpp"

using namespace Emx;

NtpServerLib::NtpServerLib() { m_server = new NtpServer; }

NtpServerLib::~NtpServerLib() { delete m_server; }

void NtpServerLib::Create(OnCreated cb, bool newThread) { m_server->Create(std::move(cb), newThread); }

void NtpServerLib::Destroy() { m_server->Destroy(); }
