/*
* @Author: xiong
* @Date: 2023/6/1
*/

#include "EmxOnvifService.hpp"
#include "OnvifServiceLib.hpp"

using namespace Emx;

OnvifServiceLib::OnvifServiceLib() {
    m_service = new EmxOnvifService;
}

OnvifServiceLib::~OnvifServiceLib() {
    delete m_service;
}

void OnvifServiceLib::Create(bool newThread) {
    m_service->Create(newThread);
}

void OnvifServiceLib::Destroy() {
    m_service->Destroy();
}

