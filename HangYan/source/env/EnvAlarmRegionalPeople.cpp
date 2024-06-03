#include "EnvAlarmRegionalPeople.hpp"

using namespace Emx;

ErrCodeE EnvAlarmRegionalPeople::Load() {
    Json::Value json;
    ErrCodeE e = Get(json);
    if (e != ErrCodeE::Success) {
        emxlogc("param get failed\n");
        return e;
    }
    gatReportOn = json["gatReportOn"].asBool();
    on = json["on"].asBool();
    regionalId = json["regionalId"].asInt();
    regionalName = json["regionalName"].asString();
    regionalPeopleCount = json["regionalPeopleCount"].asInt();
    alarmReportDuration = json["alarmReportDuration"].asInt();
    gatReportDuration = json["gatReportDuration"].asInt();
    osdStatus = json["osdStatus"].asBool();
    if (zone.SetOvdZoneEx(json["ovdZone"]) == ErrCodeE::Success)
        zone.ovdZone = json["ovdZone"];
    schedule.Parse(json["schedule"]);
    strategy.Parse(json["strategy"]);
    strategy.type = SoundAlarmControl::Type::RegionalPeople;
    return ErrCodeE::Success;
}

ErrCodeE EnvAlarmRegionalPeople::Save() {
    try {
        Json::Value json;
        json["gatReportOn"] = gatReportOn;
        json["on"] = on;
        json["regionalId"] = regionalId;
        json["regionalName"] = regionalName;
        json["regionalPeopleCount"] = regionalPeopleCount;
        json["alarmReportDuration"] = alarmReportDuration;
        json["gatReportDuration"] = gatReportDuration;
        json["osdStatus"] = osdStatus;
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
