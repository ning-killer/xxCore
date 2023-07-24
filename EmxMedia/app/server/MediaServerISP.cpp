//
// Created by xiong on 2021/4/23.
//
#include "EmxCore.hpp"
#include "MediaInner.hpp"
#include "MediaServerISP.hpp"
#include "MediaConfigs.hpp"
#include "MediaFactory.hpp"

using namespace Emx;
std::vector<MediaServerISP *> MediaServerISP::m_array;

void MediaServerISP::Create(MediaServerResource &res) {
    Json::Value json;
    res.configs.GetCfg("isp", json);
    for (int chn = 0; chn < (int) json.size(); chn++) {
        auto e = res.factory->MakeISP(chn, res);
        m_array.push_back(e);
        e->Start();
    }
}

void MediaServerISP::Destroy() {
    for (auto e: m_array) {
        e->Stop();
        delete e;
    }
    m_array.clear();
}

ErrCodeE MediaServerISP::OnRecvReq(uint32_t module, uint32_t method,
                                   const char *data, int32_t size, std::string &out) {
    int32_t chn;
    memcpy(&chn, data, sizeof(chn));
    if (chn >= (int32_t) m_array.size()) {
        emxloge("chn %d not exists\n", chn);
        return ErrCodeE::ResNotExist;
    }
    data += sizeof(chn);
    size -= sizeof(chn);
    switch (method) {
        case (uint32_t) MediaParamMethodE::SetParam: {
            MediaISP::Param param = {};
            memcpy(&param, data, sizeof(param));
            return m_array[chn]->SetParam(param);
        }
        case (uint32_t) MediaParamMethodE::GetParam: {
            MediaISP::Param param = {};
            ErrCodeE e = m_array[chn]->GetParam(param);
            out.assign((const char *) &param, sizeof(param));
            return e;
        }
        case (uint32_t) MediaParamMethodE::GetParamRange: {
            Json::Value range;
            ErrCodeE e = m_array[chn]->GetParamRange(range);
            out = range.toStyledString();
            return e;
        }
        case (uint32_t) MediaParamMethodE::GetChnNum: {
            int chnNum = (int) m_array.size();
            out.assign((const char *) &chnNum, sizeof(chnNum));
            return ErrCodeE::Success;
        }
        case (uint32_t) MediaIspMethodE::SetRunMode: {
            MediaISP::RunModeE mode;
            memcpy(&mode, data, sizeof(mode));
            return m_array[chn]->SetRunMode(mode);
        }
        case (uint32_t) MediaIspMethodE::GetRunMode: {
            MediaISP::RunModeE mode;
            auto e = m_array[chn]->GetRunMode(mode);
            out.assign((const char *) &mode, sizeof(mode));
            return e;
        }
        default:
            return m_array[chn]->OnRecvReqSub(module, method, data, size, out);
    }
}

ErrCodeE MediaServerISP::OnRecvReqSub(uint32_t module, uint32_t method,
                                      const char *data, int32_t size, std::string &out) {
    emxloge("not support method[%u]\n", method);
    return ErrCodeE::OperationNotSupport;
}

ErrCodeE MediaServerISP::LoadParam() {
    Json::Value root;
    ErrCodeE e = m_res.configs.GetParam("isp", root);
    if (e != ErrCodeE::Success) {
        emxlogc("Read isp param failed : %d\n", e);
        return e;
    }
    auto &json = root[m_chn];
    m_param.expMode = (OptModeE) json["expMode"].asInt();
    m_param.autoMaxExpGain = (ExpGainE) json["autoMaxExpGain"].asInt();
    m_param.compensation = json["compensation"].asInt();
    m_param.manualExpGain = (ExpGainE) json["manualExpGain"].asInt();
    m_param.expTime = json["expTime"].asInt();
    m_param.brightness = json["brightness"].asInt();
    m_param.sharpness = json["sharpness"].asInt();
    m_param.hue = json["hue"].asInt();
    m_param.contrast = json["contrast"].asInt();
    m_param.saturation = json["saturation"].asInt();
    m_param.deNoise = json["deNoise"].asInt();
    m_param.wbMode = (OptModeE) json["wbMode"].asInt();
    m_param.redGain = json["redGain"].asInt();
    m_param.blueGain = json["blueGain"].asInt();
    m_param.antiFlickerHz = json["antiFlickerHz"].asInt();
    m_param.forceAntiFlickerEna = json["forceAntiFlickerEna"].asInt();
    return ErrCodeE::Success;
}

ErrCodeE MediaServerISP::SaveParam() {
    Json::Value root;
    ErrCodeE e = m_res.configs.GetParam("isp", root);
    if (e != ErrCodeE::Success) {
        emxlogc("Read isp param failed : %d\n", e);
        return e;
    }
    auto &json = root[m_chn];
    json["expMode"] = (int) m_param.expMode;
    json["autoMaxExpGain"] = (int) m_param.autoMaxExpGain;
    json["compensation"] = m_param.compensation;
    json["manualExpGain"] = (int) m_param.manualExpGain;
    json["expTime"] = m_param.expTime;
    json["brightness"] = m_param.brightness;
    json["sharpness"] = m_param.sharpness;
    json["hue"] = m_param.hue;
    json["contrast"] = m_param.contrast;
    json["saturation"] = m_param.saturation;
    json["deNoise"] = m_param.deNoise;
    json["wbMode"] = (int) m_param.wbMode;
    json["redGain"] = m_param.redGain;
    json["blueGain"] = m_param.blueGain;
    json["antiFlickerHz"] = (int) m_param.antiFlickerHz;
    json["forceAntiFlickerEna"] = (int) m_param.forceAntiFlickerEna;
    return m_res.configs.SetParam("isp", root);
}

ErrCodeE MediaServerISP::GetParamRange(Json::Value &range) {
    return m_res.configs.GetRange("isp", range);
}

bool MediaServerISP::ParamValid(const MediaISP::Param &param) {
    Json::Value range;
    if (GetParamRange(range) != ErrCodeE::Success)
        return false;
    if (!MediaConfigs::CheckItemLegal("expMode", (int) param.expMode, range))
        return false;
    if (!MediaConfigs::CheckRangeLegal("autoMaxExpGain", (int) param.autoMaxExpGain, range))
        return false;
    if (!MediaConfigs::CheckRangeLegal("compensation", (int) param.compensation, range))
        return false;
    if (!MediaConfigs::CheckItemLegal("manualExpGain", (int) param.manualExpGain, range))
        return false;
    if (!MediaConfigs::CheckRangeLegal("expTime", (int) param.expTime, range))
        return false;
    if (!MediaConfigs::CheckRangeLegal("brightness", (int) param.brightness, range))
        return false;
    if (!MediaConfigs::CheckRangeLegal("sharpness", (int) param.sharpness, range))
        return false;
    if (!MediaConfigs::CheckRangeLegal("hue", (int) param.hue, range))
        return false;
    if (!MediaConfigs::CheckRangeLegal("contrast", (int) param.contrast, range))
        return false;
    if (!MediaConfigs::CheckRangeLegal("saturation", (int) param.saturation, range))
        return false;
    if (!MediaConfigs::CheckRangeLegal("deNoise", (int) param.deNoise, range))
        return false;
    if (!MediaConfigs::CheckItemLegal("wbMode", (int) param.wbMode, range))
        return false;
    if (!MediaConfigs::CheckRangeLegal("redGain", (int) param.redGain, range))
        return false;
    if (!MediaConfigs::CheckRangeLegal("blueGain", (int) param.blueGain, range))
        return false;
    if (!MediaConfigs::CheckItemLegal("antiFlickerHz", (int) param.antiFlickerHz, range))
        return false;
    return true;
}

