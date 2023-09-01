#include "ManageCfgFile.hpp"
#include "ConfigService.hpp"
#include "Sn.hpp"
#include "SDCardClient.hpp"
#include "NetClient.hpp"
#include "libconfig.h"
#include "EmxMedia.hpp"
using namespace Emx;

ManageCfgFile::ManageCfgFile(EuvLoop *loop)
    : m_loop(loop)
    , m_factory_mode(ExitFactory)
    , m_isOnled(false) {
    ImportCfgPath();
    InitFactoryMode();
}

ManageCfgFile::~ManageCfgFile() {
    m_bsp.led->Set(false, 0);
    m_lightTimer.Destroy();
}

void ManageCfgFile::OnSDCardInfoEvent(int chn, SDCard::Info &info) {
    emxlogd("ManageCfgFile::OnSDCardInfoEvent\n");
    bool normal = info.stat == SDCard::StatE::Normal;
    if (m_sdcard.normal == normal) {
        return;
    }
    m_sdcard.normal = normal;
    emxlogd("stat normal = %s\n", normal ? "true" : "false");
    if (normal) {
        LoadLogCfg();
    } else {
        Log log_client;
        Log::Config log_config;
        if (log_client.GetConfig(log_config) != ErrCodeE::Success) {
            emxloge("get logconfig failed\n");
            return;
        }
        if (strncmp(log_config.normal.path, m_logConfig.normal.path, sizeof(m_logConfig.normal.path)) != 0) {
            log_client.SetConfig(m_logConfig);
        }
    }
}

void ManageCfgFile::Start() {
    Guide();
    m_sdcard.event = new SDCardClientEvent(*m_loop,
                            m_buffer, sizeof(m_buffer),
                            std::bind(&ManageCfgFile::OnSDCardInfoEvent, this, ph_1, ph_2));
    m_sdcard.event->Create();
}

void ManageCfgFile::Guide() {
    m_lightTimer.Create(*m_loop);
    if (m_factory_mode != EnterFactory) {
        return;
    }
    //note: 语音提示
    char audio_path[EMX_MAX_PATH_SIZE] = {};
    snprintf(audio_path, sizeof(audio_path) - 1 
            , "%s/audio/come_factory_mode.aac"
            , Misc::GetAppConfigsDir());
    MediaClientAdecStreamSync::PlayFileWithVolume(0, audio_path, 38);

    //note: led提示
    m_isOnled = false;
    m_lightTimer.Start(0, 1000, [this]() {
        if (m_isOnled) {
            m_bsp.led->Set(false, 0);
            m_isOnled = false;
        } else {
            m_bsp.led->Set(true, 0);
            m_isOnled = true;
        }
    });
}

void ManageCfgFile::Load() {
    //note: 加载log.cfg
    // LoadLogCfg();
    Log log_client;
    Log::Config log_config;
    if (log_client.GetConfig(log_config) == ErrCodeE::Success) {
        //note: 备份日志配置信息
        memset(&m_logConfig, 0, sizeof(m_logConfig));
        memcpy(&m_logConfig, &log_config, sizeof(log_config));
        emxlogd("log config file path: %s\n", m_logConfig.normal.path);
    } else {
        emxloge("get log config failed!\n");
    }
    
    if (m_factory_mode == EnterFactory) {
        //note: EnterFactory模式, 检验网络.cfg(优先加载有线)
        if (!LoadIpCfg()) {
            LoadAutoWifiCfg();
        }
    } 
    if (m_factory_mode == ExitFactory) {
        //note: 禁用telnet端口
        Misc::KillBeforeRun("telnetd");
        //note: 禁用rtsp端口
        Misc::KillBeforeRun("RtspServer");
    }
}

void ManageCfgFile::ImportCfgPath() {
    // if (SDCardClient::GetChnNum() < 1) {
    //     emxlogd("sdcard chn res not exitst\n");
    //     return;
    // }
    // //note: 默认chn0
    // SDCardClient sd(0);
    // SDCard::Info sd_info;
    // if(sd.GetInfo(sd_info) != ErrCodeE::Success) {
    //     emxlogd("sdcard get info failed\n");
    //     return;
    // }
    char mountPoint[EMX_MAX_PATH_SIZE] = { 0 };
    snprintf(mountPoint, sizeof(mountPoint) - 1, "/mnt/sdcard");
    snprintf(m_cfg_path.zn_debug
            , sizeof(m_cfg_path.zn_debug) - 1
            , "%s/zn_debug"
            , mountPoint);
    snprintf(m_cfg_path.log_cfg
            , sizeof(m_cfg_path.log_cfg) - 1
            , "%s/log.cfg"
            , mountPoint);
    snprintf(m_cfg_path.ip_cfg
            , sizeof(m_cfg_path.ip_cfg) - 1
            , "%s/ip.cfg"
            , mountPoint);
    snprintf(m_cfg_path.autowifi_cfg
            , sizeof(m_cfg_path.autowifi_cfg) - 1
            , "%s/autowifi.cfg"
            , mountPoint);
    snprintf(m_cfg_path.normal_log
            , sizeof(m_cfg_path.normal_log) - 1
            , "%s/deviceNormal.log"
            , mountPoint);
    snprintf(m_cfg_path.flash_log
            , sizeof(m_cfg_path.flash_log) - 1
            , "%s/deviceFlash.log"
            , mountPoint);
}

void ManageCfgFile::InitFactoryMode() {
    m_factory_mode = ExitFactory;
    //note: 检验sn(优先级最高)
    Sn sn;
    if (sn.Load() != ErrCodeE::Success) {
        m_factory_mode = EnterFactory;
        return;
    }
    if (std::string(sn.Get(), strlen(sn.Get())).size() == 0) {
        m_factory_mode = EnterFactory;
        return;
    }
    
    if (!File::Exist(m_cfg_path.zn_debug)) {
        emxloge("%s not exitst\n", m_cfg_path.zn_debug);
        return;
    }
    m_factory_mode = KeepFactory;

    ZnDebugInfo info;
    if (!GetZnDebugConfig(&info)) {
        emxloge("GetZnDebugConfig failed\n");
        return;
    }
    if (info.factory_mode) {
        m_factory_mode = EnterFactory;
    }
}

bool ManageCfgFile::GetZnDebugConfig(ZnDebugInfo *info) {
    bool ret = false;
    config_t cfg;
    config_setting_t *cfgSetting = nullptr;
    bool is_need_destroy = false;
    do {
        if (!File::Exist(m_cfg_path.zn_debug)) {
            emxloge("%s not exitst\n", m_cfg_path.zn_debug);
            break;
        }
        config_init(&cfg);
        is_need_destroy = true;
        if(!config_read_file(&cfg, m_cfg_path.zn_debug)) {
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

bool ManageCfgFile::LoadLogCfg() {
    bool result = false;
    do {
        if (!File::Exist(m_cfg_path.log_cfg)) {
            emxloge("%s not exitst\n", m_cfg_path.log_cfg);
            break;
        }

        FILE *fp = fopen(m_cfg_path.log_cfg, "r");
        if (fp == nullptr) {
            break;
        }
        char data[16] = { 0 };
        fgets(data, sizeof(data), fp);
        int log_level = atoi(data);
        fclose(fp);
        fp = nullptr;

        if ((log_level > (int)Log::LevelE::Trace) || (log_level < (int)Log::LevelE::Crt)) {
            emxloge("set log level is error(%d)\n", log_level);
            break;
        }
        emxlogd("set log level(%d)\n", log_level);
        Log log_client;
        Log::Config log_config;
        if (log_client.GetConfig(log_config) != ErrCodeE::Success) {
            emxloge("get logconfig failed\n");
            break;
        }
        log_config.shared.level = (Log::LevelE)log_level;
        log_config.normal.maxSizeKB = -1;
        memset(log_config.normal.path, 0 ,sizeof(log_config.normal.path));
        memcpy(log_config.normal.path, m_cfg_path.normal_log, sizeof(m_cfg_path.normal_log));
        log_config.flash.maxSizeKB = -1;
        memset(log_config.flash.path, 0 ,sizeof(log_config.flash.path));
        memcpy(log_config.flash.path, m_cfg_path.flash_log, sizeof(m_cfg_path.flash_log));
        if (log_client.SetConfig(log_config) != ErrCodeE::Success) {
            emxloge("set logconfig failed\n");
            break;
        }
        emxlogd("set logconfig ok\n");
        result = true;
    } while(false);
    return result;
}

bool ManageCfgFile::LoadIpCfg() {
    bool result = false;
    do {
        if (!File::Exist(m_cfg_path.ip_cfg)) {
            emxloge("%s not exitst\n", m_cfg_path.ip_cfg);
            break;
        }
        if (!NetMainDev::IsSupported(Net::DevE::Eth)) {
            emxloge("eth unsupported\n");
            break;
        }

        //note: 读取数据(注意数据长度检验)
        FILE *fp = fopen(m_cfg_path.ip_cfg, "r");
        if (fp == nullptr) {
            break;
        }
        int line_maxlen = 64;
        char data[line_maxlen] = { 0 };
        char ip[line_maxlen] = { 0 };
        char mask[line_maxlen] = { 0 };
        char gateway[line_maxlen] = { 0 };
        while (fgets(data, sizeof(data), fp) != nullptr) {
            if (strstr(data, "ip=") != nullptr) {
                sscanf(data, "ip=%s", ip);
            } else if (strstr(data, "mask=") != nullptr) {
                sscanf(data, "mask=%s", mask);
            } else if (strstr(data, "gateway=") != nullptr) {
                sscanf(data, "gateway=%s", gateway);
            }
        }
        fclose(fp);
        fp = nullptr;

        //note: 配置主网络为有线
        Net::DevE type;
        NetMainDev::Get(type);
        if (type != Net::DevE::Eth) {
            emxlogd("need switch mainDev(%d)\n", type);
            if (NetMainDev::Set(Net::DevE::Eth) != ErrCodeE::Success) {
                break;
            }
        }

        Net::DevE dev;
        NetMainDev::Get(dev);
        Net::Addr addr = { 0 };
        NetClient net(dev);
        net.GetAddr(addr);
        if (strlen(ip) > sizeof(addr.ip4)
            || strlen(mask) > sizeof(addr.netmask4)
            || strlen(gateway) > sizeof(addr.gateway4)) {
            break;
        }
        addr.dhcp = false;
        if (strlen(ip) > 0) {
            memset(addr.ip4, 0, sizeof(addr.ip4));
            memcpy(addr.ip4, ip, sizeof(addr.ip4));
        }
        if (strlen(mask) > 0) {
            memset(addr.netmask4, 0, sizeof(addr.netmask4));
            memcpy(addr.netmask4, mask, sizeof(addr.netmask4));
        }
        if (strlen(gateway) > 0) {
            memset(addr.gateway4, 0, sizeof(addr.gateway4));
            memcpy(addr.gateway4, gateway, sizeof(addr.gateway4));
        }
        emxlogd("set dhcp(%d); ip(%s); mask(%s); gateway(%s)\n", addr.dhcp, addr.ip4, addr.netmask4, addr.gateway4);

        //note: 网络配置
        if (net.SetAddr(addr) == ErrCodeE::Success) {
            result = true;
            emxlogd("enter factory mode, set eth static ip ok!\n");
        } else {
            emxlogd("enter factory mode, set eth stattc ip failed!\n");
        }
    } while(false);
    return result;
}

bool ManageCfgFile::LoadAutoWifiCfg() {
    bool result = false;
    do {
        if (!File::Exist(m_cfg_path.autowifi_cfg)) {
            emxloge("%s not exitst\n", m_cfg_path.autowifi_cfg);
            break;
        }
        if (!NetMainDev::IsSupported(Net::DevE::Wifi)) {
            emxloge("wifi unsupported\n");
            break;
        }

        //note: 读取数据(注意数据长度检验)
        FILE *fp = fopen(m_cfg_path.autowifi_cfg, "r");
        if (fp == nullptr) {
            break;
        }
        int line_maxlen = 64;
        int flag = 0;
        char data[line_maxlen] = { 0 };
        char ip[line_maxlen] = { 0 };
        char mask[line_maxlen] = { 0 };
        char gateway[line_maxlen] = { 0 };
        char ssid[line_maxlen] = { 0 };
        char psk[line_maxlen] = { 0 };
        while (fgets(data, sizeof(data), fp) != nullptr) {
            if (strstr(data, "ssid=") != nullptr) {
                sscanf(data, "ssid=%s", ssid);
            } else if (strstr(data, "psk=") != nullptr) {
                sscanf(data, "psk=%s", psk);
            } else if (strstr(data, "static=") != nullptr) {
                sscanf(data, "static=%d", &flag);
            } else if (strstr(data, "ip=") != nullptr) {
                sscanf(data, "ip=%s", ip);
            } else if (strstr(data, "gateway=") != nullptr) {
                sscanf(data, "gateway=%s", gateway);
            } else if (strstr(data, "mask=") != nullptr) {
                sscanf(data, "mask=%s", mask);
            }
        }
        fclose(fp);
        fp = nullptr;
        if (strlen(ssid) < 1 || strlen(psk) < 1) {
            break;
        }

        //note: 配置主网络为无线
        Net::DevE type;
        NetMainDev::Get(type);
        if (type != Net::DevE::Wifi) {
            if (NetMainDev::Set(Net::DevE::Wifi) != ErrCodeE::Success) {
                break;
            }
        }

        NetClientWifi wifi;
        if (wifi.SetSTA(ssid, psk) != ErrCodeE::Success) {
            emxloge("wifi SetSTA failed\n");
            break;
        }
        if (flag) {
            Net::DevE dev;
            NetMainDev::Get(dev);
            Net::Addr addr = { 0 };
            NetClient net(dev);
            net.GetAddr(addr);
            if (strlen(ip) > sizeof(addr.ip4)
                || strlen(mask) > sizeof(addr.netmask4)
                || strlen(gateway) > sizeof(addr.gateway4)) {
                break;
            }
            addr.dhcp = false;
            if (strlen(ip) > 0) {
                memset(addr.ip4, 0, sizeof(addr.ip4));
                memcpy(addr.ip4, ip, sizeof(addr.ip4));
            }
            if (strlen(mask) > 0) {
                memset(addr.netmask4, 0, sizeof(addr.netmask4));
                memcpy(addr.netmask4, mask, sizeof(addr.netmask4));
            }
            if (strlen(gateway) > 0) {
                memset(addr.gateway4, 0, sizeof(addr.gateway4));
                memcpy(addr.gateway4, gateway, sizeof(addr.gateway4));
            }
            if (net.SetAddr(addr) == ErrCodeE::Success) {
                result = true;
            }
            break;
        }
        result = true;
    } while(false);

    if (result) {
        emxlogd("enter factory mode, set wifi ok!\n");
    } else {
        emxlogd("enter factory mode, set wifi failed!\n");
    }
    return result;
}