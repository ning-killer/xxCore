//
// Created by xiong on 2022/6/10.
//

#include "Sn.hpp"

using namespace Emx;

ErrCodeE Sn::Load() {
    Param param("sn");
    Json::Value json;
    if (param.Get(json) != ErrCodeE::Success) {
        emxloge("load sn failed\n");
        return ErrCodeE::Failure;
    }
    strncpy(m_sn, json["sn"].asCString(), sizeof(m_sn));
    return ErrCodeE::Success;
}

ErrCodeE Sn::Set(const char *sn) {
    if (strcmp(sn, m_sn) == 0)
        return ErrCodeE::Success;
    int size = (int) strlen(sn);
    if (size >= (int) sizeof(m_sn)) {
        emxloge("sn %s size %d > max size %d\n", sn, size, sizeof(m_sn));
        return ErrCodeE::IllegalParam;
    }
    strncpy(m_sn, sn, sizeof(m_sn));
    Param param("sn");
    Json::Value json;
    json["sn"] = m_sn;
    return param.Set(json);
}
