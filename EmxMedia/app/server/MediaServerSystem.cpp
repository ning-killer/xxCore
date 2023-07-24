//
// Created by xiong on 2020/10/27.
//
#include "EmxCore.hpp"
#include "MediaServerSystem.hpp"
#include "MediaConfigs.hpp"
#include "MediaFactory.hpp"
#include "MediaServerVenc.hpp"

using namespace Emx;
MediaServerSystem *MediaServerSystem::m_inst;

ErrCodeE MediaServerSystem::Create(MediaServerResource &res) {
    m_inst = res.factory->MakeSystem(res);
    return m_inst->Start();
}

void MediaServerSystem::Destroy() {
    m_inst->Stop();
    delete m_inst;
}

ErrCodeE MediaServerSystem::OnRecvReq(uint32_t module, uint32_t method,
                                      const char *data, int32_t size, std::string &out) {
    return m_inst->OnRecvReqSub(module, method, data, size, out);
}

ErrCodeE MediaServerSystem::OnRecvReqSub(uint32_t module, uint32_t method,
                                         const char *data, int32_t size, std::string &out) {
    emxloge("not support method[%u]\n", method);
    return ErrCodeE::OperationNotSupport;
}
