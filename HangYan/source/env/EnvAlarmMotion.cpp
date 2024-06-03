//
// Created by xiong on 2022/7/9.
//

#include "EnvAlarmMotion.hpp"

using namespace Emx;

ErrCodeE EnvAlarmMotion::Load() {
    Json::Value json;
    ErrCodeE e = Get(json);
    if (e != ErrCodeE::Success) {
        emxlogc("param get failed\n");
        return e;
    }
    ena = json["ena"].asBool();
    sensitivity = json["sensitivity"].asInt();
    if (zone.SetOvdZoneEx(json["ovdZone"]) == ErrCodeE::Success)
        zone.ovdZone = json["ovdZone"];
    auto &jsonMap = json["map"];
    for (int i = 0; i < (int) sizeof(map) / (int) sizeof(map[0]) && i < (int) jsonMap.size(); i++) {
        map[i] = jsonMap[i].asInt();
    }
    auto &jsonThreshold = json["threshold"];
    for (int i = 0; i < (int) sizeof(threshold) / (int) sizeof(threshold[0]) && i < (int) jsonThreshold.size(); i++) {
        threshold[i] = jsonThreshold[i].asInt();
    }
    return ErrCodeE::Success;
}

ErrCodeE EnvAlarmMotion::Save() {
    try {
        Json::Value json;
        json["ena"] = ena;
        json["sensitivity"] = sensitivity;
        json["ovdZone"] = zone.ovdZone;
        auto &jsonMap = json["map"];
        for (auto m : map) {
            jsonMap.append(m);
        }
        auto &jsonThreshold = json["threshold"];
        for (auto m : threshold) {
            jsonThreshold.append(m);
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