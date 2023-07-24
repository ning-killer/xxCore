/*
* @Author: xiong
* @Date: 2023/1/12
*/

#include "EmxOnvifService.hpp"
#include "OnvifInner.hpp"
#include "OnvifService.h"
#include "RtspClient.hpp"

using namespace Emx;

void EmxOnvifService::Create(bool newThread) {
    m_loop.Init("OnvifService", [this]() {
        m_resp.DestroyMsgRpcRep();
        if (m_onvif) {
            m_onvif.reset();
        }
    }, newThread ? EuvLoop::ModeE::Dynamic : EuvLoop::Default);
    MsgRpcRep::Config cfg = {};
    cfg.loop = &m_loop;
    cfg.clientId = EMX_ONVIF_MSG_ID;
    cfg.buffer = m_buffer;
    cfg.size = sizeof(m_buffer);
    cfg.OnConnected = [this]() {
        m_onvif = std::make_shared<OnvifService>(this);
        m_onvif->start();
    };
    cfg.OnDisConnected = []() {};
    cfg.OnRecvReq = std::bind(&EmxOnvifService::OnRecvMsg, this, ph_1, ph_2, ph_3, ph_4, ph_5);
    m_resp.CreateMsgRpcRep(cfg);

    emxlogi("OnvifService Created\n");
    m_loop.Start(newThread);
}

void EmxOnvifService::Destroy() {
    m_loop.StopAndDeInit();
}

ErrCodeE EmxOnvifService::OnRecvMsg(uint32_t module, uint32_t method,
                                    const char *data, int32_t size, std::string &out) {
    switch ((OnvifModuleE) module) {
        case OnvifModuleE::Onvif:
            switch ((OnvifMethodE) method) {
                case OnvifMethodE::GetParam: {
                    out.clear();
                    Param param;
                    if (GetParam(param) != ErrCodeE::Success) {
                        return ErrCodeE::Failure;
                    }
                    param.Serialization(out);
                    return ErrCodeE::Success;
                }
                case OnvifMethodE::SetParam: {
                    Param param;
                    param.Parse(data, size);
                    return SetParam(param);
                }
                default:
                    emxloge("unknown method 0x%x\n", method);
                    return ErrCodeE::OperationNotSupport;
            }
        default:
            emxloge("unknown module 0x%x\n", module);
            return ErrCodeE::OperationNotSupport;
    }
}

ErrCodeE EmxOnvifService::LoadParam(Onvif::Param &param) {
    ::Param onvifParam("onvif");
    Json::Value json;
    if (onvifParam.Get(json) != ErrCodeE::Success) {
        emxloge("load param failed\n");
        return ErrCodeE::Failure;
    }
    param.port = json["port"].asInt();
    for (auto &userJson : json["user"]) {
        User user;
        user.level = (UserLevel) userJson["level"].asInt();
        user.name = userJson["name"].asString();
        user.password = userJson["password"].asString();
        param.userArray.emplace_back(user);
    }
    return ErrCodeE::Success;
}

ErrCodeE EmxOnvifService::SaveParam(const Onvif::Param &param) {
    ::Param onvif("onvif");
    Json::Value json;
    json["port"] = param.port;
    auto &userArray = json["user"];
    userArray.resize(0);
    for (auto &user : param.userArray) {
        Json::Value userJson;
        userJson["level"] = (int) user.level;
        userJson["name"] = user.name;
        userJson["password"] = user.password;
        userArray.append(userJson);
    }
    if (onvif.Set(json) != ErrCodeE::Success) {
        emxloge("save param failed\n");
        return ErrCodeE::Failure;
    }
    return ErrCodeE::Success;
}

ErrCodeE EmxOnvifService::GetParam(Param &param) {
    return LoadParam(param);
}

ErrCodeE EmxOnvifService::SetParam(const Param &param) {
    Param localParam = {};
    if (GetParam(localParam) != ErrCodeE::Success) {
        return ErrCodeE::Failure;
    }
    if (!ParamChanged(localParam, param)) {
        return ErrCodeE::Success;
    }
    if (UserChanged(localParam, param)) {
        Rtsp::Param rtspParam = {};
        RtspClient rtsp;
        if (rtsp.GetParam(rtspParam) == ErrCodeE::Success) {
            rtspParam.userArray.clear();
            for (auto &onvifUser:param.userArray) {
                Rtsp::User rtspUser = {};
                rtspUser.name = onvifUser.name;
                rtspUser.password = onvifUser.password;
                rtspParam.userArray.emplace_back(rtspUser);
            }
            rtsp.SetParam(rtspParam);
        }
    }
    m_onvif.reset();
    m_onvif = std::make_shared<OnvifService>(this);
    m_onvif->start();
    return SaveParam(param);
}

