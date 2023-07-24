/*
* @Author: xiong
* @Date: 2022/10/11
*/

#include "EnvSleep.hpp"

using namespace Emx;

ErrCodeE EnvSleep::Load() {
    Json::Value json;
    ErrCodeE e = Get(json);
    if (e != ErrCodeE::Success) {
        emxlogc("param get failed\n");
        return e;
    }
    return schedule.Parse(json["schedule"]);
}

ErrCodeE EnvSleep::Save() {
    try {
        Json::Value json;
        schedule.Serialize(json["schedule"]);
        ErrCodeE e = Set(json);
        if (e != ErrCodeE::Success) {
            emxlogc("param set failed\n");
        }
        return e;
    } catch (std::exception &e) {
        emxlogc("failed\n");
        return ErrCodeE::Failure;
    }
}
