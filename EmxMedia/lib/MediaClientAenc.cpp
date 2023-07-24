//
// Created by xiong on 2020/11/19.
//

#include "json/json.h"
#include "EmxCore.hpp"
#include "media/MediaClientAenc.hpp"
#include "media/MediaClientParam.hpp"
#include "MediaInner.hpp"

using namespace Emx;

ErrCodeE MediaClientAenc::SetParam(const MediaAenc::Param &param) {
    MediaClientParam p(m_chn, (uint32_t) MediaModE::Aenc);
    return p.SetParam(&param, sizeof(param));
}

ErrCodeE MediaClientAenc::GetParam(MediaAenc::Param &param) {
    MediaClientParam p(m_chn, (uint32_t) MediaModE::Aenc);
    return p.GetParam(&param, sizeof(param));
}

ErrCodeE MediaClientAenc::GetParamRange(Json::Value &range) {
    MediaClientParam p(m_chn, (uint32_t) MediaModE::Aenc);
    return p.GetParamRange(range);
}

ErrCodeE MediaClientAenc::GetChnNum(int &num) {
    MediaClientParam p(0, (uint32_t) MediaModE::Aenc);
    return p.GetChnNum(num);
}

ErrCodeE MediaClientAenc::SetVolume(int32_t volume) {
    std::string rep;
    int32_t chn = m_chn;
    char data[sizeof(chn) + sizeof(volume)];
    memcpy(data, &chn, sizeof(chn));
    memcpy(data + sizeof(chn), &volume, sizeof(volume));
    ErrCodeE e = MsgRpcReqSync::Request(MEDIA_SERVER_MSG_ID,
                                        (uint32_t) MediaModE::Aenc,
                                        (uint32_t) MediaAencMethodE::SetVolume,
                                        data, sizeof(data), &(rep));
    if (e != ErrCodeE::Success) {
        emxloge("failed\n");
        return e;
    }
    return e;
}

ErrCodeE MediaClientAenc::Mute(bool ena) {
    std::string rep;
    int32_t chn = m_chn;
    char data[sizeof(chn) + sizeof(ena)];
    memcpy(data, &chn, sizeof(chn));
    memcpy(data + sizeof(chn), &ena, sizeof(ena));
    ErrCodeE e = MsgRpcReqSync::Request(MEDIA_SERVER_MSG_ID,
                                        (uint32_t) MediaModE::Aenc,
                                        (uint32_t) MediaAencMethodE::Mute,
                                        data, sizeof(data), &(rep));
    if (e != ErrCodeE::Success) {
        emxloge("failed\n");
        return e;
    }
    return e;
}
