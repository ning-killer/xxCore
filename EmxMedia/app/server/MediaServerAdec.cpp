//
// Created by xiong on 2020/11/23.
//

#include "EmxCore.hpp"
#include "MediaServerAdec.hpp"
#include "MediaConfigs.hpp"
#include "MediaFactory.hpp"
#include "MediaInner.hpp"

using namespace Emx;
std::vector<MediaServerAdec *> MediaServerAdec::m_array;

void MediaServerAdec::Create(MediaServerResource &res) {
    Json::Value json;
    res.configs.GetCfg("adec", json);
    for (int chn = 0; chn < (int) json.size(); chn++) {
        auto e = res.factory->MakeAdec(chn, res);
        m_array.push_back(e);
        e->Start();
    }
}

void MediaServerAdec::Destroy() {
    for (auto e: m_array) {
        e->Stop();
        delete e;
    }
    m_array.clear();
}

ErrCodeE MediaServerAdec::OnRecvReq(uint32_t module, uint32_t method,
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
            MediaAdec::Param param = {};
            memcpy(&param, data, sizeof(param));
            return m_array[chn]->SetParam(param);
        }
        case (uint32_t) MediaParamMethodE::GetParam: {
            MediaAdec::Param param = {};
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
        case (uint32_t) MediaAdecMethodE::SetVolume: {
            int32_t volume;
            memcpy(&volume, data, sizeof(volume));
            return m_array[chn]->SetVolume(volume);
        }
        case (uint32_t) MediaAdecMethodE::SetVolumeTemp: {
            int32_t volume;
            memcpy(&volume, data, sizeof(volume));
            return m_array[chn]->SetVolumeTemp(volume);
        }
        case (uint32_t) MediaAdecMethodE::GetPlayStatus: {
            MediaAdec::PlayStatusE status = m_array[chn]->GetPlayStatus();
            out.assign((const char *) &status, sizeof(status));
            return ErrCodeE::Success;
        }
        default:
            return m_array[chn]->OnRecvReqSub(module, method, data, size, out);
    }
}

ErrCodeE MediaServerAdec::OnRecvReqSub(uint32_t module, uint32_t method,
                                       const char *data, int32_t size, std::string &out) {
    emxloge("not support method[%u]\n", method);
    return ErrCodeE::OperationNotSupport;
}

ErrCodeE MediaServerAdec::LoadParam() {
    Json::Value root;
    ErrCodeE e = m_res.configs.GetParam("adec", root);
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
    return ErrCodeE::Success;
}

ErrCodeE MediaServerAdec::SaveParam() {
    Json::Value root;
    ErrCodeE e = m_res.configs.GetParam("adec", root);
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
    return m_res.configs.SetParam("adec", root);
}

ErrCodeE MediaServerAdec::GetParamRange(Json::Value &range) {
    return m_res.configs.GetRange("adec", range);
}

bool MediaServerAdec::ParamValid(const MediaAdec::Param &param) {
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
