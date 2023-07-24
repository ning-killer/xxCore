//
// Created by xiong on 2021/5/21.
//
#include "EmxCore.hpp"
#include "EmxMedia.hpp"
#include "EmxProtoV1.hpp"
#include "DevInfo.hpp"
#include "Sn.hpp"
#include "SDCardClient.hpp"

#define EP_CFG_CODE_OK 0
#define EP_CFG_CODE_ERR 400
#define EP_CFG_DESC_OK "successed"
#define EP_CFG_DESC_FAILED "failed"

using namespace Emx;
#define EP_PROTO_BIND_FUNC(x) std::bind(&EmxProtoV1::x, this, ph_1, ph_2);

EmxProtoV1::EmxProtoV1() {
    m_funcMap[CmdE::BaseGet] = EP_PROTO_BIND_FUNC(BaseGet);
    m_funcMap[CmdE::VideoMajorGet] = EP_PROTO_BIND_FUNC(VideoMajorGet);
    m_funcMap[CmdE::VideoMajorSet] = EP_PROTO_BIND_FUNC(VideoMajorSet);
    m_funcMap[CmdE::VideoMajorQue] = EP_PROTO_BIND_FUNC(VideoMajorQue);
    m_funcMap[CmdE::VideoMinorGet] = EP_PROTO_BIND_FUNC(VideoMinorGet);
    m_funcMap[CmdE::VideoMinorSet] = EP_PROTO_BIND_FUNC(VideoMinorSet);
    m_funcMap[CmdE::VideoMinorQue] = EP_PROTO_BIND_FUNC(VideoMinorQue);
    m_funcMap[CmdE::AudioGet] = EP_PROTO_BIND_FUNC(AudioGet);
    m_funcMap[CmdE::AudioSet] = EP_PROTO_BIND_FUNC(AudioSet);
    m_funcMap[CmdE::AudioQue] = EP_PROTO_BIND_FUNC(AudioQue);
    m_funcMap[CmdE::NetGet] = EP_PROTO_BIND_FUNC(NetGet);
    m_funcMap[CmdE::NetSet] = EP_PROTO_BIND_FUNC(NetSet);
    m_funcMap[CmdE::WifiGet] = EP_PROTO_BIND_FUNC(NetWifiGet);
    m_funcMap[CmdE::WifiSet] = EP_PROTO_BIND_FUNC(NetWifiSet);
    m_funcMap[CmdE::SnGet] = EP_PROTO_BIND_FUNC(SnGet);
    m_funcMap[CmdE::SnSet] = EP_PROTO_BIND_FUNC(SnSet);
    m_funcMap[CmdE::IspGet] = EP_PROTO_BIND_FUNC(IspGet);
    m_funcMap[CmdE::IspSet] = EP_PROTO_BIND_FUNC(IspSet);
    m_funcMap[CmdE::IspQue] = EP_PROTO_BIND_FUNC(IspQue);
//    m_funcMap[CmdE::SNAP] = EP_PROTO_BIND_FUNC(SnapGet);
    m_funcMap[CmdE::Music] = EP_PROTO_BIND_FUNC(MusicPlay);
    m_funcMap[CmdE::Reboot] = EP_PROTO_BIND_FUNC(Reboot);
    m_funcMap[CmdE::Reset] = EP_PROTO_BIND_FUNC(Reset);
    m_funcMap[CmdE::SdInfo] = EP_PROTO_BIND_FUNC(SDInfo);
    m_funcMap[CmdE::SdFormat] = EP_PROTO_BIND_FUNC(SDFormat);
    m_funcMap[CmdE::SdCheck] = EP_PROTO_BIND_FUNC(SDCheck);
    m_funcMap[CmdE::TimeGet] = EP_PROTO_BIND_FUNC(TimeGet);
    m_funcMap[CmdE::TimeSet] = EP_PROTO_BIND_FUNC(TimeSet);
    m_funcMap[CmdE::IcrGet] = EP_PROTO_BIND_FUNC(ICRGet);
    m_funcMap[CmdE::IcrSet] = EP_PROTO_BIND_FUNC(ICRSet);
    m_funcMap[CmdE::LdsGet] = EP_PROTO_BIND_FUNC(LDSGet);
    m_funcMap[CmdE::LdsSet] = EP_PROTO_BIND_FUNC(LDSSet);
    m_funcMap[CmdE::OsdGet] = EP_PROTO_BIND_FUNC(OSDGet);
    m_funcMap[CmdE::OsdSet] = EP_PROTO_BIND_FUNC(OSDSet);
    m_funcMap[CmdE::TmplGet] = EP_PROTO_BIND_FUNC(TemplateGet);
    m_funcMap[CmdE::TmplSet] = EP_PROTO_BIND_FUNC(TemplateSet);
    m_funcMap[CmdE::CenterGet] = EP_PROTO_BIND_FUNC(CenterGet);
    m_funcMap[CmdE::CenterSet] = EP_PROTO_BIND_FUNC(CenterSet);
}

void EmxProtoV1::Proc(EmxProtoV1::CmdE cmd, const char *data, int32_t size, std::string &out) {
    emxlogd("cmd=0x%x\n", cmd);
    Json::Value jsonIn;
    if (size) {
        if (EasyJson::Parse(data, size, jsonIn) != ErrCodeE::Success)
            return;
        emxlogd("Req#%s#\n", data);
    }
    if (cmd == CmdE::Snap) {
        SnapGet(jsonIn, out);
    } else if (m_funcMap.count(cmd) != 0) {
        Json::Value jsonOut;
        m_funcMap[cmd](jsonIn, jsonOut);
        EasyJson::Serialize(jsonOut, out);
        emxlogd("Resp#%s#\n", out.data());
    } else {
        emxloge("not support cmd %x\n", cmd);
    }
}

static void SetCodeDesc(Json::Value &json, int code, const char *desc) {
    json["code"] = code;
    json["desc"] = desc;
}

void EmxProtoV1::BaseGet(Json::Value &in, Json::Value &out) {
    SetCodeDesc(out, EP_CFG_CODE_OK, EP_CFG_DESC_OK);
    DevInfo devInfo;
    devInfo.Load();
    out["dev_type"] = devInfo.GetOuter()->type;
    out["firmware"] = devInfo.GetOuter()->version;
    out["dev_id"] = devInfo.GetInner()->type;
    out["dev_ver"] = devInfo.GetInner()->version;
    out["hardware"] = devInfo.GetHardware();
    Sn sn;
    sn.Load();
    out["sn"] = sn.Get();
}

void EmxProtoV1::NetGet(Json::Value &in, Json::Value &out) {

}

void EmxProtoV1::NetSet(Json::Value &in, Json::Value &out) {
}

void EmxProtoV1::NetWifiGet(Json::Value &in, Json::Value &out) {
}

void EmxProtoV1::NetWifiSet(Json::Value &in, Json::Value &out) {
}

void EmxProtoV1::SnGet(Json::Value &in, Json::Value &out) {
}

void EmxProtoV1::SnSet(Json::Value &in, Json::Value &out) {
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
        SetCodeDesc(out, EP_CFG_CODE_ERR, EP_CFG_DESC_FAILED);
        return;
    }
    SetCodeDesc(out, EP_CFG_CODE_OK, EP_CFG_DESC_OK);
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
        SetCodeDesc(out, EP_CFG_CODE_ERR, EP_CFG_DESC_FAILED);
        return;
    }
    SetCodeDesc(out, EP_CFG_CODE_OK, EP_CFG_DESC_OK);
    const char *tmp;
    if (param.codec == VideoCodecE::H264)
        tmp = "h264";
    else if (param.codec == VideoCodecE::H265)
        tmp = "h265";
    else if (param.codec == VideoCodecE::MJPEG)
        tmp = "mjpeg";
    else
        tmp = nullptr;
    out["codec"] = tmp;

    if (param.rcMode == MediaVenc::RcModeE::VBR)
        tmp = "vbr";
    else if (param.rcMode == MediaVenc::RcModeE::CBR)
        tmp = "cbr";
    else
        tmp = nullptr;
    out["rc_mode"] = tmp;
    out["profile"] = 1;
    out["resolution"] = std::to_string(param.width) + "x" + std::to_string(param.height);
    out["frame_rate"] = param.fps;
    out["bitrate"] = param.bitRate;
    out["gop"] = param.iFrameInv * param.fps;
}

static void VideoSet(int chn, Json::Value &in, Json::Value &out) {
    MediaClientVenc venc(chn);
    MediaVenc::Param param = {};
    if (venc.GetParam(param) != ErrCodeE::Success) {
        emxloge("cannot got chn %d venc param\n", chn);
        SetCodeDesc(out, EP_CFG_CODE_ERR, EP_CFG_DESC_FAILED);
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
            SetCodeDesc(out, EP_CFG_CODE_ERR, EP_CFG_DESC_FAILED);
            return;
        }
    }

    if (in.isMember("frame_rate") && in.isMember("gop")) {
        param.fps = in["frame_rate"].asInt();
        if (param.fps == 0)
            SetCodeDesc(out, EP_CFG_CODE_ERR, EP_CFG_DESC_FAILED);
        param.iFrameInv = in["gop"].asInt() / param.fps;
    }

    if (in.isMember("rc_mode")) {
        if (in["rc_mode"].asString() == "vbr") {
            param.rcMode = MediaVenc::RcModeE::VBR;
        } else if (in["rc_mode"].asString() == "cbr") {
            param.rcMode = MediaVenc::RcModeE::CBR;
        } else {
            emxloge("rc_mode %s not be supported\n", in["rc_mode"].asCString());
            SetCodeDesc(out, EP_CFG_CODE_ERR, EP_CFG_DESC_FAILED);
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
        SetCodeDesc(out, EP_CFG_CODE_OK, EP_CFG_DESC_OK);
    else
        SetCodeDesc(out, EP_CFG_CODE_ERR, EP_CFG_DESC_FAILED);
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
        SetCodeDesc(out, EP_CFG_CODE_ERR, EP_CFG_DESC_FAILED);
        return;
    }
    if (adec.GetParam(adecParam) != ErrCodeE::Success) {
        emxloge("get adec param failed\n");
        SetCodeDesc(out, EP_CFG_CODE_ERR, EP_CFG_DESC_FAILED);
        return;
    }

    if (aencParam.codec == AudioCodecE::AAC)
        out["codec"] = "aac";
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
    SetCodeDesc(out, EP_CFG_CODE_OK, EP_CFG_DESC_OK);
}

void EmxProtoV1::AudioSet(Json::Value &in, Json::Value &out) {
    MediaClientAenc aenc(0);
    MediaClientAdec adec(0);
    MediaAenc::Param aencParam = {};
    MediaAdec::Param adecParam = {};

    if (aenc.GetParam(aencParam) != ErrCodeE::Success) {
        emxloge("get aenc param failed\n");
        SetCodeDesc(out, EP_CFG_CODE_ERR, EP_CFG_DESC_FAILED);
        return;
    }
    if (adec.GetParam(adecParam) != ErrCodeE::Success) {
        emxloge("get adec param failed\n");
        SetCodeDesc(out, EP_CFG_CODE_ERR, EP_CFG_DESC_FAILED);
        return;
    }

    if (in.isMember("mic_vol")) {
        aencParam.volume = in["mic_vol"].asInt();
    }

    if (in.isMember("spk_vol")) {
        adecParam.volume = in["spk_vol"].asInt();
    }

    if (aenc.SetParam(aencParam) == Emx::ErrCodeE::Success &&
        adec.SetParam(adecParam) == Emx::ErrCodeE::Success)
        SetCodeDesc(out, EP_CFG_CODE_OK, EP_CFG_DESC_OK);
    else
        SetCodeDesc(out, EP_CFG_CODE_ERR, EP_CFG_DESC_FAILED);
}

void EmxProtoV1::AudioQue(Json::Value &in, Json::Value &out) {
    MediaClientAenc aenc(0);
    MediaClientAdec adec(0);
    Json::Value rangeAenc;
    Json::Value rangeAdec;
    if (aenc.GetParamRange(rangeAenc) != ErrCodeE::Success) {
        emxloge("get aenc range failed\n");
        SetCodeDesc(out, EP_CFG_CODE_ERR, EP_CFG_DESC_FAILED);
        return;
    }
    if (adec.GetParamRange(rangeAdec) != ErrCodeE::Success) {
        emxloge("get adec range failed\n");
        SetCodeDesc(out, EP_CFG_CODE_ERR, EP_CFG_DESC_FAILED);
        return;
    }
    out["codec"] = "g711a,acc";
    out["sample_rate"] = "8000,16000";
    out["bitrate"] = "[32,128]";
    out["bit_width"] = "16";
    out["channel_num"] = "1";
    SetRange(out, "mic_vol", rangeAenc["volume"]);
    SetRange(out, "spk_vol", rangeAdec["volume"]);
    SetCodeDesc(out, EP_CFG_CODE_OK, EP_CFG_DESC_OK);
}

void EmxProtoV1::IspGet(Json::Value &in, Json::Value &out) {
    MediaClientISP isp(0);
    MediaISP::Param param = {};
    if (isp.GetParam(param) != ErrCodeE::Success) {
        SetCodeDesc(out, EP_CFG_CODE_ERR, EP_CFG_DESC_FAILED);
        return;
    }
    SetCodeDesc(out, EP_CFG_CODE_OK, EP_CFG_DESC_OK);
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
//        SetCodeDesc(out, EP_CFG_CODE_ERR, EP_CFG_DESC_FAILED);
//        return;
//    }
//    SetCodeDesc(out, EP_CFG_CODE_OK, EP_CFG_DESC_OK);
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
        SetCodeDesc(in, EP_CFG_CODE_ERR, EP_CFG_DESC_FAILED);
        return;
    }
    MediaClientSnap::Trigger(0, 3000);
    MediaFrame frame = {};
    ErrCodeE e = snap.GetFrame(frame, 3000);
    if (e != ErrCodeE::Success) {
        SetCodeDesc(in, EP_CFG_CODE_ERR, EP_CFG_DESC_FAILED);
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
                SetCodeDesc(out, EP_CFG_CODE_OK, EP_CFG_DESC_OK);
                return;
            } else {
                SetCodeDesc(out, EP_CFG_CODE_ERR, "cannot found music file");
                return;
            }
        } else if (strcmp(in["play"].asCString(), "stop") == 0) {
            MediaClientAdecStreamSync::PlayCtrlQuit(0);
            SetCodeDesc(out, EP_CFG_CODE_OK, EP_CFG_DESC_OK);
            return;
        }
    }
    SetCodeDesc(out, EP_CFG_CODE_ERR, EP_CFG_DESC_FAILED);
}

void EmxProtoV1::Reboot(Json::Value &in, Json::Value &out) {
    SetCodeDesc(out, EP_CFG_CODE_OK, EP_CFG_DESC_OK);
    Reboot::DoReboot(3);
}

void EmxProtoV1::Reset(Json::Value &in, Json::Value &out) {
    if (Param::ResetAllToFactory() == ErrCodeE::Success) {
        SetCodeDesc(out, EP_CFG_CODE_OK, EP_CFG_DESC_OK);
    } else {
        SetCodeDesc(out, EP_CFG_CODE_ERR, EP_CFG_DESC_FAILED);
    }
}

void EmxProtoV1::SDInfo(Json::Value &in, Json::Value &out) {
    SDCardClient sd(0);
    SDCard::Info info = {};
    if (sd.GetInfo(info) != ErrCodeE::Success) {
        SetCodeDesc(out, EP_CFG_CODE_ERR, EP_CFG_DESC_FAILED);
        return;
    }
    SetCodeDesc(out, EP_CFG_CODE_OK, EP_CFG_DESC_OK);
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
        SetCodeDesc(out, EP_CFG_CODE_ERR, EP_CFG_DESC_FAILED);
        return;
    }
    if (sd.Format() == ErrCodeE::Success) {
        SetCodeDesc(out, EP_CFG_CODE_OK, EP_CFG_DESC_OK);
    } else {
        SetCodeDesc(out, EP_CFG_CODE_ERR, EP_CFG_DESC_FAILED);
    }
}

void EmxProtoV1::SDCheck(Json::Value &in, Json::Value &out) {

}

void EmxProtoV1::TimeGet(Json::Value &in, Json::Value &out) {

}

void EmxProtoV1::TimeSet(Json::Value &in, Json::Value &out) {

}

void EmxProtoV1::ICRGet(Json::Value &in, Json::Value &out) {

}

void EmxProtoV1::ICRSet(Json::Value &in, Json::Value &out) {

}

void EmxProtoV1::LDSGet(Json::Value &in, Json::Value &out) {

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
