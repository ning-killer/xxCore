#include "EnvAlarmLaneLine.hpp"

using namespace Emx;

ErrCodeE EnvAlarmLaneLine::Load() {
    Json::Value json;
    ErrCodeE e = Get(json);
    if (e != ErrCodeE::Success) {
        emxlogc("param get failed\n");
        return e;
    }
    on = json["on"].asBool();
    laneLineNum = json["laneLineNum"].asInt();
    if (zone.SetOvdZoneEx(json["ovdZone"]) == ErrCodeE::Success) {
        zone.ovdZone = json["ovdZone"];
    }
    return ErrCodeE::Success;
}

ErrCodeE EnvAlarmLaneLine::Save() {
    try {
        Json::Value json;
        json["on"] = on;
        json["laneLineNum"] = laneLineNum;
        json["ovdZone"] = zone.ovdZone;
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