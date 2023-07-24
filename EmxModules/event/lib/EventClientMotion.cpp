/*
* @Author: xiong
* @Date: 2023/2/16
*/

#include "EventClientMotion.hpp"
#include "EventInner.hpp"

using namespace Emx;


ErrCodeE EventClientMotion::GetParam(std::shared_ptr<Param> &param) {
    std::string rep;
    std::string req;
    req.append((char *) &m_chn, sizeof(m_chn));
    ErrCodeE e = MsgRpcReqSync::Request(EVENT_RESP_ID,
                                        (uint32_t) EventModuleE::Motion,
                                        (uint32_t) EventMotionMethodE::MotionGet,
                                        req.data(), (int) req.size(),
                                        &(rep), 2000);
    if (e != ErrCodeE::Success) {
        emxloge("failed\n");
        return e;
    }
    int off = 0;
    memcpy(&param->ena, rep.data() + off, sizeof(param->ena));
    off += sizeof(param->ena);
    memcpy(&param->sensitivity, rep.data() + off, sizeof(param->sensitivity));
    off += sizeof(param->sensitivity);
    memcpy(param->zone.map, rep.data() + off, sizeof(param->zone.map));
    off += sizeof(param->zone.map);
    return e;
}

ErrCodeE EventClientMotion::SetParam(const std::shared_ptr<Param> &param) {
    std::string rep;
    std::string req;
    req.append((char *) &m_chn, sizeof(m_chn));
    req.append((char *) &param->ena, sizeof(param->ena));
    req.append((char *) &param->sensitivity, sizeof(param->sensitivity));
    req.append((char *) param->zone.map, sizeof(param->zone.map));
    ErrCodeE e = MsgRpcReqSync::Request(EVENT_RESP_ID,
                                        (uint32_t) EventModuleE::Motion,
                                        (uint32_t) EventMotionMethodE::MotionSet,
                                        req.data(), (int) req.size(),
                                        &(rep), 2000);
    if (e != ErrCodeE::Success) {
        emxloge("failed\n");
        return e;
    }
    return e;
}