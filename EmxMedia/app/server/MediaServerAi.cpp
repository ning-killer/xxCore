//
// Created by xiong on 2022/4/27.
//

#include "EmxCore.hpp"
#include "MediaServerAi.hpp"
#include "MediaConfigs.hpp"
#include "MediaFactory.hpp"
#include "MediaInner.hpp"

using namespace Emx;


std::vector<MediaServerAi *> MediaServerAi::m_array;

void MediaServerAi::Create(MediaServerResource &res) {
    Json::Value json;
    res.configs.GetCfg("ai", json);
    for (int chn = 0; chn < (int) json.size(); chn++) {
        auto e = res.factory->MakeAi(chn, res);
        m_array.push_back(e);
        e->Start();
    }
}

void MediaServerAi::Destroy() {
    for (auto e : m_array) {
        e->Stop();
        delete e;
    }
    m_array.clear();
}

ErrCodeE MediaServerAi::OnRecvReq(uint32_t module, uint32_t method,
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
            MediaAi::Param param = {};
            memcpy(&param, data, sizeof(param));
            return m_array[chn]->SetParam(param);
        }
        case (uint32_t) MediaParamMethodE::GetParam: {
            MediaAi::Param param = {};
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
        default:
            return m_array[chn]->OnRecvReqSub(module, method, data, size, out);
    }
}

ErrCodeE MediaServerAi::OnRecvReqSub(uint32_t module, uint32_t method,
                                     const char *data, int32_t size, std::string &out) {
    emxloge("not support method[%u]\n", method);
    return ErrCodeE::OperationNotSupport;
}

ErrCodeE MediaServerAi::LoadParam() {
    Json::Value root;
    ErrCodeE e = m_res.configs.GetParam("ai", root);
    if (e != ErrCodeE::Success) {
        emxlogc("Read param failed : %d\n", e);
        return e;
    }
    try {
        auto &json = root[m_chn];
        m_param.ena = json["ena"].asBool();

        auto &motionDetection = json["motionDetection"];
        m_param.motionDetection.ena = motionDetection["ena"].asBool();
        m_param.motionDetection.threshold = motionDetection["threshold"].asInt();

        auto &personDetection = json["personDetection"];
        m_param.personDetection.ena = personDetection["ena"].asBool();

        auto &classify = json["classify"];
        m_param.classify.ena = classify["ena"].asBool();
        m_param.classify.num = 0;
        memset(m_param.classify.type, 0, sizeof(m_param.classify.type));
        for (int i = 0; i < (int) classify["type"].size() && i < Param::Classify::MaxNum; i++) {
            m_param.classify.type[i] = (ClassifyTypeE) classify["type"][i].asInt();
            m_param.classify.num++;
        }

        auto &personRecognition = json["personRecognition"];
        m_param.personRecognition.ena = personRecognition["ena"].asBool();

        auto &faceRecognition = json["faceRecognition"];
        m_param.faceRecognition.ena = faceRecognition["ena"].asBool();

        auto &facePersonDetection = json["facePersonDetection"];
        m_param.facePersonDetection.ena = facePersonDetection["ena"].asBool();
        m_param.facePersonDetection.faceDraw = facePersonDetection["faceDraw"].asBool();
        m_param.facePersonDetection.personDraw = facePersonDetection["personDraw"].asBool();
        m_param.facePersonDetection.aeEna = facePersonDetection["aeEna"].asInt();
        m_param.facePersonDetection.aeValue = facePersonDetection["aeValue"].asInt();
        m_param.facePersonDetection.captureNum = facePersonDetection["captureNum"].asInt();
        m_param.facePersonDetection.captureMode = facePersonDetection["captureMode"].asInt();
        m_param.facePersonDetection.captureInterval = facePersonDetection["captureInterval"].asInt();
        m_param.facePersonDetection.captureQuality = facePersonDetection["captureQuality"].asFloat();
        m_param.facePersonDetection.captureSensitivity = facePersonDetection["captureSensitivity"].asFloat();

        return ErrCodeE::Success;
    } catch (std::exception &e) {
        emxloge("failed what[%s]\n", e.what());
        return ErrCodeE::ParseFailed;
    }
}

ErrCodeE MediaServerAi::SaveParam() {
    Json::Value root;
    ErrCodeE e = m_res.configs.GetParam("ai", root);
    if (e != ErrCodeE::Success) {
        emxlogc("Read param failed : %d\n", e);
        return e;
    }
    auto &json = root[m_chn];
    json["ena"] = m_param.ena;

    Json::Value motionDetection;
    motionDetection["ena"] = m_param.motionDetection.ena;
    motionDetection["threshold"] = m_param.motionDetection.threshold;
    json["motionDetection"] = motionDetection;

    Json::Value personDetection;
    personDetection["ena"] = m_param.personDetection.ena;
    json["personDetection"] = personDetection;

    Json::Value classify;
    classify["ena"] = m_param.classify.ena;
    Json::Value classifyType;
    classifyType.resize(0);
    for (int i = 0; i < m_param.classify.num; i++) {
        classifyType.append((int) m_param.classify.type[i]);
    }
    classify["type"] = classifyType;
    json["classify"] = classify;

    Json::Value personRecognition;
    personRecognition["ena"] = m_param.personRecognition.ena;
    json["personRecognition"] = personRecognition;

    Json::Value faceRecognition;
    faceRecognition["ena"] = m_param.faceRecognition.ena;
    json["faceRecognition"] = faceRecognition;

    Json::Value facePersonDetection;
    facePersonDetection["ena"] = m_param.facePersonDetection.ena;
    facePersonDetection["faceDraw"] = m_param.facePersonDetection.faceDraw;
    facePersonDetection["personDraw"] = m_param.facePersonDetection.personDraw;
    facePersonDetection["aeEna"] = m_param.facePersonDetection.aeEna;
    facePersonDetection["aeValue"] = m_param.facePersonDetection.aeValue;
    facePersonDetection["captureNum"] = m_param.facePersonDetection.captureNum;
    facePersonDetection["CaptureMode"] = m_param.facePersonDetection.captureMode;
    facePersonDetection["captureInterval"] = m_param.facePersonDetection.captureInterval;
    facePersonDetection["captureQuality"] = m_param.facePersonDetection.captureQuality;
    facePersonDetection["captureSensitivity"] = m_param.facePersonDetection.captureSensitivity;
    json["facePersonDetection"] = facePersonDetection;

    return m_res.configs.SetParam("ai", root);
}

ErrCodeE MediaServerAi::GetParamRange(Json::Value &range) {
    return m_res.configs.GetRange("ai", range);
}

bool MediaServerAi::ParamValid(const MediaAi::Param &param) {
    return true;
}
