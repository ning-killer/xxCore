//
// Created by xiong on 2022/7/9.
//

#include "EnvSchedule.hpp"

using namespace Emx;

ErrCodeE EnvSchedule::Parse(const Json::Value &json) {
    try {
        onOff = json["onOff"].asBool();
        strncpy(startTime, json["startTime"].asCString(), sizeof(startTime));
        strncpy(endTime, json["endTime"].asCString(), sizeof(endTime));
        for (int i = 0; i < (int) json["repeat"].size(); i++)
            repeat[i] = json["repeat"][i].asBool();
        once = json["once"].asBool();
        strncpy(onceStartTime, json["onceStartTime"].asCString(), sizeof(onceStartTime));
        strncpy(onceEndTime, json["onceEndTime"].asCString(), sizeof(onceEndTime));
        manual = json["manual"].asBool();
        strncpy(manualTime, json["manualTime"].asCString(), sizeof(manualTime));
        return ErrCodeE::Success;
    } catch (std::exception &e) {
        emxlogc("parse failed\n");
        return ErrCodeE::Failure;
    }
}

ErrCodeE EnvSchedule::Serialize(Json::Value &json) {
    try {
        json["onOff"] = onOff;
        json["startTime"] = startTime;
        json["endTime"] = endTime;
        auto &jsonRepeat = json["repeat"];
        for (bool item : repeat)
            jsonRepeat.append(item);
        json["once"] = once;
        json["onceStartTime"] = onceStartTime;
        json["onceEndTime"] = onceEndTime;
        json["manual"] = manual;
        json["manualTime"] = manualTime;
        return ErrCodeE::Success;
    } catch (std::exception &e) {
        emxlogc("serialize failed\n");
        return ErrCodeE::Failure;
    }
}
