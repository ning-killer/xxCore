#include "EnvAlarmPassengerFlow.hpp"

using namespace Emx;

ErrCodeE EnvAlarmPassengerFlow::Load() {
    Json::Value json;
    ErrCodeE e = Get(json);
    if (e != ErrCodeE::Success) {
        emxlogc("param get failed\n");
        return e;
    }
    on = json["on"].asBool();
    exposureOn = json["exposureOn"].asBool();
    exposureAdjust = json["exposureAdjust"].asInt();
    mode = (SnapMode)json["mode"].asInt();
    qualityValue = json["qualityValue"].asInt();
    sensitivity = json["sensitivity"].asInt();
    statisticsMode = (DirectionRule)json["statistics_mode"].asInt();
    osdStatus = json["osdStatus"].asBool();    
    line.Parse(json["statistics_line"]);
    if (zone.SetOvdZoneEx(json["ovdZone"]) == ErrCodeE::Success) {
        zone.ovdZone = json["ovdZone"];
    }
    return ErrCodeE::Success;
}

ErrCodeE EnvAlarmPassengerFlow::Save() {
    try {
        Json::Value json;
        json["on"] = on;
        json["exposureOn"] = exposureOn;
        json["exposureAdjust"] = exposureAdjust;
        json["mode"] = (int)mode;
        json["qualityValue"] = qualityValue;
        json["sensitivity"] = sensitivity;
        json["statistics_mode"] = (int)statisticsMode;
        json["osdStatus"] = osdStatus;    
        line.Serialize(json["statistics_line"]);
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
