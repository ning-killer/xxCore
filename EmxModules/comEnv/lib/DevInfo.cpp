//
// Created by xiong on 2022/6/10.
//

#include "DevInfo.hpp"

using namespace Emx;

ErrCodeE DevInfo::Load() {
    char path[EMX_MAX_PATH_SIZE] = {};
    snprintf(path, sizeof(path), "%s/devInfo.json", Misc::GetAppConfigsDir());
    Json::Value json;
    if (EasyJson::Load(path, json) != ErrCodeE::Success) {
        emxloge("load %s failed\n", path);
        return ErrCodeE::Failure;
    }
    strncpy(m_inner.type, json["inner"]["type"].asCString(), sizeof(m_inner.type));
    strncpy(m_inner.version, json["inner"]["version"].asCString(), sizeof(m_inner.version));
    strncpy(m_outer.type, json["outer"]["type"].asCString(), sizeof(m_outer.type));
    strncpy(m_outer.version, json["outer"]["version"].asCString(), sizeof(m_outer.version));
    strncpy(m_hardware.type, json["hardware"]["type"].asCString(), sizeof(m_hardware.type));
    strncpy(m_hardware.version, json["hardware"]["version"].asCString(), sizeof(m_hardware.version));
    return ErrCodeE::Success;
}
