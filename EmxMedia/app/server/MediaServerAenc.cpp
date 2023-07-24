//
// Created by xiong on 2020/11/19.
//

#include "EmxCore.hpp"
#include "MediaServerAenc.hpp"
#include "MediaConfigs.hpp"
#include "MediaFactory.hpp"
#include "MediaInner.hpp"

using namespace Emx;
std::vector<MediaServerAenc *> MediaServerAenc::m_array;

void MediaServerAenc::Create(MediaServerResource &res) {
    Json::Value json;
    res.configs.GetCfg("aenc", json);
    for (int chn = 0; chn < (int) json.size(); chn++) {
        auto e = res.factory->MakeAenc(chn, res);
        m_array.push_back(e);
        e->Start();
    }
}

void MediaServerAenc::Destroy() {
    for (auto e : m_array) {
        e->Stop();
        delete e;
    }
    m_array.clear();
}

ErrCodeE MediaServerAenc::OnRecvReq(uint32_t module, uint32_t method,
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
            MediaAenc::Param param = {};
            memcpy(&param, data, sizeof(param));
            return m_array[chn]->SetParam(param);
        }
        case (uint32_t) MediaParamMethodE::GetParam: {
            MediaAenc::Param param = {};
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
            int chnNum = (int)m_array.size();
            out.assign((const char *) &chnNum, sizeof(chnNum));
            return ErrCodeE::Success;
        }
        case (uint32_t) MediaAencMethodE::SetVolume: {
            int32_t volume;
            memcpy(&volume, data, sizeof(volume));
            return m_array[chn]->SetVolume(volume);
        }
        case (uint32_t) MediaAencMethodE::Mute: {
            bool mute;
            memcpy(&mute, data, sizeof(mute));
            return m_array[chn]->Mute(mute);
        }
        default:
            return m_array[chn]->OnRecvReqSub(module, method, data, size, out);
    }
}

ErrCodeE MediaServerAenc::OnRecvReqSub(uint32_t module, uint32_t method,
                                       const char *data, int32_t size, std::string &out) {
    emxloge("not support method[%u]\n", method);
    return ErrCodeE::OperationNotSupport;
}

ErrCodeE MediaServerAenc::LoadParam() {
    Json::Value root;
    ErrCodeE e = m_res.configs.GetParam("aenc", root);
    if (e != ErrCodeE::Success) {
        emxlogc("Read param failed : %d\n", e);
        return e;
    }
    auto &json = root[m_chn];
    m_param.codec = (AudioCodecE) json["codec"].asInt();
    m_param.bitRate = json["bitRate"].asInt();
    m_param.sampleRate = json["sampleRate"].asInt();
    m_param.volume = json["volume"].asInt();
    m_param.bitWidth = json["bitWidth"].asInt();
    m_param.mute = json["mute"].asBool();
    return ErrCodeE::Success;
}

ErrCodeE MediaServerAenc::SaveParam() {
    Json::Value root;
    ErrCodeE e = m_res.configs.GetParam("aenc", root);
    if (e != ErrCodeE::Success) {
        emxlogc("Read param failed : %d\n", e);
        return e;
    }
    auto &json = root[m_chn];
    json["codec"] = (int32_t) m_param.codec;
    json["bitRate"] = (int32_t) m_param.bitRate;
    json["sampleRate"] = (int32_t) m_param.sampleRate;
    json["volume"] = (int32_t) m_param.volume;
    json["bitWidth"] = (int32_t) m_param.bitWidth;
    json["mute"] = m_param.mute;
    return m_res.configs.SetParam("aenc", root);
}

ErrCodeE MediaServerAenc::GetParamRange(Json::Value &range) {
    return m_res.configs.GetRange("aenc", range);
}

bool MediaServerAenc::ParamValid(const MediaAenc::Param &param) {
    Json::Value range;
    if (GetParamRange(range) != ErrCodeE::Success)
        return false;
    if (!MediaConfigs::CheckItemLegal("codec", (int) param.codec, range))
        return false;
    if (!MediaConfigs::CheckRangeLegal("bitRate", (int) param.bitRate, range))
        return false;
    if (!MediaConfigs::CheckItemLegal("sampleRate", (int) param.sampleRate, range))
        return false;
    if (!MediaConfigs::CheckRangeLegal("volume", (int) param.volume, range))
        return false;
    if (!MediaConfigs::CheckItemLegal("bitWidth", (int) param.bitWidth, range))
        return false;
    return true;
}
