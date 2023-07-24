//
// Created by xiong on 2022/7/9.
//

#include "EnvModelId.hpp"

using namespace Emx;

ErrCodeE EnvModelId::Load() {
    try {
        Json::Value json;
        ErrCodeE e = Get(json);
        if (e != ErrCodeE::Success) {
            emxlogc("param get failed\n");
            return e;
        }
        strncpy(id, json["modelId"].asCString(), sizeof(id));
        return ErrCodeE::Success;
    } catch (std::exception &e) {
        emxlogc("failed\n");
        return ErrCodeE::Failure;
    }
}
