//
// Created by xiong on 2021/7/10.
//
#include "EmxCore.hpp"
#include "MediaServerSnap.hpp"
#include "MediaConfigs.hpp"
#include "MediaFactory.hpp"
#include "MediaInner.hpp"

using namespace Emx;

std::vector<MediaServerSnap *> MediaServerSnap::m_array;

void MediaServerSnap::Create(MediaServerResource &res) {
    Json::Value json;
    res.configs.GetCfg("snap", json);
    for (int chn = 0; chn < (int) json.size(); chn++) {
        auto e = res.factory->MakeSnap(chn, res);
        m_array.push_back(e);
        e->Start();
    }
}

void MediaServerSnap::Destroy() {
    for (auto e : m_array) {
        e->Stop();
        delete e;
    }
    m_array.clear();
}

ErrCodeE MediaServerSnap::OnRecvReq(uint32_t module, uint32_t method,
                                    const char *data, int32_t size, std::string &out) {
    int32_t chn;
    memcpy(&chn, data, sizeof(chn));
    if (chn >= (int32_t) m_array.size()) {
        emxloge("chn %d not exists\n", chn);
        return ErrCodeE::ResNotExist;
    }
    data += sizeof(chn);
    size -= sizeof(chn);
    if ((MediaSnapMethodE) method == MediaSnapMethodE::Trigger) {
        return m_array[chn]->Trigger();
    } else {
        return m_array[chn]->OnRecvReqSub(module, method, data, size, out);
    }
}

ErrCodeE MediaServerSnap::OnRecvReqSub(uint32_t module, uint32_t method,
                                       const char *data, int32_t size, std::string &out) {
    emxloge("not support method[%u]\n", method);
    return ErrCodeE::OperationNotSupport;
}

