
#include "ModulesServer.hpp"
#include "libconfig.h"
#include "Sn.hpp"

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
#ifdef MERGE_PROCESS
    ModulesServer tool;
    tool.Create();
#endif // MERGE_PROCESS
    return 0;
}

ModulesServer::ModulesServer() {
}

ModulesServer::~ModulesServer() {
    Destroy();
}

ErrCodeE ModulesServer::Create() {
    m_loop.Init("AgeingServer", [this]() {
        m_subModules.Destroy();
        Destroy();
    }, EuvLoop::Default);
    m_factoryMode = GetFactoryMode();
    m_subModules.Create(m_loop);
    m_subModules.Start(0, 0, [this]() {
        CreateSubSystem();
    });
    m_loop.Start(false);
    return ErrCodeE::Success;
}

ErrCodeE ModulesServer::Destroy() {
    m_netServer.Destroy();
    // m_ntpClient.Destroy();
    m_sdcardServer.Destroy();
    m_rtspServer.Destroy();
    m_updateServer.Destroy();
    m_nightServer.Destroy();
    m_sdcardUpdate.Destroy();
    return ErrCodeE::Success;
}

void ModulesServer::CreateSubSystem() {
    m_netServer.Create(nullptr, true);
    // m_ntpClient.Create(true);
    m_sdcardServer.Create(nullptr, true);
    if (m_factoryMode != ExitFactory) {
        m_rtspServer.Create(true);
    }
    m_updateServer.Create(true);
    m_nightServer.Create(nullptr, true);
    m_sdcardUpdate.Create(true);
}

bool ModulesServer::GetZnDebugConfig(ZnDebugInfo *info, const char* path) {
    bool ret = false;
    config_t cfg;
    config_setting_t *cfgSetting = nullptr;
    bool is_need_destroy = false;
    do {
        if (!File::Exist(path)) {
            emxloge("%s not exitst\n", path);
            break;
        }
        config_init(&cfg);
        is_need_destroy = true;
        if(!config_read_file(&cfg, path)) {
            emxloge(" %s;%d;%s\n"
                    , config_error_file(&cfg)
                    , config_error_line(&cfg)
                    , config_error_text(&cfg));
            break;
        }
        cfgSetting = config_lookup(&cfg, "factory_mode");
        if (cfgSetting != nullptr) {
            info->factory_mode = config_setting_get_bool(cfgSetting);
        }
        cfgSetting = config_lookup(&cfg, "agingtest");
        if (cfgSetting != nullptr) {
            info->agingtest = config_setting_get_bool(cfgSetting);
        }
        cfgSetting = config_lookup(&cfg, "agingtest_time");
        if (cfgSetting != nullptr) {
            info->agingtest_time = config_setting_get_int(cfgSetting) * 50;
        }
        cfgSetting = config_lookup(&cfg, "ircut_interval");
        if (cfgSetting != nullptr) {
            info->ircut_interval = config_setting_get_int(cfgSetting);
        }
        ret = true;
    } while(false);
    if (is_need_destroy) {
        config_destroy(&cfg);
    }
    return ret;
}

FactoryMode ModulesServer::GetFactoryMode() {
    FactoryMode mode = ExitFactory;
    Sn sn;
    if (sn.Load() != ErrCodeE::Success) {
        mode = EnterFactory;
        return mode;
    }
    if (std::string(sn.Get(), strlen(sn.Get())).size() == 0) {
        mode = EnterFactory;
        return mode;
    }
    char path[64] = { 0 };
    snprintf(path, sizeof(path) - 1, "/mnt/sdcard/zn_debug");
    if (!File::Exist(path)) {
        emxlogd("%s not exitst\n", path);
        return mode;
    }
    mode = KeepFactory;
    ZnDebugInfo info;
    if (!GetZnDebugConfig(&info, path)) {
        emxloge("GetZnDebugConfig failed\n");
        return mode;
    }
    if (info.factory_mode) {
        mode = EnterFactory;
    }
    return mode;
}
    