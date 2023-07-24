/*
* @Author: xiong
* @Date: 2023/1/12
*/

#include "OnvifClient.hpp"
#include "OnvifInner.hpp"

using namespace Emx;

ErrCodeE OnvifClient::GetParam(Onvif::Param &param) {
    std::string rep;
    ErrCodeE e = MsgRpcReqSync::Request(EMX_ONVIF_MSG_ID,
                                        (uint32_t) OnvifModuleE::Onvif,
                                        (uint32_t) OnvifMethodE::GetParam,
                                        nullptr, 0,
                                        &(rep), 1000);
    if (e != ErrCodeE::Success) {
        emxloge("failed\n");
        return e;
    }
    param.Parse(rep.data(), (int) rep.size());
    return ErrCodeE::Success;
}

ErrCodeE OnvifClient::SetParam(const Onvif::Param &param) {
    std::string rep;
    std::string req;
    param.Serialization(req);
    ErrCodeE e = MsgRpcReqSync::Request(EMX_ONVIF_MSG_ID,
                                        (uint32_t) OnvifModuleE::Onvif,
                                        (uint32_t) OnvifMethodE::SetParam,
                                        req.data(), (int) req.size(),
                                        &(rep), 1000);
    if (e != ErrCodeE::Success) {
        emxloge("failed\n");
        return e;
    }
    return e;
}
