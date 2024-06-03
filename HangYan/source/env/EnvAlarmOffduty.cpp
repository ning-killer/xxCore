#include "EnvAlarmOffduty.hpp"

using namespace Emx;

ErrCodeE EnvAlarmOffduty::Load() {
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
    onDutyCount = json["onDutyCount"].asInt();
    offdutyDuration = json["offdutyDuration"].asInt();
    if (zone.SetOvdZoneEx(json["ovdZone"]) == ErrCodeE::Success)
        zone.ovdZone = json["ovdZone"];
    schedule.Parse(json["schedule"]);
    return ErrCodeE::Success;
}

ErrCodeE EnvAlarmOffduty::Save() {
    try {
        Json::Value json;
        json["gatReportOn"] = gatReportOn;
        json["on"] = on;
        json["regionalId"] = regionalId;
        json["regionalName"] = regionalName;
        json["onDutyCount"] = onDutyCount;
        json["offdutyDuration"] = offdutyDuration;
        json["ovdZone"] = zone.ovdZone;
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
