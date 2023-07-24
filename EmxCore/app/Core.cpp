//
// Created by xiong on 2021/7/19.
//

#include "Core.hpp"
#include "LogServer.hpp"
#include "MsgRpcServer.hpp"
#include "MsgTopicServer.hpp"
#include "ParamServer.hpp"
#include "CoreConfigs.hpp"

using namespace Emx;

void Core::Create(const char *cfgPath) {
    if (m_created)
        return;
    CoreConfigs::GetInst()->SetCfgPath(cfgPath);
    if (CoreConfigs::GetInst()->Load() != ErrCodeE::Success)
        return;

    m_log = new LogServer;
    m_log->Create();
    m_rpc = new MsgRpcServer;
    m_rpc->CreateMsgRpcServer();
    m_topic = new MsgTopicServer;
    m_topic->CreateMsgTopicServer();
    m_param = new ParamServer;
    m_param->Create();
    m_created = true;
}

void Core::Destroy() {
    if (m_created) {
        m_param->Destroy();
        delete m_param;
        m_rpc->DestroyMsgRpcServer();
        delete m_rpc;
        m_topic->DestroyMsgTopicServer();
        delete m_topic;
        m_log->Destroy();
        delete m_log;
        m_created = false;
    }
}
