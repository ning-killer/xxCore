//
// Created by xiong on 2020/11/20.
//


#include "json/json.h"
#include "core/EmxLog.hpp"
#include "core/EmxMsg.hpp"
#include "media/MediaClientParam.hpp"
#include "MediaInner.hpp"

using namespace Emx;

ErrCodeE MediaClientParam::SetParam(const void *param, int32_t size, int32_t timeoutMs) {
    std::string rep;
    int32_t chn = m_chn;
    char data[sizeof(chn) + size];
    memcpy(data, &chn, sizeof(chn));
    memcpy(data + sizeof(chn), param, size);
    ErrCodeE e = MsgRpcReqSync::Request(MEDIA_SERVER_MSG_ID,
                                        m_moduleId,
                                        (uint32_t) MediaParamMethodE::SetParam,
                                        data, (int) sizeof(data),
                                        &(rep), timeoutMs);
    if (e != ErrCodeE::Success) {
        emxloge("failed\n");
        return e;
    }
    return e;
}

ErrCodeE MediaClientParam::GetParam(void *param, int32_t size, int32_t timeoutMs) {
    std::string rep;
    int32_t chn = m_chn;
    char dataReq[sizeof(chn)];
    memcpy(dataReq, &chn, sizeof(chn));
    ErrCodeE e = MsgRpcReqSync::Request(MEDIA_SERVER_MSG_ID,
                                        m_moduleId,
                                        (uint32_t) MediaParamMethodE::GetParam,
                                        dataReq, (int) sizeof(dataReq),
                                        &(rep), timeoutMs);
    if (e != ErrCodeE::Success) {
        emxloge("failed\n");
        return e;
    }
    memcpy(param, rep.data(), size);
    return e;
}

ErrCodeE MediaClientParam::GetParamRange(Json::Value &range, int32_t timeoutMs) {
    std::string rep;
    int32_t chn = m_chn;
    char dataReq[sizeof(chn)];
    memcpy(dataReq, &chn, sizeof(chn));
    ErrCodeE e = MsgRpcReqSync::Request(MEDIA_SERVER_MSG_ID,
                                        m_moduleId,
                                        (uint32_t) MediaParamMethodE::GetParamRange,
                                        dataReq, sizeof(dataReq),
                                        &(rep), timeoutMs);
    if (e != ErrCodeE::Success) {
        emxloge("failed\n");
        return e;
    }
    JSONCPP_STRING errs;
    Json::CharReaderBuilder builder;
    std::unique_ptr<Json::CharReader> const reader(builder.newCharReader());
    if (!reader->parse(rep.data(), rep.data() + rep.size(), &range, &errs)) {
        emxloge("cannot parse : %s\n", rep.data(),
               errs.empty() ? "unknown" : errs.data());
        return ErrCodeE::ParseFailed;
    }
    return e;
}

ErrCodeE MediaClientParam::GetChnNum(int &num, int32_t timeoutMs) {
    std::string rep;
    int32_t chn = 0;
    char dataReq[sizeof(chn)];
    memcpy(dataReq, &chn, sizeof(chn));
    ErrCodeE e = MsgRpcReqSync::Request(MEDIA_SERVER_MSG_ID,
                                        m_moduleId,
                                        (uint32_t) MediaParamMethodE::GetChnNum,
                                        dataReq, (int) sizeof(dataReq),
                                        &(rep), timeoutMs);
    if (e != ErrCodeE::Success) {
        emxloge("failed\n");
        return e;
    }
    memcpy(&num, rep.data(), sizeof(num));
    return e;
}
