//
// Created by xiong on 2021/5/21.
//
#include "EmxCore.hpp"
#include "EmxMedia.hpp"
#include "EmxProtoV1.hpp"
#include "DevInfo.hpp"
#include "Sn.hpp"
#include "SDCardClient.hpp"
#include "Mac.hpp"
#include "Sn.hpp"
#include "EmxCrypto.hpp"
#include "NightVisionClient.hpp"
#include "NetClient.hpp"
#include "EmxBsp.hpp"
#include "BspLed.hpp"
#include "BspKeyReset.hpp"
#include "BspWhiteLightCvitek.hpp"
#include "BspIrLight.hpp"
#include "TimeZone.hpp"

#define EP_CFG_CODE_OK 0
#define EP_CFG_CODE_ERR 400
#define EP_CFG_DESC_OK "successed"
#define EP_CFG_DESC_FAILED "failed"
#define EP_PLTBURN_FILE   "pltBurn"

using namespace Emx;
#define EP_PROTO_BIND_FUNC(x) std::bind(&EmxProtoV1::x, this, ph_1, ph_2);

EmxProtoV1::EmxProtoV1()
    : m_loop(nullptr) {
    m_funcMap[CmdE::BASE_GET] = EP_PROTO_BIND_FUNC(BaseGet);
    m_funcMap[CmdE::VIDEO_MAJOR_GET] = EP_PROTO_BIND_FUNC(VideoMajorGet);
    m_funcMap[CmdE::VIDEO_MAJOR_SET] = EP_PROTO_BIND_FUNC(VideoMajorSet);
    m_funcMap[CmdE::VIDEO_MAJOR_QUE] = EP_PROTO_BIND_FUNC(VideoMajorQue);
    m_funcMap[CmdE::VIDEO_MINOR_GET] = EP_PROTO_BIND_FUNC(VideoMinorGet);
    m_funcMap[CmdE::VIDEO_MINOR_SET] = EP_PROTO_BIND_FUNC(VideoMinorSet);
    m_funcMap[CmdE::VIDEO_MINOR_QUE] = EP_PROTO_BIND_FUNC(VideoMinorQue);
    m_funcMap[CmdE::AUDIO_GET] = EP_PROTO_BIND_FUNC(AudioGet);
    m_funcMap[CmdE::AUDIO_SET] = EP_PROTO_BIND_FUNC(AudioSet);
    m_funcMap[CmdE::AUDIO_QUE] = EP_PROTO_BIND_FUNC(AudioQue);
    m_funcMap[CmdE::NET_GET] = EP_PROTO_BIND_FUNC(NetGet);
    m_funcMap[CmdE::NET_SET] = EP_PROTO_BIND_FUNC(NetSet);
    m_funcMap[CmdE::WIFI_GET] = EP_PROTO_BIND_FUNC(NetWifiGet);
    m_funcMap[CmdE::WIFI_SET] = EP_PROTO_BIND_FUNC(NetWifiSet);
    m_funcMap[CmdE::SN_GET] = EP_PROTO_BIND_FUNC(SnGet);
    m_funcMap[CmdE::SN_SET] = EP_PROTO_BIND_FUNC(SnSet);
    m_funcMap[CmdE::ISP_GET] = EP_PROTO_BIND_FUNC(IspGet);
    m_funcMap[CmdE::ISP_SET] = EP_PROTO_BIND_FUNC(IspSet);
    m_funcMap[CmdE::ISP_QUE] = EP_PROTO_BIND_FUNC(IspQue);
//    m_funcMap[CmdE::SNAP] = EP_PROTO_BIND_FUNC(SnapGet);
    m_funcMap[CmdE::MUSIC] = EP_PROTO_BIND_FUNC(MusicPlay);
    m_funcMap[CmdE::REBOOT] = EP_PROTO_BIND_FUNC(Reboot);
    m_funcMap[CmdE::RESET] = EP_PROTO_BIND_FUNC(Reset);
    m_funcMap[CmdE::SD_INFO] = EP_PROTO_BIND_FUNC(SDInfo);
    m_funcMap[CmdE::SD_FORMAT] = EP_PROTO_BIND_FUNC(SDFormat);
    m_funcMap[CmdE::SD_CHECK] = EP_PROTO_BIND_FUNC(SDCheck);
    m_funcMap[CmdE::TIME_GET] = EP_PROTO_BIND_FUNC(TimeGet);
    m_funcMap[CmdE::TIME_SET] = EP_PROTO_BIND_FUNC(TimeSet);
    m_funcMap[CmdE::ICR_GET] = EP_PROTO_BIND_FUNC(ICRGet);
    m_funcMap[CmdE::ICR_SET] = EP_PROTO_BIND_FUNC(ICRSet);
    m_funcMap[CmdE::LDS_GET] = EP_PROTO_BIND_FUNC(LDSGet);
    m_funcMap[CmdE::LDS_SET] = EP_PROTO_BIND_FUNC(LDSSet);
    m_funcMap[CmdE::OSD_GET] = EP_PROTO_BIND_FUNC(OSDGet);
    m_funcMap[CmdE::OSD_SET] = EP_PROTO_BIND_FUNC(OSDSet);
    m_funcMap[CmdE::TMPL_GET] = EP_PROTO_BIND_FUNC(TemplateGet);
    m_funcMap[CmdE::TMPL_SET] = EP_PROTO_BIND_FUNC(TemplateSet);
    m_funcMap[CmdE::CENTER_GET] = EP_PROTO_BIND_FUNC(CenterGet);
    m_funcMap[CmdE::CENTER_SET] = EP_PROTO_BIND_FUNC(CenterSet);
    m_funcMap[CmdE::PTZ_GETV2] = EP_PROTO_BIND_FUNC(PtzGetV2);
    m_funcMap[CmdE::PTZ_SETV2] = EP_PROTO_BIND_FUNC(PtzSetV2);
    m_funcMap[CmdE::FACE_GET] = EP_PROTO_BIND_FUNC(FaceGet);
    m_funcMap[CmdE::FACE_SET] = EP_PROTO_BIND_FUNC(FaceSet);
    m_funcMap[CmdE::FACE_DEL] = EP_PROTO_BIND_FUNC(FaceDel);
    m_funcMap[CmdE::IREDW_GET] = EP_PROTO_BIND_FUNC(IredwGet);
    m_funcMap[CmdE::IREDW_SET] = EP_PROTO_BIND_FUNC(IredwSet);
    m_funcMap[CmdE::ENCRYPT_SPT] = EP_PROTO_BIND_FUNC(EncryptInfo);
    m_funcMap[CmdE::LED_START] = EP_PROTO_BIND_FUNC(LedStart);
    m_funcMap[CmdE::LED_STOP] = EP_PROTO_BIND_FUNC(LedStop);
    m_funcMap[CmdE::RESET_START] = EP_PROTO_BIND_FUNC(ResetStart);
    m_funcMap[CmdE::RESET_STOP] = EP_PROTO_BIND_FUNC(ResetStop);
    m_funcMap[CmdE::SPK_START] = EP_PROTO_BIND_FUNC(SpkStart);
    m_funcMap[CmdE::SPK_STOP] = EP_PROTO_BIND_FUNC(SpkStop);
    m_funcMap[CmdE::WIFIMAC_GET] = EP_PROTO_BIND_FUNC(WifiMacGet);
    m_funcMap[CmdE::WIFIMAC_SET] = EP_PROTO_BIND_FUNC(WifiMacSet);
    m_funcMap[CmdE::IMEI_GET] = EP_PROTO_BIND_FUNC(ImeiGet);
    m_funcMap[CmdE::IMEI_SET] = EP_PROTO_BIND_FUNC(ImeiSet);
    m_funcMap[CmdE::PTZ_SETV1] = EP_PROTO_BIND_FUNC(PtzSetV1);
    m_funcMap[CmdE::WIFI_QTY_GET] = EP_PROTO_BIND_FUNC(WifiQtyGet);
    m_funcMap[CmdE::BAND_WIDTH_SET] = EP_PROTO_BIND_FUNC(BandWidthSet);
    m_funcMap[CmdE::AGING_GET] = EP_PROTO_BIND_FUNC(AgingGet);
    m_funcMap[CmdE::AGING_SET] = EP_PROTO_BIND_FUNC(AgingSet);
    m_funcMap[CmdE::IRCUT_EXT_SET] = EP_PROTO_BIND_FUNC(IRcutExtSet);
    m_funcMap[CmdE::SINGLE_BOARD_SET] = EP_PROTO_BIND_FUNC(SingleBoardSet);
    m_funcMap[CmdE::BURNCLEAR_SET] = EP_PROTO_BIND_FUNC(BurnInfoClearSet);
    m_funcMap[CmdE::DEV21CN_GET] = EP_PROTO_BIND_FUNC(Dev21CNGet);
    m_funcMap[CmdE::DEV21CN_SET] = EP_PROTO_BIND_FUNC(Dev21CNSet);
    m_funcMap[CmdE::DEVHJQV1_GET] = EP_PROTO_BIND_FUNC(DevHJQV1Set);
    m_funcMap[CmdE::DEVHJQV1_SET] = EP_PROTO_BIND_FUNC(DevHJQV1Get);
    m_funcMap[CmdE::DEVHJQV2_GET] = EP_PROTO_BIND_FUNC(DevHJQV2Get);
    m_funcMap[CmdE::DEVHJQV2_SET] = EP_PROTO_BIND_FUNC(DevHJQV2Set);
    m_funcMap[CmdE::DEVYY_GET] = EP_PROTO_BIND_FUNC(DevYYGet);
    m_funcMap[CmdE::DEVYY_SET] = EP_PROTO_BIND_FUNC(DevYYSet);
    m_funcMap[CmdE::DEVZJDX_GET] = EP_PROTO_BIND_FUNC(DevZJDXGet);
    m_funcMap[CmdE::DEVZJDX_SET] = EP_PROTO_BIND_FUNC(DevZJDXSet);
    m_funcMap[CmdE::DEVTYKD_GET] = EP_PROTO_BIND_FUNC(DevTYKDSet);
    m_funcMap[CmdE::DEVTYKD_SET] = EP_PROTO_BIND_FUNC(DevTYKDGet);
    m_funcMap[CmdE::DEVKJB_GET] = EP_PROTO_BIND_FUNC(DevKJBGet);
    m_funcMap[CmdE::DEVKJB_SET] = EP_PROTO_BIND_FUNC(DevKJBSet);
    m_funcMap[CmdE::DEVGB_GET] = EP_PROTO_BIND_FUNC(DevGBGet);
    m_funcMap[CmdE::DEVGB_SET] = EP_PROTO_BIND_FUNC(DevGBSet);
    m_funcMap[CmdE::DEVXC_GET] = EP_PROTO_BIND_FUNC(DevXCSet);
    m_funcMap[CmdE::DEVXC_SET] = EP_PROTO_BIND_FUNC(DevXCGet);
    m_funcMap[CmdE::DEVTY_GET] = EP_PROTO_BIND_FUNC(DevTYGet);
    m_funcMap[CmdE::DEVTY_SET] = EP_PROTO_BIND_FUNC(DevTYSet);
    m_pubkeyPath = "";
}

void MakeCodeDesc(Json::Value &out, int code, const char *desc) {
    out["code"] = (int)code;
    out["desc"] = desc;
}

void EmxProtoV1::Proc(EmxProtoV1::CmdE cmd, const char *data, int32_t size, std::string &out) {
    emxlogd("cmd=0x%x\n", cmd);
    Json::Value jsonIn;
    if (size) {
        if (EasyJson::Parse(data, size, jsonIn) != ErrCodeE::Success)
            return;
        emxlogd("Req#%s#\n", data);
    }
    if (cmd == CmdE::SNAP) {
        SnapGet(jsonIn, out);
    } else if (m_funcMap.count(cmd) != 0) {
        Json::Value jsonOut;
        MakeCodeDesc(jsonOut, (int)ErrCodeE::Success, "successed");
        m_funcMap[cmd](jsonIn, jsonOut);
        EasyJson::Serialize(jsonOut, out);
        emxlogd("Resp#%s#\n", out.data());
    } else {
        emxloge("not support cmd %x\n", cmd);
    }
}

void EmxProtoV1::SnGet(Json::Value &in, Json::Value &out) {
    ErrCodeE e;
    Sn sn;
    e = sn.Load();
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(out, (int)e, "load sn failed");
        return;
    }
    out["sn"] = sn.Get();
}

void EmxProtoV1::SnSet(Json::Value &in, Json::Value &out) {
    ErrCodeE e;
    std::string snEncrypted;
    std::string ketPath(Misc::GetAppConfigsDir());
    ketPath+="/key/public.key";
    Base64::Decode(in["sn"].asString(), snEncrypted);
    std::string snPlain;
    e = EmxCrypto::RsaPubKeyDecrypt(ketPath.c_str(), snEncrypted, snPlain);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(out, (int)e, "decrypt sn failed");
        return;
    }
    Sn sn;
    e = sn.Set(snPlain.c_str());
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(out, (int)e, "Set sn failed");
        return;
    }
}

void EmxProtoV1::WifiMacGet(Json::Value &in, Json::Value &out) {
    Mac mac;
    auto e = mac.Load();
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(out, (int)e, "load mac failed");
        return;
    }
    out["mac"] = mac.Get();
}

void EmxProtoV1::WifiMacSet(Json::Value &in, Json::Value &out) {
    Mac mac;
    auto e = mac.Load();
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(out, (int)e, "load mac failed");
        return;
    }
    e = mac.Set(in["mac"].asString().c_str());
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(out, (int)e, "set mac failed");
    }
}

void EmxProtoV1::ImeiGet(Json::Value &in, Json::Value &out) {
    Param param("burnInfo");
    Json::Value json;
    auto e = param.Get(json);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(out, (int)e, "Get failed");
        return;
    }
    out["imei"] = json["cmei"].asString();
}

void EmxProtoV1::ImeiSet(Json::Value &in, Json::Value &out) {
    Param param("burnInfo");
    Json::Value json;
    auto e = param.Get(json);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(out, (int)e, "Get failed");
        return;
    }
    json["cmei"] = in["imei"].asString();
    e = param.Set(json);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(out, (int)e, "Set failed");
    }
}

void EmxProtoV1::SpkStart(Json::Value &in, Json::Value &out) {
    char audioDir[EMX_MAX_PATH_SIZE] = {};
    snprintf(audioDir, sizeof(audioDir), "%s/audio", Misc::GetAppConfigsDir());
    DIR *dp = opendir(audioDir);
    if (!dp) {
        MakeCodeDesc(out, (int)ErrCodeE::ResNotExist, "cannot open audio dir");
        return;
    }
    char path[EMX_MAX_PATH_SIZE] = {};
    dirent *entry;
    while ((entry = readdir(dp)) != nullptr) {
        if (strstr(entry->d_name, "music.")) {
            snprintf(path, sizeof(path), "%s/%s", audioDir, entry->d_name);
            break;
        }
    }
    closedir(dp);
    if (path[0] == '\0') {
        MakeCodeDesc(out, (int)ErrCodeE::ResNotExist, "cannot found music file");
        return;   
    }
    auto e = MediaClientAdecStreamSync::PlayFileWithVolume(0, path, 38);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(out, (int)e, "cannot found music file");
    }
}

void EmxProtoV1::SpkStop(Json::Value &in, Json::Value &out) {
    auto e = MediaClientAdecStreamSync::PlayCtrlQuit(0);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(out, (int)e, "spkstop failed");
    }
}

void EmxProtoV1::ICRSet(Json::Value &in, Json::Value &out) {
    ErrCodeE code = ErrCodeE::Success;
    std::string desc = "successed";
    NightVisionClient nv;
    NightVision::Param param = { 0 };

    do {
        if (!in["mode"].isString()) {
            code = ErrCodeE::Failure;
            desc = "request no string mode";
            break;
        }
        std::string mode = in["mode"].asString();

        code = nv.GetParam(param);
        if (code != ErrCodeE::Success) {
            desc = "GetParam failed";
            break;
        }
        if (mode == "manual") {
            param.manual = true;
        } else if (mode == "auto") {
            //auto模式下不关注switch
            param.manual = false;
            break;
        } else {
            emxlogd("request mode no value");
        }
        
        if (!in["switch"].isString()) {
            code = ErrCodeE::Failure;
            desc = "request no string switch";
            break;
        }
        std::string str_switch = in["switch"].asString();
        if (str_switch == "off") {
            param.manualMode = NightVision::ModeE::Day;
        } else if (str_switch == "on") {
            param.manualMode = NightVision::ModeE::IrNightVision;
        } else {
            emxlogd("request switch no value");
        }
    } while (false);

    if (code == ErrCodeE::Success) {
        code = nv.SetParam(param);
        if (code != ErrCodeE::Success) {
            desc = "SetParam failed";
        }
    }
    MakeCodeDesc(out, (int)code, desc.c_str());
}

void EmxProtoV1::SDCheck(Json::Value &in, Json::Value &out) {
    std::string desc = "";
    bool is_success = true;
    int chns = SDCardClient::GetChnNum();
    for (int chn = 0; chn < chns; ++chn) {
        SDCardClient sd(chn);
        if (sd.Check() != ErrCodeE::Success) {
            emxloge("sd chn(%d) check failed\n", chn);
            char c_desc[32] = { 0 };
            snprintf(c_desc, sizeof(c_desc) - 1, "sd chn(%d) check failed", chn);
            desc.append(std::string(c_desc,sizeof(c_desc)));
            is_success = false;
        }
    }
    if (!is_success) {
        MakeCodeDesc(out, (int)ErrCodeE::Failure, desc.c_str());
    }
 }

void EmxProtoV1::BaseGet(Json::Value &in, Json::Value &out) {
    ErrCodeE e;
    Sn sn;
    e = sn.Load();
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(out, (int)e, "load sn failed");
        return;
    }
    out["sn"] = sn.Get();

    DevInfo devInfo;
    e = devInfo.Load();
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(out, (int)e, "load devInfo failed");
        return;
    }
    out["dev_type"] = devInfo.GetOuter()->type;
    out["firmware"] = devInfo.GetOuter()->version;
    out["dev_id"] = devInfo.GetInner()->type;
    out["dev_ver"] = devInfo.GetInner()->version;
    out["hardware"] = devInfo.GetHardware();
}

void EmxProtoV1::ResetStart(Json::Value &in, Json::Value &out) {
    bool presskey_status_down = false;
    Bsp::KeyReset *keyreset = new BspKeyReset(); 
    int timeout = 200; //检测间隔：200ms
    int count = (2 * 1000) / timeout; //2s检测
    while (count--) {
        if (keyreset->Get()) {
            presskey_status_down = true;
            break;
        }
        usleep(timeout * 1000);
    }
    if (!presskey_status_down) {
        MakeCodeDesc(out, (int)ErrCodeE::Failure, "no check presskey down");
    }

    if (keyreset != nullptr) {
        delete keyreset;
        keyreset = nullptr;
    }
}

void EmxProtoV1::PtzSetV1(Json::Value &in, Json::Value &out) {
    MakeCodeDesc(out, (int)ErrCodeE::Failure, "no support ptz check");
}

void EmxProtoV1::LedStart(Json::Value &in, Json::Value &out) {
    Bsp::Led *led = new BspLed();
    int chns = led->GetChnsNum();
    if (chns <= 0) {
        MakeCodeDesc(out, (int)ErrCodeE::Failure, "no led");
        return;
    }
    struct LedControlArg {
        int chns = 0;
        int intervals = 500;
        int repeat = 10;
        Bsp::Led *led;
        EuvWork work;
    };
    LedControlArg* handle = new LedControlArg;
    handle->chns = chns;
    handle->led = led;
    handle->work.Create(m_loop, handle,
                                [](void *arg) {
                                    LedControlArg* handle = (LedControlArg*)arg;
                                    while(handle->repeat) {
                                        --handle->repeat;
                                        for (int chn = 0; chn < handle->chns; ++chn) {
                                            handle->led->Set(true, chn);
                                            usleep(handle->intervals * 1000);
                                            handle->led->Set(false, chn);
                                        }
                                    }
                                },
                                [this](ErrCodeE e, void *arg) {
                                    LedControlArg* handle = (LedControlArg*)arg;
                                    if (handle != nullptr) {
                                        handle->work.Destroy();
                                        if (handle->led != nullptr) {
                                            delete handle->led;
                                            handle->led = nullptr;
                                        }
                                        delete handle;
                                        handle = nullptr;
                                    }
                                });
    handle->work.Run();
}

void EmxProtoV1::WifiQtyGet(Json::Value &in, Json::Value &out) {
    if (!NetMainDev::IsSupported(Net::DevE::Wifi)) {
        MakeCodeDesc(out, (int)ErrCodeE::ResNotExist, "wifi unsupported");
        return;
    }
    NetClientWifi wifi;
    Wifi::Info info = { };
    auto e = wifi.GetInfo(info);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(out, (int)e, "GetInfo failed");
        return;
    }
    out["wifi_quality"] = (int) info.signalLevel - 100;

    Net::DevE dev;
    NetMainDev::Get(dev);
    Net::Addr addr = {};
    NetClient net(dev);
    net.GetAddr(addr);
    Net::LinkStat link = {};
    net.GetLinkStat(link);
    if (link.lan4) {
        out["ip"] = std::string(addr.ip4, sizeof(addr.ip4));
    }
}

void EmxProtoV1::AgingGet(Json::Value &in, Json::Value &out) {
    ErrCodeE ret = ErrCodeE::Success;
    std::string resp = "";
    AgingInfo info;
    ret = MsgRpcReqSync::Request(AgeingServerMsgId, AGEING_SERVER_MOD
                                    , (uint32_t) EmxAgeingModule::GetInfo
                                    , nullptr, 0, &resp);
    if (ret != ErrCodeE::Success) {
        emxloge("failed:%d\n", ret);
        MakeCodeDesc(out, (int)ret, "get ageing info failed\n");
        return;
    } else {
        memcpy(&info, resp.data(), sizeof(info));
    }
    if (info.aging_state == Ageing_Finish) {
        out["agingState"] = true;
    } else {
        out["agingState"] = false;
    }
}

void EmxProtoV1::DevHJQV2Get(Json::Value &in, Json::Value &out) {
    ErrCodeE e;
    Sn sn;
    e = sn.Load();
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(out, (int)e, "load sn failed");
        return;
    }
    out["sn"] = sn.Get();

    Mac mac;
    e = mac.Load();
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(out, (int)e, "load mac failed");
        return;
    }
    out["mac"] = mac.Get();

    Param param("burnInfo");
    Json::Value json;
    e = param.Get(json);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(out, (int)e, "Get burnInfo failed");
        return;
    }
    out["imei"] = json["cmei"].asString();
    out["password"] = json["OVDLoginPassword"].asString();
    out["mediaEncPsd"] = json["OVDMediaEncPassword"].asString();
    out["appkey"] = json["andlinkID"].asString();
    out["pwd"] = json["andlinkKey"].asString();
}

void EmxProtoV1::DevHJQV2Set(Json::Value &in, Json::Value &out)
{
    ErrCodeE e;
    std::string snEncrypted;
    std::string keyPath(Misc::GetAppConfigsDir());
    keyPath+="/key/public.key";
    Base64::Decode(in["sn"].asString(), snEncrypted);
    char snPlain[32] = { 0 };
    e = EmxCrypto::RsaPubKeyDecrypt(keyPath, snEncrypted, (unsigned char*)snPlain, sizeof(snPlain));
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(out, (int)e, "decrypt sn failed");
        return;
    }
    Sn sn;
    emxlogd("sn: %s\n", snPlain);
    e = sn.Set(snPlain);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(out, (int)e, "Set sn failed");
        return;
    }

    if (!in["mac"].isString() || in["mac"].asString().size() < 1) {
        MakeCodeDesc(out, (int)e, "mac data error");
        return;
    }
    Mac mac;
    e = mac.Load();
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(out, (int)e, "load mac failed");
        return;
    }
    e = mac.Set(in["mac"].asString().c_str());
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(out, (int)e, "set mac failed");
        return;
    }
    Net::DevE dev;
    NetMainDev::Get(dev);
    Net::Addr addr = { 0 };
    NetClient net(dev);
    net.GetAddr(addr);
    memset(addr.mac, 0, sizeof(addr.mac));
    int cpy_size = in["mac"].asString().size() > sizeof(addr.mac) ? sizeof(addr.mac) : in["mac"].asString().size();
    memcpy(addr.mac, in["mac"].asString().c_str(), cpy_size);
    net.SetAddr(addr);

    Param param("burnInfo");
    Json::Value json;
    e = param.Get(json);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(out, (int)e, "Get burnInfo failed");
        return;
    }
    json["cmei"] = in["imei"].asString();
    json["OVDLoginPassword"] = in["password"].asString();
    json["OVDMediaEncPassword"] = in["mediaEncPsd"].asString();
    json["andlinkID"] = in["appkey"].asString();
    json["andlinkKey"] = in["pwd"].asString();
    e = param.Set(json);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(out, (int)e, "Set burnInfo failed");
    }
}

void EmxProtoV1::BandWidthSet(Json::Value &in, Json::Value &out) {
    ErrCodeE code = ErrCodeE::Success;
    std::string desc = "success";
    char server_ip[64] = { 0 };
    int interval_time = 0;
    int mode = 0;
    do {
        if (!in["serverIp"].isString()) {
            code = ErrCodeE::Failure;
            desc = "request no string serverIp";    
            break;
        }
        int size = in["serverIp"].asString().size();
        if (size > (int)sizeof(server_ip) || size < 1) {
            code = ErrCodeE::Failure;
            desc = "request serverIp size is error";    
            break;
        }
        memcpy(server_ip, in["serverIp"].asString().c_str(), size);

        if (!in["intervalTime"].isInt() 
            || in["intervalTime"].asInt() < 1) {
            code = ErrCodeE::Failure;
            desc = "request error intervalTime";
            break;
        }
        interval_time = in["intervalTime"].asInt();

        if (!in["mode"].isInt() 
            || (in["mode"].asInt() != 1 && in["mode"].asInt() != 2)) {
            code = ErrCodeE::Failure;
            desc = "request error mode";
            break;
        }
        mode = in["mode"].asInt();
        
        if (Cmd::Run("iperf") != ErrCodeE::Success) {
            code = ErrCodeE::Failure;
            desc = "run iperf cmd failed";
            break;
        }
        char cmd[128] = { 0 };
        if (mode == 1) {
            snprintf(cmd, sizeof(cmd) - 1
                        ,  "iperf -c %s -t %d -i 1 > /tmp/iperf.log &"
                        , server_ip, interval_time);
        } else {
            snprintf(cmd, sizeof(cmd) - 1
                        ,  "iperf -s -i 1 -f k -P 0 > /tmp/iperf.log &");
        }

        if (Cmd::Run(cmd) != ErrCodeE::Success) {
            code = ErrCodeE::Failure;
            desc = "run iperf cmd failed";
        }

    } while(false);

    MakeCodeDesc(out, (int)code, desc.c_str());
}

void EmxProtoV1::EncryptInfo(Json::Value &in, Json::Value &out) {
    out["encrypt"] = true;
}

void EmxProtoV1::IRcutExtSet(Json::Value &in, Json::Value &out) {
    ErrCodeE code = ErrCodeE::Success;
    std::string desc = "successed";
    NightVisionClient nv;
    NightVision::Param param = { 0 };

    do {
        if (!in["light"].isString()) {
            code = ErrCodeE::Failure;
            desc = "request no string light";
            break;
        }
        std::string light = in["light"].asString();
        bool is_ir_control = false;
        bool is_wl_control = false;
        if (light == "IR") {
            is_ir_control = true;
        } else if (light == "WL") {
            is_wl_control = true;
        } else {
            code = ErrCodeE::Failure;
            desc = "request no right light";
            break;
        }

        if (!in["mode"].isString()) {
            code = ErrCodeE::Failure;
            desc = "request no string mode";
            break;
        }
        std::string mode = in["mode"].asString();

        code = nv.GetParam(param);
        if (code != ErrCodeE::Success) {
            desc = "GetParam failed";
            break;
        }
        if (mode == "manual") {
            param.manual = true;
        } else if (mode == "auto") {
            //auto模式下不关注switch
            param.manual = false;
            break;
        } else {
            emxlogd("request mode no value\n");
        }
        
        if (!in["switch"].isString()) {
            code = ErrCodeE::Failure;
            desc = "request no string switch";
            break;
        }
        std::string str_switch = in["switch"].asString();

        if (is_ir_control && str_switch == "off") {
            param.manualMode = NightVision::ModeE::Day;
        } else if (is_ir_control && str_switch == "on") {
            param.manualMode = NightVision::ModeE::IrNightVision;
        } else if (is_wl_control && str_switch == "off") {
            param.manualMode = NightVision::ModeE::Day;
        } else if (is_wl_control && str_switch == "on") {
            param.manualMode = NightVision::ModeE::ColorNightVision;
        } else {
            emxlogd("request info error\n");
        }
    } while (false);

    if (code == ErrCodeE::Success) {
        code = nv.SetParam(param);
        if (code != ErrCodeE::Success) {
            desc = "SetParam failed";
        }
    }
    MakeCodeDesc(out, (int)code, desc.c_str());
}

void EmxProtoV1::LDSGet(Json::Value &in, Json::Value &out) {
    MakeCodeDesc(out, (int)ErrCodeE::Failure, "no support lds check");
}

void EmxProtoV1::SetPlatform(const EmxPlatform::PlatE &pe)
{
    m_pe = pe;
}

void EmxProtoV1::NetGet(Json::Value &in, Json::Value &out) {

}

void EmxProtoV1::NetSet(Json::Value &in, Json::Value &out) {
}

void EmxProtoV1::NetWifiGet(Json::Value &in, Json::Value &out) {
}

void EmxProtoV1::NetWifiSet(Json::Value &in, Json::Value &out) {
}

static void SetRange(Json::Value &out, const char *key, Json::Value &range) {
    char tmp[256];
    snprintf(tmp, 256, "[%d,%d]", range["min"].asInt(), range["max"].asInt());
    out[key] = tmp;
}

static void VideoQue(int chn, Json::Value &in, Json::Value &out) {
    MediaClientVenc venc(chn);
    Json::Value range;
    if (venc.GetParamRange(range) != ErrCodeE::Success) {
        MakeCodeDesc(out, EP_CFG_CODE_ERR, EP_CFG_DESC_FAILED);
        return;
    }
    MakeCodeDesc(out, EP_CFG_CODE_OK, EP_CFG_DESC_OK);
    std::string tmpStr;
    for (auto &e : range["codec"]) {
        if (!tmpStr.empty())
            tmpStr.append(",");
        if (e == (int) VideoCodecE::H264) {
            tmpStr.append("h264");
        } else if (e == (int) VideoCodecE::H265) {
            tmpStr.append("h265");
        }
    }
    out["codec"] = tmpStr;

    tmpStr.clear();
    for (auto &e : range["rcMode"]) {
        if (!tmpStr.empty())
            tmpStr.append(",");
        if (e == (int) MediaVenc::RcModeE::VBR) {
            tmpStr.append("vbr");
        } else if (e == (int) MediaVenc::RcModeE::CBR) {
            tmpStr.append("cbr");
        }
    }
    out["rc_mode"] = tmpStr;
    out["profile"] = "[0,1]";

    tmpStr.clear();
    for (auto &e : range["resolution"]) {
        if (!tmpStr.empty())
            tmpStr.append(",");
        tmpStr.append(std::to_string(e["w"].asInt()));
        tmpStr.append("x");
        tmpStr.append(std::to_string(e["h"].asInt()));
    }
    out["resolution"] = tmpStr;

    SetRange(out, "frame_rate", range["fps"]);
    SetRange(out, "bitrate", range["bitRate"]);
    out["gop"] = "15,30,45,60,100";
    out["flip"] = "[0,1]";
    out["mirror"] = "[0,1]";
}

static void VideoGet(int chn, Json::Value &in, Json::Value &out) {
    MediaClientVenc venc(chn);
    MediaVenc::Param param = {};
    if (venc.GetParam(param) != ErrCodeE::Success) {
        MakeCodeDesc(out, EP_CFG_CODE_ERR, EP_CFG_DESC_FAILED);
        return;
    }
    MakeCodeDesc(out, EP_CFG_CODE_OK, EP_CFG_DESC_OK);
    char codec[16] = { 0 };
    char rc_mode[16] = { 0 };
    if (param.codec == VideoCodecE::H264) {
        snprintf(codec, sizeof(codec) - 1, "h264");
    } else if (param.codec == VideoCodecE::H265) {
        snprintf(codec, sizeof(codec) - 1, "h265");
    } else if (param.codec == VideoCodecE::MJPEG) {
        snprintf(codec, sizeof(codec) - 1, "mjpeg");
    } else if (param.codec == VideoCodecE::BUTT) {
        snprintf(codec, sizeof(codec) - 1, "butt");
    } else {
        emxloge("no codec param\n");
    }
    
    if (param.rcMode == MediaVenc::RcModeE::VBR) {
        snprintf(rc_mode, sizeof(rc_mode) - 1, "vbr");
    } else if (param.rcMode == MediaVenc::RcModeE::CBR) {
        snprintf(rc_mode, sizeof(rc_mode) - 1, "cbr");
    } else if (param.rcMode == MediaVenc::RcModeE::AVBR) {
        snprintf(rc_mode, sizeof(rc_mode) - 1, "avbr");
    } else if (param.rcMode == MediaVenc::RcModeE::BUTT) {
        snprintf(rc_mode, sizeof(rc_mode) - 1, "butt");
    } else {
        emxloge("no rc_mode param\n");
    }
    
    out["codec"] = std::string(codec, strlen(codec));
    out["rc_mode"] = std::string(rc_mode, strlen(rc_mode));
    out["profile"] = 1;
    out["resolution"] = std::to_string(param.width) + "x" + std::to_string(param.height);
    out["frame_rate"] = param.fps;
    out["bitrate"] = param.bitRate;
    out["gop"] = param.iFrameInv * param.fps;

    int vi_chn_num = 0;
    MediaClientVi::GetChnNum(vi_chn_num);
    MediaVi::Param vi_param = { 0 };
    if (vi_chn_num > 0) {
        MediaClientVi client(0);
        if (client.GetParam(vi_param) == ErrCodeE::Success) {
            out["flip"] = vi_param.flip;
            out["mirror"] = vi_param.mirror;
        } else {
            emxloge("vi GetParam is failed.\n");
        }
    }
}

static void VideoSet(int chn, Json::Value &in, Json::Value &out) {
    MediaClientVenc venc(chn);
    MediaVenc::Param param = {};
    if (venc.GetParam(param) != ErrCodeE::Success) {
        emxloge("cannot got chn %d venc param\n", chn);
        MakeCodeDesc(out, EP_CFG_CODE_ERR, EP_CFG_DESC_FAILED);
        return;
    }
    if (in.isMember("bitrate")) {
        param.bitRate = in["bitrate"].asInt();
    }

    if (in.isMember("codec")) {
        if (in["codec"].asString() == "h264") {
            param.codec = VideoCodecE::H264;
        } else if (in["codec"].asString() == "h265") {
            param.codec = VideoCodecE::H265;
        } else if (in["codec"].asString() == "mjpeg") {
            param.codec = VideoCodecE::MJPEG;
        } else {
            emxloge("codec %s not be supported\n", in["codec"].asCString());
            MakeCodeDesc(out, EP_CFG_CODE_ERR, EP_CFG_DESC_FAILED);
            return;
        }
    }

    if (in.isMember("frame_rate") && in.isMember("gop")) {
        param.fps = in["frame_rate"].asInt();
        if (param.fps == 0)
            MakeCodeDesc(out, EP_CFG_CODE_ERR, EP_CFG_DESC_FAILED);
        param.iFrameInv = in["gop"].asInt() / param.fps;
    }

    if (in.isMember("rc_mode")) {
        if (in["rc_mode"].asString() == "vbr") {
            param.rcMode = MediaVenc::RcModeE::VBR;
        } else if (in["rc_mode"].asString() == "cbr") {
            param.rcMode = MediaVenc::RcModeE::CBR;
        } else if (in["rc_mode"].asString() == "avbr") {
            param.rcMode = MediaVenc::RcModeE::AVBR;
        } else {
            emxloge("rc_mode %s not be supported\n", in["rc_mode"].asCString());
            MakeCodeDesc(out, EP_CFG_CODE_ERR, EP_CFG_DESC_FAILED);
            return;
        }
    }

    if (in.isMember("resolution")) {
        std::string res = in["resolution"].asString();
        char *end = nullptr;
        param.width = strtol(res.data(), &end, 10);
        param.height = strtol(end + 1, nullptr, 10);
    }
    if (venc.SetParam(param) == Emx::ErrCodeE::Success)
        MakeCodeDesc(out, EP_CFG_CODE_OK, EP_CFG_DESC_OK);
    else
        MakeCodeDesc(out, EP_CFG_CODE_ERR, EP_CFG_DESC_FAILED);
    

    int vi_chn_num = 0;
    MediaClientVi::GetChnNum(vi_chn_num);
    MediaVi::Param vi_param = { 0 };
    if (vi_chn_num > 0) {
        emxlogd("vi chn num(%d)\n", vi_chn_num);
        MediaClientVi client(0);
        if (in.isMember("flip")) {
            vi_param.flip = in["flip"].asInt() == 0 ? false : true;
        }
        if (in.isMember("mirror")) {
            vi_param.mirror = in["mirror"].asInt() == 0 ? false : true;
        }
        if (client.SetParam(vi_param) != ErrCodeE::Success) {
            emxloge("SetParam is failed\n");
        }
    }
}


void EmxProtoV1::VideoMajorGet(Json::Value &in, Json::Value &out) {
    VideoGet(0, in, out);
}

void EmxProtoV1::VideoMajorSet(Json::Value &in, Json::Value &out) {
    VideoSet(0, in, out);
}

void EmxProtoV1::VideoMajorQue(Json::Value &in, Json::Value &out) {
    VideoQue(0, in, out);
}

void EmxProtoV1::VideoMinorGet(Json::Value &in, Json::Value &out) {
    VideoGet(1, in, out);
}

void EmxProtoV1::VideoMinorSet(Json::Value &in, Json::Value &out) {
    VideoSet(1, in, out);
}

void EmxProtoV1::VideoMinorQue(Json::Value &in, Json::Value &out) {
    VideoQue(1, in, out);
}

void EmxProtoV1::AudioGet(Json::Value &in, Json::Value &out) {
    MediaClientAenc aenc(0);
    MediaClientAdec adec(0);
    MediaAenc::Param aencParam = {};
    MediaAdec::Param adecParam = {};

    if (aenc.GetParam(aencParam) != ErrCodeE::Success) {
        emxloge("get aenc param failed\n");
        MakeCodeDesc(out, EP_CFG_CODE_ERR, EP_CFG_DESC_FAILED);
        return;
    }
    if (adec.GetParam(adecParam) != ErrCodeE::Success) {
        emxloge("get adec param failed\n");
        MakeCodeDesc(out, EP_CFG_CODE_ERR, EP_CFG_DESC_FAILED);
        return;
    }

    if (aencParam.codec == AudioCodecE::AAC)
        out["codec"] = "acc";
    else if (aencParam.codec == AudioCodecE::G711A)
        out["codec"] = "g711a";
    else if (aencParam.codec == AudioCodecE::G711U)
        out["codec"] = "g711u";
    else if (aencParam.codec == AudioCodecE::LPCM)
        out["codec"] = "pcm";
    else
        out["codec"] = "unknown";
    out["sample_rate"] = aencParam.sampleRate;
    out["bitrate"] = aencParam.bitRate;
    out["bit_width"] = aencParam.bitWidth;
    out["channel_num"] = 1;
    out["mic_vol"] = aencParam.volume;
    out["spk_vol"] = adecParam.volume;
    MakeCodeDesc(out, EP_CFG_CODE_OK, EP_CFG_DESC_OK);
}

void EmxProtoV1::AudioSet(Json::Value &in, Json::Value &out) {
    MediaClientAenc aenc(0);
    MediaClientAdec adec(0);
    MediaAenc::Param aencParam = {};
    MediaAdec::Param adecParam = {};

    if (aenc.GetParam(aencParam) != ErrCodeE::Success) {
        emxloge("get aenc param failed\n");
        MakeCodeDesc(out, EP_CFG_CODE_ERR, EP_CFG_DESC_FAILED);
        return;
    }
    if (adec.GetParam(adecParam) != ErrCodeE::Success) {
        emxloge("get adec param failed\n");
        MakeCodeDesc(out, EP_CFG_CODE_ERR, EP_CFG_DESC_FAILED);
        return;
    }
    if (in["codec"].asString() == "acc") {
        aencParam.codec = AudioCodecE::AAC;
        adecParam.codec = AudioCodecE::AAC;
    } else if (in["codec"].asString() == "g711a") {
        aencParam.codec = AudioCodecE::G711A;
        adecParam.codec = AudioCodecE::G711A;
    } else if (in["codec"].asString() == "g711u") {
        aencParam.codec = AudioCodecE::G711U;
        adecParam.codec = AudioCodecE::G711U;
    } else if (in["codec"].asString() == "pcm") {
        aencParam.codec = AudioCodecE::LPCM;
        adecParam.codec = AudioCodecE::LPCM;
    }

    if (in["sample_rate"].isInt()) {
        aencParam.sampleRate = in["sample_rate"].asInt();
        adecParam.sampleRate = in["sample_rate"].asInt();
    }
    if (in["bitrate"].isInt()) {
        aencParam.bitRate = in["bitrate"].asInt();
        adecParam.bitRate = in["bitrate"].asInt();
    }
    if (in["bit_width"].isInt()) {
        aencParam.bitWidth = in["bit_width"].asInt();
        adecParam.bitWidth = in["bit_width"].asInt();
    }

    if (in.isMember("mic_vol")) {
        aencParam.volume = in["mic_vol"].asInt();
    }
    if (in.isMember("spk_vol")) {
        adecParam.volume = in["spk_vol"].asInt();
    }
    if (aenc.SetParam(aencParam) == Emx::ErrCodeE::Success &&
        adec.SetParam(adecParam) == Emx::ErrCodeE::Success)
        MakeCodeDesc(out, EP_CFG_CODE_OK, EP_CFG_DESC_OK);
    else
        MakeCodeDesc(out, EP_CFG_CODE_ERR, EP_CFG_DESC_FAILED);
}

void EmxProtoV1::AudioQue(Json::Value &in, Json::Value &out) {
    MediaClientAenc aenc(0);
    MediaClientAdec adec(0);
    Json::Value rangeAenc;
    Json::Value rangeAdec;
    if (aenc.GetParamRange(rangeAenc) != ErrCodeE::Success) {
        emxloge("get aenc range failed\n");
        MakeCodeDesc(out, EP_CFG_CODE_ERR, EP_CFG_DESC_FAILED);
        return;
    }
    if (adec.GetParamRange(rangeAdec) != ErrCodeE::Success) {
        emxloge("get adec range failed\n");
        MakeCodeDesc(out, EP_CFG_CODE_ERR, EP_CFG_DESC_FAILED);
        return;
    }
    out["codec"] = "g711a,acc";
    out["sample_rate"] = "8000,16000";
    out["bitrate"] = "[32,128]";
    out["bit_width"] = "16";
    out["channel_num"] = "1";
    SetRange(out, "mic_vol", rangeAenc["volume"]);
    SetRange(out, "spk_vol", rangeAdec["volume"]);
    MakeCodeDesc(out, EP_CFG_CODE_OK, EP_CFG_DESC_OK);
}

void EmxProtoV1::IspGet(Json::Value &in, Json::Value &out) {
    MediaClientISP isp(0);
    MediaISP::Param param = {};
    if (isp.GetParam(param) != ErrCodeE::Success) {
        MakeCodeDesc(out, EP_CFG_CODE_ERR, EP_CFG_DESC_FAILED);
        return;
    }
    MakeCodeDesc(out, EP_CFG_CODE_OK, EP_CFG_DESC_OK);
    if (param.expMode == MediaISP::OptModeE::Manual)
        out["exp_mode"] = "manual";
    else
        out["exp_mode"] = "auto";
    out["auto_max_exp_gain"] = (int) param.autoMaxExpGain;
    out["manual_exp_gain"] = (int) param.manualExpGain;
    out["compensation"] = (int) param.compensation;
    out["shutter"] = 1000 / (int) param.expTime;
    out["brightness"] = param.brightness;
    out["sharpness"] = param.sharpness;
    out["hue"] = param.hue;
    out["contrast"] = param.contrast;
    out["saturation"] = param.saturation;
    out["3Ddenoise"] = param.deNoise;
    if (param.wbMode == MediaISP::OptModeE::Manual)
        out["wb_mode"] = "manual";
    else
        out["wb_mode"] = "auto";
    out["red_gain"] = param.redGain;
    out["blue_gain"] = param.blueGain;
}

void EmxProtoV1::IspSet(Json::Value &in, Json::Value &out) {

}

void EmxProtoV1::IspQue(Json::Value &in, Json::Value &out) {
//    Json::Value range;
//    MediaClientISP isp(0);
//    if (isp.GetParamRange(range) != ErrCodeE::Success) {
//        MakeCodeDesc(out, EP_CFG_CODE_ERR, EP_CFG_DESC_FAILED);
//        return;
//    }
//    MakeCodeDesc(out, EP_CFG_CODE_OK, EP_CFG_DESC_OK);
//    out["exp_mode"] = "auto,manual";
//    out["auto_max_exp_gain"] = "1,2,4,8,16,32,64";
//    out["manual_exp_gain"] = "1,2,4,8,16,32,64";
//    out["compensation"] = "[0,255]";
//    out["shutter"] = "1,2,3,5,8,10,15,20,25,50,100,150,200,250,500,1000,2000,4000,8000";
//    out["brightness"] = "[0,255]";
//    out["hue"] = "[0,255]";
//    out["contrast"] = "[0,255]";
//    out["saturation"] = "[0,255]";
//    out["red_gain"] = "[0,255]";
//    out["blue_gain"] = "[0,255]";
//    out["sharpness"] = "[0,255]";
//    out["3Ddenoise"] = "[0,255]";
//    out["wb_mode"] = "auto,manual";
}

void EmxProtoV1::SnapGet(Json::Value &in, std::string &out) {
    MediaClientStream::Channel channel;
    channel.Add(MediaFrame::TypeE::Snap, 0);
    MediaClientStreamSync snap;
    if (snap.Open(channel, 3000) != ErrCodeE::Success) {
        emxloge("open EP_CFG_DESC_FAILED\n");
        MakeCodeDesc(in, EP_CFG_CODE_ERR, EP_CFG_DESC_FAILED);
        return;
    }
    MediaClientSnap::Trigger(0, 3000);
    MediaFrame frame = {};
    ErrCodeE e = snap.GetFrame(frame, 3000);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(in, EP_CFG_CODE_ERR, EP_CFG_DESC_FAILED);
    } else {
        out.assign((char *) frame.data, frame.size);
    }
    snap.Close();
}

void EmxProtoV1::MusicPlay(Json::Value &in, Json::Value &out) {
    if (in.isMember("play")) {
        if (strcmp(in["play"].asCString(), "start") == 0) {
            char audioDir[EMX_MAX_PATH_SIZE] = {};
            snprintf(audioDir, sizeof(audioDir), "%s/audio", Misc::GetAppConfigsDir());
            DIR *dp = opendir(audioDir);
            if (!dp) {
                emxloge("cannot open dir %s\n", audioDir);
                return;
            }
            char path[EMX_MAX_PATH_SIZE] = {};
            dirent *entry;
            while ((entry = readdir(dp)) != nullptr) {
                if (strstr(entry->d_name, "music.")) {
                    snprintf(path, sizeof(path), "%s/%s", audioDir, entry->d_name);
                    break;
                }
            }
            closedir(dp);
            if (path[0]) {
                MediaClientAdecStreamSync::PlayFile(0, path);
                MakeCodeDesc(out, EP_CFG_CODE_OK, EP_CFG_DESC_OK);
                return;
            } else {
                MakeCodeDesc(out, EP_CFG_CODE_ERR, "cannot found music file");
                return;
            }
        } else if (strcmp(in["play"].asCString(), "stop") == 0) {
            MediaClientAdecStreamSync::PlayCtrlQuit(0);
            MakeCodeDesc(out, EP_CFG_CODE_OK, EP_CFG_DESC_OK);
            return;
        }
    }
    MakeCodeDesc(out, EP_CFG_CODE_ERR, EP_CFG_DESC_FAILED);
}

void EmxProtoV1::Reboot(Json::Value &in, Json::Value &out) {
    Reboot::DoReboot(3);
    MakeCodeDesc(out, EP_CFG_CODE_ERR, EP_CFG_DESC_FAILED);
}

void EmxProtoV1::Reset(Json::Value &in, Json::Value &out) {

}

void EmxProtoV1::SDInfo(Json::Value &in, Json::Value &out) {
    SDCardClient sd(0);
    SDCard::Info info = {};
    if (sd.GetInfo(info) != ErrCodeE::Success) {
        MakeCodeDesc(out, EP_CFG_CODE_ERR, EP_CFG_DESC_FAILED);
        return;
    }
    MakeCodeDesc(out, EP_CFG_CODE_OK, EP_CFG_DESC_OK);
    switch (info.stat) {
        case SDCard::StatE::Normal:
            out["status"] = "normal";
            break;
        case SDCard::StatE::Rejected:
            out["status"] = "uninsert";
            break;
        case SDCard::StatE::Inserted:
            out["status"] = "unmount";
            break;
        case SDCard::StatE::Formatting:
            out["status"] = "formatting";
            break;
        default:
            out["status"] = "unknown";
            break;
    }
    double size;
    char tmpStr[32] = {};

    size = info.total;
    if (size >= 1024) {
        sprintf(tmpStr, "%.1fGB", size / 1024);
    } else {
        sprintf(tmpStr, "%.1fMB", size);
    }
    out["total"] = tmpStr;

    size = info.free;
    if (size >= 1024) {
        sprintf(tmpStr, "%.1fGB", size / 1024);
    } else {
        sprintf(tmpStr, "%.1fMB", size);
    }
    out["free"] = tmpStr;
}

void EmxProtoV1::SDFormat(Json::Value &in, Json::Value &out) {
    SDCardClient sd(0);
    SDCard::Info info = {};
    if (sd.GetInfo(info) != ErrCodeE::Success) {
        MakeCodeDesc(out, EP_CFG_CODE_ERR, EP_CFG_DESC_FAILED);
        return;
    }
    if (sd.Format() == ErrCodeE::Success) {
        MakeCodeDesc(out, EP_CFG_CODE_OK, EP_CFG_DESC_OK);
    } else {
        MakeCodeDesc(out, EP_CFG_CODE_ERR, EP_CFG_DESC_FAILED);
    }
}

void EmxProtoV1::TimeGet(Json::Value &in, Json::Value &out) {
    char time[64] = { 0 };
    Time::GetTimeStrNow(time, sizeof(time));
    out["time"] = std::string(time, strlen(time));
    
    TimeZone tz;
    auto e = tz.Load();
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(out, (int)e, "load zone failed");
    }
    out["zone"] = tz.Get();
}

void EmxProtoV1::TimeSet(Json::Value &in, Json::Value &out) {
    if (in.isMember("utc")) {
        Time::SetSysTime(in["utc"].asInt64());
    }
    if (in.isMember("zone")) {
        ErrCodeE e;
        TimeZone tz;
        e = tz.Load();
        if (e != ErrCodeE::Success) {
            MakeCodeDesc(out, (int)e, "load zone failed");
        }
        e = tz.Set(in["zone"].asCString());
        if (e != ErrCodeE::Success) {
            MakeCodeDesc(out, (int)e, "set zone failed");
        }
    }
}

void EmxProtoV1::ICRGet(Json::Value &in, Json::Value &out) {
    ErrCodeE code = ErrCodeE::Success;
    std::string desc = "successed";
    NightVisionClient nv;
    NightVision::Param param = { 0 };

    do {
        code = nv.GetParam(param);
        if (code != ErrCodeE::Success) {
            desc = "GetParam failed";
            break;
        }
        if (param.manual) {
            out["mode"] = "manual";
        } else {
            out["mode"] = "auto";
            out["switch"] = "null";
            break;
        }

        if (param.manualMode == NightVision::ModeE::Day) {
            out["switch"] = "off";
        } else if(param.manualMode == NightVision::ModeE::IrNightVision) {
            out["switch"] = "on";
        } else {
            out["switch"] = "null";
        }
    } while (false);
    MakeCodeDesc(out, (int)code, desc.c_str());
}

void EmxProtoV1::LDSSet(Json::Value &in, Json::Value &out) {
   
}

void EmxProtoV1::OSDGet(Json::Value &in, Json::Value &out) {

}

void EmxProtoV1::OSDSet(Json::Value &in, Json::Value &out) {

}

void EmxProtoV1::TemplateGet(Json::Value &in, Json::Value &out) {

}

void EmxProtoV1::TemplateSet(Json::Value &in, Json::Value &out) {

}

void EmxProtoV1::CenterGet(Json::Value &in, Json::Value &out) {

}

void EmxProtoV1::CenterSet(Json::Value &in, Json::Value &out) {

}

/*电机指令获取*/
void EmxProtoV1::PtzGetV2(Json::Value &in, Json::Value &out)
{

}

/*电机指令设置*/
void EmxProtoV1::PtzSetV2(Json::Value &in, Json::Value &out)
{

}

/*人脸获取*/
void EmxProtoV1::FaceGet(Json::Value &in, Json::Value &out)
{

}

/*人脸设置*/
void EmxProtoV1::FaceSet(Json::Value &in, Json::Value &out)
{

}

/*人脸删除*/
void EmxProtoV1::FaceDel(Json::Value &in, Json::Value &out)
{

}

/*红外功率设置*/
void EmxProtoV1::IredwGet(Json::Value &in, Json::Value &out)
{

}

/*红外功率获取*/
void EmxProtoV1::IredwSet(Json::Value &in, Json::Value &out)
{

}

/*LED 灯光颜色停止闪烁*/
void EmxProtoV1::LedStop(Json::Value &in, Json::Value &out){
    
}

/*复位按键开始检测停止*/
void EmxProtoV1::ResetStop(Json::Value &in, Json::Value &out)
{
    
}

/*设置老化模式*/
void EmxProtoV1::AgingSet(Json::Value &in, Json::Value &out) {
    
}

/*单板模式设置(设备端关闭红外、关闭白光灯，设备处于强制白天模式)*/
void EmxProtoV1::SingleBoardSet(Json::Value &in, Json::Value &out)
{
    
}

/*烧录信息清除*/
void EmxProtoV1::BurnInfoClearSet(Json::Value &in, Json::Value &out)
{

}

/*21CN平台号码获取*/
void EmxProtoV1::Dev21CNGet(Json::Value &in, Json::Value &out)
{
    if (m_pe != EmxPlatform::PlatE::E21CN) {
        MakeCodeDesc(out, EP_CFG_CODE_ERR, "unsupport platform!");
        return;
    }

    ErrCodeE ecode;
    Json::Value jsval;
    Mac mac;
    Param pms(EP_PLTBURN_FILE);

    ecode = pms.Get(jsval);
    if (ecode != ErrCodeE::Success) {
        MakeCodeDesc(out, EP_CFG_CODE_ERR, EP_CFG_DESC_FAILED);
        return;
    }

    out["mac"] = mac.Get();
    MakeCodeDesc(out, EP_CFG_CODE_OK, EP_CFG_DESC_OK);
}

/*21CN平台号码烧录*/
void EmxProtoV1::Dev21CNSet(Json::Value &in, Json::Value &out)
{
    if (m_pe != EmxPlatform::PlatE::E21CN) {
        MakeCodeDesc(out, EP_CFG_CODE_ERR, "unsupport platform!");
        return;
    }

    ErrCodeE ecode;
    Json::Value jsval;
    std::string macStr, snStr;
    std::string srcEn, dstEn;
    Mac mac;
    Sn sn;
    Param pms(EP_PLTBURN_FILE);

    srcEn = in["p2p_uid"].asString();
    ecode = EmxCrypto::RsaPubKeyDecrypt(m_pubkeyPath, srcEn, dstEn);
    if (ecode != ErrCodeE::Success) {
        MakeCodeDesc(out, EP_CFG_CODE_ERR, "decrypt sn failed\n");
        return;
    }

    jsval["p2p_uid"] = dstEn;
    jsval["user_psd"] = in["user_psd"].asString();
    jsval["p2p_ctei"] = in["p2p_ctei"].asString();
    jsval["p2p_sn"] = in["p2p_sn"].asString();
    jsval["p2p_bind_code"] = in["p2p_bind_code"].asString();
    jsval["mac"] = in["mac"].asString();

    ecode = pms.Set(jsval);
    if (ecode != ErrCodeE::Success) {
        MakeCodeDesc(out, EP_CFG_CODE_ERR, "set common params failed");
        return;
    }

    macStr = in["mac"].asString();
    ecode = mac.Set(macStr.c_str());
    if (ecode != ErrCodeE::Success) {
        MakeCodeDesc(out, EP_CFG_CODE_ERR, "mac set failed");
        return;
    }

    snStr = dstEn;
    ecode = sn.Set(snStr.c_str());
    if (ecode != ErrCodeE::Success) {
        MakeCodeDesc(out, EP_CFG_CODE_ERR, "sn set failed");
        return;
    }

    MakeCodeDesc(out, EP_CFG_CODE_OK, EP_CFG_DESC_OK);
}

/*和家亲1.0平台号码烧录*/
void EmxProtoV1::DevHJQV1Set(Json::Value &in, Json::Value &out)
{
    if (m_pe != EmxPlatform::PlatE::EHJQV1) {
        MakeCodeDesc(out, EP_CFG_CODE_ERR, "unsupport platform!");
        return;
    }

    ErrCodeE ecode;
    Json::Value jsval;
    std::string macStr, snStr;
    std::string srcEn, dstEn;
    Mac mac;
    Sn sn;
    Param pms(EP_PLTBURN_FILE);

    srcEn = in["sn"].asString();
    ecode = EmxCrypto::RsaPubKeyDecrypt(m_pubkeyPath, srcEn, dstEn);
    if (ecode != ErrCodeE::Success) {
        MakeCodeDesc(out, EP_CFG_CODE_ERR, "decrypt sn failed\n");
        return;
    }

    jsval["sn"] = dstEn;
    jsval["imei"] = in["imei"].asString();
    jsval["mac"] = in["mac"].asString();

    ecode = pms.Set(jsval);
    if (ecode != ErrCodeE::Success) {
        MakeCodeDesc(out, EP_CFG_CODE_ERR, "set common params failed");
        return;
    }

    macStr = in["mac"].asString();
    ecode = mac.Set(macStr.c_str());
    if (ecode != ErrCodeE::Success) {
        MakeCodeDesc(out, EP_CFG_CODE_ERR, "mac set failed");
        return;
    }

    snStr = dstEn;
    ecode = sn.Set(snStr.c_str());
    if (ecode != ErrCodeE::Success) {
        MakeCodeDesc(out, EP_CFG_CODE_ERR, "sn set failed");
        return;
    }

    MakeCodeDesc(out, EP_CFG_CODE_OK, EP_CFG_DESC_OK);
}

/*和家亲1.0平台号码获取*/
void EmxProtoV1::DevHJQV1Get(Json::Value &in, Json::Value &out)
{
    if (m_pe != EmxPlatform::PlatE::EHJQV1) {
        MakeCodeDesc(out, EP_CFG_CODE_ERR, "unsupport platform!");
        return;
    }

    ErrCodeE ecode;
    Json::Value jsval;
    Mac mac;
    Param pms(EP_PLTBURN_FILE);

    ecode = pms.Get(jsval);
    if (ecode != ErrCodeE::Success) {
        MakeCodeDesc(out, EP_CFG_CODE_ERR, EP_CFG_DESC_FAILED);
        return;
    }

    out["mac"] = mac.Get();
    MakeCodeDesc(out, EP_CFG_CODE_OK, EP_CFG_DESC_OK);
}

/*云眼平台号码烧录*/
void EmxProtoV1::DevYYSet(Json::Value &in, Json::Value &out)
{
    if (m_pe != EmxPlatform::PlatE::EYY) {
        MakeCodeDesc(out, EP_CFG_CODE_ERR, "unsupport platform!");
        return;
    }       
}

/*云眼平台号码获取*/
void EmxProtoV1::DevYYGet(Json::Value &in, Json::Value &out)
{
    if (m_pe != EmxPlatform::PlatE::EYY) {
        MakeCodeDesc(out, EP_CFG_CODE_ERR, "unsupport platform!");
        return;
    }        
}

/*浙江电信平台号码获取*/
void EmxProtoV1::DevZJDXGet(Json::Value &in, Json::Value &out)
{   
}

/*浙江电信平台号码烧录*/
void EmxProtoV1::DevZJDXSet(Json::Value &in, Json::Value &out)
{
    
}

/*天翼看店平台号码烧录*/
void EmxProtoV1::DevTYKDSet(Json::Value &in, Json::Value &out)
{
    if (m_pe != EmxPlatform::PlatE::ETYKJ) {
        MakeCodeDesc(out, EP_CFG_CODE_ERR, "unsupport platform!");
        return;
    }      
}

/*天翼看店平台号码获取*/
void EmxProtoV1::DevTYKDGet(Json::Value &in, Json::Value &out)
{
    if (m_pe != EmxPlatform::PlatE::ETYKJ) {
        MakeCodeDesc(out, EP_CFG_CODE_ERR, "unsupport platform!");
        return;
    }     
}

/*看家宝平台号码获取*/
void EmxProtoV1::DevKJBGet(Json::Value &in, Json::Value &out)
{
    
}

/*看家宝平台号码烧录*/
void EmxProtoV1::DevKJBSet(Json::Value &in, Json::Value &out)
{
    
}

/*国标平台号码烧录、获取*/
void EmxProtoV1::DevGBGet(Json::Value &in, Json::Value &out)
{
    
}

/*国标平台号码烧录、获取*/
void EmxProtoV1::DevGBSet(Json::Value &in, Json::Value &out)
{
    
}

/*小草平台号码烧录、获取*/
void EmxProtoV1::DevXCSet(Json::Value &in, Json::Value &out)
{
    if (m_pe != EmxPlatform::PlatE::ECAMTELLS) {
        MakeCodeDesc(out, EP_CFG_CODE_ERR, "unsupport platform!");
        return;
    }     

    ErrCodeE ecode;
    Json::Value jsval;
    std::string macStr, snStr;
    std::string srcEn, dstEn;
    Mac mac;
    Sn sn;
    Param pms(EP_PLTBURN_FILE);

    srcEn = in["did"].asString();
    ecode = EmxCrypto::RsaPubKeyDecrypt(m_pubkeyPath, srcEn, dstEn);
    if (ecode != ErrCodeE::Success) {
        MakeCodeDesc(out, EP_CFG_CODE_ERR, "decrypt sn failed\n");
        return;
    }

    jsval["did"] = srcEn;
    jsval["signkey"] = in["signkey"].asString();
    jsval["lslat"] = in["lslat"].asString();
    jsval["scode"] = in["scode"].asString();
    jsval["gipaddr"] = in["gipaddr"].asString();
    jsval["mac"] = in["mac"].asString();

    ecode = pms.Set(jsval);
    if (ecode != ErrCodeE::Success) {
        MakeCodeDesc(out, EP_CFG_CODE_ERR, "set common params failed");
        return;
    }

    macStr = in["mac"].asString();
    ecode = mac.Set(macStr.c_str());
    if (ecode != ErrCodeE::Success) {
        MakeCodeDesc(out, EP_CFG_CODE_ERR, "mac set failed");
        return;
    }

    snStr = srcEn;
    ecode = sn.Set(snStr.c_str());
    if (ecode != ErrCodeE::Success) {
        MakeCodeDesc(out, EP_CFG_CODE_ERR, "sn set failed");
        return;
    }

    MakeCodeDesc(out, EP_CFG_CODE_OK, EP_CFG_DESC_OK);
}

/*小草平台号码烧录、获取*/
void EmxProtoV1::DevXCGet(Json::Value &in, Json::Value &out)
{
    if (m_pe != EmxPlatform::PlatE::ECAMTELLS) {
        MakeCodeDesc(out, EP_CFG_CODE_ERR, "unsupport platform!");
        return;
    }      

    ErrCodeE ecode;
    Json::Value jsval;
    Mac mac;
    Param pms(EP_PLTBURN_FILE);

    ecode = pms.Get(jsval);
    if (ecode != ErrCodeE::Success) {
        MakeCodeDesc(out, EP_CFG_CODE_ERR, EP_CFG_DESC_FAILED);
        return;
    }

    out["mac"] = mac.Get();
    MakeCodeDesc(out, EP_CFG_CODE_OK, EP_CFG_DESC_OK);
}

/*涂鸦平台号码烧录、获取*/
void EmxProtoV1::DevTYGet(Json::Value &in, Json::Value &out)
{
    if (m_pe != EmxPlatform::PlatE::ETY) {
        MakeCodeDesc(out, EP_CFG_CODE_ERR, "unsupport platform!");
        return;
    }     
}

/*涂鸦平台号码烧录、获取*/
void EmxProtoV1::DevTYSet(Json::Value &in, Json::Value &out)
{
    if (m_pe != EmxPlatform::PlatE::ETY) {
        MakeCodeDesc(out, EP_CFG_CODE_ERR, "unsupport platform!");
        return;
    }      
}        



