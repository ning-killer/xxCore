//
// Created by xiong on 2022/7/9.
//

#include "EnvAlarmFace.hpp"

using namespace Emx;

ErrCodeE EnvAlarmFace::Load() {
    Json::Value json;
    ErrCodeE e = Get(json);
    if (e != ErrCodeE::Success) {
        emxlogc("param get failed\n");
        return e;
    }
    ena = json["ena"].asBool();
    sensitivity = json["sensitivity"].asInt();
    exposure_on = json["exposure_on"].asBool();
    exposure_adjust = json["exposure_adjust"].asInt();
    mode = (SnapMode)json["mode"].asInt();
    quality = json["quality"].asInt();
    if (zone.SetOvdZoneEx(json["ovdZone"]) == ErrCodeE::Success) {
        zone.ovdZone = json["ovdZone"];
    }
    auto &sensitivityMap = json["sensitivity_map"];
    for (int i = 0; i < (int) sensitivityMap.size(); i++) {
        sensitivity_map[i] = sensitivityMap[i].asFloat();
    }
    auto &qualityMap = json["quality_map"];
    for (int i = 0; i < (int) qualityMap.size(); i++) {
        quality_map[i] = qualityMap[i].asInt();
    }

    // mask_detection
    mask_detection = json["mask_detection"]["on"].asBool();
    schedule.Parse(json["mask_detection"]["schedule"]);
    strategy.Parse(json["mask_detection"]["strategy"]);
    strategy.type = SoundAlarmControl::Type::FACE_MASK;
    return ErrCodeE::Success;
}

ErrCodeE EnvAlarmFace::Save() {
    try {
        Json::Value json;
        json["ena"] = ena;
        json["sensitivity"] = sensitivity;
        json["exposure_on"] = exposure_on;
        json["exposure_adjust"] = exposure_adjust;
        json["mode"] = mode;
        json["quality"] = quality;
        json["ovdZone"] = zone.ovdZone;
        auto &sensitivityMap = json["sensitivity_map"];
        for (auto m : sensitivity_map) {
            sensitivityMap.append(m);
        }
        auto &qualityMap = json["quality_map"];
        for (auto m : quality_map) {
            qualityMap.append(m);
        }

        // mask_detection
        json["mask_detection"]["on"] = mask_detection;
        schedule.Serialize(json["mask_detection"]["schedule"]);
        strategy.Serialize(json["mask_detection"]["strategy"]);
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