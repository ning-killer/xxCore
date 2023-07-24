//
// Created by xiong on 2022/7/9.
//

#include "EnvAlarmVoice.hpp"

using namespace Emx;

ErrCodeE EnvAlarmVoice::Load() {
    Json::Value json;
    ErrCodeE e = Get(json);
    if (e != ErrCodeE::Success) {
        emxlogc("param get failed\n");
        return e;
    }
    ena = json["ena"].asBool();
    sensitivity = json["sensitivity"].asInt();
    auto &jsonMap = json["map"];
    for (int i = 0; i < (int) sizeof(map) / (int) sizeof(map[0]) && i < (int) jsonMap.size(); i++) {
        map[i] = jsonMap[i].asInt();
    }
    return ErrCodeE::Success;
}

ErrCodeE EnvAlarmVoice::Save() {
    try {
        Json::Value json;
        json["ena"] = ena;
        json["sensitivity"] = sensitivity;
        auto &jsonMap = json["map"];
        for (auto m : map) {
            jsonMap.append(m);
        }
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