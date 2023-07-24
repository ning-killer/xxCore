/*
* @Author: xiong
* @Date: 2023/2/16
*/

#include "EventClientGlobal.hpp"
#include "EventInner.hpp"

using namespace Emx;

ErrCodeE EventClientGlobal::GetSupportedEventChannel(std::vector<Event::Channel> &channels) {
    std::string rep;
    ErrCodeE e = MsgRpcReqSync::Request(EVENT_RESP_ID,
                                        (uint32_t) EventModuleE::Global,
                                        (uint32_t) EventGlobalMethodE::ListGet,
                                        nullptr, 0,
                                        &(rep), 2000);
    if (e != ErrCodeE::Success) {
        emxloge("failed\n");
        return e;
    }
    int off = 0;
    int num;
    memcpy(&num, rep.data() + off, sizeof(num));
    off += sizeof(num);
    for (int i = 0; i < num; i++) {
        Event::Channel c = {};
        memcpy(&c, rep.data() + off, sizeof(c));
        channels.emplace_back(c);
        off += sizeof(c);
    }
    return e;
}

ErrCodeE EventClientGlobal::GetParam(std::shared_ptr<Param> &param) {
    std::string rep;
    ErrCodeE e = MsgRpcReqSync::Request(EVENT_RESP_ID,
                                        (uint32_t) EventModuleE::Global,
                                        (uint32_t) EventGlobalMethodE::GlobalGet,
                                        nullptr, 0,
                                        &(rep), 2000);
    if (e != ErrCodeE::Success) {
        emxloge("failed\n");
        return e;
    }
    int off = 0;
    memcpy(&param->ena, rep.data() + off, sizeof(param->ena));
    off += sizeof(param->ena);
    for (auto &sectionArray : param->schedule.section) {
        int num;
        memcpy(&num, rep.data() + off, sizeof(num));
        off += sizeof(num);
        for (int j = 0; j < num; j++) {
            Schedule::Param::Section s = {};
            memcpy(&s, rep.data() + off, sizeof(s));
            sectionArray.emplace_back(s);
            off += sizeof(s);
        }
    }
    return e;
}

ErrCodeE EventClientGlobal::SetParam(const std::shared_ptr<Param> &param) {
    std::string rep;
    std::string req;
    req.append((char *) &param->ena, sizeof(param->ena));
    for (auto &sectionArray : param->schedule.section) {
        int num = (int) sectionArray.size();
        req.append((char *) &num, sizeof(num));
        for (auto &s:sectionArray) {
            req.append((char *) &s, sizeof(s));
        }
    }
    ErrCodeE e = MsgRpcReqSync::Request(EVENT_RESP_ID,
                                        (uint32_t) EventModuleE::Global,
                                        (uint32_t) EventGlobalMethodE::GlobalSet,
                                        req.data(), (int) req.size(),
                                        &(rep), 2000);
    if (e != ErrCodeE::Success) {
        emxloge("failed\n");
        return e;
    }
    return e;
}
