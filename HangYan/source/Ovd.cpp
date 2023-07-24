//
// Created by xiong on 2022/6/23.
//

#include "Ovd.hpp"
#include "VoicePlay.hpp"
#include "EmxCore.hpp"
#include "libconfig.h"
#include "Gat1400Client.hpp"

using namespace Emx;

#define OVD_LOOP_BUFFER_SIZE (64*1024)

bool Ovd::GetZnDebugConfig(ZnDebugInfo *info, const char* path) {
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

FactoryMode Ovd::GetFactoryMode() {
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

void Ovd::Create() {
    FactoryMode m_factoryMode = GetFactoryMode();
    //检验工厂模式
    if (m_factoryMode == UnknownFactory 
        || m_factoryMode == EnterFactory) {
        emxloge("factory mode is (%d), EapilMain exit\n", m_factoryMode);
        return;
    }
    m_ctx.loop.Init("OVD", std::bind(&Ovd::OnQuit, this));

    //加载设备差异性配置文件：例如支持的网络类型等
    char path[EMX_MAX_PATH_SIZE] = {};
    snprintf(path, sizeof(path), "%s/ovd/ovdDeviceCfg.json", Misc::GetAppConfigsDir());
    EasyJson::Load(path, m_ctx.deviceJsonCfg);

    //加载所有参数
    if (m_ctx.env.LoadAll() != ErrCodeE::Success) {
        emxlogc("load OVD env failed\n");
        return;
    }

    //配置时区
    m_ctx.env.timeZone.CopyTimeZoneToLocalTime();

    m_timeSync = false;
    m_startService = true;
    m_connected = false;

    //初始化OVD的大loop，放到ctx中供各个模块使用
    m_ctx.buffer = (char *) malloc(OVD_LOOP_BUFFER_SIZE);//给loop中各个模块使用的公共缓冲区
    m_ctx.bufferSize = OVD_LOOP_BUFFER_SIZE;

    //注册当设备通过OVD SDK的回调同步时间后的回调函数
    m_ctx.timeSyncInfo.Create(m_ctx.loop, std::bind(&Ovd::OnTimeSync, this));

    //创建Led灯逻辑控制模块
    m_ctx.led = LedFactory::Make(m_ctx.loop, m_ctx.bsp.led.get(), m_ctx.deviceJsonCfg["ledType"].asCString());
    m_ctx.led->Create();

    //启动各个模块，这里使用一个立刻超时的timer作为launcher触发器，保证各个模块在loop中启动
    m_launcher.Create(m_ctx.loop);
    m_launcher.Start(0, 0, [this]() {
        //开始监听OVD回调
        m_cbResp.Start();
        //初始化sdk
        m_sdk.Init(&m_ctx, std::bind(&Ovd::OnInitSDKDone, this));
    });

    //开始大循环
    m_ctx.loop.Start(false);
}

void Ovd::Destroy() {
    m_ctx.loop.StopAndDeInit();
    free(m_ctx.buffer);
    delete m_ctx.led;
#ifndef EAPIL_SPARE
    delete m_key;
#endif
}

void Ovd::OnQuit() {
    m_launcher.Destroy();
    m_ctx.led->Destroy();
#ifndef EAPIL_SPARE
    if (m_key)
        m_key->Destroy();
#endif
    m_sdk.DeInit();
}

void Ovd::OnTimeSync() {
    if (!m_timeSync) {
        m_timeSync = true;
        if (m_connected) {
            emxlogi("time sync\n");
            ServiceStart();
        }
    }
}

void Ovd::OnInitSDKDone() {
    m_net.Create(&m_ctx,
                 std::bind(&Ovd::OnNetInitDone, this),
                 std::bind(&Ovd::OnNetConnectionChanged, this, ph_1));
}

void Ovd::OnNetInitDone() {
    if (m_net.GetMainDev() == Net::DevE::Wifi && !m_ctx.env.wifi.valid) {
        m_net.WifiConfig();
    }
#ifndef EAPIL_SPARE
    //创建复位按键事件监测模块
    m_key = new Key("Reset", m_ctx.loop, m_ctx.bsp.keyReset.get(), 3000);
    m_key->Create(std::bind(&Ovd::OnResetKeyEvent, this, ph_1, ph_2));
#endif
    //监听升级状态改变
    m_updateEvent.Create(&m_ctx.loop, m_ctx.buffer, m_ctx.bufferSize,
                         std::bind(&Ovd::OnUpdateEvent, this, ph_1));
    m_ctx.led->StatChange(Led::StatE::StartingUp, false);
}

void Ovd::OnNetConnectionChanged(bool connected) {
    emxlogi("net connection status changed [%s]\n", connected ? "connected" : "disconnected");
    m_connected = connected;
    Gat1400Client::SetNetWorkStatus(m_connected);
    if (m_connected) {
        OVD_ServiceStart();
    }
    if (m_connected && m_timeSync) {
        //设备服务准备好后调用，即设备连接网络成功、音视频接口准备完毕等工作之后再调用
        ServiceStart();
    }
}

#ifndef EAPIL_SPARE

void Ovd::OnResetKeyEvent(const char *name, Key::EventE e) {
    switch (e) {
        case Key::EventE::Click: {
            VoicePlay::Play(VoicePlay::VoiceE::DI);
            ServiceStop();
            //结束OVD媒体服务,结束动作会触发杭研SDK的OVCConnectStatus回调，会造成死锁，放到线程池中运行
            auto work = new EuvWork;
            work->Create(&m_ctx.loop, work, [](void *arg) {
                OVD_ServiceStop();
            }, [](ErrCodeE e, void *arg) {
                delete (EuvWork *) arg;
            });
            auto dev = m_net.GetMainDev();
            if (dev == Net::DevE::Wifi) {
                m_net.WifiConfig();
            } else if (dev == Net::DevE::Eth) {
                NetClient net(dev);
                net.ReLink();
            }
            break;
        }
        case Key::EventE::LongPress:
            VoicePlay::Play(VoicePlay::VoiceE::DI);
            m_record.Destroy();
            Param::ResetAllToFactory();
            Reboot::DoReboot(0);
            break;
        default:
            break;
    }
}

#endif

void Ovd::OnUpdateEvent(UpdateEvent::Event &e) {
    m_ctx.running.updateEvent = e;
    if (e.stat == UpdateEvent::StatE::Start) {
        m_ctx.led->StatChange(Led::StatE::Updating, true);
        if (m_ctx.deviceJsonCfg["releaseMemoryBeforeUpdate"].asBool()) {
            ServiceStop();
            //结束OVD媒体服务
            OVD_ServiceStop();
            MediaClientServer::DestroyMedia();
        }
    } else {
        m_ctx.led->StatChange(Led::StatE::Updating, false);
    }
}

void Ovd::ServiceStart() {
    if (m_serviceStarted)
        return;
    emxlogd("into\n");
#ifndef EAPIL_SPARE
    //启动休眠排程
    m_scheduleSleep.Create("sleep", &m_ctx, &m_ctx.env.sleep.schedule, [this](bool ena) {
        emxlogi("sleep schedule active %d\n", ena);
        m_ctx.running.sleep = !ena;
        if (m_ctx.running.sleep) {
            m_record.Destroy();
            m_scheduleInv.Destroy();
            Gat1400Client::Instance()->Stop();
            m_alarm.Destroy();
            m_avPush.Destroy();
            //夜视配置为手动白天模式
            NightVisionClient nightVision;
            NightVision::Param param = {};
            param.manual = true;
            param.manualMode = NightVision::ModeE::Day;
            nightVision.SetParamTemp(param);
            m_andLink.Destroy();
            m_voip.Destroy();
        } else {
            //恢复持久化夜视配置
            NightVisionClient nightVision;
            NightVision::Param param = {};
            nightVision.GetParam(param);
            nightVision.SetParamTemp(param);
            //启动音视频实时流推送
            m_avPush.Create(&m_ctx);

            //启动录像
            m_record.Create(&m_ctx);

            //启动gat1400业务
            Gat1400Client::Instance()->Create(&m_ctx);

            //启动告警
            m_alarm.Create(&m_ctx);

            //启动警戒排程
            m_scheduleInv.Create("invasion", &m_ctx, &m_ctx.env.invasion.schedule, [this](bool ena) {
                //当警戒排程改变时通知告警模块
                m_alarm.SetEna(OVD_ALERTAREA, ena);
                emxlogi("invasion change %d\n", ena);
            });
            //开启andlink
            m_andLink.Create(&m_ctx);

            //开启voip
            m_voip.Create(&m_ctx);
        }
        m_ctx.led->StatChange(Led::StatE::Sleeping, !ena);
    });

    //启动自动维护
    m_autoMaintain.Create(&m_ctx, [this]() {
        if (m_ctx.running.updateEvent.stat != UpdateEvent::StatE::Start) {
            emxlogi("auto maintain reboot\n");
            time_t xtime;
            struct tm xtm = {};
            time(&xtime);
            localtime_r(&xtime, &xtm);
            snprintf(m_ctx.env.cfg.last_reboot, sizeof(m_ctx.env.cfg.last_reboot) - 1
                    , "%04d-%02d-%02dT%02d:%02d:%02d"
                    , xtm.tm_year + 1900, xtm.tm_mon + 1, xtm.tm_mday
                    , xtm.tm_hour, xtm.tm_min, xtm.tm_sec);
            m_ctx.env.cfg.Save();
            m_record.Destroy();
            sync();
            Reboot::DoReboot(3);
        }
    });

    //开启软探针
    m_softProbe.Create(&m_ctx);

#endif
    m_ctx.led->StatChange(Led::StatE::ManualLedEna, m_ctx.env.cfg.led);
    emxlogd("leave\n");
    m_serviceStarted = true;
}

void Ovd::ServiceStop() {
    if (!m_serviceStarted)
        return;
    TimeRec cost;
    emxlogd("into\n");
#ifndef EAPIL_SPARE
    //结束录像
    m_record.Destroy();
    //结束软探针
    m_softProbe.Destroy();
    //结束自动维护
    m_autoMaintain.Destroy();
    //结束警戒排程
    m_scheduleInv.Destroy();
    //结束休眠排程
    m_scheduleSleep.Destroy();
    //停止gat1400业务
    Gat1400Client::Instance()->Stop();
    //结束告警
    m_alarm.Destroy();
    //结束音视频实时流推送
    m_avPush.Destroy();
    //结束andlink
    m_andLink.Destroy();
    m_voip.Destroy();
#endif
    emxlogd("leave cost %ums\n", cost.GetInv());
    m_serviceStarted = false;
}
