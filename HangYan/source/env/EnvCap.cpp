//
// Created by xiong on 2022/7/9.
//

#include "EnvCap.hpp"

using namespace Emx;

ErrCodeE EnvCap::Load() {
    try {
        Json::Value json;
        ErrCodeE e = Get(json);
        if (e != ErrCodeE::Success) {
            emxlogc("param get ovd cap failed\n");
            return e;
        }
        memset(&info, 0, sizeof(info));
        info.have_ptz = json["have_ptz"].asBool();
        info.support_ptz_preset = json["support_ptz_preset"].asBool();
        info.have_battery = json["have_battery"].asBool();
        info.have_audio_out = json["have_audio_out"].asBool();
        info.have_voice_out = json["have_voice_out"].asBool();
        info.have_trace = json["have_trace"].asBool();
        info.have_ivrs = json["have_ivrs"].asBool();

        //ovdAICapInfo begin
        auto &ovdAICapInfo = info.ovdAICapInfo;
        auto &ovdAICapInfoJson = json["ovdAICapInfo"];

        auto &AIface = ovdAICapInfo.AIface;
        auto &AIfaceJson = ovdAICapInfoJson["AIface"];
        AIface.face = AIfaceJson["face"].asBool();
        AIface.faceExposureAdjust = AIfaceJson["faceExposureAdjust"].asBool();
        strncpy(AIface.CaptureModeList, AIfaceJson["CaptureModeList"].asCString(), sizeof(AIface.CaptureModeList));
        AIface.alarm_zone = AIfaceJson["alarm_zone"].asInt();

        auto &AIkitchen = ovdAICapInfo.AIkitchen;
        auto &AIkitchenJson = ovdAICapInfoJson["AIkitchen"];
        AIkitchen.kitchen_detection = AIkitchenJson["kitchen_detection"].asBool();
        strncpy(AIkitchen.CaptureModeList, AIkitchenJson["CaptureModeList"].asCString(),
                sizeof(AIkitchen.CaptureModeList));
        strncpy(AIkitchen.ClothesColorList, AIkitchenJson["ClothesColorList"].asCString(),
                sizeof(AIkitchen.ClothesColorList));
        AIkitchen.alarm_zone = AIkitchenJson["alarm_zone"].asInt();

        auto &AIvehicle = ovdAICapInfo.AIvehicle;
        auto &AIvehicleJson = ovdAICapInfoJson["AIvehicle"];
        AIvehicle.vehicle_detection = AIvehicleJson["vehicle_detection"].asBool();
#ifdef OVDSDK_APIVER_1_0
        strncpy(AIvehicle.CaptureModeList, AIvehicleJson["CaptureModeList"].asCString(),
                sizeof(AIvehicle.CaptureModeList));
#endif 
        
        AIvehicle.alarm_zone = AIvehicleJson["alarm_zone"].asInt();
        //ovdAICapInfo end

        //alarm begin
        auto &ovdCapInfo_alarms = info.ovdCapInfo_alarms;
        auto &ovdCapInfo_alarmsJson = json["ovdCapInfo_alarms"];
        ovdCapInfo_alarms.have_alarms_io = ovdCapInfo_alarmsJson["have_alarms_io"].asBool();
        ovdCapInfo_alarms.have_alarms_face = ovdCapInfo_alarmsJson["have_alarms_face"].asBool();
        ovdCapInfo_alarms.support_alarms_face_zone = ovdCapInfo_alarmsJson["support_alarms_face_zone"].asInt();
        ovdCapInfo_alarms.have_alarms_cry = ovdCapInfo_alarmsJson["have_alarms_cry"].asBool();
        ovdCapInfo_alarms.have_alarms_voice = ovdCapInfo_alarmsJson["have_alarms_voice"].asBool();
        ovdCapInfo_alarms.have_alarms_motion = ovdCapInfo_alarmsJson["have_alarms_motion"].asBool();
        ovdCapInfo_alarms.support_alarms_motion_zone = ovdCapInfo_alarmsJson["support_alarms_motion_zone"].asInt();
        ovdCapInfo_alarms.have_alarms_body = ovdCapInfo_alarmsJson["have_alarms_body"].asBool();
        ovdCapInfo_alarms.support_alarms_body_zone = ovdCapInfo_alarmsJson["support_alarms_body_zone"].asInt();
        ovdCapInfo_alarms.have_alarms_cross = ovdCapInfo_alarmsJson["have_alarms_cross"].asBool();
        ovdCapInfo_alarms.have_alarms_pir = ovdCapInfo_alarmsJson["have_alarms_pir"].asBool();
        ovdCapInfo_alarms.have_alarms_pir_staymode = ovdCapInfo_alarmsJson["have_alarms_pir_staymode"].asBool();
        ovdCapInfo_alarms.have_alarms_lossLock = ovdCapInfo_alarmsJson["have_alarms_lossLock"].asBool();
        ovdCapInfo_alarms.have_alarms_alertarea = ovdCapInfo_alarmsJson["have_alarms_alertarea"].asBool();
        ovdCapInfo_alarms.support_alarms_alertarea_zone = ovdCapInfo_alarmsJson["support_alarms_alertarea_zone"].asInt();
        //alarm end

        //linkage begin
        auto &ovdlinkage_mode = info.ovdlinkage_mode;
        auto &ovdlinkage_modeJson = json["ovdlinkage_mode"];
        strncpy(ovdlinkage_mode.alertarea, ovdlinkage_modeJson["alertarea"].asCString(),
                sizeof(ovdlinkage_mode.alertarea));
        strncpy(ovdlinkage_mode.vehicle_detection, ovdlinkage_modeJson["vehicle_detection"].asCString(),
                sizeof(ovdlinkage_mode.vehicle_detection));
        //linkage end
#ifdef OVDSDK_APIVER_1_0
        info.log_upload_https = json["log_upload_https"].asBool();
        info.firmware_download_https = json["firmware_download_https"].asBool();
        info.video_encrypt = json["video_encrypt"].asBool();
#endif
        info.have_switch = json["have_switch"].asBool();
        info.have_sd = json["have_sd"].asBool();
        info.have_led = json["have_led"].asBool();
        info.have_support_softprobe = json["have_support_softprobe"].asBool();
        info.have_auto_reboot = json["have_auto_reboot"].asBool();
        strncpy(info.video_quality, json["video_quality"].asCString(), sizeof(info.video_quality));
        strncpy(info.video_formats_supportlists, json["video_formats_supportlists"].asCString(),
                sizeof(info.video_formats_supportlists));
        info.support_set_video_formats = json["support_set_video_formats"].asBool();
        info.have_voiceout_volume = json["have_voiceout_volume"].asBool();
        info.have_doorbell_volume = json["have_doorbell_volume"].asBool();
        info.support_set_normal_nightvision_mode = json["support_set_normal_nightvision_mode"].asBool();
        info.support_set_color_nightvision_mode = json["support_set_color_nightvision_mode"].asBool();
        info.osd_text = json["osd_text"].asBool();
        info.osd_logo = json["osd_logo"].asBool();
        info.enable_hjkh = json["enable_hjkh"].asBool();

        //hjkh_mode begin
        auto &hjkh_mode = info.hjkh_mode;
        auto &hjkh_modeJson = json["hjkh_mode"];

        hjkh_mode.preference_call = hjkh_modeJson["preference_call"].asBool();
        hjkh_mode.set_calling_number = hjkh_modeJson["set_calling_number"].asBool();
        hjkh_mode.not_disturb_mode = hjkh_modeJson["not_disturb_mode"].asBool();
        hjkh_mode.not_disturb_device_mode = hjkh_modeJson["not_disturb_device_mode"].asBool();
        hjkh_mode.auto_hangup = hjkh_modeJson["auto_hangup"].asBool();
        //hjkh_mode end
        return ErrCodeE::Success;
    } catch (std::exception &e) {
        emxlogc("load ovd cap failed:%s\n", e.what());
        return ErrCodeE::Failure;
    }
}