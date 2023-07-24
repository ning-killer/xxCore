//
// Created by xiong on 2022/3/4.
//
#include "core/EmxParam.hpp"
#include "core/EmxMsg.hpp"
#include "core/EmxUtils.hpp"
#include "ParamInner.hpp"

using namespace Emx;

Param::Param(const std::string &name) {
    strncpy(m_name, name.c_str(), sizeof(m_name));
}

Param::Param(const char *name) {
    strncpy(m_name, name, sizeof(m_name));
}

ErrCodeE Param::Set(Json::Value &json) {
    std::string jsonStr;
    jsonStr.append(m_name, strlen(m_name) + 1);
    EasyJson::Serialize(json, jsonStr);
    return MsgRpcReqSync::Request(PARAM_SERVER_MSG_ID,
                                  PARAM_SERVER_MOD,
                                  (uint32_t) ParamServerMethodE::Set,
                                  jsonStr.data(), (int) jsonStr.size() + 1);
}

ErrCodeE Param::Get(Json::Value &json) {
    std::string jsonStr;
    auto e = MsgRpcReqSync::Request(PARAM_SERVER_MSG_ID,
                                    PARAM_SERVER_MOD,
                                    (uint32_t) ParamServerMethodE::Get,
                                    m_name, (int) strlen(m_name) + 1,
                                    &jsonStr);
    if (e == ErrCodeE::Success)
        return EasyJson::Parse(jsonStr.data(), json);
    else
        return e;
}

ErrCodeE Param::GetDefault(Json::Value &json) {
    std::string jsonStr;
    auto e = MsgRpcReqSync::Request(PARAM_SERVER_MSG_ID,
                                    PARAM_SERVER_MOD,
                                    (uint32_t) ParamServerMethodE::GetDefault,
                                    m_name, (int) strlen(m_name) + 1,
                                    &jsonStr);
    if (e == ErrCodeE::Success)
        return EasyJson::Parse(jsonStr.data(), json);
    else
        return e;
}

ErrCodeE Param::Reset() {
    return MsgRpcReqSync::Request(PARAM_SERVER_MSG_ID,
                                  PARAM_SERVER_MOD,
                                  (uint32_t) ParamServerMethodE::Reset,
                                  m_name, (int) strlen(m_name) + 1);
}

ErrCodeE Param::ResetAllToFactory() {
    return MsgRpcReqSync::Request(PARAM_SERVER_MSG_ID,
                                  PARAM_SERVER_MOD,
                                  (uint32_t) ParamServerMethodE::ResetToFactory);
}
