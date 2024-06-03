#include "EnvAlarmLicensePlate.hpp"

using namespace Emx;

ErrCodeE EnvAlarmLicensePlate::Load() {
    Json::Value json;
    ErrCodeE e = Get(json);
    if (e != ErrCodeE::Success) {
        emxlogc("param get failed\n");
        return e;
    }
    on = json["on"].asBool();
    sensitivity = json["alarm_sensitivity"].asInt();
    quality = json["quality"].asInt();
    mode = (CaptureMode)json["capture_mode"].asInt();
    scheduledCaptureTime = json["scheduled_capture_time"].asInt();
    detectSite = json["detect_site"].asInt();
    exitMode = json["exit_mode"].asInt();
    if (zone.SetOvdZoneEx(json["ovdZone"]) == ErrCodeE::Success) {
        zone.ovdZone = json["ovdZone"];
    }
    strategy.Parse(json["strategy"]);
    strategy.type = SoundAlarmControl::Type::VEHICLE_GEN;
    return ErrCodeE::Success;
}

ErrCodeE EnvAlarmLicensePlate::Save() {
    try {
        Json::Value json;
        json["on"] = on;
        json["alarm_sensitivity"] = sensitivity;
        json["quality"] = quality;
        json["capture_mode"] = mode;
        json["scheduled_capture_time"] = scheduledCaptureTime;
        json["detect_site"] = detectSite;
        json["exit_mode"] = exitMode;
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
