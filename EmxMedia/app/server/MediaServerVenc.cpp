//
// Created by xiong on 2020/10/27.
//
#include "EmxCore.hpp"
#include "MediaServerVenc.hpp"
#include "MediaConfigs.hpp"
#include "MediaFactory.hpp"
#include "MediaInner.hpp"

using namespace Emx;
std::vector<MediaServerVenc *> MediaServerVenc::m_array;

void MediaServerVenc::Create(MediaServerResource &res) {
    Json::Value json;
    res.configs.GetCfg("venc", json);
    for (int chn = 0; chn < (int) json.size(); chn++) {
        auto e = res.factory->MakeVenc(chn, res);
        m_array.push_back(e);
        e->Start();
    }
}

void MediaServerVenc::Destroy() {
    for (auto e: m_array) {
        e->Stop();
        delete e;
    }
    m_array.clear();
}

ErrCodeE MediaServerVenc::OnRecvReq(uint32_t module, uint32_t method,
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
            MediaVenc::Param param = {};
            memcpy(&param, data, sizeof(param));
            return m_array[chn]->SetParam(param);
        }
        case (uint32_t) MediaParamMethodE::GetParam: {
            MediaVenc::Param param = {};
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
        case (uint32_t) MediaVencMethodE::FlushIdr: {
            return m_array[chn]->FlushIDR();
        }
        default:
            return m_array[chn]->OnRecvReqSub(module, method, data, size, out);
    }
}

ErrCodeE MediaServerVenc::OnRecvReqSub(uint32_t module, uint32_t method,
                                       const char *data, int32_t size, std::string &out) {
    emxloge("not support method[%u]\n", method);
    return ErrCodeE::OperationNotSupport;
}

ErrCodeE MediaServerVenc::LoadParam() {
    Json::Value root;
    ErrCodeE e = m_res.configs.GetParam("venc", root);
    if (e != ErrCodeE::Success) {
        emxlogc("Read param failed : %d\n", e);
        return e;
    }
    auto &json = root[m_chn];
    m_param.codec = (VideoCodecE) json["codec"].asInt();
    m_param.width = json["width"].asInt();
    m_param.height = json["height"].asInt();
    m_param.fps = json["fps"].asInt();
    m_param.bitRate = json["bitRate"].asInt();
    m_param.rcMode = (RcModeE) json["rcMode"].asInt();
    m_param.iFrameInv = json["iFrameInv"].asInt();
    m_param.drawRect = json["drawRect"].asBool();
    return ErrCodeE::Success;
}

ErrCodeE MediaServerVenc::SaveParam() {
    Json::Value root;
    ErrCodeE e = m_res.configs.GetParam("venc", root);
    if (e != ErrCodeE::Success) {
        emxlogc("Read param failed : %d\n", e);
        return e;
    }
    auto &json = root[m_chn];
    json["codec"] = (int32_t) m_param.codec;
    json["width"] = (int32_t) m_param.width;
    json["height"] = (int32_t) m_param.height;
    json["fps"] = (int32_t) m_param.fps;
    json["bitRate"] = (int32_t) m_param.bitRate;
    json["rcMode"] = (int32_t) m_param.rcMode;
    json["iFrameInv"] = (int32_t) m_param.iFrameInv;
    json["drawRect"] = m_param.drawRect;
    return m_res.configs.SetParam("venc", root);
}

ErrCodeE MediaServerVenc::GetParamRange(Json::Value &range) {
    return m_res.configs.GetRange("venc", range);
}

bool MediaServerVenc::ParamValid(const MediaVenc::Param &param) {
    Json::Value range;
    if (GetParamRange(range) != ErrCodeE::Success)
        return false;
    if (!MediaConfigs::CheckItemLegal("codec", (int) param.codec, range))
        return false;

    bool find = false;
    for (auto &e: range["resolution"]) {
        if (e["w"].asInt() == param.width &&
            e["h"].asInt() == param.height) {
            find = true;
            break;
        }
    }
    if (!find) {
        emxloge("param resolution %dx%d invalid\n", param.width, param.height);
        return false;
    }
    if (!MediaConfigs::CheckRangeLegal("fps", (int) param.fps, range))
        return false;
    if (!MediaConfigs::CheckRangeLegal("bitRate", (int) param.bitRate, range))
        return false;
    if (!MediaConfigs::CheckItemLegal("rcMode", (int) param.rcMode, range))
        return false;
    if (!MediaConfigs::CheckRangeLegal("iFrameInv", (int) param.iFrameInv, range))
        return false;
    return true;
}
