/*
* @Author: xiong
* @Date: 2022/10/11
*/

#include "EnvAlarmInvasion.hpp"

using namespace Emx;

ErrCodeE EnvAlarmInvasion::Load() {
    Json::Value json;
    ErrCodeE e = Get(json);
    if (e != ErrCodeE::Success) {
        emxlogc("param get failed\n");
        return e;
    }
    ena = json["ena"].asBool();
    sensitivity = json["sensitivity"].asInt();
    if (zone.SetOvdZone(json["ovdZone"]) == ErrCodeE::Success)
        zone.ovdZone = json["ovdZone"];
    schedule.Parse(json["schedule"]);
    strategy.Parse(json["strategy"]);
    return ErrCodeE::Success;
}

ErrCodeE EnvAlarmInvasion::Save() {
    try {
        Json::Value json;
        json["ena"] = ena;
        json["sensitivity"] = sensitivity;
        json["ovdZone"] = zone.ovdZone;
        schedule.Serialize(json["schedule"]);
        strategy.Serialize(json["strategy"]);
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
