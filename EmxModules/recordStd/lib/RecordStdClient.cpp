/*
* @Author: xiong
* @Date: 2023/2/7
*/

#include "RecordStdClient.hpp"
#include "RecordStdInner.hpp"

using namespace Emx;

ErrCodeE RecordStdClient::GetParam(std::shared_ptr<Param> &param) {
    std::string resp;
    ErrCodeE e = MsgRpcReqSync::Request(RECORD_STD_MSG_ID,
                                        (uint32_t) RecordStdModuleE::RecordStd,
                                        (uint32_t) RecordStdMethodE::GetParam,
                                        nullptr, 0, &resp);
    if (e != ErrCodeE::Success) {
        emxloge("failed:%d\n", e);
    } else {
        param->Parse(resp.data(), (int) resp.size());
    }
    return e;
}

ErrCodeE RecordStdClient::SetParam(const std::shared_ptr<Param> &param) {
    std::string resp;
    std::string req;
    param->Serialize(req);
    ErrCodeE e = MsgRpcReqSync::Request(RECORD_STD_MSG_ID,
                                        (uint32_t) RecordStdModuleE::RecordStd,
                                        (uint32_t) RecordStdMethodE::SetParam,
                                        req.data(), (int) req.size(), &resp);
    if (e != ErrCodeE::Success) {
        emxloge("failed:%d\n", e);
    }
    return e;
}

ErrCodeE RecordStdClient::GetManualRecordStat(bool &ena) {
    std::string resp;
    ErrCodeE e = MsgRpcReqSync::Request(RECORD_STD_MSG_ID,
                                        (uint32_t) RecordStdModuleE::RecordStdRecord,
                                        (uint32_t) RecordStdRecordMethodE::GetManualRecord,
                                        nullptr, 0, &resp);
    if (e != ErrCodeE::Success) {
        emxloge("failed:%d\n", e);
    } else {
        memcpy(&ena, resp.data(), sizeof(ena));
    }
    return e;
}

ErrCodeE RecordStdClient::SetManualRecordStat(bool ena) {
    std::string resp;
    std::string req;
    req.append((char *) &ena, sizeof(ena));
    ErrCodeE e = MsgRpcReqSync::Request(RECORD_STD_MSG_ID,
                                        (uint32_t) RecordStdModuleE::RecordStdRecord,
                                        (uint32_t) RecordStdRecordMethodE::SetManualRecord,
                                        req.data(), (int) req.size(), &resp);
    if (e != ErrCodeE::Success) {
        emxloge("failed:%d\n", e);
    }
    return e;
}

ErrCodeE RecordStdClient::GetRecordList(uint32_t startUtc, uint32_t endUtc, bool merge, std::vector<RecFile> &list) {
    std::string resp;
    std::string req;
    req.append((char *) &startUtc, sizeof(startUtc));
    req.append((char *) &endUtc, sizeof(endUtc));
    req.append((char *) &merge, sizeof(merge));
    ErrCodeE e = MsgRpcReqSync::Request(RECORD_STD_MSG_ID,
                                        (uint32_t) RecordStdModuleE::RecordStdRecord,
                                        (uint32_t) RecordStdRecordMethodE::GetManualRecord,
                                        req.data(), (int) req.size(), &resp);
    if (e != ErrCodeE::Success) {
        emxloge("failed:%d\n", e);
    } else {
        int off = 0;
        int num;
        memcpy(&num, resp.data() + off, sizeof(num));
        off += sizeof(num);
        list.clear();
        for (int i = 0; i < num; i++) {
            RecFile file = {};
            memcpy(&file, resp.data() + off, sizeof(file));
            off += sizeof(file);
            list.emplace_back(file);
        }
    }
    return e;
}

ErrCodeE RecordStdClient::ManualSnap() {
    return MsgRpcReqSync::Request(RECORD_STD_MSG_ID,
                                  (uint32_t) RecordStdModuleE::RecordStdSnap,
                                  (uint32_t) RecordStdSnapMethodE::TriggerManualSnap,
                                  nullptr, 0, nullptr);
}

ErrCodeE RecordStdClient::GetSnapList(uint32_t startUtc, uint32_t endUtc, std::vector<RecFile> &list) {
    std::string resp;
    std::string req;
    req.append((char *) &startUtc, sizeof(startUtc));
    req.append((char *) &endUtc, sizeof(endUtc));
    ErrCodeE e = MsgRpcReqSync::Request(RECORD_STD_MSG_ID,
                                        (uint32_t) RecordStdModuleE::RecordStdSnap,
                                        (uint32_t) RecordStdSnapMethodE::GetSnapList,
                                        req.data(), (int) req.size(), &resp);
    if (e != ErrCodeE::Success) {
        emxloge("failed:%d\n", e);
    } else {
        int off = 0;
        int num;
        memcpy(&num, resp.data() + off, sizeof(num));
        off += sizeof(num);
        list.clear();
        for (int i = 0; i < num; i++) {
            RecFile file = {};
            memcpy(&file, resp.data() + off, sizeof(file));
            off += sizeof(file);
            list.emplace_back(file);
        }
    }
    return e;
}

