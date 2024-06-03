#include "EnvInitMsg.hpp"

using namespace Emx;

ErrCodeE EnvInitMsg::Load() {
    Json::Value json;
    ErrCodeE e = Get(json);
    if (e != ErrCodeE::Success) {
        emxlogc("param get failed\n");
        return e;
    }
    reason = json["rebootParam"]["reason"].asInt();
    ts = json["rebootParam"]["ts"].asUInt64();
    message = json["rebootParam"]["message"].asString();

    machash = json["clientParam"]["machash"].asString();
    enable_servicschedule = json["clientParam"]["enable_servicschedule"].asInt();
    servicescheduleurl = json["clientParam"]["servicescheduleurl"].asString();
    cafilepath = json["clientParam"]["cafilepath"].asString();
    tz = json["clientParam"]["tz"].asInt();
    local_storage_path = json["clientParam"]["local_storage_path"].asString();
    ovd_data_path = json["clientParam"]["ovd_data_path"].asString();
    ovd_log_path = json["clientParam"]["ovd_log_path"].asString();
    ota_update_status = (OtaUpdateStatus)json["updateStatus"]["state"].asInt();
    is_normal_reboot = json["updateStatus"]["isNormalReboot"].asBool();
    return ErrCodeE::Success;
}

ErrCodeE EnvInitMsg::Save() {
    try {
        Json::Value json;
        json["rebootParam"]["reason"] = reason;
        json["rebootParam"]["ts"] = ts;
        json["rebootParam"]["message"] = message;
        json["clientParam"]["machash"] = machash;
        json["clientParam"]["enable_servicschedule"] = enable_servicschedule;
        json["clientParam"]["servicescheduleurl"] = servicescheduleurl;
        json["clientParam"]["cafilepath"] = cafilepath;
        json["clientParam"]["tz"] = tz;
        json["clientParam"]["local_storage_path"] = local_storage_path;
        json["clientParam"]["ovd_data_path"] = ovd_data_path;
        json["clientParam"]["ovd_log_path"] = ovd_log_path;
        json["updateStatus"]["state"] = ota_update_status;
        json["updateStatus"]["isNormalReboot"] = is_normal_reboot;
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
