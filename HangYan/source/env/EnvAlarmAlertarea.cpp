#include "EnvAlarmAlertarea.hpp"

using namespace Emx;

ErrCodeE EnvAlarmAlertarea::Load() {
    Json::Value json;
    ErrCodeE e = Get(json);
    if (e != ErrCodeE::Success) {
        emxlogc("param get failed\n");
        return e;
    }
    on = json["on"].asBool();
    sensitivity = json["sensitivity"].asInt();
    type = (TargetType)json["targetType"].asInt();
    staymodeTime = json["staymode_time"].asInt();
    if (zone.SetOvdZoneEx(json["ovdZone"]) == ErrCodeE::Success)
        zone.ovdZone = json["ovdZone"];
    schedule.Parse(json["schedule"]);
    strategy.Parse(json["strategy"]);
    strategy.type = SoundAlarmControl::Type::ALERTAREA;
    return ErrCodeE::Success;
}

ErrCodeE EnvAlarmAlertarea::Save() {
    try {
        Json::Value json;
        json["on"] = on;
        json["sensitivity"] = sensitivity;
        json["targetType"] = (TargetType)type;
        json["staymode_time"] = staymodeTime;
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
