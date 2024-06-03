#include "EnvAlarmTransgression.hpp"

using namespace Emx;

ErrCodeE EnvAlarmTransgression::Load() {
    Json::Value json;
    ErrCodeE e = Get(json);
    if (e != ErrCodeE::Success) {
        emxlogc("param get failed\n");
        return e;
    }
    on = json["on"].asBool();
    type = (TargetType)json["targetType"].asInt();
    statisticsModeStyle = (DirectionControl)json["statistics_mode_style"].asInt();
    statisticsModeDirection = (Direction)json["statistics_mode_direction"].asInt();
    schedule.Parse(json["schedule"]);
    strategy.Parse(json["strategy"]);
    line.Parse(json["statistics_line"]);
    strategy.type = SoundAlarmControl::Type::TRANSGRESSION;
    return ErrCodeE::Success;
}

ErrCodeE EnvAlarmTransgression::Save() {
    try {
        Json::Value json;
        json["on"] = on;
        json["targetType"] = type;
        json["statistics_mode_style"] = (int)statisticsModeStyle;
        json["statistics_mode_direction"] = (int)statisticsModeDirection;
        schedule.Serialize(json["schedule"]);
        strategy.Serialize(json["strategy"]);
        line.Serialize(json["statistics_line"]);
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
