//
// Created by xiong on 2021/5/21.
//
#include "EmxCore.hpp"
#include "EmxMedia.hpp"
#include "CamerakingProto.hpp"
#include "DevInfo.hpp"
#include "Sn.hpp"
#include "SDCardClient.hpp"
#include "Mac.hpp"
#include "Sn.hpp"
#include "EmxCrypto.hpp"
#include "NightVisionClient.hpp"
#include "NetClient.hpp"

using namespace Emx;

void CameraKingProto::MakeCodeDesc(Json::Value &out, ErrCodeE e, const char *desc) {
    out["code"] = (int)e;
    out["desc"] = desc;
}

void CameraKingProto::SnGet(Json::Value &in, Json::Value &out) {
    ErrCodeE e;
    Sn sn;
    e = sn.Load();
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(out, e, "load sn failed");
        return;
    }
    out["sn"] = sn.Get();
}

void CameraKingProto::SnSet(Json::Value &in, Json::Value &out) {
    ErrCodeE e;
    std::string snEncrypted;
    Base64::Decode(in["sn"].asString(), snEncrypted);
    std::string snPlain;
    e = EmxCrypto::RsaPubKeyDecrypt("", snEncrypted, snPlain);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(out, e, "decrypt sn failed");
        return;
    }
    Sn sn;
    e = sn.Set(snPlain.c_str());
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(out, e, "Set sn failed");
        return;
    }
}

void CameraKingProto::WifiMacGet(Json::Value &in, Json::Value &out) {
    Mac mac;
    auto e = mac.Load();
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(out, e, "load mac failed");
        return;
    }
    out["mac"] = mac.Get();
}

void CameraKingProto::WifiMacSet(Json::Value &in, Json::Value &out) {
    Mac mac;
    auto e = mac.Load();
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(out, e, "load mac failed");
        return;
    }
    e = mac.Set(out["mac"].asCString());
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(out, e, "set mac failed");
    }
}

void CameraKingProto::ImeiGet(Json::Value &in, Json::Value &out) {
    Param param("burnInfo");
    Json::Value json;
    auto e = param.Get(json);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(out, e, "Get failed");
        return;
    }
    out["imei"] = json["cmei"];
}

void CameraKingProto::ImeiSet(Json::Value &in, Json::Value &out) {
    Param param("burnInfo");
    Json::Value json;
    auto e = param.Get(json);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(out, e, "Get failed");
        return;
    }
    json["cmei"] = in["imei"];
    e = param.Set(json);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(out, e, "Set failed");
    }
}

void CameraKingProto::SpkStart(Json::Value &in, Json::Value &out) {
    char audioDir[EMX_MAX_PATH_SIZE] = {};
    snprintf(audioDir, sizeof(audioDir), "%s/audio", Misc::GetAppConfigsDir());
    DIR *dp = opendir(audioDir);
    if (!dp) {
        MakeCodeDesc(out, ErrCodeE::ResNotExist, "cannot open audio dir");
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
        MakeCodeDesc(out, ErrCodeE::ResNotExist, "cannot found music file");
        return;   
    }
    auto e = MediaClientAdecStreamSync::PlayFileWithVolume(0, path, 38);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(out, e, "cannot found music file");
    }
}

void CameraKingProto::ICRSet(Json::Value &in, Json::Value &out) {
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
    MakeCodeDesc(out, code, desc.c_str());
}

void CameraKingProto::SDCheck(Json::Value &in, Json::Value &out) {

}

void CameraKingProto::BaseGet(Json::Value &in, Json::Value &out) {
    ErrCodeE e;
    Sn sn;
    e = sn.Load();
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(out, e, "load sn failed");
        return;
    }
    out["sn"] = sn.Get();

    DevInfo devInfo;
    e = devInfo.Load();
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(out, e, "load devInfo failed");
        return;
    }
    out["dev_type"] = devInfo.GetOuter()->type;
    out["firmware"] = devInfo.GetOuter()->version;
    out["dev_id"] = devInfo.GetInner()->type;
    out["dev_ver"] = devInfo.GetInner()->version;
    out["hardware"] = devInfo.GetHardware();
}

void CameraKingProto::ResetStart(Json::Value &in, Json::Value &out) {
    bool presskey_status_down = false;
    Gpio gpio(in["num"].asInt(),
              static_cast<Gpio::DirE>(in["dir"].asInt()),
              static_cast<Gpio::ActiveE>(in["active"].asInt()),
              static_cast<Gpio::ValueE>(in["def"].asInt()));
    uint32_t timeout = in["timeout"].asInt() * 10;
    while (timeout--) {
        usleep(100000);
        if (gpio.GetValue() == Gpio::ValueE::Enable) {
            presskey_status_down = true;
            break;
        }
    }
    if (!presskey_status_down) {
        MakeCodeDesc(out, ErrCodeE::Failure, "no check presskey down");
    }
}

void CameraKingProto::PtzSetV1(Json::Value &in, Json::Value &out) {
    
}

void CameraKingProto::LedStart(Json::Value &in, Json::Value &out) {
    
}

void CameraKingProto::WifiQtyGet(Json::Value &in, Json::Value &out) {
    if (!NetMainDev::IsSupported(Net::DevE::Wifi)) {
        MakeCodeDesc(out, ErrCodeE::ResNotExist, "wifi unsupported");
        return;
    }
    NetClientWifi wifi;
    Wifi::Info info = { };
    auto e = wifi.GetInfo(info);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(in, e, "GetInfo failed");
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

void CameraKingProto::AgingGet(Json::Value &in, Json::Value &out) {
    
}