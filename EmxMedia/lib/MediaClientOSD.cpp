/*
* @Author: xiong
* @Date: 2022/10/11
*/
#include "EmxCore.hpp"
#include "media/MediaClientOSD.hpp"
#include "media/MediaClientParam.hpp"
#include "MediaInner.hpp"

using namespace Emx;

ErrCodeE MediaClientOSD::SetParam(const std::vector<std::shared_ptr<Param>> &param) {
    std::string rep;
    int32_t chn = m_chn;
    char data[sizeof(chn) + sizeof(Param) * param.size()];
    memcpy(data, &chn, sizeof(chn));
    int off = sizeof(chn);
    for (const auto &p : param) {
        memcpy(data + off, p.get(), sizeof(Param));
        off += sizeof(Param);
    }
    ErrCodeE e = MsgRpcReqSync::Request(MEDIA_SERVER_MSG_ID,
                                        (uint32_t) MediaModE::Osd,
                                        (uint32_t) MediaParamMethodE::SetParam,
                                        data, (int) sizeof(data),
                                        &(rep));
    if (e != ErrCodeE::Success) {
        emxloge("failed\n");
        return e;
    }
    return e;
}

ErrCodeE MediaClientOSD::GetParam(std::vector<std::shared_ptr<Param>> &param) {
    std::string rep;
    int32_t chn = m_chn;
    char dataReq[sizeof(chn)];
    memcpy(dataReq, &chn, sizeof(chn));
    ErrCodeE e = MsgRpcReqSync::Request(MEDIA_SERVER_MSG_ID,
                                        (uint32_t) MediaModE::Osd,
                                        (uint32_t) MediaParamMethodE::GetParam,
                                        dataReq, (int) sizeof(dataReq),
                                        &(rep));
    if (e != ErrCodeE::Success) {
        emxloge("failed\n");
        return e;
    }
    param.clear();
    param.reserve(rep.size() / (int) sizeof(Param));
    auto p = (Param *) rep.data();
    for (int i = 0; i < (int) rep.size() / (int) sizeof(Param); i++) {
        param.push_back(std::make_shared<Param>(*(p + i)));
    }
    return e;
}

ErrCodeE MediaClientOSD::GetParamRange(Json::Value &range) {
    MediaClientParam p(m_chn, (uint32_t) MediaModE::Osd);
    return p.GetParamRange(range);
}

ErrCodeE MediaClientOSD::GetChnNum(int &num) {
    MediaClientParam p(0, (uint32_t) MediaModE::Osd);
    return p.GetChnNum(num);
}
