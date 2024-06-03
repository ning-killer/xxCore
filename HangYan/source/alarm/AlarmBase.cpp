/*
 * @Author: xiong
 * @Date: 2022/11/4
*/

#include "AlarmBase.hpp"
#include "NightVisionClient.hpp"

using namespace Emx;


bool AlarmBase::AlarmInZone(Size &srcImgSize, Rect &rect, Zone &zone) {
    Size dstSize(Zone::Width, Zone::Height);
    auto dstRect = rect.Transfer(srcImgSize, dstSize);
    for (int y = dstRect.y; y < dstRect.y + dstRect.h; y++)
        for (int x = dstRect.x; x < dstRect.x + dstRect.w; x++)
            if (zone.map[dstSize.w * y + x])
                return true;
    return false;
}

bool AlarmBase::CheckHeadSrcSize(const Size &srcImgSize) {
    if (srcImgSize.h == 0 || srcImgSize.w == 0) {
        return false;
    }
    return true;
}

const char *AlarmBase::GetAlarmName(OVDAlarmType type) {
    switch (type) {
        case OVD_OUTTER:
            return "OutterAlarm";
        case OVD_MOTIOM:
            return "MotionAlarm";
        case OVD_CROSS:
            return "CrossAlarm";
        case OVD_CRY:
            return "CryAlarm";
        case OVD_FACE:
            return "FaceAlarm";
        case OVD_VOICE:
            return "VoiceAlarm";
        case OVD_MAN:
            return "OVD_MAN";
        case OVD_PIR:
            return "OVD_PIR";
        case OVD_ALERTAREA:
            return "OVD_ALERTAREA";
        case OVD_OTHER:
            return "OtherAlarm";
        case OVD_TRANSGRESSION:
            return "OVD_TRANSGRESSION";
        case OVD_VEHICLE_GEN:
            return "OVD_VEHICLE_GEN";
        case OVD_VEHICLE_MOTOR:
            return "OVD_VEHICLE_MOTOR";
        case OVD_FACE_MASK:
            return "OVD_FACE_MASK";
        case OVD_REGIONAL_PEOPLE_STAT:
            return "OVD_REGIONAL_PEOPLE_STAT";
        case OVD_OFF_DUTY:
            return "OVD_OFF_DUTY";
        default:
            return "unknown alarm";
    }
}

void AlarmBase::AlarmTrigger() {
    if (!m_ena)
        return;
    if (m_started) {
        //如果SDK未联网则告警立刻结束
        if (!m_ctx->running.ovdSdkConnected) {
            AlarmEnd();
            return;
        }
        //更新告警事件结束timer，重新记时AlarmEndJudgeTime
        m_endJudgeTimer.Start(AlarmEndJudgeTime, 0, [this]() {
            //检测到告警事件已经结束，如果此时达到了告警最小持续时间则触发告警结束回调
            //如果网络未连接
            m_alarmEnd = true;
            if (m_alarmContinueReachMin) AlarmEnd();
        });
    } else {
        //如果SDK未联网则不进行告警
        if (!m_ctx->running.ovdSdkConnected)
            return;
        m_started = true;
        m_cb(m_type, true);
        m_endJudgeTimer.Start(AlarmEndJudgeTime, 0, [this]() {
            //检测到告警事件已经结束，如果此时达到了告警最小持续时间则触发告警结束回调
            m_alarmEnd = true;
            if (m_alarmContinueReachMin) AlarmEnd();
        });
        m_continueMinTimer.Start(AlarmContinueTimeMin, 0, [this]() {
            //已经达到告警最小持续时间，如果此时告警事件已经结束则触发告警结束回调
            m_alarmContinueReachMin = true;
            if (m_alarmEnd) AlarmEnd();
        });
        m_continueMaxTimer.Start(AlarmContinueTimeMax, 0, [this]() {
            //已经达到告警最大持续时间，直接触发告警结束回调
            AlarmEnd();
        });
    }
}

void AlarmBase::AlarmEnd() {
    m_started = false;
    m_cb(m_type, false);
    m_alarmEnd = false;
    m_alarmContinueReachMin = false;
    m_endJudgeTimer.Stop();
    m_continueMinTimer.Stop();
    m_continueMaxTimer.Stop();
}

void AlarmBase::Strategy(EnvStrategy *strategy, bool *ena, std::string alarmVoice, bool ContorlIntelligentLight) {
    emxlogd("AlarmBase::Strategy\n");
    m_strategyType = strategy->type;
    // 中断控制
    if (strategy->speech.ena) {
        BreakVoip();
    }

    // 智能夜视控制
    if (ContorlIntelligentLight) {
        m_isIntelligentNightVision = m_ctx->env.cfg.intelligentNightVision; 
        IntelligentLight(ena);
    }
    
    // 告警声控
    if (strategy->speech.ena) {
        char path[EMX_MAX_PATH_SIZE] = {};
        snprintf(path, sizeof(path), "%s/%s%c.wav",
                 m_ctx->deviceJsonCfg["alarmVoiceDir"].asCString(),
                 alarmVoice.c_str(),
                 strategy->speech.playVoiceA ? 'A' : 'B');
        if (!File::Exist(path)) {
            snprintf(path, sizeof(path), "%s/audio/alarm.aac", Misc::GetAppConfigsDir());
        }
        auto &spkVolumeMap = m_ctx->deviceJsonCfg["spkVolumeMap"];
        int volume = strategy->speech.volume;
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
        emxlogd("alarmInvasion play volume: %d\n", adecVolume);
        UpdatePlayStatus();
        MediaClientAdec adec(0);
        adec.SetVolumeTemp(adecVolume);
        m_ctx->soundAlarmControl.Update(m_strategyType, SoundAlarmControl::Status::On);
        for (int i = 0; i < strategy->speech.repeat; i++) {
            if (IsStopAudio(ena)) { break; }
            MediaClientAdecStreamSync::PlayFileWithVolume(0, path, -2);
        }
    }

    // 告警灯控
    if (strategy->light.ena) {
        m_isProcessing = true;
        // 检验是否控灯
        if(!IsFlashingLight()) {
            emxlogd("no flashing light!.\n");
            m_ctx->running.alarm.isLighting = false;
            m_lightDuration.Start(3000, 0, [this]() { 
                m_isProcessing = false;
                m_ctx->running.alarm.isLighting = false;
            });
            return;
        }

        m_ctx->bsp.whiteLight->SetLuma(Bsp::WhiteLight::LumaMaxValue);
        m_lightDuration.Start(strategy->light.duration * 1000, 0, [this]() {
            ExitLightContorl();
        });
        m_cloudBroadcastEnable = m_ctx->running.voip.isRunVoiceCall;
        if (strategy->light.mode == EnvStrategy::Light::ModeE::Blink) {
            m_enaFlag = ena;
            m_lightBlinkEnaFlag = &strategy->light.ena;
            m_lightBlink.Start(300, 300, [this]() {
                m_ctx->running.alarm.isLighting = true;
                //trigger on off
                m_ctx->bsp.whiteLight->SetEna(!m_ctx->bsp.whiteLight->GetEna());
                CheckLightContorl(m_enaFlag, m_lightBlinkEnaFlag);
            });
        } else {
            m_ctx->bsp.whiteLight->SetEna(true);
        }
    }
}

void AlarmBase::BreakVoip() {
    if (m_ctx->running.voip.handle != nullptr) {
        m_ctx->running.voip.handle->HangUp();
    }
}

void AlarmBase::IntelligentLight(bool *ena, const uint32_t frequency_time) {
    if (!m_ctx->env.cfg.intelligentNightVision) {
        return;
    }
    uint32_t currentTime = Time::GetS();
    if ((currentTime - m_lastCheckTime) < frequency_time) {
        return;
    }
    m_lastCheckTime = currentTime;
    // 智能夜视
    NightVisionClient nightVision;
    NightVision::Param param = {};
    NightVision::Status status = { 0 };
    nightVision.GetStatus(status);
    param.manual = true; 
    param.manualMode = status.mode;
    if (status.mode == NightVision::ModeE::IrNightVision) {
        param.manualMode = NightVision::ModeE::ColorNightVision;
        emxlogd("current status: %d\n", param.manualMode);
        nightVision.SetParamTemp(param);
    } else {
        emxlogd("current is ColorNightVision\n");
        // return;
    }
    m_lightIntelligentEnaFlag = ena;
    m_lightIntelligent.Start(500, 300, [this]() {
        if (m_ctx->running.alarm.isAudioAlarm && (*m_lightIntelligentEnaFlag)) {
            return;
        }
        m_lightIntelligent.Start(35 * 1000, 0, [this]() { 
            NightVisionClient nightVision;
            NightVision::Param param = {};
            nightVision.GetParam(param);
            nightVision.SetParamTemp(param);
            m_lightIntelligent.Stop();
        });
    });
}

void AlarmBase::UpdatePlayStatus() {
    if (m_ctx->running.voip.isRunVoiceCall) {
        m_ctx->running.alarm.isPlayVoiceCall = false;    
    } else {
        m_ctx->running.alarm.isPlayVoiceCall = true;   
    }
    m_ctx->running.alarm.isAudioAlarm = true;
    //延迟200ms，防止play还未送至buffer
    m_playStatusUpate.Start(200, 200, [this]() {
        do {
            if (!GetIsPlaying()) {
                m_playStatusCount++;
            } else {
                m_playStatusCount = 0;
            }
            if (m_playStatusCount < 4) {
                //需校验连续捕获到5次结束play状态
                emxlogd("alarm playing: count(%d)\n", m_playStatusCount);
                break;
            }
            m_playStatusCount = 0;
            emxlogd("alarm play end\n");
            m_ctx->running.alarm.isPlayVoiceCall = true;
            m_ctx->running.alarm.isAudioAlarm = false;
            m_ctx->soundAlarmControl.Update(m_strategyType, SoundAlarmControl::Status::Off);
            m_playStatusUpate.Stop();
        } while(false);
    });
}

bool AlarmBase::GetIsPlaying() { 
    MediaClientAdec adec_client(0);
    MediaAdec::PlayStatusE status = adec_client.GetPlayStatus();
    emxlogd("status(%d)\n", status);
    return status == MediaAdec::PlayStatusE::Playing ? true : false; 
}

bool AlarmBase::IsStopAudio(bool *ena) {
    if (!(*ena)
        || m_ctx->running.voip.isRunVideoCall) {
        MediaClientAdecStreamSync::PlayCtrlQuit(0);
        return true;
    }
    return false;
}

bool AlarmBase::IsFlashingLight() {
    if (m_ctx->bsp.whiteLight->GetEna()
        && (m_ctx->bsp.whiteLight->GetLuma() > (Bsp::WhiteLight::LumaMinValue + 1))) {
        emxloge("whiteLight is turn on\n");
        return false;
    }
    return true;
}

void AlarmBase::ExitLightContorl(bool isResetDay) {
    {
        std::lock_guard<std::mutex> locker(m_lightExit_mtx); 
        m_lightBlink.Stop();
        NightVisionClient nightVision;
        NightVision::Param param = {};
        if (isResetDay) {
            param.manual = true;
            param.manualMode = NightVision::ModeE::Day;
            // 先配置为白天
            nightVision.SetParamTemp(param);
            // 然后恢复持久化配置
            nightVision.GetParam(param);
            nightVision.SetParamTemp(param);
        }
    }
    //这里借用一下m_lightDuration这个timer
    //为了防止画面闪烁或IR-全彩切换后的收敛过程引起移动告警
    //延迟3秒恢复Motion的告警
    m_lightDuration.Start(3000, 0, [this]() { 
        m_isProcessing = false;
        m_ctx->running.alarm.isLighting = false;
        });
}

void AlarmBase::CheckLightContorl(bool *ena, bool *light) {
    // 动态检测告警开关关闭或者对讲开启后，中断灯控闪烁。
    if (!(*ena) 
        || !(*light)
        || m_ctx->running.voip.isRunVideoCall) {
        ExitLightContorl();
    }

    // 闪灯前，若云广播已经开启则不中断灯控闪烁。
    // 闪灯前，若云广播未开启则需要动态检测广播状态并允许中断灯控闪烁。
    if (!m_cloudBroadcastEnable && m_ctx->running.voip.isRunVoiceCall) {
        ExitLightContorl();
    }

    // 闪灯中，中断广播状态，将m_cloudBroadcastEnable标志位还原，保证该场景重新唤起广播可中断灯控闪烁
    if (m_cloudBroadcastEnable) {
        if (!m_ctx->running.voip.isRunVoiceCall) {
            m_cloudBroadcastEnable = false;
        }
    }
}

void AlarmBase::BreakLastSoundAndLightAlarm() {
    MediaClientAdecStreamSync::PlayCtrlQuit(0);
    ExitLightContorl(false);
}