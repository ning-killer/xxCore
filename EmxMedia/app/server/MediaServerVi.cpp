/*
* @Author: xiong
* @Date: 2023/4/3
*/

#include "EmxCore.hpp"
#include "MediaServerVi.hpp"
#include "MediaConfigs.hpp"
#include "MediaFactory.hpp"
#include "MediaInner.hpp"

using namespace Emx;

std::vector<MediaServerVi *> MediaServerVi::m_array;

void MediaServerVi::Create(MediaServerResource &res) {
    Json::Value json;
    res.configs.GetCfg("vi", json);
    for (int chn = 0; chn < (int) json.size(); chn++) {
        auto e = res.factory->MakeVi(chn, res);
        m_array.push_back(e);
        e->Start();
    }
}

void MediaServerVi::Destroy() {
    for (auto e: m_array) {
        e->Stop();
        delete e;
    }
    m_array.clear();
}

ErrCodeE MediaServerVi::OnRecvReq(uint32_t module, uint32_t method,
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
            MediaVi::Param param = {};
            memcpy(&param, data, sizeof(param));
            return m_array[chn]->SetParam(param);
        }
        case (uint32_t) MediaParamMethodE::GetParam: {
            MediaVi::Param param = {};
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
        default:
            return m_array[chn]->OnRecvReqSub(module, method, data, size, out);
    }
}

ErrCodeE MediaServerVi::OnRecvReqSub(uint32_t module, uint32_t method,
                                     const char *data, int32_t size, std::string &out) {
    emxloge("not support method[%u]\n", method);
    return ErrCodeE::OperationNotSupport;
}

ErrCodeE MediaServerVi::LoadParam() {
    Json::Value root;
    ErrCodeE e = m_res.configs.GetParam("vi", root);
    if (e != ErrCodeE::Success) {
        emxlogc("Read param failed : %d\n", e);
        return e;
    }
    auto &json = root[m_chn];
    m_param.flip = json["flip"].asBool();
    m_param.mirror = json["mirror"].asBool();
    return ErrCodeE::Success;
}

ErrCodeE MediaServerVi::SaveParam() {
    Json::Value root;
    ErrCodeE e = m_res.configs.GetParam("vi", root);
    if (e != ErrCodeE::Success) {
        emxlogc("Read param failed : %d\n", e);
        return e;
    }
    auto &json = root[m_chn];
    json["flip"] = m_param.flip;
    json["mirror"] = m_param.mirror;
    return m_res.configs.SetParam("vi", root);
}

ErrCodeE MediaServerVi::GetParamRange(Json::Value &range) {
    return m_res.configs.GetRange("vi", range);
}

bool MediaServerVi::ParamValid(const MediaVi::Param &param) {
    return true;
}
