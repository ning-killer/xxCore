/*
* @Author: xiong
* @Date: 2022/10/26
*/

#include "AlarmInvasion.hpp"
#include "NightVisionClient.hpp"

using namespace Emx;

ErrCodeE AlarmInvasion::Create() {
    m_ai = new MediaClientAiDataAsync(m_ctx->loop);
    m_ai->Start(nullptr, [this](MediaAi::AiInfoHeader &header) {
        if (header.type != MediaAi::AiTypeE::PersonDetection)
            return;
        if (!m_ctx->env.invasion.ena) {
            IntelligentLight(15);
            return;
        }
        if (m_ai->GetDataByInfo(header) != ErrCodeE::Success)
            return;
        auto *data = (Rect *) header.data;
        for (int i = 0; i < header.size / (int) sizeof(Rect); i++) {
            if (AlarmInZone(header.srcPicSize, data[i], m_ctx->env.invasion.zone)) {
                bool started = m_started;
                AlarmTrigger();
                //触发开始
                if (m_started && !started) {
                    Strategy();
                }
            }
        }
    });
    m_lightDuration.Create(m_ctx->loop);
    m_lightBlink.Create(m_ctx->loop);
    m_playStatusUpate.Create(m_ctx->loop);
    m_lightIntelligent.Create(m_ctx->loop);
    return ErrCodeE::Success;
}

void AlarmInvasion::Destroy() {
    m_ai->Stop();
    delete m_ai;
    m_lightDuration.Destroy();
    m_lightBlink.Destroy();
    m_playStatusUpate.Destroy();
    m_lightIntelligent.Destroy();
}

bool AlarmInvasion::IsStopAudio() {
    if (!m_ctx->env.invasion.ena
        || m_ctx->running.voip.isRunVideoCall) {
        MediaClientAdecStreamSync::PlayCtrlQuit(0);
        return true;
    }
    return false;
}

void AlarmInvasion::ExitLightContorl() {
    {
        std::lock_guard<std::mutex> locker(m_lightExit_mtx); 
        m_lightBlink.Stop();
        NightVisionClient nightVision;
        NightVision::Param param = {};
        param.manual = true;
        param.manualMode = NightVision::ModeE::Day;
        // 先配置为白天
        nightVision.SetParamTemp(param);
        // 然后恢复持久化配置
        nightVision.GetParam(param);
        nightVision.SetParamTemp(param);
    }
    //这里借用一下m_lightDuration这个timer
    //为了防止画面闪烁或IR-全彩切换后的收敛过程引起移动告警
    //延迟3秒恢复Motion的告警
    m_lightDuration.Start(3000, 0, [this]() { 
        m_isProcessing = false;
        m_ctx->running.alarm.isLighting = false;
        });
}

void AlarmInvasion::CheckLightContorl() {
    // 动态检测告警开关关闭或者对讲开启后，中断灯控闪烁。
    if (!m_ctx->env.invasion.ena 
        || m_ctx->running.voip.isRunVideoCall) {
        ExitLightContorl();
    }

    // 闪灯前，若云广播已经开启则不中断灯控闪烁。
    // 闪灯前，若云广播未开启则需要动态检测广播状态并允许中断灯控闪烁。
    if (!m_cloudBroadcastEnable && m_ctx->running.voip.isRunVoiceCall) {
        ExitLightContorl();
    }
}

void AlarmInvasion::BreakVoip() {
    if (m_ctx->running.voip.handle != nullptr) {
        m_ctx->running.voip.handle->HangUp();
    }
}

bool AlarmInvasion::GetIsPlaying() { 
    MediaClientAdec adec_client(0);
    MediaAdec::PlayStatusE status = adec_client.GetPlayStatus();
    emxlogd("status(%d)\n", status);
    return status == MediaAdec::PlayStatusE::Playing ? true : false; 
}

void AlarmInvasion::UpdatePlayStatus() {
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
            m_playStatusUpate.Stop();
        } while(false);
    });
}

void AlarmInvasion::Strategy() {
    emxlogd("AlarmInvasion::Strategy\n");
    // 中断控制
    BreakVoip();

    // 智能夜视控制
    m_isIntelligentNightVision = m_ctx->env.cfg.intelligentNightVision; 
    IntelligentLight();

    // 告警声控
    if (m_ctx->env.invasion.strategy.speech.ena) {
        char path[EMX_MAX_PATH_SIZE] = {};
        snprintf(path, sizeof(path), "%s/alarmVoice%c.wav",
                 m_ctx->deviceJsonCfg["alarmVoiceDir"].asCString(),
                 m_ctx->env.invasion.strategy.speech.playVoiceA ? 'A' : 'B');
        if (!File::Exist(path)) {
            snprintf(path, sizeof(path), "%s/audio/alarm.aac", Misc::GetAppConfigsDir());
        }
        auto &spkVolumeMap = m_ctx->deviceJsonCfg["spkVolumeMap"];
        int volume = m_ctx->env.invasion.strategy.speech.volume;
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
        for (int i = 0; i < m_ctx->env.invasion.strategy.speech.repeat; i++) {
            if (IsStopAudio()) { break; }
            MediaClientAdecStreamSync::PlayFileWithVolume(0, path, -2);
        }
    }

    // 告警灯控
    if (m_ctx->env.invasion.strategy.light.ena) {
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
        m_lightDuration.Start(m_ctx->env.invasion.strategy.light.duration * 1000, 0, [this]() {
            ExitLightContorl();
        });
        m_cloudBroadcastEnable = m_ctx->running.voip.isRunVoiceCall;
        if (m_ctx->env.invasion.strategy.light.mode == EnvStrategy::Light::ModeE::Blink) {
            m_lightBlink.Start(300, 300, [this]() {
                m_ctx->running.alarm.isLighting = true;
                //trigger on off
                m_ctx->bsp.whiteLight->SetEna(!m_ctx->bsp.whiteLight->GetEna());
                CheckLightContorl();
            });
        } else {
            m_ctx->bsp.whiteLight->SetEna(true);
        }
    }
}

bool AlarmInvasion::IsFlashingLight() {
    if (m_ctx->bsp.whiteLight->GetEna()
        && (m_ctx->bsp.whiteLight->GetLuma() > (Bsp::WhiteLight::LumaMinValue + 1))) {
        emxloge("whiteLight is turn on\n");
        return false;
    }
    return true;
}

void AlarmInvasion::IntelligentLight(const uint32_t frequency_time) {
    if (!m_isIntelligentNightVision) {
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
    }
    emxlogd("current status: %d\n", param.manualMode);
    nightVision.SetParamTemp(param);

    m_lightIntelligent.Start(500, 300, [this]() {
        if (m_ctx->running.alarm.isAudioAlarm 
            && m_ctx->env.invasion.ena) {
            return;
        }
        m_lightIntelligent.Start(28 * 1000, 0, [this]() { 
            NightVisionClient nightVision;
            NightVision::Param param = {};
            nightVision.GetParam(param);
            nightVision.SetParamTemp(param);
            m_lightIntelligent.Stop();
        });
    });
}