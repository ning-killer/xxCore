//
// Created by xiong on 2020/11/2.
//
#include "json/json.h"
#include "core/EmxLog.hpp"
#include "core/EmxMsg.hpp"
#include "media/MediaClientVenc.hpp"
#include "media/MediaClientParam.hpp"
#include "MediaInner.hpp"

using namespace Emx;

ErrCodeE MediaClientVenc::FlushIDR() {
    MsgRpcReqSync req;
    std::string rep;
    int32_t chn = m_chn;
    char data[sizeof(chn)];
    memcpy(data, &chn, sizeof(chn));
    ErrCodeE e = req.Request(MEDIA_SERVER_MSG_ID,
                             (uint32_t) MediaModE::Venc,
                             (uint32_t) MediaVencMethodE::FlushIdr,
                             data, sizeof(data), &(rep), 1000);

    if (e != ErrCodeE::Success) {
        emxloge("failed\n");
        return e;
    }
    return e;
}

ErrCodeE MediaClientVenc::SetParam(const MediaVenc::Param &param) {
    MediaClientParam p(m_chn, (uint32_t) MediaModE::Venc);
    return p.SetParam(&param, sizeof(param));
}

ErrCodeE MediaClientVenc::GetParam(MediaVenc::Param &param) {
    MediaClientParam p(m_chn, (uint32_t) MediaModE::Venc);
    return p.GetParam(&param, sizeof(param));
}

ErrCodeE MediaClientVenc::GetParamRange(Json::Value &range) {
    MediaClientParam p(m_chn, (uint32_t) MediaModE::Venc);
    return p.GetParamRange(range);
}

ErrCodeE MediaClientVenc::GetChnNum(int &num) {
    MediaClientParam p(0, (uint32_t) MediaModE::Venc);
    return p.GetChnNum(num);
}