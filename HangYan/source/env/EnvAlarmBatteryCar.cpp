#include "EnvAlarmBatteryCar.hpp"

using namespace Emx;

ErrCodeE EnvAlarmBatteryCar::Load() {
    Json::Value json;
    ErrCodeE e = Get(json);
    if (e != ErrCodeE::Success) {
        emxlogc("param get failed\n");
        return e;
    }
    on = json["on"].asBool();
    detectModelist = json["detect_mode_list"].asString();
    sensitivity = json["alarm_sensitivity"].asInt();
    quality = json["quality"].asInt();
    mode = (CaptureMode)json["capture_mode"].asInt();
    scheduledCaptureTime = json["scheduled_capture_time"].asInt();
    if (zone.SetOvdZoneEx(json["ovdZone"]) == ErrCodeE::Success) {
        zone.ovdZone = json["ovdZone"];
    }
    strategy.Parse(json["strategy"]);
    strategy.type = SoundAlarmControl::Type::VEHICLE_MOTOR;
    return ErrCodeE::Success;
}

ErrCodeE EnvAlarmBatteryCar::Save() {
    try {
        Json::Value json;
        json["on"] = on;
        json["detect_mode_list"] = detectModelist;
        json["alarm_sensitivity"] = sensitivity;
        json["quality"] = quality;
        json["capture_mode"] = mode;
        json["scheduled_capture_time"] = scheduledCaptureTime;
        json["ovdZone"] = zone.ovdZone;
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
