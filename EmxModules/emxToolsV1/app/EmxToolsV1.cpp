//
// Created by xiong on 2021/5/20.
//
#include "EmxToolsV1.hpp"
#include "ToolsInner.hpp"
#include "DetectionService.hpp"
#include "ConfigService.hpp"
#include "StreamService.hpp"

using namespace Emx;

EmxToolsV1::EmxToolsV1() {
    m_detection = new DetectionService;
    m_config = new ConfigService;
    m_stream = new StreamService;
}

EmxToolsV1::~EmxToolsV1() {
    Destroy();
}

void EmxToolsV1::Create(bool newThread) {
    auto path = new std::string(Misc::GetAppConfigsDir());
    path->append("/emxTools.json");
    if (!File::Exist(path->c_str())) {
        printf("%s not exist\n", path->c_str());
        delete path;
        return;
    }
    if (EasyJson::Load(path->c_str(), m_json) != ErrCodeE::Success) {
        emxloge("load %s failed\n", path->c_str());
        delete path;
        return;
    }
    delete path;
    m_isRunning = m_json["runAfterLaunching"].asBool();
    if (m_isRunning) {
        m_detection->Create(m_json);
        m_config->CreateConfigService(m_json);
        m_stream->CreateService(m_json);
        emxlogi("tools started\n");
    }
    m_loop.Init("EmxToolsV1", [this]() {
        m_resp.DestroyMsgRpcRep();
    }, newThread ? EuvLoop::ModeE::Dynamic : EuvLoop::Default);
    MsgRpcRep::Config cfg = {};
    cfg.loop = &m_loop;
    cfg.clientId = EMX_TOOLS_MSG_ID;
    cfg.buffer = m_buffer;
    cfg.size = EMX_TOOLS_BUFFER_SIZE;
    cfg.OnConnected = []() {};
    cfg.OnDisConnected = []() {};
    cfg.OnRecvReq = std::bind(&EmxToolsV1::OnRecvMsg, this, ph_1, ph_2, ph_3, ph_4, ph_5);
    m_resp.CreateMsgRpcRep(cfg);
    emxlogi("EmxToolsV1 Created\n");
    m_loop.Start(newThread);
}

void EmxToolsV1::Destroy() {
    m_loop.StopAndDeInit();
    if (m_isRunning) {
        m_detection->Destroy();
        m_config->DestroyConfigService();
        m_stream->DestroyService();
        m_isRunning = false;
        emxlogi("tools stopped\n");
    }
    delete m_detection;
    delete m_config;
    delete m_stream;
}

ErrCodeE EmxToolsV1::OnRecvMsg(uint32_t module, uint32_t method,
                               const char *data, int32_t size, std::string &out) {
    if ((EmxToolsModuleE) module == EmxToolsModuleE::Tools) {
        if ((EmxToolsMethodE) method == EmxToolsMethodE::Start) {
            if (!m_isRunning) {
                m_detection->Create(m_json);
                m_config->CreateConfigService(m_json);
                m_stream->CreateService(m_json);
                m_isRunning = true;
                emxlogi("tools started\n");
            } else {
                emxlogw("tools is running\n");
            }
        } else if ((EmxToolsMethodE) method == EmxToolsMethodE::Stop) {
            if (m_isRunning) {
                m_detection->Destroy();
                m_config->DestroyConfigService();
                m_stream->DestroyService();
                m_isRunning = false;
                emxlogi("tools stopped\n");
            } else {
                emxlogw("tools is not running\n");
            }
        } else {
            return ErrCodeE::OperationNotSupport;
        }
    } else {
        return ErrCodeE::ResNotExist;
    }
    return ErrCodeE::Success;
}

