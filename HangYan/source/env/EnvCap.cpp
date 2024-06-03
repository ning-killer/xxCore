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
        memset(info.have_support_extension,0,sizeof(info.have_support_extension));
        std::string have_support_extersion = json["talk_protocol"].asString();
        int cmySize = have_support_extersion.size() > sizeof(have_support_extersion) ? sizeof(have_support_extersion) : have_support_extersion.size();
        memcpy(info.have_support_extension,have_support_extersion.c_str(),cmySize);

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
        AIface.mask_detection = AIfaceJson["mask_detection"].asBool();

        auto &AIpassenger = ovdAICapInfo.AIpassenger;
        auto &AIpassengerJson = ovdAICapInfoJson["AIpassenger"];
        AIpassenger.passenger = AIpassengerJson["passenger_flow_statistics_detection"].asBool();
        AIpassenger.passengerExposureAdjust = AIpassengerJson["exposure_adjust"].asBool();
        strncpy(AIpassenger.passengerCaptureModeList, AIpassengerJson["capture_mode_list"].asCString(), sizeof(AIpassenger.passengerCaptureModeList));
        AIpassenger.passengerAlarmzone = AIpassengerJson["alarm_zone"].asInt();
        AIpassenger.support_osd_status = AIpassengerJson["support_osd_status"].asBool();

        auto &AIoffduty = ovdAICapInfo.AIOffDuty;
        auto &AIoffdutyJson = ovdAICapInfoJson["AIoffduty"];
        AIoffduty.offDuty_detection = AIoffdutyJson["off_duty_detection"].asBool();
        AIoffduty.support_on_duty_count = AIoffdutyJson["support_on_duty_count"].asInt();
        AIoffduty.support_off_duty_durtion = AIoffdutyJson["support_off_duty_durtion"].asInt();
        AIoffduty.support_alert_time = AIoffdutyJson["support_alert_time"].asBool();
        AIoffduty.detect_plan_num = AIoffdutyJson["detect_plans_num"].asInt();
        AIoffduty.alarm_area_num = AIoffdutyJson["alarm_area_num"].asInt();
        AIoffduty.alarm_zone = AIoffdutyJson["alarm_zone"].asInt();

        auto &AIregionalPeopleStat = ovdAICapInfo.AIregionalPeopleStat;
        auto &AIregionalPeopleStatJson = ovdAICapInfoJson["AIregionalPeopleStat"];
        AIregionalPeopleStat.regionalPeopleStat = AIregionalPeopleStatJson["regional_people_statistics_detection"].asBool();
        AIregionalPeopleStat.support_alert_time = AIregionalPeopleStatJson["support_alert_time"].asBool();
        AIregionalPeopleStat.support_osd_status = AIregionalPeopleStatJson["support_osd_status"].asBool();
        AIregionalPeopleStat.support_regional_people_count = AIregionalPeopleStatJson["support_regional_people_count"].asInt();
        AIregionalPeopleStat.support_alarm_report_duration = AIregionalPeopleStatJson["support_alarm_report_duration"].asInt();
        AIregionalPeopleStat.support_detect_result_report_duration = AIregionalPeopleStatJson["support_detect_result_report_duration"].asInt();
        AIregionalPeopleStat.detect_plans_num = AIregionalPeopleStatJson["detect_plans_num"].asInt();
        AIregionalPeopleStat.alarm_area_num = AIregionalPeopleStatJson["alarm_area_num"].asInt();
        AIregionalPeopleStat.alarm_zone = AIregionalPeopleStatJson["alarm_zone"].asInt();


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
        AIvehicle.vehicle_detect_site = AIvehicleJson["vehicle_detect_site"].asBool();
        
        auto &AInonmotorvehicle = ovdAICapInfo.AInonmotorvehicle;
        auto &AInonmotorvehicleJson = ovdAICapInfoJson["AInonmotorvehicle"];
        AInonmotorvehicle.nonmotorvehicle_detection = AInonmotorvehicleJson["nonmotorvehicle_detection"].asBool();
        strncpy(AInonmotorvehicle.CaptureModeList, AInonmotorvehicleJson["CaptureModeList"].asCString(),
                sizeof(AInonmotorvehicle.CaptureModeList));
        AInonmotorvehicle.alarm_zone = AInonmotorvehicleJson["alarm_zone"].asInt();

        auto &AILaneLine = ovdAICapInfo.AILaneLine;
        auto &AILaneLineJson = ovdAICapInfoJson["AILaneLine"];
        AILaneLine.laneLineAlarmzone = AILaneLineJson["laneLineAlarmzone"].asInt();
        AILaneLine.laneLine = AILaneLineJson["laneLine"].asBool();
        AILaneLine.laneLineNum = AILaneLineJson["laneLineNum"].asInt();
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
#ifdef OVDSDK1_38_1
        ovdCapInfo_alarms.hava_alarms_alertarea_zone = ovdCapInfo_alarmsJson["support_alarms_alertarea_zone"].asInt();

        // 周界区域入侵
        ovdCapInfo_alarms.alertarea_params.support_set_target = ovdCapInfo_alarmsJson["alertarea_params"]["support_set_target"].asInt();
        ovdCapInfo_alarms.alertarea_params.support_staymode_time = ovdCapInfo_alarmsJson["alertarea_params"]["support_staymode_time"].asBool();
        ovdCapInfo_alarms.alertarea_params.support_set_alerttime = ovdCapInfo_alarmsJson["alertarea_params"]["support_set_alerttime"].asBool();
        ovdCapInfo_alarms.alertarea_params.support_alarms_alertarea_zone = ovdCapInfo_alarmsJson["alertarea_params"]["support_alarms_alertarea_zone"].asInt();

        // 周界越界检测
        ovdCapInfo_alarms.have_alarms_transgression = ovdCapInfo_alarmsJson["have_alarms_transgression"].asBool();
        ovdCapInfo_alarms.transgression_params.support_set_target = ovdCapInfo_alarmsJson["transgression_params"]["support_set_target"].asInt();
        ovdCapInfo_alarms.transgression_params.support_set_alerttime = ovdCapInfo_alarmsJson["transgression_params"]["support_set_alerttime"].asBool();
        ovdCapInfo_alarms.transgression_params.support_set_statistics_line = ovdCapInfo_alarmsJson["transgression_params"]["support_set_statistics_line"].asBool();
#else
        ovdCapInfo_alarms.support_alarms_alertarea_zone = ovdCapInfo_alarmsJson["support_alarms_alertarea_zone"].asInt();
#endif 
        //alarm end

        //linkage begin
        auto &ovdlinkage_mode = info.ovdlinkage_mode;
        auto &ovdlinkage_modeJson = json["ovdlinkage_mode"];
        strncpy(ovdlinkage_mode.alertarea, ovdlinkage_modeJson["alertarea"].asCString(),
                sizeof(ovdlinkage_mode.alertarea));
        strncpy(ovdlinkage_mode.vehicle_detection, ovdlinkage_modeJson["vehicle_detection"].asCString(),
                sizeof(ovdlinkage_mode.vehicle_detection));
        strncpy(ovdlinkage_mode.mask_detection, ovdlinkage_modeJson["mask_detection"].asCString(),
                sizeof(ovdlinkage_mode.mask_detection));
        strncpy(ovdlinkage_mode.nonmotorvehicle_detection, ovdlinkage_modeJson["nonmotorvechicle_detection"].asCString(),
                sizeof(ovdlinkage_mode.nonmotorvehicle_detection));
        strncpy(ovdlinkage_mode.transgression, ovdlinkage_modeJson["transgression"].asCString(),
                sizeof(ovdlinkage_mode.transgression));
        strncpy(ovdlinkage_mode.regionalPeopleStat_detection, ovdlinkage_modeJson["regionalPeopleStat_detection"].asCString(),
                sizeof(ovdlinkage_mode.regionalPeopleStat_detection));
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

#ifdef OVDSDK1_38_1
        info.stream_encryption_mode = (ovd_media_encrypt_type_e)json["stream_encryption_mode"].asInt();
        info.support_detect_nightvision_mask = json["support_set_detect_nightvision_mask"].asInt();
#endif
        return ErrCodeE::Success;
    } catch (std::exception &e) {
        emxlogc("load ovd cap failed:%s\n", e.what());
        return ErrCodeE::Failure;
    }
}