//
// Created by xiong on 2021/5/20.
//
#include "EmxToolsV1.hpp"
#include "DetectionService.hpp"
#include "ConfigService.hpp"
#include "StreamService.hpp"

using namespace Emx;

int main(int argc, char *argv[]) {
    if (!Misc::ApplicationExist("EmxCoreServer")) {
        emxloge("not found EmxCoreServer\n");
        return -1;
    }
    Misc::KillBeforeRun(argv[0]);
    signal(SIGPIPE, SIG_IGN);
    // 第一个参数为-b代表后台执行
    if (argc > 1 && strcmp(argv[1], "-b") == 0) {
        signal(SIGCHLD, SIG_IGN);
        signal(SIGHUP, SIG_IGN);
        pid_t pid = fork();
        if (pid < 0) {
            printf("fork failed");
            exit(-1);
        } else if (pid != 0) {
            return 0;
        }
    }
    char path[EMX_MAX_PATH_SIZE] = {};
    snprintf(path, sizeof(path), "%s/emxTools.json", Misc::GetAppConfigsDir());
    if (!File::Exist(path)) {
        printf("%s not exist\n", path);
        return -1;
    }
    EmxToolsV1 tools;
    tools.Create(path);
    return 0;
}

EmxToolsV1::EmxToolsV1() {
    m_detection = new DetectionService;
    m_config = new ConfigService;
    m_stream = new StreamService;
    m_managecfg = new ManageCfgFile(&m_loop);
}

EmxToolsV1::~EmxToolsV1() {
    delete m_detection;
    delete m_config;
    delete m_stream;
    delete m_managecfg;
}

void EmxToolsV1::Create(const char *path) {
    if (EasyJson::Load(path, m_json) != ErrCodeE::Success) {
        emxloge("load configure file failed\n");
        return;
    }
    m_isRunning = m_json["runAfterLaunching"].asBool();
    m_managecfg->Load();
    if (m_isRunning && (m_managecfg->GetFactoryMode() != ExitFactory)) {
        m_detection->Create(m_json);
        m_config->CreateConfigService(m_json);
        m_stream->CreateService(m_json);
        emxlogd("factory tools started\n");
    } else {
        emxlogd("factory tools no started\n");
    }
    m_loop.Init("EmxFcToolsV1", [this]() {
        m_resp.DestroyMsgRpcRep();
    }, EuvLoop::Default);
    MsgRpcRep::Config cfg = {};
    cfg.loop = &m_loop;
    cfg.clientId = EmxToolsMsgId;
    cfg.buffer = m_buffer;
    cfg.size = EMX_TOOLS_BUFFER_SIZE;
    cfg.OnConnected = []() {};
    cfg.OnDisConnected = []() {};
    cfg.OnRecvReq = std::bind(&EmxToolsV1::OnRecvMsg, this, ph_1, ph_2, ph_3, ph_4, ph_5);
    m_resp.CreateMsgRpcRep(cfg);
    m_managecfg->Start();
    emxlogi("EmxFcToolsV1 Created\n");
    m_loop.Start(false);
}

ErrCodeE EmxToolsV1::OnRecvMsg(uint32_t module, uint32_t method,
                                 const char *data, int32_t size, std::string &out) {
    emxlogd("module: %d; method: %d\n", module, method);
    if ((EmxToolsModule) module == EmxToolsModule::Tools) {
        if ((EmxToolsMethod) method == EmxToolsMethod::Start) {
            if (!m_isRunning) {
                m_detection->Create(m_json);
                m_config->CreateConfigService(m_json);
                m_stream->CreateService(m_json);
                m_isRunning = true;
                emxlogi("tools started\n");
            } else {
                emxlogw("tools is running\n");
            }
        } else if ((EmxToolsMethod) method == EmxToolsMethod::Stop) {
            if (m_isRunning) {
                m_detection->Destroy();
                m_config->DestroyConfigService();
                m_stream->DestroyService();
                m_isRunning = false;
                emxlogi("tools stopped\n");
            } else {
                emxlogw("tools is not running\n");
            }
        } else if ((EmxToolsMethod) method == EmxToolsMethod::GetFactoryMode) {
            int mode = (int)m_managecfg->GetFactoryMode();
            out.assign((char *) &mode, sizeof(mode));
        } else {
            return ErrCodeE::OperationNotSupport;
        }
    } else {
        return ErrCodeE::ResNotExist;
    }
    return ErrCodeE::Success;
}
