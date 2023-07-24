//
// Created by xiong on 2021/5/21.
//
#include "EmxCore.hpp"
#include "EmxProtoV2.hpp"
#include "EmxMedia.hpp"
#include "Sn.hpp"
#include "Mac.hpp"
#include "DevInfo.hpp"
#include "NetClient.hpp"
#include "SDCardClient.hpp"
#include "NightVisionClient.hpp"
#include "TimeZone.hpp"
#include "EmxBsp.hpp"
#include "EmxCrypto.hpp"

using namespace Emx;
#define EP_PROTO_BIND_FUNC(x) std::bind(&EmxProtoV2::x, this, ph_1, ph_2, ph_3);

EmxProtoV2::EmxProtoV2() {
    m_funcMap[CmdE::BaseGet] = EP_PROTO_BIND_FUNC(BaseGet);
    m_funcMap[CmdE::PlatBurningGet] = EP_PROTO_BIND_FUNC(PlatBurningGet);
    m_funcMap[CmdE::PlatBurningSet] = EP_PROTO_BIND_FUNC(PlatBurningSet);
    m_funcMap[CmdE::PlatCfgGet] = EP_PROTO_BIND_FUNC(PlatCfgGet);
    m_funcMap[CmdE::PlatCfgSet] = EP_PROTO_BIND_FUNC(PlatCfgSet);
    m_funcMap[CmdE::NetMainInfoGet] = EP_PROTO_BIND_FUNC(NetMainInfoGet);
    m_funcMap[CmdE::NetMainInfoSet] = EP_PROTO_BIND_FUNC(NetMainInfoSet);
    m_funcMap[CmdE::NetWifiGet] = EP_PROTO_BIND_FUNC(NetWifiGet);
    m_funcMap[CmdE::NetWifiSet] = EP_PROTO_BIND_FUNC(NetWifiSet);
    m_funcMap[CmdE::NetSupportTypeGet] = EP_PROTO_BIND_FUNC(NetSupportTypeGet);
    m_funcMap[CmdE::NetSpecTypeInfoGet] = EP_PROTO_BIND_FUNC(NetSpecTypeInfoGet);
    m_funcMap[CmdE::NetSpecTypeInfoSet] = EP_PROTO_BIND_FUNC(NetSpecTypeInfoSet);
    m_funcMap[CmdE::NetDnsGet] = EP_PROTO_BIND_FUNC(NetDnsGet);
    m_funcMap[CmdE::NetDnsSet] = EP_PROTO_BIND_FUNC(NetDnsSet);
    m_funcMap[CmdE::SnGet] = EP_PROTO_BIND_FUNC(SnGet);
    m_funcMap[CmdE::SnSet] = EP_PROTO_BIND_FUNC(SnSet);
    m_funcMap[CmdE::VideoEncQue] = EP_PROTO_BIND_FUNC(VideoEncQue);
    m_funcMap[CmdE::VideoEncGet] = EP_PROTO_BIND_FUNC(VideoEncGet);
    m_funcMap[CmdE::VideoEncSet] = EP_PROTO_BIND_FUNC(VideoEncSet);
    m_funcMap[CmdE::AudioEncQue] = EP_PROTO_BIND_FUNC(AudioEncQue);
    m_funcMap[CmdE::AudioEncGet] = EP_PROTO_BIND_FUNC(AudioEncGet);
    m_funcMap[CmdE::AudioEncSet] = EP_PROTO_BIND_FUNC(AudioEncSet);
    m_funcMap[CmdE::AudioDecQue] = EP_PROTO_BIND_FUNC(AudioDecQue);
    m_funcMap[CmdE::AudioDecGet] = EP_PROTO_BIND_FUNC(AudioDecGet);
    m_funcMap[CmdE::AudioDecSet] = EP_PROTO_BIND_FUNC(AudioDecSet);
    m_funcMap[CmdE::IspGet] = EP_PROTO_BIND_FUNC(IspGet);
    m_funcMap[CmdE::IspSet] = EP_PROTO_BIND_FUNC(IspSet);
    m_funcMap[CmdE::SnapGet] = EP_PROTO_BIND_FUNC(SnapGet);
    m_funcMap[CmdE::AudioPlay] = EP_PROTO_BIND_FUNC(AudioPlay);
    m_funcMap[CmdE::Reboot] = EP_PROTO_BIND_FUNC(Reboot);
    m_funcMap[CmdE::Reset] = EP_PROTO_BIND_FUNC(Reset);
    m_funcMap[CmdE::LogLevelGet] = EP_PROTO_BIND_FUNC(LogLevelGet);
    m_funcMap[CmdE::LogLevelSet] = EP_PROTO_BIND_FUNC(LogLevelSet);
    m_funcMap[CmdE::LogGet] = EP_PROTO_BIND_FUNC(LogGet);
    m_funcMap[CmdE::SdInfoGet] = EP_PROTO_BIND_FUNC(SdInfoGet);
    m_funcMap[CmdE::SdFormat] = EP_PROTO_BIND_FUNC(SdFormat);
    m_funcMap[CmdE::SdCheck] = EP_PROTO_BIND_FUNC(SdCheck);
    m_funcMap[CmdE::NightVisionGet] = EP_PROTO_BIND_FUNC(NightVisionGet);
    m_funcMap[CmdE::NightVisionSet] = EP_PROTO_BIND_FUNC(NightVisionSet);
    m_funcMap[CmdE::TimeGet] = EP_PROTO_BIND_FUNC(TimeGet);
    m_funcMap[CmdE::TimeSet] = EP_PROTO_BIND_FUNC(TimeSet);
    m_funcMap[CmdE::TimeZoneListGet] = EP_PROTO_BIND_FUNC(TimeZoneListGet);
    m_funcMap[CmdE::MacGet] = EP_PROTO_BIND_FUNC(MacGet);
    m_funcMap[CmdE::MacSet] = EP_PROTO_BIND_FUNC(MacSet);
    m_funcMap[CmdE::TemplateGet] = EP_PROTO_BIND_FUNC(TemplateGet);
    m_funcMap[CmdE::TemplateSet] = EP_PROTO_BIND_FUNC(TemplateSet);
    m_funcMap[CmdE::CenterGet] = EP_PROTO_BIND_FUNC(CenterGet);
    m_funcMap[CmdE::CenterSet] = EP_PROTO_BIND_FUNC(CenterSet);
    m_funcMap[CmdE::OsdGet] = EP_PROTO_BIND_FUNC(OsdGet);
    m_funcMap[CmdE::OsdSet] = EP_PROTO_BIND_FUNC(OsdSet);
    m_funcMap[CmdE::IrCutSwitch] = EP_PROTO_BIND_FUNC(IrCutSwitch);
    m_funcMap[CmdE::LumaGet] = EP_PROTO_BIND_FUNC(LumaGet);
    m_funcMap[CmdE::IrLightGet] = EP_PROTO_BIND_FUNC(IrLightGet);
    m_funcMap[CmdE::IrLightSet] = EP_PROTO_BIND_FUNC(IrLightSet);
    m_funcMap[CmdE::WhiteLightGet] = EP_PROTO_BIND_FUNC(WhiteLightGet);
    m_funcMap[CmdE::WhiteLightSet] = EP_PROTO_BIND_FUNC(WhiteLightSet);
    m_funcMap[CmdE::IndicatorLightGet] = EP_PROTO_BIND_FUNC(IndicatorLightGet);
    m_funcMap[CmdE::IndicatorLightSet] = EP_PROTO_BIND_FUNC(IndicatorLightSet);
    m_funcMap[CmdE::PressKeyGet] = EP_PROTO_BIND_FUNC(PressKeyGet);
    m_funcMap[CmdE::PressKeySet] = EP_PROTO_BIND_FUNC(PressKeySet);
    m_funcMap[CmdE::MacSet] = EP_PROTO_BIND_FUNC(MacSet);
    m_funcMap[CmdE::ExtendCmd] = EP_PROTO_BIND_FUNC(ExtendCmd);
    m_funcMap[CmdE::EventListGet] = EP_PROTO_BIND_FUNC(EventListGet);
    m_funcMap[CmdE::EventGlobalGet] = EP_PROTO_BIND_FUNC(EventGlobalGet);
    m_funcMap[CmdE::EventGlobalSet] = EP_PROTO_BIND_FUNC(EventGlobalSet);
    m_funcMap[CmdE::EventPluginGet] = EP_PROTO_BIND_FUNC(EventPluginGet);
    m_funcMap[CmdE::EventPluginSet] = EP_PROTO_BIND_FUNC(EventPluginSet);
    m_funcMap[CmdE::RecordStdGet] = EP_PROTO_BIND_FUNC(RecordStdGet);
    m_funcMap[CmdE::RecordStdSet] = EP_PROTO_BIND_FUNC(RecordStdSet);
    m_funcMap[CmdE::RecordStdRecordManualGet] = EP_PROTO_BIND_FUNC(RecordStdRecordManualGet);
    m_funcMap[CmdE::RecordStdRecordManualSet] = EP_PROTO_BIND_FUNC(RecordStdRecordManualSet);
    m_funcMap[CmdE::RecordStdRecordListGet] = EP_PROTO_BIND_FUNC(RecordStdRecordListGet);
    m_funcMap[CmdE::RecordStdSnapManualTrigger] = EP_PROTO_BIND_FUNC(RecordStdSnapManualTrigger);
    m_funcMap[CmdE::RecordStdSnapListGet] = EP_PROTO_BIND_FUNC(RecordStdSnapListGet);
}

void EmxProtoV2::Proc(const char *data, int32_t size, std::string &out) {
    Json::Value jsonIn;
    Json::Value jsonOut;
    auto e = EasyJson::Parse(data, size, jsonIn);
    if (e != ErrCodeE::Success) {
        emxloge("parse failed\n");
        jsonOut["cmd"] = -1;
        jsonOut["session"] = -1;
        MakeCodeDesc(jsonOut, e, "parse failed");
        EasyJson::Serialize(jsonOut, out);
        return;
    }
    auto cmd = (EmxProtoV2::CmdE) jsonIn["cmd"].asInt();
    if (m_funcMap.count(cmd) == 0) {
        emxloge("unsupported cmd %d\n", (int) cmd);
        jsonOut["cmd"] = jsonIn["cmd"];
        jsonOut["session"] = jsonIn["session"];
        MakeCodeDesc(jsonOut, ErrCodeE::IllegalParam, "unsupported cmd");
        EasyJson::Serialize(jsonOut, out);
        return;
    }
    jsonOut["cmd"] = jsonIn["cmd"];
    jsonOut["session"] = jsonIn["session"];
    //默认成功
    MakeCodeDesc(jsonOut, ErrCodeE::Success, "success");
    m_funcMap[cmd](jsonIn["data"], jsonOut["data"], jsonOut);
    EasyJson::Serialize(jsonOut, out);
}

void EmxProtoV2::BaseGet(Json::Value &in, Json::Value &out, Json::Value &header) {
    ErrCodeE e;
    Sn sn;
    e = sn.Load();
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(header, e, "load sn failed");
        return;
    }
    out["sn"] = sn.Get();

    Mac mac;
    e = mac.Load();
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(header, e, "load mac failed");
        return;
    }
    out["mac"] = mac.Get();

    DevInfo devInfo;
    e = devInfo.Load();
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(header, e, "load devInfo failed");
        return;
    }
    out["inner"]["type"] = devInfo.GetInner()->type;
    out["inner"]["version"] = devInfo.GetInner()->version;
    out["outer"]["type"] = devInfo.GetOuter()->type;
    out["outer"]["version"] = devInfo.GetOuter()->version;
    out["hardware"]["type"] = devInfo.GetHardware()->type;
    out["hardware"]["version"] = devInfo.GetHardware()->version;
}

void EmxProtoV2::PlatBurningGet(Json::Value &in, Json::Value &out, Json::Value &header) {
    Param burnInfo("burnInfo");
    Json::Value json,macJson,snJson;
    auto e = burnInfo.Get(json);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(header, e, "burnInfo get failed");
        return;
    }
    out["burnInfo"] = json;

    Param macInfo("mac");
    macInfo.Get(macJson);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(header, e, "mac get failed");
        return;
    }
    out["mac"] = macJson["mac"];

    Param snInfo("sn");
    snInfo.Get(snJson);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(header, e, "sn get failed");
        return;
    }
    out["sn"] = snJson["sn"];
}

void EmxProtoV2::PlatBurningSet(Json::Value &in, Json::Value &out, Json::Value &header) {
    ErrCodeE e = ErrCodeE::Success;
    if (in["burnInfo"]!= Json::nullValue){
        Param burnInfo("burnInfo");
        Json::Value inFoJson;
        e = burnInfo.Get(inFoJson);
        if (e != ErrCodeE::Success) {
            MakeCodeDesc(header, e, "get failed");
            return;
        }
        for (auto &key : in["burnInfo"].getMemberNames()) {
            inFoJson[key] = in["burnInfo"][key];
        }
        e = burnInfo.Set(inFoJson);
        if (e != ErrCodeE::Success) {
            MakeCodeDesc(header, e, "set failed");
            return;
        }
    }

    if (in["sn"]!= Json::nullValue){
        std::string snEncrypted;
        std::string ketPath(Misc::GetAppConfigsDir());
        ketPath+="/key/public.key";
        Base64::Decode(in["sn"].asString(), snEncrypted);
        std::string snPlain;
        e = EmxCrypto::RsaPubKeyDecrypt(ketPath.c_str(), snEncrypted, snPlain);
        if (e != ErrCodeE::Success) {
            MakeCodeDesc(header, e, "decrypt sn failed");
            return;
        }

        Sn sn;
        e = sn.Set(snPlain.c_str());
        if (e != ErrCodeE::Success) {
            MakeCodeDesc(header, e, "Set sn failed");
            return;
        }
    }

    if (in["mac"]!= Json::nullValue){
        Mac mac;
        e = mac.Set(in["mac"].asCString());
        if (e != ErrCodeE::Success) {
            MakeCodeDesc(header, e, "set mac failed");
            return;
        }
    }
}

void EmxProtoV2::PlatCfgGet(Json::Value &in, Json::Value &out, Json::Value &header) {
    Param platCfg("platCfg");
    Json::Value json;
    auto e = platCfg.Get(json);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(header, e, "get failed");
        return;
    }
    out = json;
}

void EmxProtoV2::PlatCfgSet(Json::Value &in, Json::Value &out, Json::Value &header) {
    ErrCodeE e;
    Param platCfg("platCfg");
    Json::Value json;
    e = platCfg.Get(json);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(header, e, "get failed");
        return;
    }
    for (auto &key : in.getMemberNames()) {
        json[key] = in[key];
    }
    e = platCfg.Set(json);
    if (e != Emx::ErrCodeE::Success) {
        MakeCodeDesc(header, e, "set failed");
    }
}

void EmxProtoV2::NetMainInfoGet(Json::Value &in, Json::Value &out, Json::Value &header) {
    ErrCodeE e;
    Net::DevE dev;
    e = NetMainDev::Get(dev);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(header, e, "get main dev failed");
        return;
    }
    NetClient net(dev);
    out["type"] = Net::GetDevName(dev);

    bool ena;
    e = net.GetEna(ena);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(header, e, "GetEna failed");
        return;
    }
    out["ena"] = ena;

    out["interface"] = net.GetInterface();

    Net::Addr addr = {};
    e = net.GetAddr(addr);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(header, e, "GetAddr failed");
        return;
    }
    out["dhcp"] = addr.dhcp;
    out["ip4"] = addr.ip4;
    out["netmask4"] = addr.netmask4;
    out["gateway4"] = addr.gateway4;
    out["ip6"] = addr.ip6;
    out["netmask6"] = addr.netmask6;
    out["gateway6"] = addr.gateway6;
    out["mac"] = addr.mac;
}

void EmxProtoV2::NetMainInfoSet(Json::Value &in, Json::Value &out, Json::Value &header) {
    Net::Addr addr = {};
    addr.dhcp = in["dhcp"].asBool();
    strncpy(addr.ip4, in["ip4"].asCString(), sizeof(addr.ip4));
    strncpy(addr.netmask4, in["netmask4"].asCString(), sizeof(addr.netmask4));
    strncpy(addr.gateway4, in["gateway4"].asCString(), sizeof(addr.gateway4));
    strncpy(addr.ip6, in["ip6"].asCString(), sizeof(addr.ip6));
    strncpy(addr.netmask6, in["netmask6"].asCString(), sizeof(addr.netmask6));
    strncpy(addr.gateway6, in["gateway6"].asCString(), sizeof(addr.gateway6));
    strncpy(addr.mac, in["mac"].asCString(), sizeof(addr.mac));

    auto ena = in["ena"].asBool();

    ErrCodeE e;
    Net::DevE dev;
    e = NetMainDev::Get(dev);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(header, e, "get main dev failed");
        return;
    }
    NetClient net(dev);
    e = net.SetAddr(addr);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(header, e, "SetAddr failed");
        return;
    }
    e = net.SetEna(ena);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(header, e, "SetEna failed");
        return;
    }
}

void EmxProtoV2::NetWifiGet(Json::Value &in, Json::Value &out, Json::Value &header) {
    if (!NetMainDev::IsSupported(Net::DevE::Wifi)) {
        MakeCodeDesc(header, ErrCodeE::ResNotExist, "wifi unsupported");
        return;
    }
    NetClientWifi wifi;
    Wifi::Info info = {};
    auto e = wifi.GetInfo(info);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(header, e, "GetInfo failed");
    }

    //mode
    if (info.mode == Wifi::ModeE::None)
        out["mode"] = "NONE";
    else if (info.mode == Wifi::ModeE::AP)
        out["mode"] = "AP";
    else if (info.mode == Wifi::ModeE::STA)
        out["mode"] = "STA";
    else {
        MakeCodeDesc(header, e, "mode unsupported");
        return;
    }

    //mgmt
    if (info.mgmt == Wifi::KeyMgmtE::NONE)
        out["mgmt"] = "NONE";
    else if (info.mgmt == Wifi::KeyMgmtE::WEP)
        out["mgmt"] = "WEP";
    else if (info.mgmt == Wifi::KeyMgmtE::WPA_PSK)
        out["mgmt"] = "WPA_PSK";
    else if (info.mgmt == Wifi::KeyMgmtE::WPA_WPA2_PSK)
        out["mgmt"] = "WPA_WPA2_PSK";
    else if (info.mgmt == Wifi::KeyMgmtE::WPA2_PSK)
        out["mgmt"] = "WPA2_PSK";
    else
        out["mgmt"] = "OTHER";

    out["ssid"] = info.ssid;
    out["bssid"] = info.bssid;
    out["signalLevel"] = info.signalLevel;
    out["linkSpeed"] = info.linkSpeed;
}

void EmxProtoV2::NetWifiSet(Json::Value &in, Json::Value &out, Json::Value &header) {
    if (!NetMainDev::IsSupported(Net::DevE::Wifi)) {
        MakeCodeDesc(header, ErrCodeE::ResNotExist, "wifi unsupported");
        return;
    }
    NetClientWifi wifi;
    ErrCodeE e;
    if (in["mode"].asString() == "NONE") {
        e = wifi.SwitchToMode(Wifi::ModeE::None);
        if (e != ErrCodeE::Success)
            MakeCodeDesc(header, e, "SwitchToMode None failed");
    } else if (in["mode"].asString() == "AP") {
        e = wifi.SetAP(in["ssid"].asCString(), in["psk"].asCString());
        if (e != ErrCodeE::Success)
            MakeCodeDesc(header, e, "SetAP failed");
    } else if (in["mode"].asString() == "STA") {
        e = wifi.SetSTA(in["ssid"].asCString(), in["psk"].asCString());
        if (e != ErrCodeE::Success)
            MakeCodeDesc(header, e, "SetSTA failed");
    } else {
        MakeCodeDesc(header, ErrCodeE::ResNotExist, "mode unsupported");
    }
}

void EmxProtoV2::NetSupportTypeGet(Json::Value &in, Json::Value &out, Json::Value &header) {
    out["typeArray"].resize(0);
    if (NetMainDev::IsSupported(Net::DevE::Wifi))
        out["typeArray"].append(Net::GetDevName(Net::DevE::Wifi));
    if (NetMainDev::IsSupported(Net::DevE::Eth))
        out["typeArray"].append(Net::GetDevName(Net::DevE::Eth));
    if (NetMainDev::IsSupported(Net::DevE::Mobile))
        out["typeArray"].append(Net::GetDevName(Net::DevE::Mobile));
}

void EmxProtoV2::NetSpecTypeInfoGet(Json::Value &in, Json::Value &out, Json::Value &header) {
    Net::DevE dev;
    if (in["type"].asString() == Net::GetDevName(Net::DevE::Wifi))
        dev = Net::DevE::Wifi;
    else if (in["type"].asString() == Net::GetDevName(Net::DevE::Eth))
        dev = Net::DevE::Eth;
    else if (in["type"].asString() == Net::GetDevName(Net::DevE::Mobile))
        dev = Net::DevE::Mobile;
    else {
        out["supported"] = false;
        return;
    }

    ErrCodeE e;
    NetClient net(dev);
    out["type"] = Net::GetDevName(dev);
    bool ena;
    e = net.GetEna(ena);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(header, e, "GetEna failed");
        return;
    }
    out["ena"] = ena;
    out["interface"] = net.GetInterface();

    Net::Addr addr = {};
    e = net.GetAddr(addr);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(header, e, "GetAddr failed");
        return;
    }
    out["dhcp"] = addr.dhcp;
    out["ip4"] = addr.ip4;
    out["netmask4"] = addr.netmask4;
    out["gateway4"] = addr.gateway4;
    out["ip6"] = addr.ip6;
    out["netmask6"] = addr.netmask6;
    out["gateway6"] = addr.gateway6;
    out["mac"] = addr.mac;
}

void EmxProtoV2::NetSpecTypeInfoSet(Json::Value &in, Json::Value &out, Json::Value &header) {
    Net::DevE dev;
    if (in["type"].asString() == Net::GetDevName(Net::DevE::Wifi))
        dev = Net::DevE::Wifi;
    else if (in["type"].asString() == Net::GetDevName(Net::DevE::Eth))
        dev = Net::DevE::Eth;
    else if (in["type"].asString() == Net::GetDevName(Net::DevE::Mobile))
        dev = Net::DevE::Mobile;
    else {
        MakeCodeDesc(header, ErrCodeE::ResNotExist, "type unsupported");
        return;
    }

    Net::Addr addr = {};
    addr.dhcp = in["dhcp"].asBool();
    strncpy(addr.ip4, in["ip4"].asCString(), sizeof(addr.ip4));
    strncpy(addr.netmask4, in["netmask4"].asCString(), sizeof(addr.netmask4));
    strncpy(addr.gateway4, in["gateway4"].asCString(), sizeof(addr.gateway4));
    strncpy(addr.ip6, in["ip6"].asCString(), sizeof(addr.ip6));
    strncpy(addr.netmask6, in["netmask6"].asCString(), sizeof(addr.netmask6));
    strncpy(addr.gateway6, in["gateway6"].asCString(), sizeof(addr.gateway6));
    strncpy(addr.mac, in["mac"].asCString(), sizeof(addr.mac));

    NetClient net(dev);
    auto e = net.SetAddr(addr);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(header, e, "SetAddr failed");
        return;
    }

    auto ena = in["ena"].asBool();
    e = net.SetEna(ena);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(header, e, "SetEna failed");
        return;
    }
}

void EmxProtoV2::NetDnsGet(Json::Value &in, Json::Value &out, Json::Value &header) {
    NetClientDns dns;
    NetDns::Param param = {};
    auto e = dns.GetParam(param);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(header, e, "GetParam failed");
        return;
    }
    out["manual"] = param.manual;
    out["server"].resize(0);
    for (auto &item:param.server) {
        out["server"].append(item.ip);
    }
    out["append"].resize(0);
    for (auto &item:param.append) {
        out["append"].append(item.ip);
    }
}

void EmxProtoV2::NetDnsSet(Json::Value &in, Json::Value &out, Json::Value &header) {
    NetClientDns dns;
    NetDns::Param param = {};
    auto e = dns.GetParam(param);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(header, e, "GetParam failed");
        return;
    }
    param.manual = in["manual"].asBool();
    param.server.clear();
    for (auto &item:in["server"]) {
        NetDns::Param::Addr addr = {};
        strncpy(addr.ip, item.asCString(), sizeof(addr.ip));
        param.server.push_back(addr);
    }
    param.append.clear();
    for (auto &item:in["append"]) {
        NetDns::Param::Addr addr = {};
        strncpy(addr.ip, item.asCString(), sizeof(addr.ip));
        param.append.push_back(addr);
    }
    e = dns.SetParam(param);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(header, e, "SetParam failed");
        return;
    }
}

void EmxProtoV2::SnGet(Json::Value &in, Json::Value &out, Json::Value &header) {
    ErrCodeE e;
    Sn sn;
    e = sn.Load();
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(header, e, "load sn failed");
        return;
    }
    out["sn"] = sn.Get();
}

void EmxProtoV2::SnSet(Json::Value &in, Json::Value &out, Json::Value &header) {
    ErrCodeE e;
    std::string snEncrypted;
    Base64::Decode(in["sn"].asString(), snEncrypted);
    std::string snPlain;
    e = EmxCrypto::RsaPubKeyDecrypt("", snEncrypted, snPlain);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(header, e, "decrypt sn failed");
        return;
    }
    Sn sn;
    e = sn.Set(snPlain.c_str());
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(header, e, "Set sn failed");
        return;
    }
}

void EmxProtoV2::VideoEncQue(Json::Value &in, Json::Value &out, Json::Value &header) {
    int num;
    auto e = MediaClientVenc::GetChnNum(num);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(header, e, "GetChnNum failed");
        return;
    }
    out["range"].resize(0);
    for (int i = 0; i < num; i++) {
        MediaClientVenc venc(i);
        Json::Value range;
        e = venc.GetParamRange(range);
        if (e != ErrCodeE::Success) {
            MakeCodeDesc(header, e, "GetParamRange failed");
            return;
        }
        out["range"].append(range);
    }
}

void EmxProtoV2::VideoEncGet(Json::Value &in, Json::Value &out, Json::Value &header) {
    int num;
    auto e = MediaClientVenc::GetChnNum(num);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(header, e, "GetChnNum failed");
        return;
    }
    out["venc"].resize(0);
    for (int i = 0; i < num; i++) {
        MediaClientVenc venc(i);
        MediaVenc::Param param = {};
        e = venc.GetParam(param);
        if (e != ErrCodeE::Success) {
            MakeCodeDesc(header, e, "GetParam failed");
            return;
        }
        Json::Value json;
        json["codec"] = (int) param.codec;
        json["width"] = param.width;
        json["height"] = param.height;
        json["fps"] = param.fps;
        json["bitRate"] = param.bitRate;
        json["rcMode"] = (int) param.rcMode;
        json["iFrameInv"] = param.iFrameInv;
        json["drawRect"] = param.drawRect;
        out["venc"].append(json);
    }
}

void EmxProtoV2::VideoEncSet(Json::Value &in, Json::Value &out, Json::Value &header) {
    int chn = in["chn"].asInt();
    MediaVenc::Param param = {};
    param.codec = (VideoCodecE) in["codec"].asInt();
    param.width = in["width"].asInt();
    param.height = in["height"].asInt();
    param.fps = in["fps"].asInt();
    param.bitRate = in["bitRate"].asInt();
    param.rcMode = (MediaVenc::RcModeE) in["rcMode"].asInt();
    param.iFrameInv = in["iFrameInv"].asInt();
    param.drawRect = in["drawRect"].asBool();
    MediaClientVenc venc(chn);
    auto e = venc.SetParam(param);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(header, e, "SetParam failed");
        return;
    }
}

void EmxProtoV2::AudioEncQue(Json::Value &in, Json::Value &out, Json::Value &header) {
    int num;
    auto e = MediaClientAenc::GetChnNum(num);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(header, e, "GetChnNum failed");
        return;
    }
    out["range"].resize(0);
    for (int i = 0; i < num; i++) {
        MediaClientAenc aenc(i);
        Json::Value range;
        e = aenc.GetParamRange(range);
        if (e != ErrCodeE::Success) {
            MakeCodeDesc(header, e, "GetParamRange failed");
            return;
        }
        out["range"].append(range);
    }
}

void EmxProtoV2::AudioEncGet(Json::Value &in, Json::Value &out, Json::Value &header) {
    int num;
    auto e = MediaClientAenc::GetChnNum(num);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(header, e, "GetChnNum failed");
        return;
    }
    out["aenc"].resize(0);
    for (int i = 0; i < num; i++) {
        MediaClientAenc aenc(i);
        MediaAenc::Param param = {};
        e = aenc.GetParam(param);
        if (e != ErrCodeE::Success) {
            MakeCodeDesc(header, e, "GetParam failed");
            return;
        }
        Json::Value json;
        json["codec"] = (int) param.codec;
        json["bitRate"] = param.bitRate;
        json["sampleRate"] = param.sampleRate;
        json["volume"] = param.volume;
        json["bitWidth"] = param.bitWidth;
        json["mute"] = param.mute;
        out["aenc"].append(json);
    }
}

void EmxProtoV2::AudioEncSet(Json::Value &in, Json::Value &out, Json::Value &header) {
    int chn = in["chn"].asInt();
    MediaAenc::Param param = {};
    param.codec = (AudioCodecE) in["codec"].asInt();
    param.bitRate = in["bitRate"].asInt();
    param.sampleRate = in["sampleRate"].asInt();
    param.volume = in["volume"].asInt();
    param.bitWidth = in["bitWidth"].asInt();
    param.mute = in["mute"].asBool();
    MediaClientAenc aenc(chn);
    auto e = aenc.SetParam(param);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(header, e, "SetParam failed");
        return;
    }
}

void EmxProtoV2::AudioDecQue(Json::Value &in, Json::Value &out, Json::Value &header) {
    int num;
    auto e = MediaClientAdec::GetChnNum(num);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(header, e, "GetChnNum failed");
        return;
    }
    out["range"].resize(0);
    for (int i = 0; i < num; i++) {
        MediaClientAdec adec(i);
        Json::Value range;
        e = adec.GetParamRange(range);
        if (e != ErrCodeE::Success) {
            MakeCodeDesc(header, e, "GetParamRange failed");
            return;
        }
        out["range"].append(range);
    }
}

void EmxProtoV2::AudioDecGet(Json::Value &in, Json::Value &out, Json::Value &header) {
    int num;
    auto e = MediaClientAdec::GetChnNum(num);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(header, e, "GetChnNum failed");
        return;
    }
    out["adec"].resize(0);
    for (int i = 0; i < num; i++) {
        MediaClientAdec adec(i);
        MediaAdec::Param param = {};
        e = adec.GetParam(param);
        if (e != ErrCodeE::Success) {
            MakeCodeDesc(header, e, "GetParam failed");
            return;
        }
        Json::Value json;
        json["codec"] = (int) param.codec;
        json["bitRate"] = param.bitRate;
        json["sampleRate"] = param.sampleRate;
        json["volume"] = param.volume;
        json["bitWidth"] = param.bitWidth;
        out["adec"].append(json);
    }
}

void EmxProtoV2::AudioDecSet(Json::Value &in, Json::Value &out, Json::Value &header) {
    int chn = in["chn"].asInt();
    MediaAdec::Param param = {};
    param.codec = (AudioCodecE) in["codec"].asInt();
    param.bitRate = in["bitRate"].asInt();
    param.sampleRate = in["sampleRate"].asInt();
    param.volume = in["volume"].asInt();
    param.bitWidth = in["bitWidth"].asInt();
    MediaClientAdec adec(chn);
    auto e = adec.SetParam(param);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(header, e, "SetParam failed");
        return;
    }
}

void EmxProtoV2::IspGet(Json::Value &in, Json::Value &out, Json::Value &header) {
    int num;
    auto e = MediaClientISP::GetChnNum(num);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(header, e, "GetChnNum failed");
        return;
    }
    out["isp"].resize(0);
    for (int i = 0; i < num; i++) {
        MediaClientISP isp(i);
        MediaISP::Param param = {};
        e = isp.GetParam(param);
        if (e != ErrCodeE::Success) {
            MakeCodeDesc(header, e, "GetParam failed");
            return;
        }
        Json::Value json;
        json["expMode"] = (int) param.expMode;
        json["autoMaxExpGain"] = (int) param.autoMaxExpGain;
        json["compensation"] = param.compensation;
        json["manualExpGain"] = (int) param.manualExpGain;
        json["expTime"] = param.expTime;
        json["brightness"] = param.brightness;
        json["sharpness"] = param.sharpness;
        json["hue"] = param.hue;
        json["contrast"] = param.contrast;
        json["saturation"] = param.saturation;
        json["deNoise"] = param.deNoise;
        json["wbMode"] = (int) param.wbMode;
        json["redGain"] = param.redGain;
        json["blueGain"] = param.blueGain;
        json["antiFlickerHz"] = param.antiFlickerHz;
        json["forceAntiFlickerEna"] = param.forceAntiFlickerEna;
        out["isp"].append(json);
    }
}

void EmxProtoV2::IspSet(Json::Value &in, Json::Value &out, Json::Value &header) {
    int chn = in["chn"].asInt();
    MediaISP::Param param = {};
    param.expMode = (MediaISP::OptModeE) in["expMode"].asInt();
    param.autoMaxExpGain = (MediaISP::ExpGainE) in["autoMaxExpGain"].asInt();
    param.compensation = in["compensation"].asInt();
    param.manualExpGain = (MediaISP::ExpGainE) in["manualExpGain"].asInt();
    param.expTime = in["expTime"].asInt();
    param.brightness = in["brightness"].asInt();
    param.sharpness = in["sharpness"].asInt();
    param.hue = in["hue"].asInt();
    param.contrast = in["contrast"].asInt();
    param.saturation = in["saturation"].asInt();
    param.deNoise = in["deNoise"].asInt();
    param.wbMode = (MediaISP::OptModeE) in["wbMode"].asInt();
    param.redGain = in["redGain"].asInt();
    param.blueGain = in["blueGain"].asInt();
    param.antiFlickerHz = in["antiFlickerHz"].asInt();
    param.forceAntiFlickerEna = in["forceAntiFlickerEna"].asBool();
    MediaClientISP isp(chn);
    auto e = isp.SetParam(param);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(header, e, "SetParam failed");
        return;
    }
}

void EmxProtoV2::SnapGet(Json::Value &in, Json::Value &out, Json::Value &header) {
    int chn = in["chn"].asInt();
    MediaClientStream::Channel channel;
    channel.Add(MediaFrame::TypeE::Snap, chn);
    MediaClientStreamSync snap;
    ErrCodeE e = snap.Open(channel, 3000);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(header, e, "Open failed");
        return;
    }
    MediaClientSnap::Trigger(chn, 3000);
    MediaFrame frame = {};
    e = snap.GetFrame(frame, 3000);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(in, e, "GetFrame failed");
        snap.Close();
        return;
    }
    int base64Size = Base64::GetEncodeSize(frame.size);
    auto base64Snap = (char *) malloc(base64Size);
    Base64::Encode(frame.data, frame.size, base64Snap);
    out["snap"] = base64Snap;
    free(base64Snap);
    snap.Close();
}

void EmxProtoV2::AudioPlay(Json::Value &in, Json::Value &out, Json::Value &header) {
    if (in["play"].asString() == "start") {
        if (in.isMember("path")) {
            MediaClientAdecStreamSync::PlayFile(0, in["path"].asCString());
        } else {
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
            } else {
                MakeCodeDesc(header, ErrCodeE::ResNotExist, "cannot found music file");
            }
        }
    } else if (in["play"].asString() == "stop") {
        MediaClientAdecStreamSync::PlayCtrlQuit(0);
    } else {
        MakeCodeDesc(in, ErrCodeE::IllegalParam, "play cmd unsupported");
    }
}

void EmxProtoV2::Reboot(Json::Value &in, Json::Value &out, Json::Value &header) {
    Reboot::DoReboot(3);
}

void EmxProtoV2::Reset(Json::Value &in, Json::Value &out, Json::Value &header) {
    auto e = Param::ResetAllToFactory();
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(header, e, "ResetAllToFactory failed");
    }
    Reboot::DoReboot(3);
}

void EmxProtoV2::LogLevelGet(Json::Value &in, Json::Value &out, Json::Value &header) {
    out["level"] = (int) emxLog.GetLevel();
}

void EmxProtoV2::LogLevelSet(Json::Value &in, Json::Value &out, Json::Value &header) {
    emxLog.SetLevel((Log::LevelE) out["level"].asInt());
}

void EmxProtoV2::LogGet(Json::Value &in, Json::Value &out, Json::Value &header) {

}

void EmxProtoV2::SdInfoGet(Json::Value &in, Json::Value &out, Json::Value &header) {
    ErrCodeE e;
    out["sdInfo"].resize(0);
    int num = SDCardClient::GetChnNum();
    for (int i = 0; i < num; i++) {
        SDCardClient sd(i);
        SDCard::Info info = {};
        e = sd.GetInfo(info);
        if (e != ErrCodeE::Success) {
            MakeCodeDesc(header, e, "GetInfo failed");
            return;
        }
        Json::Value json;
        if (info.stat == SDCard::StatE::Rejected)
            json["stat"] = "Rejected";
        else if (info.stat == SDCard::StatE::Inserted)
            json["stat"] = "Inserted";
        else if (info.stat == SDCard::StatE::Normal)
            json["stat"] = "Normal";
        else if (info.stat == SDCard::StatE::Formatting)
            json["stat"] = "Formatting";
        else {
            MakeCodeDesc(header, ErrCodeE::Failure, "unknown stat");
            return;
        }

        json["total"] = info.total;
        json["free"] = info.free;
        json["used"] = info.used;
        json["mountPoint"] = info.mountPoint;
        out["sdInfo"].append(json);
    }
}

void EmxProtoV2::SdFormat(Json::Value &in, Json::Value &out, Json::Value &header) {
    SDCardClient sd(in["chn"].asInt());
    SDCard::Info info = {};
    ErrCodeE e;
    e = sd.GetInfo(info);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(header, e, "GetInfo failed");
        return;
    }
    e = sd.Format();
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(header, e, "Format failed");
    }
}

void EmxProtoV2::SdCheck(Json::Value &in, Json::Value &out, Json::Value &header) {

}

void EmxProtoV2::NightVisionGet(Json::Value &in, Json::Value &out, Json::Value &header) {
    NightVisionClient nv;
    ErrCodeE e;
    NightVision::Param param = {};
    e = nv.GetParam(param);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(header, e, "GetParam failed");
        return;
    }
    NightVision::Status status = {};
    e = nv.GetStatus(status);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(header, e, "GetStatus failed");
        return;
    }
    out["manual"] = param.manual;

    if (param.autoMode == NightVision::AutoModeE::IrNightVision)
        out["autoMode"] = "IrNightVision";
    else if (param.autoMode == NightVision::AutoModeE::ColorNightVision)
        out["autoMode"] = "ColorNightVision";
    else {
        MakeCodeDesc(header, ErrCodeE::Failure, "autoMode unsupported");
        return;
    }

    if (param.manualMode == NightVision::ModeE::Day)
        out["manualMode"] = "Day";
    else if (param.manualMode == NightVision::ModeE::IrNightVision)
        out["manualMode"] = "IrNightVision";
    else if (param.manualMode == NightVision::ModeE::ColorNightVision)
        out["manualMode"] = "ColorNightVision";
    else {
        MakeCodeDesc(header, ErrCodeE::Failure, "manualMode unsupported");
        return;
    }

    auto &sensitivity = out["sensitivity"];
    sensitivity["day2ir"] = param.sensitivity.day2ir;
    sensitivity["ir2day"] = param.sensitivity.ir2day;
    sensitivity["day2color"] = param.sensitivity.day2color;
    sensitivity["color2day"] = param.sensitivity.color2day;


    if (status.mode == NightVision::ModeE::Day)
        out["stat"] = "Day";
    else if (status.mode == NightVision::ModeE::IrNightVision)
        out["stat"] = "IrNightVision";
    else if (status.mode == NightVision::ModeE::ColorNightVision)
        out["stat"] = "ColorNightVision";
    else {
        MakeCodeDesc(header, ErrCodeE::Failure, "status mode unsupported");
        return;
    }


}

void EmxProtoV2::NightVisionSet(Json::Value &in, Json::Value &out, Json::Value &header) {
    NightVisionClient nv;
    NightVision::Param param = {};
    param.manual = in["manual"].asBool();

    if (in["autoMode"].asString() == "IrNightVision")
        param.autoMode = NightVision::AutoModeE::IrNightVision;
    else if (in["autoMode"].asString() == "ColorNightVision")
        param.autoMode = NightVision::AutoModeE::ColorNightVision;
    else {
        MakeCodeDesc(header, ErrCodeE::Failure, "status mode unsupported");
        return;
    }

    if (out["manualMode"].asString() == "Day")
        param.manualMode = NightVision::ModeE::Day;
    else if (out["manualMode"].asString() == "IrNightVision")
        param.manualMode = NightVision::ModeE::IrNightVision;
    else if (out["manualMode"].asString() == "ColorNightVision")
        param.manualMode = NightVision::ModeE::ColorNightVision;
    else {
        MakeCodeDesc(header, ErrCodeE::Failure, "manualMode unsupported");
        return;
    }

    auto &sensitivity = out["sensitivity"];
    param.sensitivity.day2ir = sensitivity["day2ir"].asInt();
    param.sensitivity.ir2day = sensitivity["ir2day"].asInt();
    param.sensitivity.day2color = sensitivity["day2color"].asInt();
    param.sensitivity.color2day = sensitivity["color2day"].asInt();

    auto e = nv.SetParam(param);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(header, e, "SetParam failed");
        return;
    }
}

void EmxProtoV2::TimeGet(Json::Value &in, Json::Value &out, Json::Value &header) {
    out["utc"] = Time::GetS();
    TimeZone tz;
    auto e = tz.Load();
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(header, e, "load zone failed");
    }
    out["zone"] = tz.Get();
}

void EmxProtoV2::TimeSet(Json::Value &in, Json::Value &out, Json::Value &header) {
    if (in.isMember("utc")) {
        Time::SetSysTime(in["utc"].asInt64());
    }
    if (in.isMember("zone")) {
        ErrCodeE e;
        TimeZone tz;
        e = tz.Load();
        if (e != ErrCodeE::Success) {
            MakeCodeDesc(header, e, "load zone failed");
        }
        e = tz.Set(in["zone"].asCString());
        if (e != ErrCodeE::Success) {
            MakeCodeDesc(header, e, "set zone failed");
        }
    }
}

void EmxProtoV2::TimeZoneListGet(Json::Value &in, Json::Value &out, Json::Value &header) {
    TimeZone tz;
    std::vector<std::string> list;
    tz.GetZoneList(list);
    out["list"].resize(0);
    for (auto &z :list)
        out["list"].append(z);
}

void EmxProtoV2::MacGet(Json::Value &in, Json::Value &out, Json::Value &header) {
    Mac mac;
    auto e = mac.Load();
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(header, e, "load mac failed");
        return;
    }
    out["mac"] = mac.Get();
}

void EmxProtoV2::MacSet(Json::Value &in, Json::Value &out, Json::Value &header) {
    Mac mac;
    auto e = mac.Load();
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(header, e, "load mac failed");
        return;
    }
    e = mac.Set(out["mac"].asCString());
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(header, e, "set mac failed");
    }
}

void EmxProtoV2::TemplateGet(Json::Value &in, Json::Value &out, Json::Value &header) {
    Param param("template");
    Json::Value json;
    auto e = param.Get(json);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(header, e, "Get failed");
        return;
    }
    out["template"] = json["template"];
}

void EmxProtoV2::TemplateSet(Json::Value &in, Json::Value &out, Json::Value &header) {
    Param param("template");
    Json::Value json;
    json["template"] = in["template"];
    auto e = param.Set(json);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(header, e, "Set failed");
    }
}

void EmxProtoV2::CenterGet(Json::Value &in, Json::Value &out, Json::Value &header) {
    Param param("center");
    Json::Value json;
    auto e = param.Get(json);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(header, e, "Get failed");
        return;
    }
    out["center"] = json["center"];
}

void EmxProtoV2::CenterSet(Json::Value &in, Json::Value &out, Json::Value &header) {
    Param param("center");
    Json::Value json;
    json["center"] = in["center"];
    auto e = param.Set(json);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(header, e, "Set failed");
    }
}

void EmxProtoV2::OsdGet(Json::Value &in, Json::Value &out, Json::Value &header) {
    ErrCodeE e;
    int chnNum;
    e = MediaClientOSD::GetChnNum(chnNum);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(header, e, "GetChnNum failed");
        return;
    }
    out["osd"].resize(0);
    for (int i = 0; i < chnNum; i++) {
        MediaClientOSD osd(i);
        std::vector<std::shared_ptr<MediaOSD::Param>> param;
        e = osd.GetParam(param);
        if (e != ErrCodeE::Success) {
            MakeCodeDesc(header, e, "GetParam failed");
            return;
        }
        Json::Value chn;
        chn["time"].resize(0);
        chn["text"].resize(0);
        chn["image"].resize(0);
        for (auto &item : param) {
            if (item->type == MediaOSD::TypeE::Time) {
                Json::Value json;
                json["ena"] = item->time.ena;
                json["size"] = item->time.size;
                json["thickness"] = item->time.thickness;
                json["layer"] = item->time.layer;
                json["font"] = item->time.font;
                json["format"] = (int) item->time.format;
                json["margin"]["horizon"]["rate"] = item->time.margin.rateX;
                json["margin"]["horizon"]["align"] = (int) item->time.margin.horizon;
                json["margin"]["vertical"]["rate"] = item->time.margin.rateY;
                json["margin"]["vertical"]["align"] = (int) item->time.margin.vertical;
                chn["time"].append(json);
            } else if (item->type == MediaOSD::TypeE::Text) {
                Json::Value json;
                json["ena"] = item->text.ena;
                json["size"] = item->text.size;
                json["thickness"] = item->text.thickness;
                json["layer"] = item->text.layer;
                json["font"] = item->text.font;
                json["content"] = item->text.content;
                json["margin"]["horizon"]["rate"] = item->text.margin.rateX;
                json["margin"]["horizon"]["align"] = (int) item->text.margin.horizon;
                json["margin"]["vertical"]["rate"] = item->text.margin.rateY;
                json["margin"]["vertical"]["align"] = (int) item->text.margin.vertical;
                chn["text"].append(json);
            } else if (item->type == MediaOSD::TypeE::Image) {
                Json::Value json;
                json["ena"] = item->image.ena;
                json["layer"] = item->image.layer;
                json["width"] = item->image.width;
                json["height"] = item->image.height;
                json["alpha"] = item->image.alpha;
                json["path"] = item->image.path;
                json["margin"]["horizon"]["rate"] = item->image.margin.rateX;
                json["margin"]["horizon"]["align"] = (int) item->image.margin.horizon;
                json["margin"]["vertical"]["rate"] = item->image.margin.rateY;
                json["margin"]["vertical"]["align"] = (int) item->image.margin.vertical;
                chn["image"].append(json);
            }
        }
        out["osd"].append(chn);
    }
}

void EmxProtoV2::OsdSet(Json::Value &in, Json::Value &out, Json::Value &header) {
    ErrCodeE e;
    int chnNum = (int) in["osd"].size();
    for (int i = 0; i < chnNum; i++) {
        auto &chn = in["osd"][i];
        MediaClientOSD osd(i);
        std::vector<std::shared_ptr<MediaOSD::Param>> param;
        for (auto &json : chn["time"]) {
            auto item = std::make_shared<MediaOSD::Param>();
            item->type = MediaOSD::TypeE::Time;
            item->time.ena = json["ena"].asBool();
            item->time.size = json["size"].asInt();
            item->time.thickness = json["thickness"].asInt();
            item->time.layer = json["layer"].asInt();
            strncpy(item->time.font, json["font"].asCString(), sizeof(item->time.font));
            item->time.format = (MediaOSD::TimeFormatE) json["format"].asInt();
            item->time.margin.rateX = json["margin"]["horizon"]["rate"].asInt();
            item->time.margin.horizon = (MediaOSD::HAlignE) json["margin"]["horizon"]["align"].asInt();
            item->time.margin.rateY = json["margin"]["vertical"]["rate"].asInt();
            item->time.margin.vertical = (MediaOSD::VAlignE) json["margin"]["vertical"]["align"].asInt();
            param.emplace_back(item);
        }
        for (auto &json : chn["text"]) {
            auto item = std::make_shared<MediaOSD::Param>();
            item->type = MediaOSD::TypeE::Text;
            item->text.ena = json["ena"].asBool();
            item->text.size = json["size"].asInt();
            item->text.thickness = json["thickness"].asInt();
            item->text.layer = json["layer"].asInt();
            strncpy(item->text.font, json["font"].asCString(), sizeof(item->text.font));
            strncpy(item->text.content, json["content"].asCString(), sizeof(item->text.content));
            item->text.margin.rateX = json["margin"]["horizon"]["rate"].asInt();
            item->text.margin.horizon = (MediaOSD::HAlignE) json["margin"]["horizon"]["align"].asInt();
            item->text.margin.rateY = json["margin"]["vertical"]["rate"].asInt();
            item->text.margin.vertical = (MediaOSD::VAlignE) json["margin"]["vertical"]["align"].asInt();
            param.emplace_back(item);
        }
        for (auto &json : chn["image"]) {
            auto item = std::make_shared<MediaOSD::Param>();
            item->type = MediaOSD::TypeE::Image;
            item->image.ena = json["ena"].asBool();
            item->image.layer = json["layer"].asInt();
            item->image.width = json["width"].asInt();
            item->image.height = json["height"].asInt();
            item->image.alpha = json["alpha"].asInt();
            strncpy(item->image.path, json["path"].asCString(), sizeof(item->image.path));
            item->image.margin.rateX = json["margin"]["horizon"]["rate"].asInt();
            item->image.margin.horizon = (MediaOSD::HAlignE) json["margin"]["horizon"]["align"].asInt();
            item->image.margin.rateY = json["margin"]["vertical"]["rate"].asInt();
            item->image.margin.vertical = (MediaOSD::VAlignE) json["margin"]["vertical"]["align"].asInt();
            param.emplace_back(item);
        }
        e = osd.SetParam(param);
        if (e != ErrCodeE::Success) {
            MakeCodeDesc(header, e, "SetParam failed");
            return;
        }
    }
}

void EmxProtoV2::IrCutSwitch(Json::Value &in, Json::Value &out, Json::Value &header) {
    Bsp::IrCut irCut;
    irCut.SwitchToNight(in["switchToNight"].asBool(), in["chn"].asInt());
}

void EmxProtoV2::LumaGet(Json::Value &in, Json::Value &out, Json::Value &header) {
    Bsp::Adc adc;
    out["luma"] = adc.GetLumaValue();
    out["adc"] = adc.GetAdcValue();
}

void EmxProtoV2::IrLightGet(Json::Value &in, Json::Value &out, Json::Value &header) {
    Bsp::IrLight irLight;
    out["luma"] = irLight.GetLuma(in["chn"].asInt());
}

void EmxProtoV2::IrLightSet(Json::Value &in, Json::Value &out, Json::Value &header) {
    Bsp::IrLight irLight;
    irLight.SetLuma(in["luma"].asInt(), in["chn"].asInt());
}

void EmxProtoV2::WhiteLightGet(Json::Value &in, Json::Value &out, Json::Value &header) {
    Bsp::WhiteLight whiteLight;
    out["luma"] = whiteLight.GetLuma(in["chn"].asInt());
}

void EmxProtoV2::WhiteLightSet(Json::Value &in, Json::Value &out, Json::Value &header) {
    Bsp::WhiteLight whiteLight;
    whiteLight.SetLuma(in["luma"].asInt(), in["chn"].asInt());
}

void EmxProtoV2::IndicatorLightGet(Json::Value &in, Json::Value &out, Json::Value &header) {
    Bsp::Led led;
    out["ena"] = led.Get(in["chn"].asInt());
}

void EmxProtoV2::IndicatorLightSet(Json::Value &in, Json::Value &out, Json::Value &header) {
    Bsp::Led led;
    led.Set(in["ena"].asBool(), in["chn"].asInt());
}

void EmxProtoV2::PressKeyGet(Json::Value &in, Json::Value &out, Json::Value &header) {
    int ena = 0;
    Gpio gpio(in["num"].asInt(),
              static_cast<Gpio::DirE>(in["dir"].asInt()),
              static_cast<Gpio::ActiveE>(in["active"].asInt()),
              static_cast<Gpio::ValueE>(in["def"].asInt()));
    uint32_t timeout = in["timeout"].asInt() * 10;
    while (timeout--) {
        usleep(100000);
        if (gpio.GetValue() == Gpio::ValueE::Enable) {
            ena = 1;
            break;
        }
    }
    out["ena"] = ena;
}

void EmxProtoV2::PressKeySet(Json::Value &in, Json::Value &out, Json::Value &header) {
    Gpio gpio(in["num"].asInt(),
              static_cast<Gpio::DirE>(in["dir"].asInt()),
              static_cast<Gpio::ActiveE>(in["active"].asInt()),
              static_cast<Gpio::ValueE>(in["def"].asInt()));
    gpio.SetValue(static_cast<Gpio::ValueE>(in["ena"].asInt()));
}

void EmxProtoV2::ExtendCmd(Json::Value &in, Json::Value &out, Json::Value &header) {
    ErrCodeE e;
    std::string resp;
    std::string reqData;
    Json::Value respObj;
    int dstClientId = (int) in["dstClientId"].asInt();
    int moduleId = (int) in["moduleId"].asInt();
    int methodId = (int) in["methodId"].asInt();

    e = EasyJson::Serialize(in["reqData"], reqData);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(header, e, "Serialize json failed");
        return;
    }

    e = MsgRpcReqSync::Request(dstClientId,
                               (uint32_t) moduleId,
                               (uint32_t) methodId,
                               reqData.data(), reqData.size(), &resp);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(header, e, "ExtendCmd failed");
        return;
    }

    e = EasyJson::Parse(resp.data(), resp.size(), out);
    if (e != ErrCodeE::Success) {
        emxloge("resp parse failed\n");
        MakeCodeDesc(header, e, "resp parse failed");
        return;
    }
}

#ifdef SUPPORT_EVENT_STD

#include "EventClient.hpp"

void EmxProtoV2::EventListGet(Json::Value &in, Json::Value &out, Json::Value &header) {
    EventClientGlobal global;
    std::vector<Event::Channel> channels;
    auto e = global.GetSupportedEventChannel(channels);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(header, e, "GetSupportedEventChannel failed");
        return;
    }
    auto &events = out["events"];
    events.resize(0);
    for (auto &c:channels) {
        Json::Value chn;
        chn["type"] = (int) c.type;
        chn["chnNum"] = c.chnNum;
        events.append(chn);
    }
}

void EmxProtoV2::EventGlobalGet(Json::Value &in, Json::Value &out, Json::Value &header) {
    EventClientGlobal global;
    auto param = std::make_shared<EventGlobal::Param>();
    auto e = global.GetParam(param);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(header, e, "GetSupportedEventChannel failed");
        return;
    }
//    auto &events = out["events"];
//    events.resize(0);
//    for (auto &c:channels) {
//        Json::Value chn;
//        chn["type"] = (int) c.type;
//        chn["chnNum"] = c.chnNum;
//        events.append(chn);
//    }
}

void EmxProtoV2::EventGlobalSet(Json::Value &in, Json::Value &out, Json::Value &header) {

}

void EmxProtoV2::EventPluginGet(Json::Value &in, Json::Value &out, Json::Value &header) {

}

void EmxProtoV2::EventPluginSet(Json::Value &in, Json::Value &out, Json::Value &header) {

}

#else

void EmxProtoV2::EventListGet(Json::Value &in, Json::Value &out, Json::Value &header) {
    MakeCodeDesc(header, ErrCodeE::ResNotAvailable, "ResNotAvailable");
}

void EmxProtoV2::EventGlobalGet(Json::Value &in, Json::Value &out, Json::Value &header) {
    MakeCodeDesc(header, ErrCodeE::ResNotAvailable, "ResNotAvailable");
}

void EmxProtoV2::EventGlobalSet(Json::Value &in, Json::Value &out, Json::Value &header) {
    MakeCodeDesc(header, ErrCodeE::ResNotAvailable, "ResNotAvailable");
}

void EmxProtoV2::EventPluginGet(Json::Value &in, Json::Value &out, Json::Value &header) {
    MakeCodeDesc(header, ErrCodeE::ResNotAvailable, "ResNotAvailable");
}

void EmxProtoV2::EventPluginSet(Json::Value &in, Json::Value &out, Json::Value &header) {
    MakeCodeDesc(header, ErrCodeE::ResNotAvailable, "ResNotAvailable");
}

#endif

#ifdef SUPPORT_RECORD_STD

#include "RecordStdClient.hpp"

void EmxProtoV2::RecordStdGet(Json::Value &in, Json::Value &out, Json::Value &header) {
    auto param = std::make_shared<RecordStd::Param>();
    RecordStdClient record;
    auto e = record.GetParam(param);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(header, e, "GetParam failed");
        return;
    }
    param->Serialize(out);
}

void EmxProtoV2::RecordStdSet(Json::Value &in, Json::Value &out, Json::Value &header) {
    auto param = std::make_shared<RecordStd::Param>();
    RecordStdClient record;
    param->Parse(in);
    auto e = record.SetParam(param);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(header, e, "SetParam failed");
        return;
    }
}

void EmxProtoV2::RecordStdRecordManualGet(Json::Value &in, Json::Value &out, Json::Value &header) {
    RecordStdClient record;
    bool ena;
    auto e = record.GetManualRecordStat(ena);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(header, e, "GetManualRecordStat failed");
        return;
    }
    out["ena"] = ena;
}

void EmxProtoV2::RecordStdRecordManualSet(Json::Value &in, Json::Value &out, Json::Value &header) {
    RecordStdClient record;
    bool ena = in["ena"].asBool();
    auto e = record.SetManualRecordStat(ena);
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(header, e, "SetManualRecordStat failed");
        return;
    }
}

void EmxProtoV2::RecordStdRecordListGet(Json::Value &in, Json::Value &out, Json::Value &header) {
    uint32_t sUtc = in["startUtc"].asUInt();
    uint32_t eUtc = in["endUtc"].asUInt();
    bool merge = in["merge"].asBool();
    RecordStdClient record;
    std::vector<RecordStd::RecFile> index;
    record.GetRecordList(sUtc, eUtc, merge, index);

    out["index"].resize(0);
    for (auto &elem:index) {
        Json::Value item;
        item.append(elem.date);
        item.append(elem.time);
        item.append(elem.startUtc);
        item.append(elem.endUtc);
        item.append(elem.size);
        out["index"].append(item);
    }
}

void EmxProtoV2::RecordStdSnapManualTrigger(Json::Value &in, Json::Value &out, Json::Value &header) {
    RecordStdClient record;
    auto e = record.ManualSnap();
    if (e != ErrCodeE::Success) {
        MakeCodeDesc(header, e, "ManualSnap failed");
        return;
    }
}

void EmxProtoV2::RecordStdSnapListGet(Json::Value &in, Json::Value &out, Json::Value &header) {
    uint32_t sUtc = in["startUtc"].asUInt();
    uint32_t eUtc = in["endUtc"].asUInt();
    RecordStdClient record;
    std::vector<RecordStd::RecFile> index;
    record.GetSnapList(sUtc, eUtc, index);

    out["index"].resize(0);
    for (auto &elem:index) {
        Json::Value item;
        item.append(elem.date);
        item.append(elem.time);
        item.append(elem.startUtc);
        item.append(elem.size);
        out["index"].append(item);
    }
}

#else
void EmxProtoV2::RecordStdGet(Json::Value &in, Json::Value &out, Json::Value &header) {
    MakeCodeDesc(header, ErrCodeE::ResNotAvailable, "ResNotAvailable");
}

void EmxProtoV2::RecordStdSet(Json::Value &in, Json::Value &out, Json::Value &header) {
    MakeCodeDesc(header, ErrCodeE::ResNotAvailable, "ResNotAvailable");
}

void EmxProtoV2::RecordStdRecordManualGet(Json::Value &in, Json::Value &out, Json::Value &header) {
    MakeCodeDesc(header, ErrCodeE::ResNotAvailable, "ResNotAvailable");
}

void EmxProtoV2::RecordStdRecordManualSet(Json::Value &in, Json::Value &out, Json::Value &header) {
    MakeCodeDesc(header, ErrCodeE::ResNotAvailable, "ResNotAvailable");
}

void EmxProtoV2::RecordStdRecordListGet(Json::Value &in, Json::Value &out, Json::Value &header) {
    MakeCodeDesc(header, ErrCodeE::ResNotAvailable, "ResNotAvailable");
}

void EmxProtoV2::RecordStdSnapManualTrigger(Json::Value &in, Json::Value &out, Json::Value &header) {
    MakeCodeDesc(header, ErrCodeE::ResNotAvailable, "ResNotAvailable");
}

void EmxProtoV2::RecordStdSnapListGet(Json::Value &in, Json::Value &out, Json::Value &header) {
    MakeCodeDesc(header, ErrCodeE::ResNotAvailable, "ResNotAvailable");
}
#endif
