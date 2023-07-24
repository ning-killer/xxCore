//
// Created by xiong on 2022/7/9.
//

#include "EnvWifi.hpp"

using namespace Emx;


ErrCodeE EnvWifi::Load() {
    try {
        Json::Value json;
        ErrCodeE e = Get(json);
        if (e != ErrCodeE::Success) {
            emxlogc("param get ovd wifi failed\n");
            return e;
        }
        valid = json["valid"].asBool();
        strncpy(ssid, json["ssid"].asCString(), sizeof(ssid));
        strncpy(psk, json["psk"].asCString(), sizeof(psk));
        return ErrCodeE::Success;
    } catch (std::exception &e) {
        emxlogc("load ovd wifi failed\n");
        return ErrCodeE::Failure;
    }
}

ErrCodeE EnvWifi::Save() {
    try {
        Json::Value json;
        json["valid"] = valid;
        json["ssid"] = ssid;
        json["psk"] = psk;
        ErrCodeE e = Set(json);
        if (e != ErrCodeE::Success) {
            emxlogc("param set ovd wifi failed\n");
        }
        return e;
    } catch (std::exception &e) {
        emxlogc("save ovd wifi failed\n");
        return ErrCodeE::Failure;
    }
}
