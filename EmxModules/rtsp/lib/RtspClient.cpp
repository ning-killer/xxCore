/*
* @Author: xiong
* @Date: 2023/1/16
*/

#include "RtspClient.hpp"
#include "RtspInner.hpp"

using namespace Emx;

ErrCodeE RtspClient::GetParam(Rtsp::Param &param) {
    std::string rep;
    ErrCodeE e = MsgRpcReqSync::Request(EMX_RTSP_MSG_ID,
                                        (uint32_t) RtspModuleE::Rtsp,
                                        (uint32_t) RtspMethodE::GetParam,
                                        nullptr, 0,
                                        &(rep), 1000);
    if (e != ErrCodeE::Success) {
        emxloge("failed\n");
        return e;
    }
    param.Parse(rep.data(), (int) rep.size());
    return ErrCodeE::Success;
}

ErrCodeE RtspClient::SetParam(const Rtsp::Param &param) {
    std::string rep;
    std::string req;
    param.Serialization(req);
    ErrCodeE e = MsgRpcReqSync::Request(EMX_RTSP_MSG_ID,
                                        (uint32_t) RtspModuleE::Rtsp,
                                        (uint32_t) RtspMethodE::SetParam,
                                        req.data(), (int) req.size(),
                                        &(rep), 1000);
    if (e != ErrCodeE::Success) {
        emxloge("failed\n");
        return e;
    }
    return e;
}
