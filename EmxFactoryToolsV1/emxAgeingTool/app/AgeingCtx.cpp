#include "AgeingCtx.hpp"
#include "EmxBsp.hpp"
#include "EmxMedia.hpp"
#include "NightVisionClient.hpp"
#include "libconfig.h"
using namespace Emx; 

AgeingCtx::AgeingCtx(EuvLoop *loop)
    : m_loop(loop) 
    , m_started(false) 
    , m_isstop(false) {
}

AgeingCtx::~AgeingCtx() {
    m_timer.Destroy();
    SetIsStop(true);
    m_started = false;
}

bool AgeingCtx::GetIsStop() {
    {
        lock_guard<mutex> locker(m_stopflag_locker);
        return m_isstop;
    }
}

void AgeingCtx::SetIsStop(const bool is_stop) {
    lock_guard<mutex> locker(m_stopflag_locker);
    m_isstop = is_stop;
}

void AgeingCtx::Start() {
    do {
        if (m_started) {
            break;
        }
        if (m_loop == nullptr) {
            break;
        }
        if (!m_zndebug_info.agingtest) {
            break;
        }
        m_started = true;

        //note: 老化开始语音播放
        char path[EMX_MAX_PATH_SIZE] = { 0 };
        snprintf(path, sizeof(path) - 1, "%s/audio/aginging.aac", Misc::GetAppConfigsDir());
        MediaClientAdecStreamSync::PlayFileWithVolume(0, path, 38);

        //note: 老化状态文件重置
        ResetAgeingInfo();

        //note: 老化状态文件初始化
        m_aging_info.aging_state = Ageing_Running;
        SaveAgeingInfo(m_aging_info);

        //note: 老化线程组标志位重置
        SetIsStop(false);

        //note: 定时器启动，以管理计数老化时间
        m_timer.Create(*m_loop);
        m_timer.Start(0, 1000, [this]() {
            if ((--m_zndebug_info.agingtest_time) == 0) {
                Stop();
            }
        });

        //note: 新建线程资源去绑定耗时老化任务，剥离timer防止对主loop事件操作拉低效率
        if (!m_led_work.IsWorking()) {
            StartLedTest();
        }
        if (!m_voice_work.IsWorking()) {
            StartVoiceTest();
        }
        if (!m_ircut_work.IsWorking()) {
            StartIrcutTest();
        }
    #ifdef PTZ_SUPPORT
        if (!m_ptz_work.IsWorking()) {
            StartPtzTest();
        }
    #endif
    } while(false);
    return;
}

void AgeingCtx::Stop() {
    //note: 老化线程组标志位重置
    SetIsStop(true);
    //note: 等待线程耗时操作退出
    usleep(1000 * 1000);

    //note: 老化结束语音播放
    // char path[EMX_MAX_PATH_SIZE] = { 0 };
    // snprintf(path, sizeof(path) - 1, "%s/audio/aging_end.aac", Misc::GetAppConfigsDir());
    // MediaClientAdecStreamSync::PlayFileWithVolume(0, path, 38);

    //note: 老化状态文件更新
    m_aging_info.aging_state = Ageing_Finish;
    SaveAgeingInfo(m_aging_info);

    m_started = false;
}

void AgeingCtx::StartLedTest() {
    emxlogd("start led ageing\n");
    m_led_work.Create(m_loop, nullptr,
                            [this](void *arg) {
                                Bsp bsp;
                                int chns = bsp.led->GetChnsNum();
                                if (chns <= 0) {
                                    emxlogd("StartLedTest failed, no led\n");
                                    return;
                                }
                                while(!GetIsStop()) {
                                    for (int chn = 0; chn < chns; ++chn) {
                                        bsp.led->Set(true, chn);
                                        usleep(500 * 1000);
                                        bsp.led->Set(false, chn);
                                        usleep(500 * 1000);
                                    }
                                }
                            },
                            [this](ErrCodeE e, void *arg) {
                                m_led_work.Destroy();
                            });
    m_led_work.Run();
}

void AgeingCtx::StartVoiceTest() {
    emxlogd("start voice ageing\n");
    m_voice_work.Create(m_loop, nullptr,
                            [this](void *arg) {
                                char audioDir[EMX_MAX_PATH_SIZE] = {};
                                snprintf(audioDir, sizeof(audioDir), "%s/audio", Misc::GetAppConfigsDir());
                                DIR *dp = opendir(audioDir);
                                if (!dp) {
                                    emxlogd("StartVoiceTest failed, no audio file\n");
                                    return;
                                }
                                vector<string> paths;
                                dirent *entry;
                                while ((entry = readdir(dp)) != nullptr) {
                                    if (strstr(entry->d_name, ".aac") 
                                        && !strstr(entry->d_name, "aging") 
                                        && !strstr(entry->d_name, "factory")) {
                                        char path[EMX_MAX_PATH_SIZE] = { 0 };
                                        snprintf(path, sizeof(path) - 1, "%s/%s", audioDir, entry->d_name);
                                        paths.push_back(std::string(path,sizeof(path)));
                                    }
                                }
                                closedir(dp);
                                
                                while(!GetIsStop()) {
                                    for (auto path : paths) {
                                        MediaClientAdecStreamSync::PlayFileWithVolume(0, path.c_str(), 38);
                                        usleep(1000 * 1000);
                                    }
                                }
                            },
                            [this](ErrCodeE e, void *arg) {
                                MediaClientAdecStreamSync::PlayCtrlQuit(0);
                                char path[EMX_MAX_PATH_SIZE] = { 0 };
                                snprintf(path, sizeof(path) - 1, "%s/audio/aging_end.aac", Misc::GetAppConfigsDir());
                                MediaClientAdecStreamSync::PlayFileWithVolume(0, path, 38);
                                m_voice_work.Destroy();
                            });
    m_voice_work.Run();

}

void AgeingCtx::StartIrcutTest() {
    emxlogd("start ircut ageing\n");
    m_ircut_work.Create(m_loop, nullptr,
                            [this](void *arg) {
                                NightVisionClient nv;
                                NightVision::Param param = { 0 };
                                NightVision::Param saved_param = { 0 };
                                memcpy(&saved_param, &param, sizeof(NightVision::Param));
                                if (nv.GetParam(param) != ErrCodeE::Success) {
                                    emxlogd("StartIrcutTest failed, GetParam error\n");
                                    return;
                                }
                                //老化验证手动模式
                                param.manual = true;
                                bool is_change_mode = true;
                                while(!GetIsStop()) {
                                    if (is_change_mode) {
                                        param.manualMode = NightVision::ModeE::ColorNightVision;
                                        is_change_mode = false;
                                    } else {
                                        param.manualMode = NightVision::ModeE::IrNightVision;
                                        is_change_mode = true;
                                    }
                                    
                                    nv.SetParam(param);
                                    int count = 10;
                                    while(count && !GetIsStop()) {
                                        //细化睡眠时间，保证退出正常
                                        --count;
                                        usleep((m_zndebug_info.ircut_interval * 1000 * 1000) / 10);
                                    }
                                }
                                param.manualMode = NightVision::ModeE::Day;
                                nv.SetParam(param);
                                nv.SetParam(saved_param);
                            },
                            [this](ErrCodeE e, void *arg) {
                                m_ircut_work.Destroy();
                            });
    m_ircut_work.Run();
}
        
void AgeingCtx::StartPtzTest() {
    //note: 暂未支持
}

ErrCodeE AgeingCtx::InitAgeingFile(const char* path) {
    //note: 外层有安全锁保护，此处不要加锁嵌套，造成死锁
    if (File::Exist(path)) {
        emxlogd("%s is exitst\n", AGING_FILE);
        return ErrCodeE::Success;
    }
    char data[128] = { 0 };
    FILE *fp = fopen(AGING_FILE, "w+");
    if (!fp) {
        emxlogd("create ageing cfg file failed\n");
        return ErrCodeE::Failure;
    }
    strcpy(data, "agingState = 0;");
    data[strlen(data)] = '\n';
    fputs(data, fp);
    bzero(data, sizeof(data));
    strcpy(data, "time = 0;");
    data[strlen(data)] = '\n';
    fputs(data, fp);
    bzero(data, sizeof(data));
    strcpy(data, "mediaState = 0;");
    data[strlen(data)] = '\n';//写入换行符号
    fputs(data, fp);//写入字符串
    fclose(fp);
    fp = nullptr;
    return ErrCodeE::Success;
}

ErrCodeE AgeingCtx::SaveAgeingInfo(const AgingInfo &info) {
    ErrCodeE ret = ErrCodeE::Failure;
    config_t cfg;
    config_setting_t *cfgSetting = nullptr;
    bool is_need_destroy = false;
    do {
        {
            lock_guard<mutex> locker(m_agingfile_locker);
            if (InitAgeingFile(AGING_FILE) != ErrCodeE::Success) {
                emxlogd("InitAgeingFile(%s) failed\n", AGING_FILE);
                break;
            }
            config_init(&cfg);
            is_need_destroy = true;
            if(!config_read_file(&cfg, AGING_FILE)) {
                emxloge(" %s;%d;%s\n"
                            , config_error_file(&cfg)
                            , config_error_line(&cfg)
                            , config_error_text(&cfg));
                break;
            }
            cfgSetting = config_lookup(&cfg, "agingState");
            if (cfgSetting) {
                config_setting_set_int(cfgSetting, info.aging_state);
            }
            cfgSetting = config_lookup(&cfg, "time");
            if (cfgSetting) {
                config_setting_set_int(cfgSetting, info.time);
            }
            cfgSetting = config_lookup(&cfg, "mediaState");
            if (cfgSetting) {
                config_setting_set_int(cfgSetting, info.media_state);
            }
            config_write_file(&cfg, AGING_FILE);
            ret = ErrCodeE::Success;
        }
    } while(false);

    if (is_need_destroy) {
        {
            lock_guard<mutex> locker(m_agingfile_locker);
            config_destroy(&cfg);
        }
    }
    return ret;
}

ErrCodeE AgeingCtx::LoadAgeingInfo(AgingInfo &info) {
    ErrCodeE ret = ErrCodeE::Failure;
    config_t cfg;
    config_setting_t *cfgSetting = nullptr;
    bool is_need_destroy = false;
    
    do {
        {
            lock_guard<mutex> locker(m_agingfile_locker);
            if (!File::Exist(AGING_FILE)) {
                emxloge("%s not exitst\n", AGING_FILE);
                break;
            }
            
            config_init(&cfg);
            is_need_destroy = true;
            if(!config_read_file(&cfg, AGING_FILE)) {
                emxloge(" %s;%d;%s\n"
                        , config_error_file(&cfg)
                        , config_error_line(&cfg)
                        , config_error_text(&cfg));
                break;
            }

            cfgSetting = config_lookup(&cfg, "agingState");
            if (cfgSetting) {
                info.aging_state = (AgeingStatus)config_setting_get_int(cfgSetting);
            }
            cfgSetting = config_lookup(&cfg, "time");
            if (cfgSetting) {
                info.time = config_setting_get_int(cfgSetting);
            }
            cfgSetting = config_lookup(&cfg, "mediaState");
            if (cfgSetting) {
                info.media_state = (AgeingResult)config_setting_get_int(cfgSetting);
            }
            ret = ErrCodeE::Success;
        }
    } while(false);

    if (is_need_destroy) {
        {
            lock_guard<mutex> locker(m_agingfile_locker);
            config_destroy(&cfg);
        }
    }
    return ret;
}

ErrCodeE AgeingCtx::LoadZnDebugInfo(const char* path) {
    ErrCodeE ret = ErrCodeE::Failure;
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
            m_zndebug_info.factory_mode = config_setting_get_bool(cfgSetting);
        }
        cfgSetting = config_lookup(&cfg, "agingtest");
        if (cfgSetting != nullptr) {
            m_zndebug_info.agingtest = config_setting_get_bool(cfgSetting);
        }
        cfgSetting = config_lookup(&cfg, "agingtest_time");
        if (cfgSetting != nullptr) {
            m_zndebug_info.agingtest_time = config_setting_get_int(cfgSetting) * 60;
        }
        cfgSetting = config_lookup(&cfg, "ircut_interval");
        if (cfgSetting != nullptr) {
            m_zndebug_info.ircut_interval = config_setting_get_int(cfgSetting);
        }
        ret = ErrCodeE::Success;
    } while(false);

    if (is_need_destroy) {
        config_destroy(&cfg);
    }
    return ret;
}

void AgeingCtx::ResetAgeingInfo() {
    lock_guard<mutex> locker(m_agingfile_locker);
    remove(AGING_FILE);
}