//
// Created by xiong on 2022/6/11.
//

#include "Mac.hpp"

using namespace Emx;

ErrCodeE Mac::Load() {
    Param param("mac");
    Json::Value json;
    if (param.Get(json) != ErrCodeE::Success) {
        emxloge("load mac failed\n");
        return ErrCodeE::Failure;
    }
    strncpy(m_mac, json["mac"].asCString(), sizeof(m_mac));
    return ErrCodeE::Success;
}

ErrCodeE Mac::Set(const char *mac) {
    if (strcmp(mac, m_mac) == 0)
        return ErrCodeE::Success;
    int size = (int) strlen(mac);
    if (size >= (int) sizeof(m_mac)) {
        emxloge("mac %s size %d > max size %d\n", mac, size, sizeof(m_mac));
        return ErrCodeE::IllegalParam;
    }
    strncpy(m_mac, mac, sizeof(m_mac));
    Param param("mac");
    Json::Value json;
    json["mac"] = m_mac;
    return param.Set(json);
}