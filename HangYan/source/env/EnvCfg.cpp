//
// Created by xiong on 2022/7/9.
//

#include "EnvCfg.hpp"

using namespace Emx;

ErrCodeE EnvCfg::Load() {
    try {
        Json::Value json;
        ErrCodeE e = Get(json);
        if (e != ErrCodeE::Success) {
            emxlogc("param get ovd cfg failed\n");
            return e;
        }
        trace = json["trace"].asBool();
//        ovd_sleep = json["ovd_sleep"].asBool();
        manual_sleep = json["manual_sleep"].asInt();
        led = json["led"].asBool();
        rsk_encrypt = json["rsk_encrypt"].asBool();
        net_mode = json["net_mode"].asInt();
        logSTD = json["logSTD"].asInt();
        logLevel = json["logLevel"].asInt();
        logContrl = json["logContrl"].asInt();
        callback = json["callback"].asInt();
        strncpy(log_path, json["log_path"].asCString(), sizeof(log_path));
        cnl_cnt = json["cnl_cnt"].asInt();
        video_quality = json["video_quality"].asInt();
        auto_reboot_on = json["auto_reboot_on"].asBool();
        auto_reboot_cycle = json["auto_reboot_cycle"].asInt64();
        strncpy(auto_reboot_start, json["auto_reboot_start"].asCString(), sizeof(auto_reboot_start));
        strncpy(auto_reboot_end, json["auto_reboot_end"].asCString(), sizeof(auto_reboot_end));
        strncpy(last_reboot, json["last_reboot"].asCString(), sizeof(last_reboot));
        softprobe_on = json["softprobe_on"].asBool();
        last_upgrade_state = json["last_upgrade_state"].asInt();
        strncpy(last_upgrade_time, json["last_upgrade_time"].asCString(), sizeof(last_upgrade_time));
        intelligentNightVision = json["intelligentNightVision"].asBool();
        nightvision_detect_mode = (NightvisionDetectMode)json["nightvision_detect_mode"].asInt();
        return ErrCodeE::Success;
    } catch (std::exception &e) {
        emxlogc("load ovd cfg failed\n");
        return ErrCodeE::Failure;
    }
}

ErrCodeE EnvCfg::Save() {
    try {
        Json::Value json;
        json["trace"] = trace;
//        json["ovd_sleep"] = ovd_sleep;
        json["manual_sleep"] = manual_sleep;
        json["led"] = led;
        json["rsk_encrypt"] = rsk_encrypt;
        json["net_mode"] = net_mode;
        json["logSTD"] = logSTD;
        json["logLevel"] = logLevel;
        json["logContrl"] = logContrl;
        json["callback"] = callback;
        json["log_path"] = log_path;
        json["cnl_cnt"] = cnl_cnt;
        json["video_quality"] = video_quality;
        json["auto_reboot_on"] = auto_reboot_on;
        json["auto_reboot_cycle"] = (uint64_t) auto_reboot_cycle;
        json["auto_reboot_start"] = auto_reboot_start;
        json["auto_reboot_end"] = auto_reboot_end;
        json["last_reboot"] = last_reboot;
        json["softprobe_on"] = softprobe_on;
        json["last_upgrade_state"] = last_upgrade_state;
        json["last_upgrade_time"] = last_upgrade_time;
        json["intelligentNightVision"] = intelligentNightVision;
        json["nightvision_detect_mode"] = nightvision_detect_mode;
        ErrCodeE e = Set(json);
        if (e != ErrCodeE::Success) {
            emxlogc("param set ovd cfg failed\n");
        }
        return e;
    } catch (std::exception &e) {
        emxlogc("save ovd cfg failed\n");
        return ErrCodeE::Failure;
    }
}