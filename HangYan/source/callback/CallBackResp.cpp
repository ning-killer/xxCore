//
// Created by xiong on 2021/3/3.
//
#include "EmxCore.hpp"
#include "EmxMedia.hpp"
#include "CallBackResp.hpp"
#include "NetClient.hpp"
#include "curl.h"
#include "Ovd.hpp"
#include "OvdUtils.hpp"
#include "TimeZone.hpp"
#include "Gat1400Client.hpp"

using namespace Emx;

CallBackResp::CallBackResp(Ovd *ovd) : m_ovd(ovd),
                                       m_ctx(ovd->m_ctx),
                                       m_resp(ovd->m_ctx.loop),
                                       m_started(false) {
}
CallBackResp::~CallBackResp() {
    m_volumeConfig.Destroy();
}


void CallBackResp::Start() {
//    m_ovd = OVD::GetInst();
    if (m_started)
        return;
    m_resp.Start(OVD_CALL_BACK_RPC_ID, std::bind(&CallBackResp::ProcCallBack, this, std::placeholders::_1));
    m_volumeConfig.Create(m_ctx.loop);
    m_started = true;
}

void CallBackResp::Stop() {
    if (!m_started)
        return;
    m_resp.Stop();
    m_volumeConfig.Destroy();
    m_started = false;
}

void CallBackResp::ProcCallBack(ThreadInvoke::Packet &packet) {
    emxlogt("start callback id:%d\n", packet.id);
    switch ((OvdCallBackIdE) packet.id) {
        case OvdCallBackIdE::GetOVDDeviceInfo: {
            auto e = (void **) packet.data;
            //@_@!!!
            *(int *) e[0] = GetOVDDeviceInfo(*(OVDDeviceInfo **) e[1]);
            break;
        }
        case OvdCallBackIdE::GetOVDConfigureInfo: {
            auto e = (void **) packet.data;
            *(int *) e[0] = GetOVDConfigureInfo(*(OVD_char ***) e[1],
                                                *(OVD_int32 **) e[2]);
            break;
        }
        case OvdCallBackIdE::SetOVDConfigureInfo: {
            auto e = (void **) packet.data;
            *(int *) e[0] = SetOVDConfigureInfo(*(OVD_char **) e[1]);
            break;
        }
        case OvdCallBackIdE::OVCConnectStatus: {
            auto e = (void **) packet.data;
            OVCConnectStatus(*(OVD_int32 *) e[1]);
            break;
        }
        case OvdCallBackIdE::ReBootChannel: {
            auto e = (void **) packet.data;
            *(int *) e[0] = ReBootChannel(*(OVD_int32 *) e[1]);
            break;
        }
        case OvdCallBackIdE::ReBootDevice: {
            auto e = (void **) packet.data;
            #ifdef OVDSDK1_38_1
            *(int *) e[0] = ReBootDevice(*(ovd_reboot_reason_e *) e[1]);
            #else
            *(int *) e[0] = ReBootDevice();
            #endif
            
            break;
        }
        case OvdCallBackIdE::KeepAwakenUtilExpired: {
            auto e = (void **) packet.data;
            *(int *) e[0] = KeepAwakenUtilExpired(*(OVD_int32 *) e[1],
                                                  *(OVD_int32 *) e[2],
                                                  *(OVD_int32 *) e[3],
                                                  *(OVDHibernateReason *) e[4]);
            break;
        }
        case OvdCallBackIdE::ResetConfiguration: {
            auto e = (void **) packet.data;
            *(int *) e[0] = ResetConfiguration();
            break;
        }
        case OvdCallBackIdE::QueryRecordPage: {
            auto e = (void **) packet.data;
            *(int *) e[0] = QueryRecordPage(*(OVD_uint32 *) e[1],
                                            *(OVD_uint32 *) e[2],
                                            *(OVD_uint32 *) e[3],
                                            *(OVD_uint64 *) e[4],
                                            *(OVD_uint64 *) e[5],
                                            *(OVD_int32 *) e[6],
                                            *(OVD_int32 *) e[7],
                                            *(OVDRecordFileListPerPage **) e[8]);
            break;
        }
        case OvdCallBackIdE::OpenRecordFile: {
            auto e = (void **) packet.data;
            *(int *) e[0] = OpenRecordFile(*(OVD_int32 *) e[1],
                                           *(OVD_char **) e[2],
                                           *(OVDVideoDataFormat **) e[3],
                                           *(OVDAudioDataFormat **) e[4],
                                           *(OVD_int32 **) e[5]);
            break;
        }
        case OvdCallBackIdE::RecordCotrol: {
            auto e = (void **) packet.data;
            *(int *) e[0] = RecordCotrol(*(OVD_int32 *) e[1],
                                         *(OVDCONTROLTYPE *) e[2],
                                         *(OVD_int32 *) e[3]);
            break;
        }
        case OvdCallBackIdE::DMEAPI_callback_RecordSearch: {
            auto e = (void **) packet.data;
            *(int *) e[0] = DMEAPI_callback_RecordSearch(*(OVD_int32 *) e[1],
                                                         *(OVD_uint64 *) e[2],
                                                         *(OVD_uint64 *) e[3],
                                                         *(OVD_int32 *) e[4],
                                                         *(OVD_int32 *) e[5],
                                                         *(OVD_DMERecordFileListPerPage **) e[6]);
            break;
        }
        case OvdCallBackIdE::DMEAPI_callback_RecordOpen: {
            auto e = (void **) packet.data;
            *(void **) e[0] = DMEAPI_callback_RecordOpen(*(OVD_int32 *) e[1]);
            break;
        }
        case OvdCallBackIdE::DMEAPI_callback_RecordSeek: {
            auto e = (void **) packet.data;
            *(int *) e[0] = DMEAPI_callback_RecordSeek(*(OVD_void **) e[1],
                                                       *(OVD_int64 *) e[2]);
            break;
        }
        case OvdCallBackIdE::DMEAPI_callback_RecordReadFrame: {
            auto e = (void **) packet.data;
            *(int *) e[0] = DMEAPI_callback_RecordReadFrame(*(OVD_void **) e[1],
                                                            *(OVD_FrameInfo **) e[2]);
            break;
        }
        case OvdCallBackIdE::DMEAPI_callback_RecordClose: {
            auto e = (void **) packet.data;
            *(int *) e[0] = DMEAPI_callback_RecordClose(*(OVD_void **) e[1]);
            break;
        }
        case OvdCallBackIdE::FirmwareUpgrade: {
            auto e = (void **) packet.data;
            *(int *) e[0] = FirmwareUpgrade(*(OVD_char **) e[1],
                                            *(OVD_char **) e[2]);
            break;
        }
        case OvdCallBackIdE::QueryFirmwareUpgradeStatus: {
            auto e = (void **) packet.data;
            *(int *) e[0] = QueryFirmwareUpgradeStatus(*(OVDUpgradeStatus **) e[1],
                                                       *(OVD_int32 **) e[2],
                                                       *(OVD_char **) e[3],
                                                       *(OVD_int32 *) e[4],
                                                       *(OVD_char **) e[5],
                                                       *(OVD_int32 *) e[6]);
            break;
        }
        case OvdCallBackIdE::SyncTime: {
            auto e = (void **) packet.data;
            *(int *) e[0] = SyncTime(*(OVD_char **) e[1],
                                     *(OVD_int32 *) e[2],
                                     *(OVD_int32 *) e[3]);
            break;
        }
        case OvdCallBackIdE::QueryTime: {
            auto e = (void **) packet.data;
            *(int *) e[0] = QueryTime(*(OVD_char **) e[1],
                                      *(OVD_int32 *) e[2],
                                      *(OVD_int32 **) e[3]);
            break;
        }
        case OvdCallBackIdE::GetSDInfo: {
            auto e = (void **) packet.data;
            *(int *) e[0] = GetSDInfo(*(OVDSDInfo **) e[1]);
            break;
        }
        case OvdCallBackIdE::SetSDCardFormat: {
            auto e = (void **) packet.data;
            *(int *) e[0] = SetSDCardFormat();
            break;
        }
        case OvdCallBackIdE::PTZCmd: {
            auto e = (void **) packet.data;
            *(int *) e[0] = PTZCmd(*(OVD_int32 *) e[1],
                                   *(OVCPTZControlCmd *) e[2],
                                   *(OVD_int32 *) e[3]);
            break;
        }
        case OvdCallBackIdE::GetPresetList: {
            auto e = (void **) packet.data;
            *(int *) e[0] = GetPresetList(*(OVD_int32 *) e[1],
                                          *(OVD_int32 **) e[2],
                                          *(OVD_int32 *) e[3],
                                          *(OVD_int32 **) e[4]);
            break;
        }
        case OvdCallBackIdE::AudioPlayStart: {
            auto e = (void **) packet.data;
            *(int *) e[0] = AudioPlayStart(*(OVD_int32 *) e[1],
                                           *(OVDAudioOutDataFormat *) e[2]);
            break;
        }
        case OvdCallBackIdE::AudioPlayProGress: {
            auto e = (void **) packet.data;
            *(int *) e[0] = AudioPlayProGress(*(OVD_int32 *) e[1],
                                              *(OVD_uchar **) e[2],
                                              *(OVD_int32 *) e[3]);
            break;
        }
        case OvdCallBackIdE::AudioPlayStop: {
            auto e = (void **) packet.data;
            *(int *) e[0] = AudioPlayStop(*(OVD_int32 *) e[1]);
            break;
        }
        case OvdCallBackIdE::VedioSwitchQuality: {
            auto e = (void **) packet.data;
            *(int *) e[0] = VedioSwitchQuality(*(OVD_int32 *) e[1],
                                               *(OVDEncodeQuality *) e[2],
                                               *(OVDVideoDataFormat **) e[3]);
            break;
        }
        case OvdCallBackIdE::ForceIFrame: {
            auto e = (void **) packet.data;
            *(int *) e[0] = ForceIFrame(*(OVD_int32 *) e[1]);
            break;
        }
        case OvdCallBackIdE::Snapshot: {
            auto e = (void **) packet.data;
            *(int *) e[0] = Snapshot(*(OVD_int32 *) e[1],
                                     *(OVDImageInfo **) e[2],
                                     *(OVD_int32 *) e[3]);
            break;
        }
        case OvdCallBackIdE::SetAudioOutPlay: {
            auto e = (void **) packet.data;
            *(int *) e[0] = SetAudioOutPlay(*(OVD_int32 *) e[1],
                                            *(OVD_char **) e[2],
                                            *(OVD_int32 *) e[3],
                                            *(OVD_int32 *) e[4]);
            break;
        }
        case OvdCallBackIdE::AudioOutPlayCtrl: {
            // auto e = (void **) packet.data;
            // *(int *) e[0] = AudioOutPlayCtrl(*(OVD_int32 *) e[1],
            //                                  *(OVDMp3PlayCtrl *) e[2]);
            break;
        }
        case OvdCallBackIdE::GetAudioOutPlayStatus: {
            // auto e = (void **) packet.data;
            // *(int *) e[0] = GetAudioOutPlayStatus(*(OVD_int32 *) e[1],
            //                                       *(OVD_int32 **) e[2],
            //                                       *(OVD_char **) e[3]);
            break;
        }
        case OvdCallBackIdE::gettime: {
            auto e = (void **) packet.data;
            *(int *) e[0] = gettime(*(OVD_uint64 **) e[1]);
            break;
        }
        case OvdCallBackIdE::settime: {
            auto e = (void **) packet.data;
            *(int *) e[0] = settime(*(OVD_uint64 *) e[1],
                                    *(OVD_int32 *) e[2]);
            break;
        }
        case OvdCallBackIdE::syncProbeDetector: {
        #ifdef OVDSDK_APIVER_1_0
            auto e = (void **) packet.data;
            *(int *) e[0] = syncProbeDetector(*(OVD_int32 *) e[1],
                                              *(OVD_int32 *) e[2],
                                              *(OVD_int32 *) e[3],
                                              *(OVD_int32 *) e[4],
                                              *(OVD_int32 *) e[5],
                                              *(OVD_int32 *) e[6],
                                              *(OVD_int32 *) e[7],
                                              *(OVD_int32 *) e[8]);
        #endif
            break;
        }
        case OvdCallBackIdE::ProbeByval: {
            auto e = (void **) packet.data;
            *(int *) e[0] = ProbeByval();
            break;
        }
        case OvdCallBackIdE::getsimpleovdinfo: {
            auto e = (void **) packet.data;
            *(int *) e[0] = getsimpleovdinfo(*(SimpleOVDinfo **) e[1]);
            break;
        }
        case OvdCallBackIdE::GetDiskInfo: {
            auto e = (void **) packet.data;
            *(int *) e[0] = GetDiskInfo(*(OVD_int32 **) e[1],
                                        *(OVD_int32 **) e[2],
                                        *(OVD_int32 **) e[3]);
            break;
        }
        case OvdCallBackIdE::TriggerSoftProbe: {
        #ifdef OVDSDK_APIVER_1_0
            auto e = (void **) packet.data;
            *(int *) e[0] = TriggerSoftProbe();
        #endif
            break;
        }
        case OvdCallBackIdE::LogUploadAsync: {
            auto e = (void **) packet.data;
            *(int *) e[0] = LogUploadAsync(*(OVD_char **) e[1],
                                           *(OVD_char **) e[2],
                                           *(OVD_char **) e[3],
                                           *(OVD_char **) e[4]);
            break;
        }
        case OvdCallBackIdE::extension_callback: {
            auto e = (void **) packet.data;
            *(int *) e[0] = extension_callback(*(char **) e[1],
                                               *(char **) e[2],
                                               *(char **) e[3],
                                               *(OVD_int32 *) e[4],
                                               *(OVD_int32 **) e[5]);
            break;
        }
        case OvdCallBackIdE::StopAlarm: {
        #ifdef OVDSDK_APIVER_3_0
            break;
        #endif
            break;
        }
        case OvdCallBackIdE::GetDevRunningInfo: {
        #ifdef OVDSDK_APIVER_3_0
        #ifdef OVDSDK1_38_1
            auto e = (void **) packet.data;
            *(int*) e[0] = GetDevRunningInfo(
                                *(ovd_probe_devrunning_info_e *) e[1],
                                *(void **)e[2]);
        #else
            auto e = (void **) packet.data;
            *(int*) e[0] = GetDevRunningInfo(
                                *(OVD_GetDevRunningInfo_e *) e[1],
                                *(void **)e[2]);
        #endif
        #endif
            break;
        }
        case OvdCallBackIdE::SyncMacHash: {
            auto e = (void **) packet.data;
            *(int *) e[0] = SyncMacHash(*(OVD_char **) e[1]);
            break;
        }
        default:
            emxloge("unknown OVD Call back ID : %d\n", packet.id);
            break;
    }
    emxlogt("end callback id:%d\n", packet.id);

}


OVD_int32 CallBackResp::GetOVDConfigureInfo(OVD_char **output_ovdconfig, OVD_int32 *output_size) {
    //服务器来这边取，给一个json序列化的代码
    emxlogi("into");
    auto cap = m_ctx.env.cap.info;

    Json::Value root;

    root["log_level"] = m_ctx.env.cfg.logLevel;
    root["log_contrl"] = m_ctx.env.cfg.logContrl;
    root["tz"] = OvdUtils::TimeZoneToNum(m_ctx.env.timeZone.Get());
    auto &auto_reboot = root["auto_reboot"];
    auto_reboot["on"] = m_ctx.env.cfg.auto_reboot_on;
    auto_reboot["cycle"] = m_ctx.env.cfg.auto_reboot_cycle;
    auto_reboot["start"] = m_ctx.env.cfg.auto_reboot_start;
    auto_reboot["end"] = m_ctx.env.cfg.auto_reboot_end;
    auto_reboot["last_reboot"] = m_ctx.env.cfg.last_reboot;

    root["led"] = m_ctx.env.cfg.led;
    root["softprobe_on"] = m_ctx.env.cfg.softprobe_on;

    auto &channels = root["channels"];
    //IPC只有一个通道
    auto &channel = channels[std::to_string(0)];
    channel["on"] = !m_ctx.running.sleep;

    //video_encoding
    auto &video_encoding = channel["video_encoding"];
    MediaClientVenc venc(0);
    MediaVenc::Param vencParam = {};
    venc.GetParam(vencParam);
    if (vencParam.codec == VideoCodecE::H264)
        video_encoding["encoder"] = "h264";
    else if (vencParam.codec == VideoCodecE::H265)
        video_encoding["encoder"] = "h265";
    if (m_ctx.env.cfg.video_quality == OVD_1DMODE)
        video_encoding["quality"] = "ld";
    else if (m_ctx.env.cfg.video_quality == OVD_SDMODE)
        video_encoding["quality"] = "sd";
    else if (m_ctx.env.cfg.video_quality == OVD_HDMODE)
        video_encoding["quality"] = "hd";
    else if (m_ctx.env.cfg.video_quality == OVD_FHDMODE)
        video_encoding["quality"] = "fhd";
    video_encoding["fps"] = vencParam.fps;
    video_encoding["bitrate"] = vencParam.bitRate;
    video_encoding["width"] = vencParam.width;
    video_encoding["height"] = vencParam.height;
    video_encoding["gop"] = vencParam.iFrameInv * vencParam.fps;
    video_encoding["encrypt"] = m_ctx.env.cfg.rsk_encrypt;


    //audio_encoding
    auto &audio_encoding = channel["audio_encoding"];
    MediaClientAenc aenc(0);
    MediaAenc::Param aencParam = {};
    aenc.GetParam(aencParam);
    if (aencParam.codec == AudioCodecE::AAC)
        audio_encoding["encoder"] = "aac";
    else
        emxloge("only support acc\n");
    audio_encoding["sample_rate"] = aencParam.sampleRate;
    audio_encoding["bitrate"] = aencParam.bitRate;
    audio_encoding["bits_per_sample"] = aencParam.bitWidth;
    audio_encoding["sample_per_frame"] = 1024;//todo
    audio_encoding["channel"] = 1;//todo

    //image
    auto &image = channel["image"];
    image["horflip"] = false;
    image["verflip"] = false;
    int vi_chn_num = 0;
    MediaClientVi::GetChnNum(vi_chn_num);
    MediaVi::Param vi_param = { 0 };
    if (vi_chn_num > 0) {
        MediaClientVi client(0);
        if (client.GetParam(vi_param) == ErrCodeE::Success) {
            image["horflip"] = vi_param.flip;
            image["verflip"] = vi_param.mirror;
        } else {
            emxloge("GetParam is failed\n");
        }
    }

    //alarms begin
    auto &alarms = channel["alarms"];
    //face
    if (cap.ovdCapInfo_alarms.have_alarms_face) {
        auto &face = alarms["face"];
        face["on"] = m_ctx.env.face.ena;
        face["sensitivity"] = m_ctx.env.face.sensitivity;
    }

    //cry
    if (cap.ovdCapInfo_alarms.have_alarms_cry) {
        auto &cry = alarms["cry"];
        cry["on"] = false;
        cry["sensitivity"] = 0;
    }

    //voice
    if (cap.ovdCapInfo_alarms.have_alarms_voice) {
        auto &voice = alarms["voice"];
        voice["on"] = m_ctx.env.voice.ena;
        voice["sensitivity"] = m_ctx.env.voice.sensitivity;
    }

    //motion
    if (cap.ovdCapInfo_alarms.have_alarms_motion) {
        auto &motion = alarms["motion"];
        motion["on"] = m_ctx.env.motion.ena;
        motion["sensitivity"] = m_ctx.env.motion.sensitivity;
        if (cap.ovdCapInfo_alarms.support_alarms_motion_zone) {
            motion["zone"] = m_ctx.env.motion.zone.ovdZone;
        }
    }

    //body
    if (cap.ovdCapInfo_alarms.have_alarms_body) {
        auto &body = alarms["body"];
        body["on"] = m_ctx.env.person.ena;
        body["sensitivity"] = m_ctx.env.person.sensitivity;
        if (cap.ovdCapInfo_alarms.support_alarms_body_zone) {
            body["zone"] = m_ctx.env.person.zone.ovdZone;
        }
    }
    //invasion
#ifndef OVDSDK1_38_1
    if (cap.ovdCapInfo_alarms.have_alarms_alertarea) {
        auto &alertarea = alarms["alertarea"];
        alertarea["on"] = m_ctx.env.invasion.ena;
        alertarea["sensitivity"] = m_ctx.env.invasion.sensitivity;

        auto &alertareaTime = alertarea["alerttime"];
        auto &schedule = m_ctx.env.invasion.schedule;
        alertareaTime["on"] = m_ctx.env.invasion.schedule.onOff;
        alertareaTime["starttime"] = schedule.startTime;
        alertareaTime["endtime"] = schedule.endTime;
        auto &repeat = alertareaTime["repeat"];
        repeat.resize(0);
        if (schedule.repeat[0])
            repeat.append("Mon");
        if (schedule.repeat[1])
            repeat.append("Tue");
        if (schedule.repeat[2])
            repeat.append("Wed");
        if (schedule.repeat[3])
            repeat.append("Thu");
        if (schedule.repeat[4])
            repeat.append("Fri");
        if (schedule.repeat[5])
            repeat.append("Sat");
        if (schedule.repeat[6])
            repeat.append("Sun");
       
        if (cap.ovdCapInfo_alarms.support_alarms_alertarea_zone) {
            alertarea["zone"] = m_ctx.env.invasion.zone.ovdZone;
        }
        auto &linkage_strategy = alertarea["linkage_strategy"];
        auto &speech = linkage_strategy["speech"];
        speech["on"] = m_ctx.env.invasion.strategy.speech.ena;
        speech["repeat"] = m_ctx.env.invasion.strategy.speech.repeat;
        speech["vol"] = m_ctx.env.invasion.strategy.speech.volume;
        speech["url"] = m_ctx.env.invasion.strategy.speech.url;//8K 16bit alaw.wav
        auto &light = linkage_strategy["light"];
        light["on"] = m_ctx.env.invasion.strategy.light.ena;
        light["mode"] = (int) m_ctx.env.invasion.strategy.light.mode;
        light["dur"] = m_ctx.env.invasion.strategy.light.duration;
    }
#endif
#ifdef OVDSDK1_38_1
    if (cap.ovdCapInfo_alarms.have_alarms_transgression) {
        // alarm 周界-越界检测
        auto &transgression = alarms["transgression"];
        transgression["on"] = m_ctx.env.transgression.on;
        transgression["targetType"] = (int)m_ctx.env.transgression.type;
        transgression["statistics_mode_style"] = (int)m_ctx.env.transgression.statisticsModeStyle;
        transgression["statistics_mode_direction"] = (int)m_ctx.env.transgression.statisticsModeDirection;
        m_ctx.env.transgression.line.Serialize(transgression["statistics_line"]);
        GetAlertTime(transgression["alerttime"], m_ctx.env.transgression.schedule);
        GetLinkageStrategy(transgression["linkage_strategy"], m_ctx.env.transgression.strategy);
    }

    if (cap.ovdCapInfo_alarms.alertarea_params.support_set_alerttime) {
        // alarm 周界-区域入侵
        auto &alertarea = alarms["alertarea"];
        alertarea["on"] = m_ctx.env.alertarea.on;
        alertarea["targetType"] = (int)m_ctx.env.alertarea.type;
        alertarea["sensitivity"] = (int)m_ctx.env.alertarea.sensitivity;
        alertarea["staymode_time"] = (int)m_ctx.env.alertarea.staymodeTime;
        alertarea["zone"] = m_ctx.env.alertarea.zone.ovdZone;
        GetAlertTime(alertarea["alerttime"], m_ctx.env.alertarea.schedule);
        GetLinkageStrategy(alertarea["linkage_strategy"], m_ctx.env.alertarea.strategy);
    }
#endif 
    //AI
    auto &ai = channel["AI"];
    //face
    if (cap.ovdCapInfo_alarms.have_alarms_face) {
        auto &face = ai["face"];
        face["on"] = m_ctx.env.face.ena;
        face["alarm_sensitivity"] = m_ctx.env.face.sensitivity;
        face["exposure_on"] = m_ctx.env.face.exposure_on;
        face["exposure_adjust"] = m_ctx.env.face.exposure_adjust;
        std::string s_mode = "";
        if (m_ctx.env.face.mode == EnvAlarmFace::Quick) {
            s_mode = "quick";
        } else if (m_ctx.env.face.mode == EnvAlarmFace::Quality) {
            s_mode = "quality";
        }
        face["mode"] = s_mode;
        face["quality_value"] = m_ctx.env.face.quality;
        if (cap.ovdCapInfo_alarms.support_alarms_face_zone) {
            face["alarm_zone"] = m_ctx.env.face.zone.ovdZone;
        }
        if (cap.ovdAICapInfo.AIface.mask_detection) {
            face["mask_detection"]["on"] = m_ctx.env.face.mask_detection;
            GetAlertTime(face["mask_detection"]["alerttime"], m_ctx.env.face.schedule);
            GetLinkageStrategy(face["mask_detection"]["linkage_strategy"], m_ctx.env.face.strategy);
        }
    }
    // 机动车
    if (cap.ovdAICapInfo.AIvehicle.vehicle_detection) {
        auto &licensePlate = ai["vehicle_detection"];
        licensePlate["on"] = m_ctx.env.licensePlate.on;
        licensePlate["alarm_sensitivity"] = m_ctx.env.licensePlate.sensitivity;
        licensePlate["quality"] = m_ctx.env.licensePlate.quality;
        licensePlate["capture_mode"] = (int)m_ctx.env.licensePlate.mode;
        licensePlate["scheduled_capture_time"] = (int)m_ctx.env.licensePlate.scheduledCaptureTime;
        licensePlate["detect_site"] = (int)m_ctx.env.licensePlate.detectSite;
        licensePlate["exit_mode"] = (int)m_ctx.env.licensePlate.exitMode;
        licensePlate["alarm_zone"] = m_ctx.env.licensePlate.zone.ovdZone;
        GetLinkageStrategy(licensePlate["linkage_strategy"], m_ctx.env.licensePlate.strategy);
    }
    // 非机动车
    if (cap.ovdAICapInfo.AInonmotorvehicle.nonmotorvehicle_detection) {
        auto &batteryCar = ai["nonmotorvehicle_detection"];
        batteryCar["on"] = m_ctx.env.batteryCar.on;
        batteryCar["detect_mode_list"] = m_ctx.env.batteryCar.detectModelist;
        batteryCar["alarm_sensitivity"] = m_ctx.env.batteryCar.sensitivity;
        batteryCar["quality"] = m_ctx.env.batteryCar.quality;
        batteryCar["capture_mode"] = (int)m_ctx.env.batteryCar.mode;
        batteryCar["scheduled_capture_time"] = m_ctx.env.batteryCar.scheduledCaptureTime;
        batteryCar["alarm_zone"] = m_ctx.env.batteryCar.zone.ovdZone;
        GetLinkageStrategy(batteryCar["linkage_strategy"], m_ctx.env.batteryCar.strategy);
    }
    // 客流统计
    if (cap.ovdAICapInfo.AIpassenger.passenger) {
        auto &passenger = ai["passenger_flow_statistics"];
        passenger["on"] = m_ctx.env.passengerFlow.on;
        passenger["exposure_on"] = m_ctx.env.passengerFlow.exposureOn;
        passenger["exposure_adjust"] = m_ctx.env.passengerFlow.exposureAdjust;
        if (m_ctx.env.passengerFlow.mode == EnvAlarmPassengerFlow::SnapMode::Quality) {
            passenger["mode"] = "quality";
        } else if (m_ctx.env.passengerFlow.mode == EnvAlarmPassengerFlow::SnapMode::Quick) {
            passenger["mode"] = "quick";
        }
        passenger["quality_value"] = m_ctx.env.passengerFlow.qualityValue;
        passenger["osd_status"] = m_ctx.env.passengerFlow.osdStatus;
        passenger["statistics_mode"] = m_ctx.env.passengerFlow.statisticsMode;
        passenger["sensitivity"] = m_ctx.env.passengerFlow.sensitivity;
        passenger["alarm_zone"] = m_ctx.env.passengerFlow.zone.ovdZone;
        m_ctx.env.passengerFlow.line.Serialize(passenger["statistics_line"]);
    }
    // 区域人数统计
    if (cap.ovdAICapInfo.AIregionalPeopleStat.regionalPeopleStat) {
        auto &regionalPeople = ai["regional_people_statistics"];
        regionalPeople["detect_on"] = m_ctx.env.regionalPeople.gatReportOn;
        regionalPeople["detect_plans"][0]["on"] = m_ctx.env.regionalPeople.on;
        regionalPeople["detect_plans"][0]["regional_id"] = m_ctx.env.regionalPeople.regionalId;
        regionalPeople["detect_plans"][0]["regional_name"] = m_ctx.env.regionalPeople.regionalName;
        regionalPeople["detect_plans"][0]["regional_people_count"] = m_ctx.env.regionalPeople.regionalPeopleCount;
        regionalPeople["detect_plans"][0]["alarm_report_duration"] = m_ctx.env.regionalPeople.alarmReportDuration;
        regionalPeople["detect_plans"][0]["osd_status"] = m_ctx.env.regionalPeople.osdStatus;
        regionalPeople["detect_plans"][0]["detect_result_report_duration"] = m_ctx.env.regionalPeople.gatReportDuration;
        regionalPeople["detect_plans"][0]["alarm_zone"] = m_ctx.env.regionalPeople.zone.ovdZone;
        GetLinkageStrategy(regionalPeople["detect_plans"][0]["linkage_strategy"], m_ctx.env.regionalPeople.strategy);
        GetAlertTime(regionalPeople["detect_plans"][0]["alerttime"], m_ctx.env.regionalPeople.schedule);
    }
    // 离岗检测
    if (cap.ovdAICapInfo.AIOffDuty.offDuty_detection) {
        auto &offDuty = ai["off_duty_detection"];
        offDuty["detect_on"] = m_ctx.env.offDuty.gatReportOn;
        offDuty["detect_plans"][0]["on"] = m_ctx.env.offDuty.on;
        offDuty["detect_plans"][0]["regional_id"] = m_ctx.env.offDuty.regionalId;
        offDuty["detect_plans"][0]["regional_name"] = m_ctx.env.offDuty.regionalName;
        offDuty["detect_plans"][0]["on_duty_count"] = m_ctx.env.offDuty.onDutyCount;
        offDuty["detect_plans"][0]["off_duty_duration"] = m_ctx.env.offDuty.offdutyDuration;
        offDuty["detect_plans"][0]["alarm_zone"] = m_ctx.env.offDuty.zone.ovdZone;
        GetAlertTime(offDuty["detect_plans"][0]["alerttime"], m_ctx.env.offDuty.schedule);
    }

    //voiceout_volume
    if (cap.have_voiceout_volume) {
        MediaClientAdec adec(0);
        MediaAdec::Param param = {};
        adec.GetParam(param);
        auto &spkVolumeMap = m_ctx.deviceJsonCfg["spkVolumeMap"];
        int volume;
        if (param.volume <= spkVolumeMap[0].asInt())
            volume = 20;
        else if (param.volume <= spkVolumeMap[1].asInt())
            volume = 40;
        else if (param.volume <= spkVolumeMap[2].asInt())
            volume = 60;
        else if (param.volume <= spkVolumeMap[3].asInt())
            volume = 80;
        else
            volume = 100;
        channel["audio_out_volume"] = volume;
    }

    //trace
    if (cap.have_trace) {
        channel["trace"] = cap.have_trace;
    }

    //定时唤醒参数获取
    auto &switch_schedule = channel["switch_schedule"];
    switch_schedule["on"] = m_ctx.env.sleep.schedule.onOff;
    switch_schedule["start_time"] = m_ctx.env.sleep.schedule.startTime;
    switch_schedule["shutdown_time"] = m_ctx.env.sleep.schedule.endTime;
    auto &repeat = switch_schedule["repeat"];
    repeat.resize(0);
    if (m_ctx.env.sleep.schedule.repeat[0])
        repeat.append("Mon");
    if (m_ctx.env.sleep.schedule.repeat[1])
        repeat.append("Tue");
    if (m_ctx.env.sleep.schedule.repeat[2])
        repeat.append("Wed");
    if (m_ctx.env.sleep.schedule.repeat[3])
        repeat.append("Thu");
    if (m_ctx.env.sleep.schedule.repeat[4])
        repeat.append("Fri");
    if (m_ctx.env.sleep.schedule.repeat[5])
        repeat.append("Sat");
    if (m_ctx.env.sleep.schedule.repeat[6])
        repeat.append("Sun");

    //osd 参数
    MediaClientOSD osd(0);
    std::vector<std::shared_ptr<MediaOSD::Param>> osdParam;
    if (osd.GetParam(osdParam) == ErrCodeE::Success) {
        //logo
        if (cap.osd_logo) {
            auto &logo = channel["osd_logo"];
            bool logoOn = false;
            for (auto &item : osdParam) {
                if (item->type == MediaOSD::TypeE::Image && item->image.ena) {
                    logoOn = true;
                    break;
                }
            }
            logo["on"] = logoOn;
        }
        //text
        if (cap.osd_text) {
            auto &textArray = channel["osd_text"];
            textArray.resize(0);
            for (auto &item:osdParam) {
                if (item->type != MediaOSD::TypeE::Text)
                    continue;
                auto text = item->text;
                Json::Value textJson;

                auto &pos = textJson["pos"];
                if (text.margin.vertical != MediaOSD::VAlignE::Top) {
                    emxloge("only vertical margin Top is supported\n");
                }
                pos["y"] = text.margin.rateY * 10;
                //note: 特殊处理水平方向
                if (text.margin.horizon == MediaOSD::HAlignE::Left) {
                    pos["x"] = text.margin.rateX * 10;
                } else if (text.margin.horizon == MediaOSD::HAlignE::Right) {
                    pos["x"] = 9700;
                } else {
                    emxloge("horizon margin Center is supported\n");
                }

                auto &color = textJson["color"];
                color["Red"] = 0;
                color["Green"] = 0;
                color["Blue"] = 0;

                textJson["text"] = text.content;
                textJson["on"] = text.ena;
                int fontsize = 0;
                for (auto &fontMap : m_ctx.deviceJsonCfg["osdFontMap"]) {
                    if (vencParam.height == fontMap["height"].asInt()) {
                        for (int i = 0; i < 4; i++) {
                            if (fontMap["font"][i].asInt() == text.size) {
                                fontsize = i;
                                break;
                            }
                        }
                    }
                }
                textJson["fontsize"] = fontsize;
                textArray.append(textJson);
            }
        }
    } else {
        emxloge("get osd param failed\n");
    }
#ifndef EAPIL_SPARE
    if (cap.support_set_normal_nightvision_mode ||
        cap.support_set_color_nightvision_mode) {
        NightVisionClient night;
        NightVision::Param param = {};
        night.GetParam(param);

        //普通夜视 normal_nightvision_mode
        if (cap.support_set_normal_nightvision_mode) {
            if (param.manual) {
                if (param.manualMode == NightVision::ModeE::Day)
                    channel["normal_nightvision_mode"] = 2;
                else
                    channel["normal_nightvision_mode"] = 1;
            } else {
                channel["normal_nightvision_mode"] = 0;
            }
        }

        //全彩 color_nightvision_mode
        if (cap.support_set_color_nightvision_mode) {
            if (param.autoMode == NightVision::AutoModeE::ColorNightVision)
                channel["color_nightvision_mode"] = 1;
            else if (param.autoMode == NightVision::AutoModeE::IrNightVision) {
                channel["color_nightvision_mode"] = m_ctx.env.cfg.intelligentNightVision ? 2 : 0;
            }
        }

        //触发全彩行为类型分类
        if (cap.support_detect_nightvision_mask != 0) {
            channel["nightvision_detect_mode"] = (int)m_ctx.env.cfg.nightvision_detect_mode;
        }
    }
#endif
    std::string out;
    EasyJson::Serialize(root, out);
    *output_ovdconfig = (char *) malloc(out.size() + 1);
    if (!*output_ovdconfig) {
        emxloge("output_ovdconfig malloc %d failed\n", out.size() + 1);
        return -1;
    }
    memset(*output_ovdconfig, 0, out.size() + 1);
    strncpy(*output_ovdconfig, out.data(), out.size());
    *output_size = (int) out.size() + 1;
    emxlogi("output_ovdconfig:#%s#\n", *output_ovdconfig);
    emxlogi("output_ovdconfig size:%d\n", *output_size);
    return 0;
}

void CallBackResp::SetVolume() {
    m_volumeConfig.Start(0, 200, [this]() {
        if (!m_ctx.running.alarm.isAudioAlarm) {
            MediaClientAdec adec(0);
            adec.SetVolume(m_ctx.adecVolume);
            m_volumeConfig.Stop();
        }
    });
}

OVD_int32 CallBackResp::SetOVDConfigureInfo(OVD_char *in_ovdconfig) {
    emxlogi("#%s#\n", in_ovdconfig);
#ifndef EAPIL_SPARE
    Json::Value root;
    if (EasyJson::Parse(in_ovdconfig, root) != ErrCodeE::Success) {
        emxloge("parse failed\n");
        return -1;
    }
    auto timeNow = Time::GetS();
    auto &cap = m_ctx.env.cap.info;

    bool cfgChanged = false;

    ////////////////////////////////////////日志等级////////////////////////////////////////
    if (root.isMember("log_level")) {
        m_ctx.env.cfg.logLevel = root["log_level"].asInt();
        OVD_setloglevel((OVDLogLevel) m_ctx.env.cfg.logLevel);
//        eapilLog.SetLevel(OvdUtils::OvdLevelToEapilLevel((OVDLogLevel) m_ctx.env.cfg.logLevel));
        emxlogn("set ovd log level:%d\n", (int) m_ctx.env.cfg.logLevel);
        cfgChanged = true;
    }

#ifndef OVDSDK1_38_1
    if (root.isMember("log_contrl")) {
        m_ctx.env.cfg.logContrl = root["log_contrl"].asInt();
        OVD_setlogcontrol(m_ctx.env.cfg.logContrl); 
        emxlogd("OVD_setlogcontrol:%d\n", (int) m_ctx.env.cfg.logContrl);
        cfgChanged = true;
    }
#endif

    ////////////////////////////////////////时区////////////////////////////////////////
    /**tz 时区号，东为正数，西为负数**/
    if (root.isMember("tz")) {
        char tmp[32] = {};
        m_ctx.env.timeZone.Set(OvdUtils::NumToTimeZone(root["tz"].asInt(), tmp));
        cfgChanged = true;
    }

    ////////////////////////////////////////自动维护////////////////////////////////////////
    if (cap.have_auto_reboot && root.isMember("auto_reboot")) {
        cfgChanged = true;
        auto &auto_reboot = root["auto_reboot"];
        if (auto_reboot.isMember("on")) {
            m_ctx.env.cfg.auto_reboot_on = auto_reboot["on"].asBool();
        }
        if (auto_reboot.isMember("cycle")) {
            m_ctx.env.cfg.auto_reboot_cycle = (long long) auto_reboot["cycle"].asInt64();
        }
        if (auto_reboot.isMember("start") && auto_reboot.isMember("end")) {
            strncpy(m_ctx.env.cfg.auto_reboot_start, auto_reboot["start"].asCString(),
                    sizeof(m_ctx.env.cfg.auto_reboot_start));
            strncpy(m_ctx.env.cfg.auto_reboot_end, auto_reboot["end"].asCString(),
                    sizeof(m_ctx.env.cfg.auto_reboot_end));
        }
        m_ovd->m_autoMaintain.UpdateMaintainTime();
    }

    ////////////////////////////////////////手动开关led////////////////////////////////////////
    if (cap.have_led && root.isMember("led")) {
        cfgChanged = true;
        m_ctx.env.cfg.led = root["led"].asBool();
        m_ctx.led->StatChange(Led::StatE::ManualLedEna, m_ctx.env.cfg.led);
    }

    ////////////////////////////////////////软探针////////////////////////////////////////
    if (cap.have_support_softprobe && root.isMember("softprobe_on")) {
        cfgChanged = true;
        m_ctx.env.cfg.softprobe_on = root["softprobe_on"].asBool();
        m_ovd->m_softProbe.Update();
    }

    ////////////////////////////////////////修改网卡MTU////////////////////////////////////////
    if (root.isMember("dutMTU")) {
        Net::DevE dev;
        NetMainDev::Get(dev);
        NetClient net(dev);
        Cmd::Run("ifconfig %s mtu %d up", net.GetInterface(), root["dutMTU"].asInt());
        emxlogi("cmd:[ifconfig %s mtu %d up] excute!\n", net.GetInterface(), root["dutMTU"].asInt());
    }

    //没有通道配置信息则直接返回
    if (!root.isMember("channels") || !root["channels"].isMember("0")) {
        //env.cfg发生改变
        if (cfgChanged) {
            m_ctx.env.cfg.Save();
        }
        return 0;
    }

    ////////////////////////////////////////对通道0的配置////////////////////////////////////////
    auto &channel = root["channels"]["0"];

    ////////////////////////////////////////休眠相关的配置////////////////////////////////////////
    bool sleepChanged = false;
    /**逻辑开、关控制*/
    if (channel.isMember("on")) {
        m_ctx.env.sleep.schedule.manual = channel["on"].asBool();
        Time::GetTimeStr(timeNow, m_ctx.env.sleep.schedule.manualTime,
                         sizeof(m_ctx.env.sleep.schedule.manualTime));
        sleepChanged = true;
    }

    /**switch_schedule 通道定时使能计划**/
    if (channel.isMember("switch_schedule")) {
        auto &switch_schedule = channel["switch_schedule"];
        auto &schedule = m_ctx.env.sleep.schedule;
        if (switch_schedule.isMember("on")) {
            schedule.onOff = switch_schedule["on"].asBool();
        }
        if (switch_schedule.isMember("start_time") && switch_schedule.isMember("shutdown_time")) {
            // schedule.manual = false;
            strncpy(schedule.startTime, switch_schedule["start_time"].asCString(), sizeof(schedule.startTime));
            strncpy(schedule.endTime, switch_schedule["shutdown_time"].asCString(), sizeof(schedule.endTime));
            if (switch_schedule.isMember("repeat")) {
                //如果repeat为空则代表是一次性计划
                schedule.once = switch_schedule["repeat"].empty();
                memset(schedule.repeat, 0, sizeof(schedule.repeat));
                if (schedule.once) {
                    auto st = OvdUtils::GetSecondByTimeStr(schedule.startTime);
                    auto et = OvdUtils::GetSecondByTimeStr(schedule.endTime);
                    auto nt = OvdUtils::GetDayOffSecondByUtc(timeNow);
                    //如果结束时间小于等于起始时间则认为结束时间是隔天时间
                    if (et <= st)
                        et += 60 * 60 * 24;
                    auto currentDay0TimeUtc = OvdUtils::GetDay0TimeUtcByUtc(timeNow);
                    auto startUtc = currentDay0TimeUtc + st;
                    auto endUtc = currentDay0TimeUtc + et;
                    if (nt > et) {
                        //如果当前时间大于等于结束时间则隔天执行
                        startUtc += 60 * 60 * 24;
                        endUtc += 60 * 60 * 24;
                    }
                    Time::GetTimeStr(startUtc, schedule.onceStartTime, sizeof(schedule.onceStartTime));
                    Time::GetTimeStr(endUtc, schedule.onceEndTime, sizeof(schedule.onceEndTime));
                } else {
                    //如果repeat不为空则代表是常规计划
                    for (auto &item : switch_schedule["repeat"]) {
                        if (!schedule.repeat[0] && strcmp(item.asCString(), "Mon") == 0)
                            schedule.repeat[0] = true;
                        else if (!schedule.repeat[1] && strcmp(item.asCString(), "Tue") == 0)
                            schedule.repeat[1] = true;
                        else if (!schedule.repeat[2] && strcmp(item.asCString(), "Wed") == 0)
                            schedule.repeat[2] = true;
                        else if (!schedule.repeat[3] && strcmp(item.asCString(), "Thu") == 0)
                            schedule.repeat[3] = true;
                        else if (!schedule.repeat[4] && strcmp(item.asCString(), "Fri") == 0)
                            schedule.repeat[4] = true;
                        else if (!schedule.repeat[5] && strcmp(item.asCString(), "Sat") == 0)
                            schedule.repeat[5] = true;
                        else if (!schedule.repeat[6] && strcmp(item.asCString(), "Sun") == 0)
                            schedule.repeat[6] = true;
                    }
                }
            }
        }
        sleepChanged = true;
    }//end switch_schedule
    //休眠计划发生改变
    if (sleepChanged) {
        m_ctx.env.sleep.Save();
        m_ovd->m_scheduleSleep.UpdateSchedule();
    }

    ////////////////////////////////////////视频编码和OSD相关的配置////////////////////////////////////////
    bool osdParamChanged = false;
    bool vencParamChanged = false;
    bool vencCodecChanged = false;//需要重启
    bool videoEncryptChanged = false;

    MediaClientVenc venc(0);
    MediaVenc::Param vencParam = {};
    bool vencParamValid = false;
    //是否需要用到venc参数
    if (channel.isMember("video_encoding") || channel.isMember("osd_text")) {
        vencParamValid = venc.GetParam(vencParam) == ErrCodeE::Success;
        if (!vencParamValid) {
            emxloge("get venc param failed\n");
        }
    }

    MediaClientOSD osd(0);
    std::vector<std::shared_ptr<MediaOSD::Param>> osdParam;
    auto osdParamValid = false;
    //是否需要用到osd参数
    if ((channel.isMember("video_encoding") && channel["video_encoding"].isMember("quality")) ||
        (cap.osd_text && channel.isMember("osd_text")) ||
        (cap.osd_logo && channel.isMember("osd_logo"))) {
        osdParamValid = osd.GetParam(osdParam) == ErrCodeE::Success;
        if (!osdParamValid) {
            emxloge("get osd param failed\n");
        }
    }

    /**video_encoding**/
    if (channel.isMember("video_encoding") && vencParamValid) {
        auto &video_encoding = channel["video_encoding"];
        if (video_encoding.isMember("encoder")) {
            auto codec = vencParam.codec;
            if (strcmp(video_encoding["encoder"].asCString(), "h265") == 0) {
                codec = VideoCodecE::H265;
            } else if (strcmp(video_encoding["encoder"].asCString(), "h264") == 0) {
                codec = VideoCodecE::H264;
            }
            if (codec != vencParam.codec) {
                vencParam.codec = codec;
                vencCodecChanged = vencParamChanged = true;
            }
        }

        if (video_encoding.isMember("quality") && osdParamValid) {
            auto video_quality = m_ctx.env.cfg.video_quality;
            if (0 == strcmp(video_encoding["quality"].asCString(), "ld")) {
                video_quality = OVD_1DMODE;
            } else if (0 == strcmp(video_encoding["quality"].asCString(), "sd")) {
                video_quality = OVD_SDMODE;
            } else if (0 == strcmp(video_encoding["quality"].asCString(), "hd")) {
                video_quality = OVD_HDMODE;
            } else if (0 == strcmp(video_encoding["quality"].asCString(), "fhd")) {
                video_quality = OVD_FHDMODE;
            } else {
                emxloge("not support quality %s\n", video_encoding["quality"].asCString());
            }
            if (video_quality != m_ctx.env.cfg.video_quality) {
                Json::Value res;
                if (video_quality == OVD_1DMODE)
                    res = m_ctx.deviceJsonCfg["videoQuality"]["ld"];
                else if (video_quality == OVD_SDMODE)
                    res = m_ctx.deviceJsonCfg["videoQuality"]["sd"];
                else if (video_quality == OVD_HDMODE)
                    res = m_ctx.deviceJsonCfg["videoQuality"]["hd"];
                else if (video_quality == OVD_FHDMODE)
                    res = m_ctx.deviceJsonCfg["videoQuality"]["fhd"];
                else {
                    emxloge("not support quality %d,use OVD_HDMODE as default\n", video_quality);
                    res = m_ctx.deviceJsonCfg["videoQuality"]["hd"];
                    video_quality = OVD_HDMODE;
                }
                vencParam.width = res["venc"]["width"].asInt();
                vencParam.height = res["venc"]["height"].asInt();
                vencParam.bitRate = res["venc"]["bitRate"].asInt();
                //配置osd参数
                for (auto &item : osdParam) {
                    if (item->type == MediaOSD::TypeE::Time) {
                        item->time.size = res["font"]["size"].asInt();
                    } else if (item->type == MediaOSD::TypeE::Text) {
                        item->text.size = res["font"]["size"].asInt();
                    } else if (item->type == MediaOSD::TypeE::Image) {
                        item->image.width = res["logo"]["width"].asInt();
                        item->image.height = res["logo"]["height"].asInt();
                        snprintf(item->image.path, sizeof(item->image.path), "%s/logo/%s",
                                 Misc::GetAppConfigsDir(), res["logo"]["file"].asCString());
                    }
                }
                m_ctx.env.cfg.video_quality = video_quality;
                cfgChanged = osdParamChanged = vencParamChanged = true;
            }
        }

        if (video_encoding.isMember("encrypt")) {
            auto rsk_encrypt = video_encoding["encrypt"].asBool();
            if (rsk_encrypt != m_ctx.env.cfg.rsk_encrypt) {
                m_ctx.env.cfg.rsk_encrypt = rsk_encrypt;
                cfgChanged = videoEncryptChanged = true;
            }
        }
    }

    bool onlyOsdChanged = false;
    /**OSD配置**/
    if (cap.osd_text && channel.isMember("osd_text") && osdParamValid) {
        auto &textArray = channel["osd_text"];
        if (textArray.size() > MediaOSD::MaxOSDNum - 2) {//minus logo and time
            emxlogw("osd_text array_size:%d > 6, we can just support 6 row!\n", textArray.size());
        }
        //这里为了遍历的过程中安全的删除链表中的元素，++操作不能随意修改
        for (auto iter = osdParam.begin(); iter != osdParam.end();) {
            if ((*iter)->type == MediaOSD::TypeE::Text)
                iter = osdParam.erase(iter);
            else
                iter++;
        }
        for (int i = 0; i < 6 && i < (int) textArray.size(); i++) {
            if (!textArray[i].isMember("on") ||
                !textArray[i].isMember("pos") ||
                !textArray[i]["pos"].isMember("x") ||
                !textArray[i]["pos"].isMember("y") ||
                !textArray[i].isMember("text") ||
                textArray[i]["text"].asString().empty())
                continue;
            auto param = std::make_shared<MediaOSD::Param>();
            memset(param.get(), 0, sizeof(MediaOSD::Param));
            param->type = MediaOSD::TypeE::Text;
            param->text.ena = textArray[i].isMember("on") && textArray[i]["on"].asBool();
            
            //note: 水平方向特殊处理
            if ((int)(textArray[i]["pos"]["x"].asInt() / 10) < 50) {
                param->text.margin.horizon = MediaOSD::HAlignE::Left;
                param->text.margin.rateX = (textArray[i]["pos"]["x"].asInt() / 10) - 10;
            } else {
                //note: 杭研要求右侧osd元素右对齐
                param->text.margin.horizon = MediaOSD::HAlignE::Right;
                param->text.margin.rateX = 20;
            }
           
            param->text.margin.vertical = MediaOSD::VAlignE::Top;
            param->text.margin.rateY = (textArray[i]["pos"]["y"].asInt() / 10 - 10);
            param->text.thickness = 1;
            strncpy(param->text.content, textArray[i]["text"].asCString(), sizeof(param->text.content));
            int fontsize = 0;
            if (textArray[i].isMember("fontsize")) {
                fontsize = textArray[i]["fontsize"].asInt();
                if (fontsize < 0 || fontsize > 3) {
                    emxloge("illegal fontsize %d,use default\n", fontsize);
                    fontsize = 0;
                }
            }
            for (auto &fontMap : m_ctx.deviceJsonCfg["osdFontMap"]) {
                if (vencParamValid && vencParam.height == fontMap["height"].asInt()) {
                    param->text.size = fontMap["font"][fontsize].asInt();
                    strncpy(param->text.font, "font.ttf", sizeof(param->text.font));
                    break;
                }
            }
            osdParam.push_back(param);
        }
        osdParamChanged = true;
        onlyOsdChanged = true;
    }
    if (cap.osd_logo &&
        channel.isMember("osd_logo") &&
        channel["osd_logo"].isMember("on") &&
        osdParamValid) {
        for (auto &param : osdParam) {
            if (param->type == MediaOSD::TypeE::Image) {
                param->image.ena = channel["osd_logo"]["on"].asBool();
                break;
            }
        }
        osdParamChanged = true;
    }
    //可能涉及到编码器格式的改变需要重启，放到最后去执行venc和osd的配置

    ////////////////////////////////////////音频编码配置////////////////////////////////////////
    if (channel.isMember("audio_encoding")) {
        if (channel["audio_encoding"].isMember("encoder")) {
            //todo:nothing todo for now
        }
    }

    ////////////////////////////////////////image 图像相关配置////////////////////////////////////////
    int vi_chn_num = 0;
    MediaClientVi::GetChnNum(vi_chn_num);
    MediaVi::Param vi_param = { 0 };
    if (channel.isMember("image") && vi_chn_num > 0) {
        emxlogd("vi chn num(%d)\n", vi_chn_num);
        MediaClientVi client(0);
        if (channel["image"].isMember("horflip")) {
            vi_param.flip = channel["image"]["horflip"].asBool();
        }
        if (channel["image"].isMember("verflip")) {
            vi_param.mirror = channel["image"]["verflip"].asBool();
        }
        if (client.SetParam(vi_param) != ErrCodeE::Success) {
            emxloge("SetParam is failed\n");
        }
    }

    ////////////////////////////////////////alarms 各种报警配置////////////////////////////////////////
    if (channel.isMember("alarms")) {
        auto &alarms = channel["alarms"];
        if (cap.ovdCapInfo_alarms.have_alarms_face &&
            alarms.isMember("face")) {
            auto &face = alarms["face"];
            if (face.isMember("on"))
                m_ctx.env.face.ena = face["on"].asBool();
            if (face.isMember("sensitivity"))
                m_ctx.env.face.sensitivity = face["sensitivity"].asInt();
        }
        if (cap.ovdCapInfo_alarms.have_alarms_voice &&
            alarms.isMember("voice")) {
            auto &voice = alarms["voice"];
            if (voice.isMember("on"))
                m_ctx.env.voice.ena = voice["on"].asBool();
            if (voice.isMember("sensitivity"))
                m_ctx.env.voice.sensitivity = voice["sensitivity"].asInt();
            m_ctx.env.voice.Save();
        }
        if (cap.ovdCapInfo_alarms.have_alarms_motion &&
            alarms.isMember("motion")) {
            auto &motion = alarms["motion"];
            if (motion.isMember("on"))
                m_ctx.env.motion.ena = motion["on"].asBool();
            if (motion.isMember("sensitivity"))
                m_ctx.env.motion.sensitivity = motion["sensitivity"].asInt();
            if (cap.ovdCapInfo_alarms.support_alarms_motion_zone &&
                motion.isMember("zone")) {
                m_ctx.env.motion.zone.ovdZone = motion["zone"];
                m_ctx.env.motion.zone.SetOvdZoneEx(m_ctx.env.motion.zone.ovdZone);
            }
            m_ctx.env.motion.Save();
            MediaClientAi ai_client(0);
            MediaAi::Param ai_param;
            if (ai_client.GetParam(ai_param) == ErrCodeE::Success) {
                ai_param.motionDetection.ena = m_ctx.env.motion.ena;
                if (ai_client.SetParam(ai_param) == ErrCodeE::Success) {
                    emxlogd("ai motionDetection param set ok.\n");
                }
            }
        }
        if (cap.ovdCapInfo_alarms.have_alarms_body &&
            alarms.isMember("body")) {
            auto &body = alarms["body"];
            if (body.isMember("on"))
                m_ctx.env.person.ena = body["on"].asBool();
            if (body.isMember("sensitivity"))
                m_ctx.env.person.sensitivity = body["sensitivity"].asInt();
            if (cap.ovdCapInfo_alarms.support_alarms_body_zone &&
                body.isMember("zone")) {
                m_ctx.env.person.zone.ovdZone = body["zone"];
                m_ctx.env.person.zone.SetOvdZoneEx(m_ctx.env.person.zone.ovdZone);
            }
            m_ctx.env.person.Save();
        }
#ifndef OVDSDK1_38_1
        if (cap.ovdCapInfo_alarms.have_alarms_alertarea 
            && alarms["alertarea"].isObject()
            && !alarms["alertarea"]["targetType"].isInt()) {
            auto &alertarea = alarms["alertarea"];
            if (alertarea.isMember("on")) {
                m_ctx.env.invasion.ena = alertarea["on"].asBool();
                if (!m_ctx.env.invasion.ena) {
                    MediaClientAdecStreamSync::PlayCtrlQuit(0);
                }
            }
            if (alertarea.isMember("sensitivity"))
                m_ctx.env.invasion.sensitivity = alertarea["sensitivity"].asInt();

            if (cap.ovdCapInfo_alarms.support_alarms_alertarea_zone &&
                alertarea.isMember("zone")) {
                m_ctx.env.invasion.zone.ovdZone = alertarea["zone"];
                m_ctx.env.invasion.zone.SetOvdZoneEx(m_ctx.env.invasion.zone.ovdZone);
            }
            
            if (alertarea.isMember("alerttime")) {
                auto &alerttime = alertarea["alerttime"];
                auto &schedule = m_ctx.env.invasion.schedule;
                if (alerttime.isMember("on")) {
                    schedule.onOff = alerttime["on"].asBool();
                }
                if (alerttime.isMember("starttime") && alerttime.isMember("endtime")) {
                    schedule.manual = false;
                    strncpy(schedule.startTime, alerttime["starttime"].asCString(), sizeof(schedule.startTime));
                    strncpy(schedule.endTime, alerttime["endtime"].asCString(), sizeof(schedule.endTime));
                    if (alerttime.isMember("repeat")) {
                        //如果repeat为空则代表是一次性计划
                        schedule.once = alerttime["repeat"].empty();
                        emxlogd("schedule one(%d)\n", schedule.once);
                        memset(schedule.repeat, 0, sizeof(schedule.repeat));
                        if (schedule.once) {
                            auto st = OvdUtils::GetSecondByTimeStr(schedule.startTime);
                            auto et = OvdUtils::GetSecondByTimeStr(schedule.endTime);
                            auto nt = OvdUtils::GetDayOffSecondByUtc(timeNow);
                            //如果结束时间小于等于起始时间则认为结束时间是隔天时间
                            if (et <= st)
                                et += 60 * 60 * 24;
                            auto currentDay0TimeUtc = OvdUtils::GetDay0TimeUtcByUtc(timeNow);
                            auto startUtc = currentDay0TimeUtc + st;
                            auto endUtc = currentDay0TimeUtc + et;
                            if (nt > et) {
                                //如果当前时间大于等于结束时间则隔天执行
                                startUtc += 60 * 60 * 24;
                                endUtc += 60 * 60 * 24;
                            }
                            Time::GetTimeStr(startUtc, schedule.onceStartTime, sizeof(schedule.onceStartTime));
                            Time::GetTimeStr(endUtc, schedule.onceEndTime, sizeof(schedule.onceEndTime));
                            emxlogd("schedule.onceStartTime(%s);schedule.onceEndTime(%s)\n"
                                            , schedule.onceStartTime, schedule.onceEndTime);
                        } else {
                            //如果repeat不为空则代表是常规计划
                            for (auto &item : alerttime["repeat"]) {
                                if (!schedule.repeat[0] && strcmp(item.asCString(), "Mon") == 0)
                                    schedule.repeat[0] = true;
                                else if (!schedule.repeat[1] && strcmp(item.asCString(), "Tue") == 0)
                                    schedule.repeat[1] = true;
                                else if (!schedule.repeat[2] && strcmp(item.asCString(), "Wed") == 0)
                                    schedule.repeat[2] = true;
                                else if (!schedule.repeat[3] && strcmp(item.asCString(), "Thu") == 0)
                                    schedule.repeat[3] = true;
                                else if (!schedule.repeat[4] && strcmp(item.asCString(), "Fri") == 0)
                                    schedule.repeat[4] = true;
                                else if (!schedule.repeat[5] && strcmp(item.asCString(), "Sat") == 0)
                                    schedule.repeat[5] = true;
                                else if (!schedule.repeat[6] && strcmp(item.asCString(), "Sun") == 0)
                                    schedule.repeat[6] = true;
                            }
                        }
                    }
                }
                m_ovd->m_scheduleInv.UpdateSchedule();
            }
            if (alertarea.isMember("linkage_strategy")) {
                auto &linkage_strategy = alertarea["linkage_strategy"];
                if (linkage_strategy.isMember("speech")) {
                    auto &speech = linkage_strategy["speech"];
                    if (speech.isMember("on"))
                        m_ctx.env.invasion.strategy.speech.ena = speech["on"].asBool();
                    if (speech.isMember("vol")) {
                        int volume = speech["vol"].asInt();
                        m_ctx.env.invasion.strategy.speech.volume = volume;
                        auto &spkVolumeMap = m_ctx.deviceJsonCfg["spkVolumeMap"];
                        if (volume <= 20) {
                            m_ctx.running.alarm.alarmVolume = spkVolumeMap[0].asInt();
                        } else if (volume <= 40) {
                            m_ctx.running.alarm.alarmVolume = spkVolumeMap[1].asInt();
                        } else if (volume <= 60) {
                            m_ctx.running.alarm.alarmVolume = spkVolumeMap[2].asInt();
                        } else if (volume <= 80) {
                            m_ctx.running.alarm.alarmVolume = spkVolumeMap[3].asInt();
                        } else {
                            m_ctx.running.alarm.alarmVolume = spkVolumeMap[4].asInt();
                        }  
                        MediaClientAdec adec(0);
                        adec.SetVolumeTemp(m_ctx.running.alarm.alarmVolume);
                    }
                    if (speech.isMember("repeat"))
                        m_ctx.env.invasion.strategy.speech.repeat = speech["repeat"].asInt();
                    if (speech.isMember("url")) {
                        if (speech["url"].asString() != m_ctx.env.invasion.strategy.speech.url) {
                            m_invasion.url = speech["url"].asString();
                            m_invasion.urlChanged = true;
                            if (!m_invasion.work.IsWorking()) {
                                m_invasion.urlChanged = false;
                                auto arg = new InvasionAlarmVoice::Arg;
                                arg->url = m_invasion.url;
                                arg->playVoiceA = m_ctx.env.invasion.strategy.speech.playVoiceA;
                                arg->ret = -1;
                                m_invasion.work.Create(&m_ctx.loop, arg,
                                                       [this](void *arg) {
                                                           auto invasionArg = (InvasionAlarmVoice::Arg *) arg;
                                                           long http_code = 0;
                                                           char path[EMX_MAX_PATH_SIZE] = {};
                                                           snprintf(path, sizeof(path), "%s/alarmVoice%c.wav",
                                                                    m_ctx.deviceJsonCfg["alarmVoiceDir"].asCString(),
                                                                    invasionArg->playVoiceA ? 'B' : 'A');
                                                           invasionArg->ret = OvdUtils::https_download_file_func(
                                                                   invasionArg->url.c_str(),
                                                                   path, &http_code);
                                                       },
                                                       [this](ErrCodeE e, void *arg) {
                                                           auto invasionArg = (InvasionAlarmVoice::Arg *) arg;
                                                           if (m_invasion.urlChanged) {
                                                               m_invasion.urlChanged = false;
                                                               invasionArg->url = m_invasion.url;
                                                               invasionArg->playVoiceA = m_ctx.env.invasion.strategy.speech.playVoiceA;
                                                               invasionArg->ret = -1;
                                                               m_invasion.work.Run();
                                                           } else {
                                                               m_invasion.work.Destroy();
                                                               strncpy(m_ctx.env.invasion.strategy.speech.url,
                                                                       invasionArg->url.c_str(),
                                                                       sizeof(m_ctx.env.invasion.strategy.speech.url));
                                                               delete invasionArg;
                                                               m_ctx.env.invasion.strategy.speech.playVoiceA =
                                                                       !m_ctx.env.invasion.strategy.speech.playVoiceA;
                                                               m_ctx.env.invasion.Save();
                                                           }
                                                       });
                                m_invasion.work.Run();
                            }
                        }
                    }
                    //此处暂时不保存下载地址，等实际下载完成后再进行保存
//                    strncpy(m_ctx.env.invasion.strategy.speech.url, speech["url"].asCString(),
//                            sizeof(m_ctx.env.invasion.strategy.speech.url));
                }
                if (linkage_strategy.isMember("light")) {
                    auto &light = linkage_strategy["light"];
                    if (light.isMember("on"))
                        m_ctx.env.invasion.strategy.light.ena = light["on"].asBool();
                    if (light.isMember("mode"))
                        m_ctx.env.invasion.strategy.light.mode = (EnvStrategy::Light::ModeE) light["mode"].asInt();
                    if (light.isMember("dur"))
                        m_ctx.env.invasion.strategy.light.duration = light["dur"].asInt();
                }
            }
            m_ctx.env.invasion.Save();
        }//end alertarea
#endif
#ifdef OVDSDK1_38_1
        // 周界-越界检测
        if (alarms["transgression"].isObject() &&
            cap.ovdCapInfo_alarms.have_alarms_transgression) {
            auto &transgression = alarms["transgression"];
            if (transgression["on"].isBool()) {
                m_ctx.env.transgression.on = transgression["on"].asBool();
                if (!m_ctx.env.transgression.on) {
                    m_ctx.soundAlarmControl.NotifyControlOff(m_ctx.env.transgression.strategy.type);
                }
            }
            if (transgression["targetType"].isInt()) {
                m_ctx.env.transgression.type = (EnvAlarmTransgression::TargetType)transgression["targetType"].asInt();
            }
            if (transgression["statistics_mode_style"].isInt()) {
                m_ctx.env.transgression.statisticsModeStyle = (EnvAlarmTransgression::DirectionControl)transgression["statistics_mode_style"].asInt();
            }
            if (transgression["statistics_mode_direction"].isInt()) {
                m_ctx.env.transgression.statisticsModeDirection = (EnvAlarmTransgression::Direction)transgression["statistics_mode_direction"].asInt();
            }
            if (transgression["statistics_line"].isObject()) {
                m_ctx.env.transgression.line.Parse(transgression["statistics_line"]);
            }
            if (transgression["linkage_strategy"].isObject()) {
                SetLinkageStrategy(transgression, m_ctx.env.transgression.strategy, "transgressionAlarmVoice");
            }
            if (transgression["alerttime"].isObject()) {
                SetAlertTime(transgression, m_ctx.env.transgression.schedule);
                m_ovd->m_scheduleTransgression.UpdateSchedule();
            }
            m_ctx.env.transgression.Save();
            // MediaClientAi ai_client(0);
            // MediaAi::Param ai_param;
            // if (ai_client.GetParam(ai_param) == ErrCodeE::Success) {
            //     ai_param.classifyDetection.ena = m_ctx.env.transgression.on; 
            //     if (ai_client.SetParam(ai_param) == ErrCodeE::Success) {
            //         emxlogd("ai classifyDetection param set ok.\n");
            //     }
            // }
        }

        // 周界-区域入侵
        if (alarms["alertarea"].isObject()
            && cap.ovdCapInfo_alarms.alertarea_params.support_set_alerttime) {
            auto &alertarea = alarms["alertarea"];
            if (alertarea["on"].isBool()) {
                m_ctx.env.alertarea.on = alertarea["on"].asBool();
                if (!m_ctx.env.alertarea.on) {
                    m_ctx.soundAlarmControl.NotifyControlOff(m_ctx.env.alertarea.strategy.type);
                }
            }
            if (alertarea["sensitivity"].isInt()) {
                m_ctx.env.alertarea.sensitivity = alertarea["sensitivity"].asInt();
            }
            if (alertarea["targetType"].isInt()) {
                m_ctx.env.alertarea.type = (EnvAlarmAlertarea::TargetType)alertarea["targetType"].asInt();
            }
            if (alertarea["staymode_time"].isInt()) {
                m_ctx.env.alertarea.staymodeTime = alertarea["staymode_time"].asInt();
            }
            if (alertarea["zone"].isArray()) {
                if (alertarea["zone"].size() != 0) {
                    m_ctx.env.alertarea.zone.ovdZone = alertarea["zone"];
                    m_ctx.env.alertarea.zone.SetOvdZoneEx(m_ctx.env.alertarea.zone.ovdZone);
                } else {
                    emxloge("alertarea zone is null\n");
                }
            }
            if (alertarea["linkage_strategy"].isObject()) {
                SetLinkageStrategy(alertarea, m_ctx.env.alertarea.strategy, "alertareaAlarmVoice");
            }
            if (alertarea["alerttime"].isObject()) {
                SetAlertTime(alertarea, m_ctx.env.alertarea.schedule);
                m_ovd->m_scheduleAlertarea.UpdateSchedule();
            }
            m_ctx.env.alertarea.Save();
            // MediaClientAi ai_client(0);
            // MediaAi::Param ai_param;
            // if (ai_client.GetParam(ai_param) == ErrCodeE::Success) {
            //     ai_param.classifyDetection.ena = m_ctx.env.alertarea.on; 
            //     if (ai_client.SetParam(ai_param) == ErrCodeE::Success) {
            //         emxlogd("ai classifyDetection param set ok.\n");
            //     }
            // }
        }

#endif
    }//end alarms


    ////////////////////////////////////////ai 能力配置/////////////////////////////////////////////
    if (channel.isMember("AI")) {
        auto &ai = channel["AI"];
        // bool isCreateGat1400 = false;
        bool isReboot = false; 
        // 人脸
        if (cap.ovdCapInfo_alarms.have_alarms_face &&
            ai.isMember("face")) {
            auto &face = ai["face"];
            if (face.isMember("on")) {
                if (m_ctx.env.face.ena != face["on"].asBool() 
                    && face["on"].asBool()) {
                    isReboot = true;
                }
                m_ctx.env.face.ena = face["on"].asBool();
            }
            if (face.isMember("alarm_sensitivity")) {
                m_ctx.env.face.sensitivity = face["alarm_sensitivity"].asInt();
            }
            if (face.isMember("exposure_on")) {
                m_ctx.env.face.exposure_on = face["exposure_on"].asBool();
            }
            if (face.isMember("exposure_adjust")) {
                m_ctx.env.face.exposure_adjust = face["exposure_adjust"].asInt();
            }
            if (face.isMember("mode")) {
                std::string s_mode = face["mode"].asString();
                if (s_mode == "quick") {
                    m_ctx.env.face.mode = EnvAlarmFace::Quick;
                } else if (s_mode == "quality") {
                    m_ctx.env.face.mode = EnvAlarmFace::Quality;
                } else {
                    emxloge("ai face no this snap mode!\n");
                }
            }
            if (face.isMember("quality_value")) {
                m_ctx.env.face.quality = face["quality_value"].asInt();
            }
            if (cap.ovdCapInfo_alarms.support_alarms_face_zone &&
                face.isMember("alarm_zone")) {
                m_ctx.env.face.zone.ovdZone = face["alarm_zone"];
                m_ctx.env.face.zone.SetOvdZoneEx(m_ctx.env.face.zone.ovdZone);
            }

            //make_detection
            if (face["mask_detection"].isObject()) {
                Json::Value mask = face["mask_detection"];
                if (mask["on"].isBool()) {
                    m_ctx.env.face.mask_detection = mask["on"].asBool();
                    if (!m_ctx.env.face.mask_detection) {
                        m_ctx.soundAlarmControl.NotifyControlOff(m_ctx.env.face.strategy.type);
                    }
                }
                if (mask["linkage_strategy"].isObject()) {
                    SetLinkageStrategy(mask, m_ctx.env.face.strategy, "maskAlarmVoice");
                }
                if (mask["alerttime"].isObject()) {
                    SetAlertTime(mask, m_ctx.env.face.schedule);
                    m_ovd->m_scheduleMaskDetection.UpdateSchedule();
                }
            }
            if (m_ctx.env.face.ena) {
                // isCreateGat1400 = true;
            }

            MediaClientAi ai_client(0);
            MediaAi::Param ai_param;
            if (ai_client.GetParam(ai_param) == ErrCodeE::Success) {
                ai_param.faceDetection.ena = m_ctx.env.face.ena;
                ai_param.faceDetection.exposureOn = m_ctx.env.face.exposure_on;
                ai_param.faceDetection.exposureAdjust = m_ctx.env.face.exposure_adjust;
                ai_param.faceDetection.qualityValue = m_ctx.env.face.quality;
                ai_param.faceDetection.maskDetected = m_ctx.env.face.mask_detection;
                ai_param.faceDetection.snapMode = (MediaAi::FaceSnapModeE)m_ctx.env.face.mode;
                if (ai_param.faceDetection.ena) {
                    ai_param.vehicleLicensePlateDetection.ena = false;
                    m_ctx.env.licensePlate.on = false;
                    ai_param.batteryCarDetection.ena = false;
                    m_ctx.env.batteryCar.on = false;
                    ai_param.passengerFlowDetection.ena = false;
                    m_ctx.env.passengerFlow.on = false;
                    m_ctx.env.regionalPeople.on = false;
                    m_ctx.env.offDuty.on = false;
                }
                if (ai_client.SetParam(ai_param) == ErrCodeE::Success) {
                    emxlogd("ai face param set ok.\n");
                    m_ctx.env.batteryCar.Save();
                    m_ctx.env.face.Save();
                    m_ctx.env.licensePlate.Save();
                    m_ctx.env.passengerFlow.Save();
                    m_ctx.env.regionalPeople.Save();
                    m_ctx.env.offDuty.Save();
                }
            }
        }
        // 机动车
        if (cap.ovdAICapInfo.AIvehicle.vehicle_detection && 
            ai.isMember("vehicle_detection")) {
            auto &licensePlate = ai["vehicle_detection"];
            if (licensePlate["on"].isBool()) {
                if (m_ctx.env.licensePlate.on != licensePlate["on"].asBool() 
                    && licensePlate["on"].asBool()) {
                    isReboot = true;
                }
                m_ctx.env.licensePlate.on = licensePlate["on"].asBool();
                if (!m_ctx.env.licensePlate.on) {
                    m_ctx.soundAlarmControl.NotifyControlOff(m_ctx.env.licensePlate.strategy.type);
                }
            }
            if (licensePlate["alarm_sensitivity"].isInt()) {
                m_ctx.env.licensePlate.sensitivity = licensePlate["alarm_sensitivity"].asInt();
            }
            if (licensePlate["quality"].isInt()) {
                m_ctx.env.licensePlate.quality = licensePlate["quality"].asInt();
            }
            if (licensePlate["capture_mode"].isInt()) {
                m_ctx.env.licensePlate.mode = (EnvAlarmLicensePlate::CaptureMode)licensePlate["capture_mode"].asInt();
            }
            if (licensePlate["scheduled_capture_time"].isInt()) {
                m_ctx.env.licensePlate.scheduledCaptureTime = licensePlate["scheduled_capture_time"].asInt();
            }
            if (licensePlate["detect_site"].isInt()) {
                m_ctx.env.licensePlate.detectSite = licensePlate["detect_site"].asInt();
            }
            if (licensePlate["exit_mode"].isInt()) {
                m_ctx.env.licensePlate.exitMode = licensePlate["exit_mode"].asInt();
            }
            if (licensePlate["alarm_zone"].isArray()) {
                m_ctx.env.licensePlate.zone.ovdZone = licensePlate["alarm_zone"];
                m_ctx.env.licensePlate.zone.SetOvdZoneEx(m_ctx.env.licensePlate.zone.ovdZone);
            }
            if (licensePlate["linkage_strategy"].isObject()) {
                SetLinkageStrategy(licensePlate, m_ctx.env.licensePlate.strategy, "vehicleAlarmVoice");
            }
            if (m_ctx.env.licensePlate.on) {
                // isCreateGat1400 = true;
            }
            MediaClientAi ai_client(0);
            MediaAi::Param ai_param;
            if (ai_client.GetParam(ai_param) == ErrCodeE::Success) {
                ai_param.vehicleLicensePlateDetection.ena = m_ctx.env.licensePlate.on;
                ai_param.vehicleLicensePlateDetection.sensitivity = m_ctx.env.licensePlate.sensitivity;
                ai_param.vehicleLicensePlateDetection.snapMode = (MediaAi::CarSnapModeE)m_ctx.env.licensePlate.mode;
                ai_param.vehicleLicensePlateDetection.regularTimer = m_ctx.env.licensePlate.scheduledCaptureTime;
                ai_param.vehicleLicensePlateDetection.detectSite = m_ctx.env.licensePlate.detectSite;
                ai_param.vehicleLicensePlateDetection.exitMode = m_ctx.env.licensePlate.exitMode;
                ai_param.vehicleLicensePlateDetection.quality = m_ctx.env.licensePlate.quality;
                if (ai_param.vehicleLicensePlateDetection.ena) {
                    ai_param.faceDetection.ena = false;
                    m_ctx.env.face.ena = false;
                    ai_param.batteryCarDetection.ena = false;
                    m_ctx.env.batteryCar.on = false;
                    ai_param.passengerFlowDetection.ena = false;
                    m_ctx.env.passengerFlow.on = false;
                    m_ctx.env.regionalPeople.on = false;
                    m_ctx.env.offDuty.on = false;
                }
                if (ai_client.SetParam(ai_param) == ErrCodeE::Success) {
                    emxlogd("ai licensePlate param set ok.\n");
                    m_ctx.env.batteryCar.Save();
                    m_ctx.env.face.Save();
                    m_ctx.env.licensePlate.Save();
                    m_ctx.env.passengerFlow.Save();
                    m_ctx.env.regionalPeople.Save();
                    m_ctx.env.offDuty.Save();
                }
            }
        }
        // 非机动车
        if (cap.ovdAICapInfo.AInonmotorvehicle.nonmotorvehicle_detection && 
            ai.isMember("nonmotorvehicle_detection")) {
            auto &batteryCar = ai["nonmotorvehicle_detection"];
            if (batteryCar["on"].isBool()) {
                if (m_ctx.env.batteryCar.on != batteryCar["on"].asBool() 
                    && batteryCar["on"].asBool()) {
                    isReboot = true;
                }
                m_ctx.env.batteryCar.on = batteryCar["on"].asBool();
                if (!m_ctx.env.batteryCar.on) {
                    m_ctx.soundAlarmControl.NotifyControlOff(m_ctx.env.batteryCar.strategy.type);
                }
            }
            if (batteryCar["detect_mode_list"].isString()) {
                m_ctx.env.batteryCar.detectModelist = batteryCar["detect_mode_list"].asString();
            }
            if (batteryCar["alarm_sensitivity"].isInt()) {
                m_ctx.env.batteryCar.sensitivity = batteryCar["alarm_sensitivity"].asInt();
            }
            if (batteryCar["quality"].isInt()) {
                m_ctx.env.batteryCar.quality = batteryCar["quality"].asInt();
            }
            if (batteryCar["capture_mode"].isInt()) {
                m_ctx.env.batteryCar.mode = (EnvAlarmBatteryCar::CaptureMode)batteryCar["capture_mode"].asInt();
            }
            if (batteryCar["scheduled_capture_time"].isInt()) {
                m_ctx.env.batteryCar.scheduledCaptureTime = batteryCar["scheduled_capture_time"].asInt();
            }
            if (batteryCar["alarm_zone"].isArray()) {
                m_ctx.env.batteryCar.zone.ovdZone = batteryCar["alarm_zone"];
                m_ctx.env.batteryCar.zone.SetOvdZoneEx(m_ctx.env.batteryCar.zone.ovdZone);
            }
            if (batteryCar["linkage_strategy"].isObject()) {
                SetLinkageStrategy(batteryCar, m_ctx.env.batteryCar.strategy, "nonmotorAlarmVoice");
            }
            if (m_ctx.env.batteryCar.on) {
                // isCreateGat1400 = true;
            }
            MediaClientAi ai_client(0);
            MediaAi::Param ai_param;
            if (ai_client.GetParam(ai_param) == ErrCodeE::Success) {
                ai_param.batteryCarDetection.ena = m_ctx.env.batteryCar.on;
                ai_param.batteryCarDetection.sensitivity = m_ctx.env.batteryCar.sensitivity;
                ai_param.batteryCarDetection.snapMode = (MediaAi::CarSnapModeE)m_ctx.env.batteryCar.mode;
                ai_param.batteryCarDetection.regularTimer = m_ctx.env.batteryCar.scheduledCaptureTime;
                ai_param.batteryCarDetection.quality = m_ctx.env.batteryCar.quality;
                if (ai_param.batteryCarDetection.ena) {
                    ai_param.faceDetection.ena = false;
                    m_ctx.env.face.ena = false;
                    ai_param.vehicleLicensePlateDetection.ena = false;
                    m_ctx.env.licensePlate.on = false;
                    ai_param.passengerFlowDetection.ena = false;
                    m_ctx.env.passengerFlow.on = false;
                    m_ctx.env.regionalPeople.on = false;
                    m_ctx.env.offDuty.on = false;
                }
                if (ai_client.SetParam(ai_param) == ErrCodeE::Success) {
                    emxlogd("ai batteryCar param set ok.\n");
                    m_ctx.env.batteryCar.Save();
                    m_ctx.env.face.Save();
                    m_ctx.env.licensePlate.Save();
                    m_ctx.env.passengerFlow.Save();
                    m_ctx.env.regionalPeople.Save();
                    m_ctx.env.offDuty.Save();
                }
            }
        }
        // 客流统计
        if (cap.ovdAICapInfo.AIpassenger.passenger && 
            ai.isMember("passenger_flow_statistics")) {
            auto &passengerFlow = ai["passenger_flow_statistics"];
            if (passengerFlow["on"].isBool()) {
                if (m_ctx.env.passengerFlow.on != passengerFlow["on"].asBool() 
                    && passengerFlow["on"].asBool()) {
                    isReboot = true;
                }
                m_ctx.env.passengerFlow.on = passengerFlow["on"].asBool();
                ShowPassengerFlowOsd();
            }
            if (passengerFlow["exposure_on"].isBool()) {
                m_ctx.env.passengerFlow.exposureOn = passengerFlow["exposure_on"].asBool();
            }
            if (passengerFlow["exposure_adjust"].isInt()) {
                m_ctx.env.passengerFlow.exposureAdjust = passengerFlow["exposure_adjust"].asInt();
            }
            if (passengerFlow["mode"].isString()) {
                if (passengerFlow["mode"].asString() == "quality") {
                    m_ctx.env.passengerFlow.mode = EnvAlarmPassengerFlow::SnapMode::Quality;
                } else if (passengerFlow["mode"].asString() == "quick") {
                    m_ctx.env.passengerFlow.mode = EnvAlarmPassengerFlow::SnapMode::Quick;
                }
                
            }
            if (passengerFlow["quality_value"].isInt()) {
                m_ctx.env.passengerFlow.qualityValue = passengerFlow["quality_value"].asInt();
            }
            if (passengerFlow["osd_status"].isBool()) {
                m_ctx.env.passengerFlow.osdStatus = passengerFlow["osd_status"].asBool();
                ShowPassengerFlowOsd();
            }
            if (passengerFlow["statistics_mode"].isInt()) {
                m_ctx.env.passengerFlow.statisticsMode = (EnvAlarmPassengerFlow::DirectionRule)passengerFlow["statistics_mode"].asInt();
            }
            if (passengerFlow["statistics_line"].isObject()) {
                m_ctx.env.passengerFlow.line.Parse(passengerFlow["statistics_line"]);
            }
            if (passengerFlow["alarm_zone"].isArray()) {
                m_ctx.env.passengerFlow.zone.ovdZone = passengerFlow["alarm_zone"];
                m_ctx.env.passengerFlow.zone.SetOvdZoneEx(m_ctx.env.passengerFlow.zone.ovdZone);
            }
            if (m_ctx.env.passengerFlow.on) {
                m_ctx.env.face.ena = false;
                m_ctx.env.licensePlate.on = false;
                m_ctx.env.batteryCar.on = false;
                m_ctx.env.regionalPeople.gatReportOn = false;
                m_ctx.env.offDuty.gatReportOn = false;
            }
            m_ctx.env.passengerFlow.Save();
            m_ctx.env.batteryCar.Save();
            m_ctx.env.face.Save();
            m_ctx.env.licensePlate.Save();
            m_ctx.env.regionalPeople.Save();
            m_ctx.env.offDuty.Save();
        }

        // 区域人数统计
        if (cap.ovdAICapInfo.AIregionalPeopleStat.regionalPeopleStat && 
            ai.isMember("regional_people_statistics")) {
            bool gatReportOnisChanged = false;
            bool gatReportDurationisChanged = false;
            auto &regionalPeople = ai["regional_people_statistics"];
            if (regionalPeople["detect_on"].isBool()) {
                if (m_ctx.env.regionalPeople.gatReportOn != regionalPeople["detect_on"].asBool()) {
                    gatReportOnisChanged = true;
                }
                if (m_ctx.env.regionalPeople.gatReportOn != regionalPeople["detect_on"].asBool() 
                    && regionalPeople["detect_on"].asBool()) {
                    isReboot = true;
                }
                m_ctx.env.regionalPeople.gatReportOn = regionalPeople["detect_on"].asBool();
                ShowRegionalPeopleOsd();
            }
            if (regionalPeople["detect_plans"].isArray() && regionalPeople["detect_plans"].size() > 0) {
                auto &regionalPeoplePlan = regionalPeople["detect_plans"][0]; //目前仅支持一组，后期需求自行拓展成数组
                if (regionalPeoplePlan["on"].isBool()) {
                    m_ctx.env.regionalPeople.on = regionalPeoplePlan["on"].asBool();
                }
                if (regionalPeoplePlan["regional_id"].isInt()) {
                    m_ctx.env.regionalPeople.regionalId = regionalPeoplePlan["regional_id"].asInt();
                }
                if (regionalPeoplePlan["regional_name"].isString()) {
                    m_ctx.env.regionalPeople.regionalName = regionalPeoplePlan["regional_name"].asString();
                }
                if (regionalPeoplePlan["regional_people_count"].isInt()) {
                    m_ctx.env.regionalPeople.regionalPeopleCount = regionalPeoplePlan["regional_people_count"].asInt();
                }
                if (regionalPeoplePlan["alarm_report_duration"].isInt()) {
                    m_ctx.env.regionalPeople.alarmReportDuration = regionalPeoplePlan["alarm_report_duration"].asInt();
                }
                if (regionalPeoplePlan["osd_status"].isBool()) {
                    m_ctx.env.regionalPeople.osdStatus = regionalPeoplePlan["osd_status"].asBool();
                    ShowRegionalPeopleOsd();
                }
                if (regionalPeoplePlan["detect_result_report_duration"].isInt()) {
                    if (m_ctx.env.regionalPeople.gatReportDuration != regionalPeoplePlan["detect_result_report_duration"].asInt()) {
                        gatReportDurationisChanged = true;
                    }
                    m_ctx.env.regionalPeople.gatReportDuration = regionalPeoplePlan["detect_result_report_duration"].asInt();
                }
                if (regionalPeoplePlan["alarm_zone"].isArray()) {
                    m_ctx.env.regionalPeople.zone.ovdZone = regionalPeoplePlan["alarm_zone"];
                    m_ctx.env.regionalPeople.zone.SetOvdZoneEx(m_ctx.env.regionalPeople.zone.ovdZone);
                }
                if (regionalPeoplePlan["linkage_strategy"].isObject()) {
                    SetLinkageStrategy(regionalPeoplePlan, m_ctx.env.regionalPeople.strategy, "regionalPeopleAlarmVoice");
                }
                if (regionalPeoplePlan["alerttime"].isObject()) {
                    SetAlertTime(regionalPeoplePlan, m_ctx.env.regionalPeople.schedule);
                    m_ovd->m_scheduleregionalPeople.UpdateSchedule();
                }
            }
            if (gatReportOnisChanged || gatReportDurationisChanged) {
                m_ctx.env.regionalPeople.isChangedGatReport = true;
            } else {
                m_ctx.env.regionalPeople.isChangedGatReport = false;
            }
            if (m_ctx.env.regionalPeople.gatReportOn) {
                m_ctx.env.face.ena = false;
                m_ctx.env.licensePlate.on = false;
                m_ctx.env.batteryCar.on = false;
                m_ctx.env.offDuty.on = false;
                m_ctx.env.offDuty.gatReportOn = false;
                m_ctx.env.passengerFlow.on = false;
            }
            m_ctx.env.passengerFlow.Save();
            m_ctx.env.batteryCar.Save();
            m_ctx.env.face.Save();
            m_ctx.env.licensePlate.Save();
            m_ctx.env.offDuty.Save();
            m_ctx.env.regionalPeople.Save();
        }

        // 离岗检测
        if (cap.ovdAICapInfo.AIOffDuty.offDuty_detection && 
            ai.isMember("off_duty_detection")) {
            bool isChangedoffdutyDuration = false;
            auto &offDuty = ai["off_duty_detection"];
            if (offDuty["detect_on"].isBool()) {
                if (m_ctx.env.offDuty.gatReportOn != offDuty["detect_on"].asBool()) {
                    isChangedoffdutyDuration = true;
                }
                if (m_ctx.env.offDuty.gatReportOn != offDuty["detect_on"].asBool() 
                    && offDuty["detect_on"].asBool()) {
                    isReboot = true;
                }
                m_ctx.env.offDuty.gatReportOn = offDuty["detect_on"].asBool();
            }
            if (offDuty["detect_plans"].isArray() && offDuty["detect_plans"].size() > 0) {
                auto &offDutyPlan = offDuty["detect_plans"][0]; //目前仅支持一组，后期需求自行拓展成数组
                if (offDutyPlan["on"].isBool()) {
                    m_ctx.env.offDuty.on = offDutyPlan["on"].asBool();
                }
                if (offDutyPlan["regional_id"].isInt()) {
                    m_ctx.env.offDuty.regionalId = offDutyPlan["regional_id"].asInt();
                }
                if (offDutyPlan["regional_name"].isString()) {
                    m_ctx.env.offDuty.regionalName = offDutyPlan["regional_name"].asString();
                }
                if (offDutyPlan["on_duty_count"].isInt()) {
                    m_ctx.env.offDuty.onDutyCount = offDutyPlan["on_duty_count"].asInt();
                }
                if (offDutyPlan["off_duty_duration"].isInt()) {
                    if (m_ctx.env.offDuty.offdutyDuration != offDutyPlan["off_duty_duration"].asInt()) {
                        isChangedoffdutyDuration = true;
                    }
                    m_ctx.env.offDuty.offdutyDuration = offDutyPlan["off_duty_duration"].asInt();
                }
                if (offDutyPlan["alarm_zone"].isArray()) {
                    m_ctx.env.offDuty.zone.ovdZone = offDutyPlan["alarm_zone"];
                    m_ctx.env.offDuty.zone.SetOvdZoneEx(m_ctx.env.offDuty.zone.ovdZone);
                }
                if (offDutyPlan["alerttime"].isObject()) {
                    SetAlertTime(offDutyPlan, m_ctx.env.offDuty.schedule);
                    m_ovd->m_scheduleOffduty.UpdateSchedule();
                }
            }
            m_ctx.env.offDuty.isChangedoffdutyDuration = isChangedoffdutyDuration;
            if (m_ctx.env.offDuty.gatReportOn) {
                m_ctx.env.face.ena = false;
                m_ctx.env.licensePlate.on = false;
                m_ctx.env.batteryCar.on = false;
                m_ctx.env.passengerFlow.on = false;
                m_ctx.env.regionalPeople.on = false;
                m_ctx.env.regionalPeople.gatReportOn = false;
            }
            m_ctx.env.passengerFlow.Save();
            m_ctx.env.batteryCar.Save();
            m_ctx.env.face.Save();
            m_ctx.env.licensePlate.Save();
            m_ctx.env.offDuty.Save();
            m_ctx.env.regionalPeople.Save();
        }

        MediaClientAi ai_client(0);
        MediaAi::Param ai_param;
        if (ai_client.GetParam(ai_param) == ErrCodeE::Success) {
            emxlogi("offDuty ena[%d]\n", m_ctx.env.offDuty.gatReportOn);
            emxlogi("passengerFlow ena[%d]\n", m_ctx.env.passengerFlow.on);
            emxlogi("regionalPeople ena[%d]\n", m_ctx.env.regionalPeople.gatReportOn);
            if (m_ctx.env.offDuty.gatReportOn || m_ctx.env.passengerFlow.on || m_ctx.env.regionalPeople.gatReportOn) {
                ai_param.passengerFlowDetection.ena = true;
            } else {
                ai_param.passengerFlowDetection.ena = false;
            }
            if (ai_client.SetParam(ai_param) == ErrCodeE::Success) {
                emxlogi("ai passengerFlow param set ok.\n");
            }
        }

        if (!m_ctx.env.face.ena && !m_ctx.env.batteryCar.on 
            && !m_ctx.env.licensePlate.on && !m_ctx.env.passengerFlow.on
            && !m_ctx.env.regionalPeople.on && !m_ctx.env.offDuty.on) {
            emxlogd("gat1400 stop\n");
            Gat1400Client::Instance()->Stop();
        } else {
            emxlogd("gat1400 create\n");
            Gat1400Client::Instance()->Create(&m_ctx);
        }
        if (isReboot) {
            sync();
            Cmd::System("reboot -f");
        }
    }
    ////////////////////////////////////////扬声器输出音量配置////////////////////////////////////////
    /**audio_out_volume 扬声器输出音量0-100**/
    if (cap.have_voiceout_volume && channel.isMember("audio_out_volume")) {
        MediaClientAdec adec(0);
        auto &spkVolumeMap = m_ctx.deviceJsonCfg["spkVolumeMap"];
        int volume = channel["audio_out_volume"].asInt();
        int adecVolume;
        if (volume <= 20)
            adecVolume = spkVolumeMap[0].asInt();
        else if (volume <= 40)
            adecVolume = spkVolumeMap[1].asInt();
        else if (volume <= 60)
            adecVolume = spkVolumeMap[2].asInt();
        else if (volume <= 80)
            adecVolume = spkVolumeMap[3].asInt();
        else
            adecVolume = spkVolumeMap[4].asInt();
        
        m_ctx.adecVolume = adecVolume;
        SetVolume();
    }

    ////////////////////////////////////////trace 移动跟踪////////////////////////////////////////
    if (cap.have_trace && channel.isMember("trace")) {
        //todo
    }


    ////////////////////////////////////////夜视////////////////////////////////////////
    NightVisionClient nightVision;
    NightVision::Param nightVisionParam = {};
    bool nightVisionChanged = false;
    bool nightVisionParamValid = false;

    //仅当需要时才去获取夜视参数
    if ((cap.support_set_normal_nightvision_mode && channel.isMember("normal_nightvision_mode")) ||
        (cap.support_set_color_nightvision_mode && channel.isMember("color_nightvision_mode"))) {
        nightVisionParamValid = nightVision.GetParam(nightVisionParam) == ErrCodeE::Success;
        if (!nightVisionParamValid) {
            emxloge("get night vision param failed\n");
        }
    }

    /**normal_nightvision_mode**/
    if (cap.support_set_normal_nightvision_mode &&
        channel.isMember("normal_nightvision_mode") &&
        nightVisionParamValid) {
        nightVisionChanged = true;
        auto normal_nightvision_mode = channel["normal_nightvision_mode"].asInt();
        if (normal_nightvision_mode == 0) {
            nightVisionParam.manual = false;
        } else if (normal_nightvision_mode == 1) {
            nightVisionParam.manual = true;
            if (nightVisionParam.autoMode == NightVision::AutoModeE::IrNightVision)
                nightVisionParam.manualMode = NightVision::ModeE::IrNightVision;
            else
                nightVisionParam.manualMode = NightVision::ModeE::ColorNightVision;
        } else {
            nightVisionParam.manualMode = NightVision::ModeE::Day;
        }
    }

    /**color_nightvision_mode**/
    if (cap.support_set_color_nightvision_mode &&
        channel.isMember("color_nightvision_mode") &&
        nightVisionParamValid) {
        nightVisionParam.manual = false;
        auto color_nightvision_mode = channel["color_nightvision_mode"].asInt();
        //全彩夜视配置为全彩，黑白夜视和智能夜视都配置为黑白，智能下的全彩由本应用程序去触发，不由nightVision负责
        m_ctx.env.cfg.intelligentNightVision = false;
        if (color_nightvision_mode == 1) {
            nightVisionParam.autoMode = NightVision::AutoModeE::ColorNightVision;
        } else {
            nightVisionParam.autoMode = NightVision::AutoModeE::IrNightVision;
            m_ctx.env.cfg.intelligentNightVision = color_nightvision_mode == 2;
            cfgChanged = true;
        }
        nightVisionChanged = true;
    }

    //触发全彩行为类型分类（默认智能夜视）
    if (cap.support_detect_nightvision_mask != 0 &&
        channel["nightvision_detect_mode"].isInt()) {
        nightVisionParam.manual = false;
        nightVisionParam.autoMode = NightVision::AutoModeE::IrNightVision;
        m_ctx.env.cfg.nightvision_detect_mode = (EnvCfg::NightvisionDetectMode)channel["nightvision_detect_mode"].asInt();
        m_ctx.env.cfg.intelligentNightVision = true;
        m_ctx.env.cfg.Save();
        nightVisionChanged = true;
    }

    if (nightVisionChanged) {
        emxlogd("nightVision(%s);autoMode(%d)\n", nightVisionParam.manual ? "manual" : "auto"
                                                , nightVisionParam.autoMode);
        nightVision.SetParam(nightVisionParam);
    }

    ////////////////////////////////////////完成////////////////////////////////////////

    //env.cfg发生改变
    if (cfgChanged) {
        m_ctx.env.cfg.Save();
    }

    if (vencParamChanged) {
        //执行视频编码和osd配置
        //先暂时关闭所有OSD通道，防止Venc分辨率切换时，水印出现短暂异常
        // bool osdChnEnabled[MediaOSD::MaxOSDNum];
        // std::vector<std::shared_ptr<MediaOSD::Param>> osdParamSubChn;
        // if (osdParamChanged) {
        //     for (int i = 0; i < (int) osdParam.size(); i++) {
        //         if (osdParam[i]->type == MediaOSD::TypeE::Time) {
        //             osdChnEnabled[i] = osdParam[i]->time.ena;
        //             osdParam[i]->time.ena = false;
        //         } else if (osdParam[i]->type == MediaOSD::TypeE::Text) {
        //             osdChnEnabled[i] = osdParam[i]->text.ena;
        //             osdParam[i]->text.ena = false;
        //         } else if (osdParam[i]->type == MediaOSD::TypeE::Image) {
        //             osdChnEnabled[i] = osdParam[i]->image.ena;
        //             osdParam[i]->image.ena = false;
        //         }
        //     }
        //     osd.SetParam(osdParam);
        // }
        bool isRecordCreated = m_ovd->m_record.IsCreated();
        bool isAvPushCreated = m_ovd->m_avPush.IsCreated();
        if (vencCodecChanged) {
            m_ovd->m_record.Destroy();
            m_ovd->m_avPush.Destroy();
        }
        venc.SetParam(vencParam);
        if (vencCodecChanged) {
            if (isRecordCreated)
                m_ovd->m_record.Create(&m_ctx);
            if (isAvPushCreated)
                m_ovd->m_avPush.Create(&m_ctx);
        }
        //重新打开OSD
        // if (osdParamChanged) {
        //     for (int i = 0; i < (int) osdParam.size(); i++) {
        //         if (osdParam[i]->type == MediaOSD::TypeE::Time) {
        //             osdParam[i]->time.ena = true;//时间默认始终打开
        //         } else if (osdParam[i]->type == MediaOSD::TypeE::Text) {
        //             osdParam[i]->text.ena = osdChnEnabled[i];
        //         } else if (osdParam[i]->type == MediaOSD::TypeE::Image) {
        //             osdParam[i]->image.ena = osdChnEnabled[i];
        //         }
        //     }
        //     osd.SetParam(osdParam);
        // }
    } else {
        if (osdParamChanged) {
            osd.SetParam(osdParam);
        }
    }
    //单独处理副码流osd
    if (osdParamChanged && onlyOsdChanged) {
        int chnNum;
        //如果存在副码流
        if (MediaClientOSD::GetChnNum(chnNum) == ErrCodeE::Success &&
            chnNum > 1) {
            MediaClientVenc vencSub(1);
            MediaVenc::Param vencSubParam = {};
            if (MediaClientVenc::GetChnNum(chnNum) == ErrCodeE::Success &&
                chnNum > 1 &&
                vencSub.GetParam(vencSubParam) == ErrCodeE::Success) {
                //todo: adapt resolution
                for (auto &item : osdParam) {
                    if (item->type == MediaOSD::TypeE::Time) {
                        item->time.size = 16;
                    } else if (item->type == MediaOSD::TypeE::Text) {
                        item->text.size = 16;
                    } else if (item->type == MediaOSD::TypeE::Image) {
                        item->image.width = 66;
                        item->image.height = 18;
                        snprintf(item->image.path, sizeof(item->image.path), "%s/logo/%s",
                                 Misc::GetAppConfigsDir(), "640x360.rgba");
                    }
                }
                MediaClientOSD osdSub(1);
                osdSub.SetParam(osdParam);
            }
        }
    }
#endif
    return 0;
}

void CallBackResp::ShowPassengerFlowOsd() {
    MediaClientOSD osd(0);
    std::vector<std::shared_ptr<MediaOSD::Param>> osdParam;
    osdParam.clear();
    if (!m_ctx.env.passengerFlow.on) {
        osd.SetTempParam(osdParam);
        return;
    }
    if (!m_ctx.env.passengerFlow.osdStatus) {
        osd.SetTempParam(osdParam);
    } else {
        if (osd.GetParam(osdParam) != ErrCodeE::Success) {
            emxloge("osd param get failed");
            return;
        }
        int fontsize = 0;
        int thickness = 0;
        MediaOSD::Margin margin;
        for (auto item : osdParam) {
            if (item->type == MediaOSD::TypeE::Time) {
                fontsize = item->time.size;
                thickness = item->time.thickness;
                margin = item->time.margin;
            }
        }

        auto paramIn = std::make_shared<MediaOSD::Param>();
        memset(paramIn.get(), 0, sizeof(MediaOSD::Param));
        paramIn->type = MediaOSD::TypeE::Text;
        paramIn->text.ena = true;
        memcpy(&paramIn->text.margin, &margin, sizeof(MediaOSD::Margin));
        paramIn->text.margin.rateY += fontsize + 5;
        paramIn->text.thickness = thickness;
        paramIn->text.size = fontsize;
        strncpy(paramIn->text.font, "font.ttf", sizeof(paramIn->text.font));
        snprintf(paramIn->text.content, sizeof(paramIn->text.content) - 1, "进入：%d", m_ctx.env.passengerFlow.inCount);
        osdParam.push_back(paramIn);

        auto paramOut = std::make_shared<MediaOSD::Param>();
        memset(paramOut.get(), 0, sizeof(MediaOSD::Param));
        paramOut->type = MediaOSD::TypeE::Text;
        paramOut->text.ena = true;
        memcpy(&paramOut->text.margin, &margin, sizeof(MediaOSD::Margin));
        // paramOut->text.margin.rateY += paramIn->text.margin.rateY + 20;
        paramOut->text.margin.rateY = paramIn->text.margin.rateY + fontsize + 5;
        paramOut->text.thickness = thickness;
        paramOut->text.size = fontsize;
        strncpy(paramOut->text.font, "font.ttf", sizeof(paramOut->text.font));
        snprintf(paramOut->text.content, sizeof(paramOut->text.content) - 1, "离开：%d", m_ctx.env.passengerFlow.outCount);
        osdParam.push_back(paramOut);

        osd.SetTempParam(osdParam);
    }
}

void CallBackResp::ShowRegionalPeopleOsd() {
    MediaClientOSD osd(0);
    std::vector<std::shared_ptr<MediaOSD::Param>> osdParam;
    osdParam.clear();
    if (!m_ctx.env.regionalPeople.gatReportOn) {
        osd.SetTempParam(osdParam);
        return;
    }
    if (!m_ctx.env.regionalPeople.osdStatus) {
        osd.SetTempParam(osdParam);
    } else {
        if (osd.GetParam(osdParam) != ErrCodeE::Success) {
            emxloge("osd param get failed");
            return;
        }
        int fontsize = 0;
        int thickness = 0;
        MediaOSD::Margin margin;
        for (auto item : osdParam) {
            if (item->type == MediaOSD::TypeE::Time) {
                fontsize = item->time.size;
                thickness = item->time.thickness;
                margin = item->time.margin;
            }
        }
        auto paramIn = std::make_shared<MediaOSD::Param>();
        memset(paramIn.get(), 0, sizeof(MediaOSD::Param));
        paramIn->type = MediaOSD::TypeE::Text;
        paramIn->text.ena = true;
        memcpy(&paramIn->text.margin, &margin, sizeof(MediaOSD::Margin));
        paramIn->text.margin.rateY += fontsize + 5;
        paramIn->text.thickness = thickness;
        paramIn->text.size = fontsize;
        strncpy(paramIn->text.font, "font.ttf", sizeof(paramIn->text.font));
        snprintf(paramIn->text.content, sizeof(paramIn->text.content) - 1, "区域：%d", m_ctx.env.regionalPeople.inPeopleCount);
        osdParam.push_back(paramIn);

        osd.SetTempParam(osdParam);
    }
}

void CallBackResp::GetLinkageStrategy(Json::Value &json, const EnvStrategy &strategy) {
    auto &linkage_strategy = json;
    auto &speech = linkage_strategy["speech"];
    speech["on"] = strategy.speech.ena;
    speech["repeat"] = strategy.speech.repeat;
    speech["vol"] = strategy.speech.volume;
    speech["url"] = strategy.speech.url;//8K 16bit alaw.wav
    auto &light = linkage_strategy["light"];
    light["on"] = strategy.light.ena;
    light["mode"] = (int) strategy.light.mode;
    light["dur"] = strategy.light.duration;
}

void CallBackResp::GetAlertTime(Json::Value &json, const EnvSchedule &schedule) {
    auto &alertareaTime = json;
    alertareaTime["on"] = schedule.onOff;
    alertareaTime["starttime"] = schedule.startTime;
    alertareaTime["endtime"] = schedule.endTime;
    auto &repeat = alertareaTime["repeat"];
    repeat.resize(0);
    if (schedule.repeat[0])
        repeat.append("Mon");
    if (schedule.repeat[1])
        repeat.append("Tue");
    if (schedule.repeat[2])
        repeat.append("Wed");
    if (schedule.repeat[3])
        repeat.append("Thu");
    if (schedule.repeat[4])
        repeat.append("Fri");
    if (schedule.repeat[5])
        repeat.append("Sat");
    if (schedule.repeat[6])
        repeat.append("Sun");
}

void CallBackResp::SetAlertTime(const Json::Value &json, EnvSchedule &schedule) {
    auto timeNow = Time::GetS();
    auto &alerttime = json["alerttime"];
    if (alerttime.isMember("on")) {
        schedule.onOff = alerttime["on"].asBool();
    }
    if (alerttime.isMember("starttime") && alerttime.isMember("endtime")) {
        schedule.manual = false;
        strncpy(schedule.startTime, alerttime["starttime"].asCString(), sizeof(schedule.startTime));
        strncpy(schedule.endTime, alerttime["endtime"].asCString(), sizeof(schedule.endTime));
        if (alerttime.isMember("repeat")) {
            //如果repeat为空则代表是一次性计划
            schedule.once = alerttime["repeat"].empty();
            emxlogd("schedule one(%d)\n", schedule.once);
            memset(schedule.repeat, 0, sizeof(schedule.repeat));
            if (schedule.once) {
                auto st = OvdUtils::GetSecondByTimeStr(schedule.startTime);
                auto et = OvdUtils::GetSecondByTimeStr(schedule.endTime);
                auto nt = OvdUtils::GetDayOffSecondByUtc(timeNow);
                //如果结束时间小于等于起始时间则认为结束时间是隔天时间
                if (et <= st)
                    et += 60 * 60 * 24;
                auto currentDay0TimeUtc = OvdUtils::GetDay0TimeUtcByUtc(timeNow);
                auto startUtc = currentDay0TimeUtc + st;
                auto endUtc = currentDay0TimeUtc + et;
                if (nt > et) {
                    //如果当前时间大于等于结束时间则隔天执行
                    startUtc += 60 * 60 * 24;
                    endUtc += 60 * 60 * 24;
                }
                Time::GetTimeStr(startUtc, schedule.onceStartTime, sizeof(schedule.onceStartTime));
                Time::GetTimeStr(endUtc, schedule.onceEndTime, sizeof(schedule.onceEndTime));
                emxlogd("schedule.onceStartTime(%s);schedule.onceEndTime(%s)\n"
                                , schedule.onceStartTime, schedule.onceEndTime);
            } else {
                //如果repeat不为空则代表是常规计划
                for (auto &item : alerttime["repeat"]) {
                    if (!schedule.repeat[0] && strcmp(item.asCString(), "Mon") == 0)
                        schedule.repeat[0] = true;
                    else if (!schedule.repeat[1] && strcmp(item.asCString(), "Tue") == 0)
                        schedule.repeat[1] = true;
                    else if (!schedule.repeat[2] && strcmp(item.asCString(), "Wed") == 0)
                        schedule.repeat[2] = true;
                    else if (!schedule.repeat[3] && strcmp(item.asCString(), "Thu") == 0)
                        schedule.repeat[3] = true;
                    else if (!schedule.repeat[4] && strcmp(item.asCString(), "Fri") == 0)
                        schedule.repeat[4] = true;
                    else if (!schedule.repeat[5] && strcmp(item.asCString(), "Sat") == 0)
                        schedule.repeat[5] = true;
                    else if (!schedule.repeat[6] && strcmp(item.asCString(), "Sun") == 0)
                        schedule.repeat[6] = true;
                }
            }
        }
    }
}

void CallBackResp::SetLinkageStrategy(const Json::Value &json, EnvStrategy &strategy, std::string alarmVoice) {
    if (json.isMember("linkage_strategy")) {
        auto &linkage_strategy = json["linkage_strategy"];
        if (linkage_strategy.isMember("speech")) {
            auto &speech = linkage_strategy["speech"];
            if (speech.isMember("on")) {
                strategy.speech.ena = speech["on"].asBool();
                if (!strategy.speech.ena) {
                    m_ctx.soundAlarmControl.NotifyControlOff(strategy.type);
                }
            }   
            if (speech.isMember("vol")) {
                int volume = speech["vol"].asInt();
                strategy.speech.volume = volume;
                auto &spkVolumeMap = m_ctx.deviceJsonCfg["spkVolumeMap"];
                if (volume <= 20) {
                    m_ctx.running.alarm.alarmVolume = spkVolumeMap[0].asInt();
                } else if (volume <= 40) {
                    m_ctx.running.alarm.alarmVolume = spkVolumeMap[1].asInt();
                } else if (volume <= 60) {
                    m_ctx.running.alarm.alarmVolume = spkVolumeMap[2].asInt();
                } else if (volume <= 80) {
                    m_ctx.running.alarm.alarmVolume = spkVolumeMap[3].asInt();
                } else {
                    m_ctx.running.alarm.alarmVolume = spkVolumeMap[4].asInt();
                }  
                MediaClientAdec adec(0);
                adec.SetVolumeTemp(m_ctx.running.alarm.alarmVolume);
            }
            if (speech.isMember("repeat"))
                strategy.speech.repeat = speech["repeat"].asInt();
            if (speech.isMember("url")) {
                if (speech["url"].asString() != strategy.speech.url) {
                    m_invasion.url = speech["url"].asString();
                    m_invasion.urlChanged = true;
                    if (!m_invasion.work.IsWorking()) {
                        m_invasion.urlChanged = false;
                        auto arg = new InvasionAlarmVoice::Arg;
                        arg->url = m_invasion.url;
                        arg->playVoiceA = strategy.speech.playVoiceA;
                        arg->ret = -1;
                        arg->alarmVoice = alarmVoice;
                        arg->strategy = &strategy;
                        m_invasion.work.Create(&m_ctx.loop, arg,
                                                [this](void *arg) {
                                                    auto invasionArg = (InvasionAlarmVoice::Arg *) arg;
                                                    long http_code = 0;
                                                    char path[EMX_MAX_PATH_SIZE] = {};
                                                    snprintf(path, sizeof(path), "%s/%s%c.wav",
                                                            m_ctx.deviceJsonCfg["alarmVoiceDir"].asCString(),
                                                            invasionArg->alarmVoice.c_str(),
                                                            invasionArg->playVoiceA ? 'B' : 'A');
                                                    invasionArg->ret = OvdUtils::https_download_file_func(
                                                            invasionArg->url.c_str(),
                                                            path, &http_code);
                                                },
                                                [this](ErrCodeE e, void *arg) {
                                                    auto invasionArg = (InvasionAlarmVoice::Arg *) arg;
                                                    if (m_invasion.urlChanged) {
                                                        m_invasion.urlChanged = false;
                                                        invasionArg->url = m_invasion.url;
                                                        invasionArg->playVoiceA = invasionArg->strategy->speech.playVoiceA;
                                                        invasionArg->ret = -1;
                                                        m_invasion.work.Run();
                                                    } else {
                                                        m_invasion.work.Destroy();
                                                        strncpy(invasionArg->strategy->speech.url, invasionArg->url.c_str(), sizeof(strategy.speech.url));
                                                        delete invasionArg;
                                                        invasionArg->strategy->speech.playVoiceA = !invasionArg->strategy->speech.playVoiceA;
                                                        m_ctx.env.invasion.Save();
                                                    }
                                                });
                        m_invasion.work.Run();
                    }
                }
            }
        }
        if (linkage_strategy.isMember("light")) {
            auto &light = linkage_strategy["light"];
            if (light.isMember("on"))
                strategy.light.ena = light["on"].asBool();
            if (light.isMember("mode"))
                strategy.light.mode = (EnvStrategy::Light::ModeE) light["mode"].asInt();
            if (light.isMember("dur"))
                strategy.light.duration = light["dur"].asInt();
        }
    }
}

void CallBackResp::InvasionAlarmVoice::OnDownloadVoice(void *arg) {

}

OVD_int32 CallBackResp::ResetConfiguration() {
    emxlogd("ovd sdk reset config call reboot\n");
    m_ctx.env.initMsg.reason = (int)OVD_REBOOT_REASON_CLOUD_FACTORY_RESET;
    m_ctx.env.initMsg.message = "ovd sdk reset config call reboot";
    m_ctx.env.initMsg.ts = Time::GetMs();
    m_ctx.env.initMsg.Save();
    return 0;
}

#ifdef OVDSDK_APIVER_1_0
OVD_int32 CallBackResp::syncProbeDetector(OVD_int32 avgRTTThreshold, OVD_int32 checkPeriod,
                                          OVD_int32 cpuLoadThreshold, OVD_int32 memoryLoadThreshold,
                                          OVD_int32 memoryAvaliable, OVD_int32 packetLossRateThreshold,
                                          OVD_int32 reportSwitch, OVD_int32 wifiQualityThreshold) {
#ifndef EAPIL_SPARE
    m_ctx.softProbe.packetLossRateThreshold = packetLossRateThreshold;
    m_ctx.softProbe.avgRTTThreshold = avgRTTThreshold;
    m_ctx.softProbe.wifiQualityThreshold = wifiQualityThreshold;
    m_ctx.softProbe.cpuLoadThreshold = cpuLoadThreshold;
    m_ctx.softProbe.memoryLoadThreshold = memoryLoadThreshold;
    m_ctx.softProbe.memoryAvaliable = memoryAvaliable;
    m_ctx.softProbe.checkPeriod = checkPeriod;
    m_ctx.softProbe.reportSwitch = reportSwitch;
    m_ovd->m_softProbe.Update();
#endif
    return 0;
}
#endif

OVD_int32 CallBackResp::ProbeByval() {
    return m_ctx.env.cfg.softprobe_on;
}

#ifdef OVDSDK_APIVER_1_0
OVD_int32 CallBackResp::TriggerSoftProbe() {
    return 0;
}
#endif

OVD_int32 CallBackResp::PTZCmd(OVD_int32 channel, OVCPTZControlCmd ptzcmd, OVD_int32 ptzvalue) {
    return 0;
}

OVD_int32 CallBackResp::GetPresetList(OVD_int32 channel, OVD_int32 *presetList,
                                      OVD_int32 array_malloc_num, OVD_int32 *count) {
    return 0;
}


OVD_int32 CallBackResp::QueryRecordPage(OVD_uint32 channelmask1, OVD_uint32 channelmask2, OVD_uint32 recordType,
                                        OVD_uint64 StartStamp, OVD_uint64 EndStamp, OVD_int32 Page,
                                        OVD_int32 numInPage, OVDRecordFileListPerPage *FilePage) {
    return 0;
}

OVD_int32 CallBackResp::OpenRecordFile(OVD_int32 channel, OVD_char *recordname, OVDVideoDataFormat *videoInfo,
                                       OVDAudioDataFormat *audioInfo, OVD_int32 *fileTotalTime) {
    return 0;
}

OVD_int32 CallBackResp::RecordCotrol(OVD_int32 channel, OVDCONTROLTYPE controlType, OVD_int32 value) {
    return 0;
}

OVD_int32 CallBackResp::DMEAPI_callback_RecordSearch(OVD_int32 channel, OVD_uint64 starttime, OVD_uint64 endtime,
                                                     OVD_int32 page, OVD_int32 numInPage,
                                                     OVD_DMERecordFileListPerPage *fileinpage) {
    if (!m_ovd->m_record.m_sdcard.normal) {
        emxlogd("sdcard no mount\n");
        return -1;
    }
#ifndef EAPIL_SPARE
    //若传进的参数numInPage=10，page=2，则fileInPage应该返回第10个到第20个录像的信息;
    int offset = numInPage * page;
    //上次查询时间和当前查询时间不一致，重新查询
    if (m_lastStartStamp != starttime || m_lastEndStamp + 60 < endtime) {
        //根据卡录像查询接口返回的数据重新归类整理
        printf("%s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__);
        m_recordIndex.clear();
        m_ovd->m_record.GetFileIndex(starttime, endtime, m_recordIndex, true);
        //保存当前查询时间信息
        m_lastStartStamp = starttime;
        m_lastEndStamp = endtime;
    }
    int m_count = 0;
    int m_offset = 0;
    for (auto &file : m_recordIndex) {
        if (m_offset >= offset && m_count < numInPage) {
            fileinpage->sectioninfo[m_count].StartStamp = file.startUtc;
            fileinpage->sectioninfo[m_count].EndStamp = file.endUtc;
            m_count++;//计数累加
        }
        if (m_count >= numInPage) break;//count累加完成，推出循环
        m_offset++;//偏移累加
    }
    fileinpage->Count = m_count;
    emxlogi("m_count:%d, fileinpage->Count:%d\n", m_count, fileinpage->Count);
    if (m_count < numInPage) {//不为numInPage时，查询信息已完成，静态变量重置，释放资源
        m_lastStartStamp = 0;
        m_lastEndStamp = 0;
        m_recordIndex.clear();
        emxlogi("release\n");
    }
#endif
    return 0;
}

OVD_void *CallBackResp::DMEAPI_callback_RecordOpen(OVD_int32 channel) {
    if (!m_ovd->m_record.m_sdcard.normal) {
        emxlogd("sdcard no mount\n");
        return nullptr;
    }
#ifndef EAPIL_SPARE
    auto handle = new RecPlayHandle;
    handle->mp4Reader = new Mp4Reader;
    emxlogi("handle:%p\n", handle);
    return (void *) handle;
#else
    return nullptr;
#endif
}

#ifndef EAPIL_SPARE

int CallBackResp::RecPlayHandle::InitRecordReader(const char *path) {
    std::vector<MediaFmt::TrackInfo> mediaInfo;
    auto e = mp4Reader->Create(path, mediaInfo, nullptr);
    if (e != ErrCodeE::Success) {
        emxloge("create mp4 reader failed\n");
        return -1;
    }
    trackMap.clear();
    for (auto &item:mediaInfo) {
        trackMap[item.trackId] = item;
    }
    return 0;
}

#endif

OVD_int32 CallBackResp::DMEAPI_callback_RecordSeek(OVD_void *ctx, OVD_int64 timestamp) {
    if (!m_ovd->m_record.m_sdcard.normal) {
        emxlogd("sdcard no mount\n");
        return OVD_RET_SEEK_NODATA;
    }
#ifndef EAPIL_SPARE
    if (!ctx) {
        emxloge("handle null\n");
        return OVD_RET_SEEK_NODATA;
    }
    emxlogi("seek utc:%lld\n", timestamp);
    auto handle = (RecPlayHandle *) ctx;
    Record::RecFile file = {};
    char path[EMX_MAX_PATH_SIZE] = {};
    ErrCodeE e = m_ovd->m_record.Seek(timestamp, file, path);
    if (e == ErrCodeE::ReachTheEof) {
        emxlogi("SEEK REACH_THE_EOF\n");
        return OVD_RET_SEEK_NODATA;
    }
    if (timestamp < file.startUtc) {
        //Seek函数按照[startUTC,endUTC)的方式寻找
        //如果找不到，会返回startUTC大于timestamp，并离timestamp最近的file
        emxlogi("SEEK NO DATA\n");
        return OVD_RET_SEEK_NODATA;
    }
    if (handle->InitRecordReader(path) != 0) {
        emxloge("OVD_RET_COMMON_ERROR\n");
        return OVD_RET_COMMON_ERROR;
    }
    int64_t pos = (timestamp - file.startUtc) * 1000;
    handle->mp4Reader->Seek(pos);
    handle->curFile = file;
    handle->baseUtcMs = (int64_t) file.startUtc * 1000;//使用baseUtcMs作为后续读取Frame的基准
    handle->duration = 0;
    handle->lastVideoPts = -1;
#endif
    return 0;
}

OVD_int32 CallBackResp::DMEAPI_callback_RecordReadFrame(OVD_void *ctx, OVD_FrameInfo *pframe_info) {
    if (!m_ovd->m_record.m_sdcard.normal) {
        emxlogd("sdcard no mount\n");
        return OVD_RET_READ_FRAME_EOF;
    }
#ifndef EAPIL_SPARE
    if (!ctx) {
        emxloge("ctx == null\n");
        return OVD_RET_COMMON_ERROR;
    }
    auto handle = (RecPlayHandle *) ctx;
    if (!handle->mp4Reader) {
        emxloge("handle->mp4Reader == null\n");
        return OVD_RET_COMMON_ERROR;
    }
    MediaFmt::Frame frame = {};
    ErrCodeE e;
    auto endUtc = handle->curFile.endUtc;
    while ((e = handle->mp4Reader->Read(frame)) == ErrCodeE::ReachTheEof) {
        MediaReader::SpecInfo specInfo = {};
        handle->mp4Reader->GetSpecInfo(specInfo);
        // 使用duration来让两个连续的录像的pts连续
        handle->duration += (int64_t) specInfo.duration;
        handle->mp4Reader->Destroy();
        Record::RecFile file = {};
        char path[EMX_MAX_PATH_SIZE] = {};
        emxlogd("new seek utc = %u\n", handle->curFile.endUtc);
        e = m_ovd->m_record.Seek(handle->curFile.endUtc, file, path);
        if (e == ErrCodeE::ReachTheEof) {
            emxlogi("SEEK REACH_THE_EOF\n");
            return OVD_RET_READ_FRAME_EOF;
        }

        // 当录像不连续时，更新计算绝对UTCms的基准偏移
        if (file.startUtc != endUtc) {
            handle->baseUtcMs = (int64_t) file.startUtc * 1000;
            handle->duration = 0;
        }
        handle->curFile = file;
        if (handle->InitRecordReader(path) != 0) {
            emxloge("OVD_RET_COMMON_ERROR\n");
            return OVD_RET_COMMON_ERROR;
        }
    }
    if (e != ErrCodeE::Success) {
        emxloge("read mp4 frame failed\n");
        return OVD_RET_COMMON_ERROR;
    }
    memset(pframe_info, 0, sizeof(OVD_FrameInfo));
    pframe_info->frame_buf = (OVD_char *) frame.data;
    pframe_info->framebuf_len = frame.size;
    if (frame.type == MediaFmt::TrackTypeE::Video) {
        if (frame.size > 256) {
            if (!OvdUtils::EncryDecryMp4Video(frame.data, 128, frame.isKeyFrame)) {
                emxloge("EncryDecryMp4Video failed\n");
            }
        }
        auto &video = handle->trackMap[frame.trackId].info.video;
        pframe_info->frame_type = OVD_Video;
        pframe_info->timestamp = handle->baseUtcMs + handle->duration + (int64_t) frame.pts;
        handle->lastVideoPts = (int64_t) pframe_info->timestamp;
        switch (video.codec) {
            case VideoCodecE::H264:
                pframe_info->codec_type = source_STREAM_CODEC_H264;
                break;
            case VideoCodecE::H265:
                pframe_info->codec_type = source_STREAM_CODEC_H265;
                break;
            default:
                emxloge("unsupported codec %d\n", (int) video.codec);
                return OVD_RET_COMMON_ERROR;
        }
        pframe_info->video_info.iskey_frame = frame.isKeyFrame;
        pframe_info->video_info.width = video.width;
        pframe_info->video_info.height = video.height;
        pframe_info->video_info.framerate = 20;//sbhy
        pframe_info->video_info.gop = 60;//sbhy
    } else if (frame.type == MediaFmt::TrackTypeE::Audio) {
        //note: audio暂未加密
        // if (!OvdUtils::EncryDecryMp4Audio(frame.data, frame.size)) {
        //     emxloge("EncryDecryMp4Audio failed\n");
        // }
//        return OVD_RET_READ_FRAME_RETRY;
        if (handle->lastVideoPts == -1) {
            emxlogd("wait video frame\n");
            return OVD_RET_READ_FRAME_RETRY;
        }
        auto &audio = handle->trackMap[frame.trackId].info.audio;
        pframe_info->frame_type = OVD_Audio;
        pframe_info->timestamp = 1 + handle->lastVideoPts++;
//        pframe_info->timestamp = handle->baseUtcMs + handle->duration + (int64_t) frame.pts;
        switch (audio.codec) {
            case AudioCodecE::AAC:
                pframe_info->codec_type = source_STREAM_CODEC_AAC_WITH_ADTS;
                break;
            default:
                emxloge("unsupported codec %d\n", (int) audio.codec);
                return OVD_RET_COMMON_ERROR;
        }
        pframe_info->audio_info.samplesRate = audio.sampleRate;
        pframe_info->audio_info.channelNumber = audio.channel;
        pframe_info->audio_info.bitsPerSample = audio.bitWidth;
        pframe_info->audio_info.samplePerFrame = 1024;//sbhy
    } else {
        printf("%s:%d\n", __FUNCTION__, __LINE__);
        return OVD_RET_READ_FRAME_RETRY;
    }
//    printf("pframe_info[frame_type=%d,timestamp=%lld,framebuf_len=%d,baseUtcMs=%lld,duration=%lld,pts=%lld]",
//           pframe_info->frame_type, pframe_info->timestamp, pframe_info->framebuf_len,
//           handle->baseUtcMs, handle->duration, frame.pts);
//    for (int i = 0; i < 10; i++) {
//        printf("%02x ", pframe_info->frame_buf[i]);
//    }
//    printf("\n");
#endif
    return OVD_RET_SUCCESS;
}

OVD_int32 CallBackResp::DMEAPI_callback_RecordClose(OVD_void *ctx) {
    // if (!m_ovd->m_record.m_sdcard.normal) {
    //     emxlogd("sdcard no mount\n");
    //     return -1;
    // }
#ifndef EAPIL_SPARE
    if (!ctx) {
        emxloge("ctx == null\n");
        return -1;
    }
    auto handle = (RecPlayHandle *) ctx;
    if (!handle->mp4Reader) {
        emxloge("handle->mp4Reader == null\n");
        return -1;
    }
    handle->mp4Reader->Destroy();
    delete handle->mp4Reader;
    free(handle);
    emxlogd("handle: %p\n", handle);
#endif
    return 0;
}

OVD_int32 CallBackResp::GetSDInfo(OVDSDInfo *sdInfo) {
    return 0;
}

OVD_int32 CallBackResp::SetSDCardFormat() {
    return 0;
}

OVD_int32 CallBackResp::GetDiskInfo(OVD_int32 *out_state, OVD_int32 *out_total, OVD_int32 *out_free) {
    return 0;
}

OVD_int32 CallBackResp::GetOVDDeviceInfo(OVDDeviceInfo *deviceInfo) {
    strncpy(deviceInfo->OVDDeviceID, m_ctx.env.sn.Get(), sizeof(deviceInfo->OVDDeviceID));
    strncpy(deviceInfo->OVDHardWareModel, m_ctx.env.devInfo.GetInner()->type, sizeof(deviceInfo->OVDHardWareModel));
    strncpy(deviceInfo->OVDSystemVersion, m_ctx.env.devInfo.GetOuter()->version, sizeof(deviceInfo->OVDSystemVersion));
    return 0;
}

OVD_void CallBackResp::OVCConnectStatus(OVD_int32 connectStatus) {
    bool ovdSdkConnected = connectStatus == 0;
    if (ovdSdkConnected == m_ctx.running.ovdSdkConnected)
        return;
    m_ctx.running.ovdSdkConnected = ovdSdkConnected;
    if (ovdSdkConnected) {
        emxlogd("OVD_ONLINE,sleep status(%s)\n", m_ctx.running.sleep ? "OVD_CHANNEL_DISABLE" : "OVD_CHANNEL_ONLINE");
        OVD_updateOVDstate(OVD_ONLINE);
        OVD_updatechannelstate(0,
                               m_ctx.running.sleep ? OVD_CHANNEL_DISABLE : OVD_CHANNEL_ONLINE);
    } else {
        emxlogd("OVD_ABNORMAL\n");
        OVD_updateOVDstate(OVD_ABNORMAL);
        OVD_updatechannelstate(0, OVD_CHANNEL_ABNORMAL);
    }
    m_ctx.led->StatChange(Led::StatE::OvdConnected, ovdSdkConnected);
    //todo:proc ovdSdkConnected
}

OVD_int32 CallBackResp::ReBootChannel(OVD_int32 channel) {
    return 0;
}

#ifdef OVDSDK1_38_1
OVD_int32 CallBackResp::ReBootDevice(ovd_reboot_reason_e reason) {
    emxlogd("reboot reason[%d]\n", (int)reason);
    m_ctx.env.initMsg.reason = (int)reason;
    m_ctx.env.initMsg.message = "ovd sdk call reboot";
    m_ctx.env.initMsg.ts = Time::GetMs();
    m_ctx.env.initMsg.Save();
    return 0;
}
#else
OVD_int32 CallBackResp::ReBootDevice() {
    return 0;
}
#endif

OVD_int32 CallBackResp::KeepAwakenUtilExpired(OVD_int32 channel, OVD_int32 notAllowHibernate, OVD_int32 expired,
                                              OVDHibernateReason reason) {
    return 0;
}

OVD_int32 CallBackResp::getsimpleovdinfo(SimpleOVDinfo *out_simpleovdinfo) {
    if (!out_simpleovdinfo)
        return 0;
    if (m_ctx.running.updateEvent.stat == UpdateEvent::StatE::Start)
        out_simpleovdinfo->state = OVD_UPDATING;
    else if (m_ctx.running.ovdSdkConnected)
        out_simpleovdinfo->state = OVD_ONLINE;
    else
        out_simpleovdinfo->state = OVD_ABNORMAL;

    emxlogi("out_simpleovdinfo->state=%d\n", (int) out_simpleovdinfo->state);
    out_simpleovdinfo->channelcount = m_ctx.env.cfg.cnl_cnt;

    // 只支持一个通道
    out_simpleovdinfo->channelarray[0].state =
            m_ctx.running.sleep ? OVD_CHANNEL_OFFLINE : OVD_CHANNEL_ONLINE;
    out_simpleovdinfo->channelarray[0].channelnum = 0;
    emxlogi("channelarray[0] state=%d,channelnum=%d\n",
           (int) out_simpleovdinfo->channelarray[0].state,
           out_simpleovdinfo->channelarray[0].channelnum);
    return 0;
}

/**
    ####参数说明：
    "trans_id": <必填，字符串；标记此日志上传的任务ID>
    "start": <必填，字符串；日志记录开始时间，格式yyyy-MM-ddTHH:mm:ss，例子：2016-12-05T02:15:32>,
    "end": <必填，字符串；日志记录结束时间，格式yyyy-MM-ddTHH:mm:ss，例子：2016-12-06T02:15:32>,
    "url": <必填，字符串；日志上传的URL，设备通过PUT方法上传相应的日志文件>
 */
OVD_int32 CallBackResp::LogUploadAsync(OVD_char *trans_id, OVD_char *start, OVD_char *end, OVD_char *url) {
    auto arg = new UploadLogArg;
    arg->trans_id = trans_id;
    arg->start = start;
    arg->start.replace(10, 1, " ");//replace "2016-12-05T02:15:32" to "2016-12-05 02:15:32"
    arg->end = end;
    arg->end.replace(10, 1, " ");
    arg->url = url;
    arg->work.Create(&m_ctx.loop, arg, [this](void *arg) {
        auto handle = (UploadLogArg *) arg;
        //get log original file
        Log::Config cfg = {};
        emxLog.GetConfig(cfg);
        //gen new file
        char targetPath[EMX_MAX_PATH_SIZE] = {};
        strncpy(targetPath, cfg.normal.path, EMX_MAX_PATH_SIZE - 10);
        strcat(targetPath, ".ovd");
        emxlogd("upload log from [%s] to [%s]\n", cfg.normal.path, targetPath);
        //filter log
        FILE *fpSrc = fopen(cfg.normal.path, "r");
        if (!fpSrc) {
            emxlogd("cannot open %s\n", cfg.normal.path);
            return;
        }
        FILE *fpDst = fopen(targetPath, "w");
        if (!fpDst) {
            emxlogd("cannot open %s\n", targetPath);
            fclose(fpSrc);
            return;
        }

        char buffer[EMX_LOG_MAX_SIZE];
        int logCnt = 0;
        char act_start[32] = {0};
        char act_end[32] = {0};
        emxlogd("between[%s]-[%s]\n", handle->start.c_str(), handle->end.c_str());
        while (fgets(buffer, sizeof(buffer), fpSrc)) {
            //[2022-11-22 10:58:44.985] compare with 2016-12-05 02:15:32
            if (strncmp(buffer + 1, handle->start.c_str(), 19) > 0 &&
                strncmp(buffer + 1, handle->end.c_str(), 19) < 0) {
                fwrite(buffer, strlen(buffer) + 1, 1, fpDst);
                if (!logCnt)
                    memcpy(act_start, buffer + 1, 19);
                logCnt++;
                printf("logCnt=%d\n", logCnt);
                memcpy(act_end, buffer + 1, 19);
            }
        }
        fflush(fpSrc);
        fsync(fileno(fpSrc));        
        fclose(fpSrc);
        fflush(fpDst);
        fsync(fileno(fpDst));
        fclose(fpDst);

        act_start[10] = 'T';//replace "2016-12-05 02:15:32" to "2016-12-05T02:15:32"
        act_end[10] = 'T';

        //日志打包
        char tarcmd[256] = { 0 };
        char voiplog1[64] = { 0 };
        char voiplog2[64] = { 0 };
        char tarfile[64] = { 0 };
        snprintf(voiplog1, sizeof(voiplog1) - 1
                , "cmcc_voip_%s_%s_%s_A.txt"
                , m_ctx.deviceJsonCfg["voip"]["aCall"].asBool() ? "CameraCall" : "Camera"
                , m_ctx.env.manufacturers.deviceBrand
                , m_ctx.env.manufacturers.chipFactory);
        snprintf(voiplog2, sizeof(voiplog2) - 1
                , "cmcc_voip_%s_%s_%s_B.txt"
                , m_ctx.deviceJsonCfg["voip"]["aCall"].asBool() ? "CameraCall" : "Camera"
                , m_ctx.env.manufacturers.deviceBrand
                , m_ctx.env.manufacturers.chipFactory);
        bool is_sdcard = strstr(targetPath, "/tmp") != nullptr ? false : true;
        snprintf(tarfile, sizeof(tarfile) - 1, "%s/log.tar", is_sdcard ? "/mnt/sdcard" : "/tmp");
        if (is_sdcard) {
            snprintf(tarcmd, sizeof(tarcmd) - 1, "cp /tmp/cmcc_voip_* /mnt/sdcard/;tar -cvf %s %s /mnt/sdcard/%s /mnt/sdcard/%s", tarfile, targetPath, voiplog1, voiplog2);
        } else {
            snprintf(tarcmd, sizeof(tarcmd) - 1, "tar -cvf %s %s /tmp/%s /tmp/%s", tarfile, targetPath, voiplog1, voiplog2);
        }
        emxlogd("LogUploadAsync tar cmd: %s\n", tarcmd);
        Cmd::Run(tarcmd);

        //上传日志文件
        bool is_upload_ok = false;
        long file_size = File::Size(tarfile);
        char result[256] = {0};
        long http_code = 0;
        struct curl_slist *headers = nullptr;  
        headers = curl_slist_append(headers, "Content-Disposition: attachment;filename=log.tar");
        int ret = OvdUtils::https_put_func_with_headers(tarfile, handle->url.c_str(), result, headers, &http_code);
        if (ret < 0) {
            is_upload_ok = false;
            emxloge("upload file to server failed, code:%d, result:%s\n", http_code, result);
        } else {
            is_upload_ok = true;
            emxlogi("upload file to server ok. log_cnt:%d\n", logCnt);
        }
        if (headers != nullptr) {
            curl_slist_free_all(headers);
        }
        remove(tarfile);

        //通知OVC日志文件上传完成
        emxlogi("log done info, trans_id:%s, is_upload_ok:%d, act_start:%s, act_end:%s, url:%s, filesize:%ld\n",
               handle->trans_id.c_str(), !is_upload_ok, act_start, act_end, (char *) handle->url.c_str(), file_size);
        if (OVD_LogDone((char *) handle->trans_id.c_str(), !is_upload_ok, act_start, act_end,
                        (char *) handle->url.c_str(), file_size) < 0) {
            emxloge("OVD_LogDone failed, is_upload_ok:%d\n", !is_upload_ok);//result: 0 代表success / 1代表fail
        } else {
            emxlogi("OVD_LogDone ok\n");//result: 0 代表success / 1代表fail
        }
        emxlogi("log upload async proc end\n");
    }, [](Emx::ErrCodeE e, void *arg) {
        auto handle = (UploadLogArg *) arg;
        handle->work.Destroy();
        delete handle;
    });
    arg->work.Run();
    return 0;
}

OVD_int32 CallBackResp::extension_callback(char *method, char *in_data,
                                           char *out_response, OVD_int32 in_outresponse_len,
                                           OVD_int32 *out_outresponse_len) {
    return 0;
}

OVD_int32 CallBackResp::AudioPlayStart(OVD_int32 channel, OVDAudioOutDataFormat andioFormat) {
    return 0;
}

OVD_int32 CallBackResp::AudioPlayProGress(OVD_int32 channel, OVD_uchar *buf, OVD_int32 size) {
    return 0;
}

OVD_int32 CallBackResp::AudioPlayStop(OVD_int32 channel) {
    return 0;
}


OVD_int32 CallBackResp::SyncTime(OVD_char *datetime, OVD_int32 tz, OVD_int32 offset) {
    struct tm tm_time;
    int time;
    strptime(datetime, "%Y-%m-%dT%H:%M:%S", &tm_time);
    time = (unsigned int)mktime(&tm_time);
    int now = Time::GetS();
    emxlogd("time sync(%ld); current(%ld); offset(%d); tz(%d)\n"
                , time, now, offset, tz);
    if (abs(now - time) <= offset) {
        return 0;
    }
    Time::SetSysTime(time);
    TimeZone time_zone;
    ErrCodeE e = time_zone.Load();
    if (e != ErrCodeE::Success) {
        return -1;
    }
    char zone[24] = { 0 };
    if (tz >= 0) {
        snprintf(zone, sizeof(zone) - 1, "GMT-%d", tz);
    } else {
        snprintf(zone, sizeof(zone) - 1, "GMT+%d", abs(tz));
    }
    
    e = time_zone.Set(zone);
    //通知Ovd主类，时间同步完成
    m_ctx.timeSyncInfo.Send();
    return e == ErrCodeE::Success ? 0 : -1;
}

OVD_int32 CallBackResp::QueryTime(OVD_char *out_datetime, OVD_int32 time_len, OVD_int32 *tz) {
    char time[64] = { 0 };
    Time::GetTimeStrNow(time, sizeof(time));
    memset(out_datetime, 0 , time_len);
    memcpy(out_datetime, time, time_len);
    TimeZone time_zone;
    auto e = time_zone.Load();
    if (e != ErrCodeE::Success) {
        return -1;
    }
    std::string zone(time_zone.Get(),strlen(time_zone.Get()));
    std::string tmp = zone.substr(3, zone.size() - 3);
    int num = 0;
    if (strstr(tmp.c_str(), "+") != nullptr) {
        sscanf(tmp.c_str(), "+%d", &num);
        num = -num;
    } else if (strstr(tmp.c_str(), "-") != nullptr) {
        sscanf(tmp.c_str(), "-%d", &num);
    } else {
        sscanf(tmp.c_str(), "%d", &num);
    }
    *tz = num;
    return 0;
}

OVD_int32 CallBackResp::gettime(OVD_uint64 *out_time) {
    return 0;
}

OVD_int32 CallBackResp::settime(OVD_uint64 input_time, OVD_int32 tolerance_value) {
    uint64_t curTime = Time::GetMs64();
    auto differ_time = abs((long long) curTime - (long long) input_time);
    if (differ_time > tolerance_value * 1000) {
        timeval tv = {(__time_t) (input_time / 1000), (__suseconds_t) (input_time % 1000) * 1000};
        settimeofday(&tv, nullptr);
    }
    //通知Ovd主类，时间同步完成
    m_ctx.timeSyncInfo.Send();
    return 0;
}

OVD_int32 CallBackResp::FirmwareUpgrade(OVD_char *firmware_model, OVD_char *upgradeURL) {
    if (m_ctx.running.updateEvent.stat == UpdateEvent::StatE::Start) {
        emxloge("is updating\n");
        return -1;
    }
    time_t xtime;
    tm xtm = {};
    time(&xtime);
    localtime_r(&xtime, &xtm);
    snprintf(m_ctx.env.cfg.last_upgrade_time, sizeof(m_ctx.env.cfg.last_upgrade_time),
             "%04d-%02d-%02dT%02d:%02d:%02d",
             xtm.tm_year + 1900, xtm.tm_mon + 1, xtm.tm_mday, xtm.tm_hour, xtm.tm_min, xtm.tm_sec);
    //保存为failed，保证下次能够正常触发升级
    m_ctx.env.cfg.last_upgrade_state = OVD_STATUS_FAILED;
    m_ctx.env.cfg.Save();
    m_update.url = upgradeURL;
    m_update.work.Create(&m_ctx.loop, &m_update,
                         [this](void *arg) {
                             Misc::KillBeforeRun("EmxMediaServer");
                             //note: 等待资源完全释放(kill操作理论上会及时释放资源)
                             usleep(200 * 1000);
                             m_update.OnCurlPerform(arg);
                         },
                         [this](Emx::ErrCodeE e, void *arg) {
                             m_update.work.Destroy();
                         }
    );
    m_update.work.Run();
    m_ctx.env.initMsg.reason = (int)OVD_REBOOT_REASON_CLOUD_UPGRADE;
    m_ctx.env.initMsg.message = "upgrade file download failed, reboot";
    m_ctx.env.initMsg.ts = Time::GetMs();
    m_ctx.env.initMsg.Save();
    return 0;
}

void CallBackResp::Update::OnCurlPerform(void *arg) {
    CURL *curl = curl_easy_init();
    if (!curl) {
        emxloge("curl_easy_init() failed\n");
        return;
    }
    if (handle.Create(OvdCtx::UpdateSession) != ErrCodeE::Success) {
        emxloge("create update handle failed\n");
        curl_easy_cleanup(curl);
        return;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str()); //设置url地址

    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 120L);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnDownload); //设置回调函数
    curl_easy_setopt(curl, CURLOPT_WRITEDATA,  reinterpret_cast<void *>(&handle));   //设置写数据

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false); //设定为不验证证书和HOST
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);

    curl_easy_setopt(curl, CURLOPT_POST, 0L);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);

    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

    CURLcode res = curl_easy_perform(curl); //执行
    handle.Destroy();
    long http_code;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    emxlogi("<==============================================\n");
    emxlogi("res: %d\n", res);
    emxlogi("http_code: %ld\n", http_code);
    emxlogi("==============================================>\n");
    curl_easy_cleanup(curl);
    if (res != CURLE_OK) {
        emxloge("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        return;
    }
}

size_t CallBackResp::Update::OnDownload(void *ptr, size_t size, size_t number, void *stream) {
    auto handle = reinterpret_cast<UpdateClient *> (stream);
    handle->Push((const uint8_t *) ptr, (int32_t) (size * number));
    return size * number;
}

OVD_int32 CallBackResp::QueryFirmwareUpgradeStatus(OVDUpgradeStatus *upgradeStatus, OVD_int32 *upgradeProgress,
                                                   OVD_char *version, OVD_int32 version_len,
                                                   OVD_char *last_upgrade_time, OVD_int32 time_len) {
    if (m_ctx.env.initMsg.ota_update_status != EnvInitMsg::OtaUpdateStatus::unknown) {
        if (m_ctx.env.initMsg.is_normal_reboot 
            || m_ctx.env.initMsg.ota_update_status == EnvInitMsg::OtaUpdateStatus::DONE) {
            *upgradeStatus = OVD_STATUS_DONE;
            *upgradeProgress = 100;
        } else {
            if (m_ctx.env.initMsg.ota_update_status == EnvInitMsg::OtaUpdateStatus::INSTALLING ||
                m_ctx.env.initMsg.ota_update_status == EnvInitMsg::OtaUpdateStatus::DOWNLOADING ||
                m_ctx.env.initMsg.ota_update_status == EnvInitMsg::OtaUpdateStatus::IDLE) {
                auto &event = m_ctx.running.updateEvent;
                switch (event.stat) {
                    case UpdateEvent::StatE::Idle:
                        m_ctx.env.initMsg.ota_update_status = EnvInitMsg::OtaUpdateStatus::IDLE;
                        *upgradeStatus = OVD_STATUS_IDLE;
                        break;
                    case UpdateEvent::StatE::Start:
                        if (event.burn.total > 0) {
                            *upgradeStatus = OVD_STATUS_INSTALLING;
                            m_ctx.env.initMsg.ota_update_status = EnvInitMsg::OtaUpdateStatus::INSTALLING;
                        } else {
                            *upgradeStatus = OVD_STATUS_DOWNLOADING;
                            m_ctx.env.initMsg.ota_update_status = EnvInitMsg::OtaUpdateStatus::DOWNLOADING;
                        }
                        break;
                    case UpdateEvent::StatE::Done:
                        // *upgradeStatus = OVD_STATUS_DONE;
                        m_ctx.env.initMsg.is_normal_reboot = true;
                        m_ctx.env.cfg.last_upgrade_state = OVD_STATUS_DONE;
                        m_ctx.env.cfg.Save();
                        m_ctx.env.initMsg.reason = (int)OVD_REBOOT_REASON_CLOUD_UPGRADE;
                        m_ctx.env.initMsg.message = "installing, reboot";
                        m_ctx.env.initMsg.ts = Time::GetMs();
                        m_ctx.env.initMsg.ota_update_status = EnvInitMsg::OtaUpdateStatus::DONE;
                        m_ctx.env.initMsg.Save();
                        Reboot::DoReboot(3);
                        break;
                    case UpdateEvent::StatE::Error:
                        // *upgradeStatus = OVD_STATUS_FAILED;
                        m_ctx.env.initMsg.ota_update_status = EnvInitMsg::OtaUpdateStatus::FAILED;
                        m_ctx.env.initMsg.Save();
                        Reboot::DoReboot(3);
                        //开始升级回调FirmwareUpgrade中已经将状态保存为失败的状态
            //            m_ctx.env.cfg.last_upgrade_state = OVD_STATUS_FAILED;
            //            m_ctx.env.cfg.Save();
                        // m_ctx.env.initMsg.Save();
                        // return 0;
                    default:
                        emxlogc("unexpected stat %d\n", (int) event.stat);
                        return -1;
                }
                
                if (event.download.total == 0)
                    *upgradeProgress = 0;
                else {
                    int downloadProgress = event.download.current * 100 / event.download.total;
                    int burnProgress = event.burn.total > 0 ? event.burn.current * 100 / event.burn.total : 0;
                    *upgradeProgress = (downloadProgress + burnProgress) >> 1;
                }
                strncpy(version, m_ctx.env.devInfo.GetOuter()->version, version_len);
                strncpy(last_upgrade_time, m_ctx.env.cfg.last_upgrade_time, time_len);
                emxlogi("upgrade status:%d, progress:%d, ver:%s, last_upgrade_time:%s\n",
                    *upgradeStatus, *upgradeProgress, version, last_upgrade_time);
                return 0;
            } else {
                *upgradeStatus = OVD_STATUS_FAILED;
            }
        }
        m_ctx.env.initMsg.ota_update_status = EnvInitMsg::OtaUpdateStatus::unknown;
        m_ctx.env.initMsg.is_normal_reboot = false;
        m_ctx.env.initMsg.Save();    
        return 0;
    }
    m_ctx.env.initMsg.is_normal_reboot = false;
    m_ctx.env.initMsg.ota_update_status = EnvInitMsg::OtaUpdateStatus::unknown;
    auto &event = m_ctx.running.updateEvent;
    switch (event.stat) {
        case UpdateEvent::StatE::Idle:
            m_ctx.env.initMsg.ota_update_status = EnvInitMsg::OtaUpdateStatus::IDLE;
            *upgradeStatus = OVD_STATUS_IDLE;
            break;
        case UpdateEvent::StatE::Start:
            if (event.burn.total > 0) {
                *upgradeStatus = OVD_STATUS_INSTALLING;
                m_ctx.env.initMsg.ota_update_status = EnvInitMsg::OtaUpdateStatus::INSTALLING;
            } else {
                *upgradeStatus = OVD_STATUS_DOWNLOADING;
                m_ctx.env.initMsg.ota_update_status = EnvInitMsg::OtaUpdateStatus::DOWNLOADING;
            }
            break;
        case UpdateEvent::StatE::Done:
            // *upgradeStatus = OVD_STATUS_DONE;
            m_ctx.env.initMsg.is_normal_reboot = true;
            m_ctx.env.cfg.last_upgrade_state = OVD_STATUS_DONE;
            m_ctx.env.cfg.Save();
            m_ctx.env.initMsg.reason = (int)OVD_REBOOT_REASON_CLOUD_UPGRADE;
            m_ctx.env.initMsg.message = "installing, reboot";
            m_ctx.env.initMsg.ts = Time::GetMs();
            m_ctx.env.initMsg.ota_update_status = EnvInitMsg::OtaUpdateStatus::DONE;
            Reboot::DoReboot(3);
            break;
        case UpdateEvent::StatE::Error:
            // *upgradeStatus = OVD_STATUS_FAILED;
            m_ctx.env.initMsg.ota_update_status = EnvInitMsg::OtaUpdateStatus::FAILED;
            Reboot::DoReboot(3);
            //开始升级回调FirmwareUpgrade中已经将状态保存为失败的状态
//            m_ctx.env.cfg.last_upgrade_state = OVD_STATUS_FAILED;
//            m_ctx.env.cfg.Save();
            // m_ctx.env.initMsg.Save();
            // return 0;
        default:
            emxlogc("unexpected stat %d\n", (int) event.stat);
            return -1;
    }
    m_ctx.env.initMsg.Save();
    if (event.download.total == 0)
        *upgradeProgress = 0;
    else {
        int downloadProgress = event.download.current * 100 / event.download.total;
        int burnProgress = event.burn.total > 0 ? event.burn.current * 100 / event.burn.total : 0;
        *upgradeProgress = (downloadProgress + burnProgress) >> 1;
    }
    strncpy(version, m_ctx.env.devInfo.GetOuter()->version, version_len);
    strncpy(last_upgrade_time, m_ctx.env.cfg.last_upgrade_time, time_len);
    emxlogi("upgrade status:%d, progress:%d, ver:%s, last_upgrade_time:%s\n",
           *upgradeStatus, *upgradeProgress, version, last_upgrade_time);
    return 0;
}

OVD_int32 CallBackResp::VedioSwitchQuality(OVD_int32 channel,
                                           OVDEncodeQuality quality, OVDVideoDataFormat *vedioInfo) {
    return 0;
}

OVD_int32 CallBackResp::ForceIFrame(OVD_int32 channel) {
    return 0;
}

OVD_int32 CallBackResp::Snapshot(OVD_int32 channel, OVDImageInfo *imageInfo, OVD_int32 maxImageSize) {
    return 0;
}

#ifdef OVDSDK_APIVER_3_0
OVD_int32 CallBackResp::StopAlarm(int alarmtype) {
    return 0;
}
#endif

#ifdef OVDSDK_APIVER_3_0
#ifdef OVDSDK1_38_1
OVD_int32 CallBackResp::GetDevRunningInfo(ovd_probe_devrunning_info_e in_info, void* out_response)
#else
OVD_int32 CallBackResp::GetDevRunningInfo(OVD_GetDevRunningInfo_e in_info, void* out_response) 
#endif
{
    memset(out_response,0,64);
    OVD_int32 ret = -1;
    switch(in_info) {
        case OVD_CMD_GET_PACKET_LOSSRATE: /*丢包率 单位：%*/ {
            break;
        }
        case OVD_CMD_GET_WIFI_QUALITY: /*WIFI强度 单位：dB*/ {
            break;
        }
        case OVD_CMD_GET_CPU_LOAD: /*CPU占用率 单位：%*/ {
            break;
        }
        case OVD_CMD_GET_MEMORY_LOAD: /*内存占用率 单位：%*/ {
            break;
        }
        case OVD_CMD_GET_RUNTIME: /*设备运行时长 单位：s*/ {
            break;
        }
        case OVD_CMD_GET_MEMORY_AVAILABLE: /*剩余内存 单位：KB*/ {
            break;
        }
        case OVD_CMD_GET_MEMORY_TOTAL: /*运行总内存 单位：KB*/ {
            break;
        }
        case OVD_CMD_GET_FLASH_TOTAL: /*运行总闪存 单位：KB*/ {
            std::string str_romsize(m_ctx.env.manufacturers.romSize,sizeof(m_ctx.env.manufacturers.romSize));
            if (str_romsize.size() > 1 && std::string::npos != str_romsize.find('M')) {
                int i_romsize = std::stoi(str_romsize.substr(0, str_romsize.find('M')));
                *(int*)out_response = i_romsize * 1024;
                ret = 0;
            }
            break;
        }
        case OVD_CMD_GET_CHIP_SUPPLIER: /*芯片厂家 char[64]*/ {
            if (m_ctx.env.manufacturers.chipFactory[0] != '0') {
                int cpy_size = sizeof(m_ctx.env.manufacturers.chipFactory) > 64 
                                    ? 64 : sizeof(m_ctx.env.manufacturers.chipFactory);
                memcpy(out_response, m_ctx.env.manufacturers.chipFactory, cpy_size);
                ret = 0;
            }
            break;
        }
        case OVD_CMD_GET_CHIP_MODEL:  /*芯片型号 char[64]*/ {
            if (m_ctx.env.manufacturers.chipModel[0] != '0') {
                int cpy_size = sizeof(m_ctx.env.manufacturers.chipModel) > 64 
                                    ? 64 : sizeof(m_ctx.env.manufacturers.chipModel);
                memcpy(out_response, m_ctx.env.manufacturers.chipModel, cpy_size);
                ret = 0;
            }
            break;
        }
        case OVD_CMD_GET_HJZH_SDKVER: /*和家智话SDK版本号 char[64]*/ {
        #ifdef VoipVersion
            int cpy_size = strlen((char*)(cmcc_rtc_get_version())) > 64
                                ? 64 : strlen((char*)(cmcc_rtc_get_version()));
            memcpy(out_response, cmcc_rtc_get_version(), cpy_size);
            ret = 0;
        #endif
            break;
        }
        case OVD_CMD_GET_ANDLINK_SDKVER: /*Andlink SDK版本号 char[64]*/ {
        #ifdef AndLinkVersion
            int cpy_size = strlen((char*)(AndLinkVersion)) > 64
                                ? 64 : strlen((char*)(AndLinkVersion));
            memcpy(out_response, (char*)(AndLinkVersion), cpy_size);
            ret = 0;
        #endif
            break;
        }
        case OVD_CMD_GET_PING:
        case OVD_CMD_GET_TRACEROUTE:
        case OVD_CMD_GET_RSSI_4G:
        #ifdef OVDSDK1_38_1
        case OVD_CMD_GET_POWER_MODE:
        #endif
            ret = 101;
        break;
    }
    return ret;
}
#endif

OVD_int32 CallBackResp::SyncMacHash(OVD_char* in_machash) {
    if (in_machash == nullptr) {
        return -1;
    }
    m_ctx.env.initMsg.machash = std::string(in_machash);
    m_ctx.env.initMsg.Save();
    return 0;
}

OVD_int32 CallBackResp::SetAudioOutPlay(OVD_int32 channel, OVD_char *url, int repeat, int volume) {
    emxlogd("channel:%d url:%s repeat:%d volume:%d\n",channel,url,repeat,volume);
    m_audioOut.url = std::string(url);
    if (!m_audioOut.work.IsWorking()) {
        auto arg = new AudioOut::Arg;
        arg->ret = -1;
        arg->url = m_audioOut.url;
        arg->channel = channel;
        arg->repeat = repeat;
        arg->volume = volume;
        m_audioOut.work.Create(&m_ctx.loop, arg,
                                [this](void *arg) {
                                    auto audioArg = (AudioOut::Arg *) arg;
                                    long http_code = 0;
                                    char path[EMX_MAX_PATH_SIZE] = {};
                                    snprintf(path, sizeof(path), "%s",m_ctx.deviceJsonCfg["audioOutPath"].asCString());
                                    MediaClientAdecStreamSync::PlayCtrlQuit(0);
                                    char cmd[EMX_MAX_PATH_SIZE] = {};
                                    snprintf(cmd, sizeof(cmd) - 1, "rm -f %s", path);
                                    Cmd::System(cmd);
                                    emxlogd("url: %s\n",  audioArg->url.c_str());
                                    audioArg->ret = OvdUtils::https_download_file_func(
                                            audioArg->url.c_str(),
                                            path, &http_code);
                                },
                                [this](ErrCodeE e, void *arg) {
                                    auto audioArg = (AudioOut::Arg *) arg;
                                    emxlogd("audioOut download file result[%s]\n", audioArg->ret == 0 ? "ok" : "failed");
                                    if (audioArg->ret == 0) {
                                        auto &spkVolumeMap = m_ctx.deviceJsonCfg["spkVolumeMap"];
                                        int volume = audioArg->volume;
                                        int adecVolume;
                                        if (volume <= 20)
                                            adecVolume = spkVolumeMap[0].asInt();
                                        else if (volume <= 40)
                                            adecVolume = spkVolumeMap[1].asInt();
                                        else if (volume <= 60)
                                            adecVolume = spkVolumeMap[2].asInt();
                                        else if (volume <= 80)
                                            adecVolume = spkVolumeMap[3].asInt();
                                        else
                                            adecVolume = spkVolumeMap[4].asInt();
                                        MediaClientAdecStreamSync::PlayCtrlQuit(0);
                                        emxlogd("audioOut play volume: %d\n", adecVolume);
                                        MediaClientAdec adec(0);
                                        std::string path = m_ctx.deviceJsonCfg["audioOutPath"].asString();
                                        for (int i = 0; i < audioArg->repeat; i++) {
                                            // MediaClientAdecStreamSync::PlayFileWithVolume(0, path.c_str(), adecVolume);
                                            MediaClientAdecStreamSync::PlayFile(0, path.c_str());
                                        }
                                    }
                                    m_invasion.work.Destroy();
                                    delete audioArg;
                                    audioArg = nullptr;
                                });
        m_audioOut.work.Run();
    }
    return 0;
}
