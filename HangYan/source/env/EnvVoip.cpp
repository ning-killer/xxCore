/*
* @Author: xiong
* @Date: 2022/12/7
*/

#include "EnvVoip.hpp"

using namespace Emx;

ErrCodeE EnvVoip::Load() {
    try {
        Json::Value json;
        ErrCodeE e = Get(json);
        if (e != ErrCodeE::Success) {
            emxlogc("param get failed\n");
            return e;
        }
        strncpy(appKey, json["appKey"].asCString(), sizeof(appKey));
        strncpy(appSecret, json["appSecret"].asCString(), sizeof(appSecret));
        bind = json["bind"].asBool();
        return ErrCodeE::Success;
    } catch (std::exception &e) {
        emxlogc("load param failed\n");
        return ErrCodeE::Failure;
    }
}

ErrCodeE EnvVoip::Save() {
    try {
        Json::Value json;
        json["appKey"] = appKey;
        json["appSecret"] = appSecret;
        json["bind"] = bind;
        ErrCodeE e = Set(json);
        if (e != ErrCodeE::Success) {
            emxlogc("param set failed\n");
        }
        return e;
    } catch (std::exception &e) {
        emxlogc("save failed\n");
        return ErrCodeE::Failure;
    }
}
