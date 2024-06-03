#include "AlarmAlertarea.hpp"
#include "cmath"

using namespace Emx;

ErrCodeE AlarmAlertarea::Create() {
    m_ctx->soundAlarmControl.Update(
        SoundAlarmControl::Type::ALERTAREA, SoundAlarmControl::Status::Off);
    m_ai = new MediaClientAiDataAsync(m_ctx->loop);
    m_ai->Start(nullptr, [this](MediaAi::AiInfoHeader &header) {
        if (!m_ctx->env.alertarea.on 
            || m_ctx->env.alertarea.type == EnvAlarmAlertarea::notsupport) {
            if (m_ctx->env.cfg.nightvision_detect_mode == EnvCfg::NightvisionDetectMode::person) {
                emxlogt("HandleIntelligentLight\n");
                HandleIntelligentLight(header);
            }
            return;
        }
        if (header.type != MediaAi::AiTypeE::ClassifyDetection) {
            return;
        }   
        if (m_ai->GetDataByInfo(header) != ErrCodeE::Success) {
            return;
        }
        if (!CheckHeadSrcSize(header.srcPicSize)) {
            emxloge("header.srcPicSize.h[%d];header.srcPicSize.w[%d]\n", header.srcPicSize.h, header.srcPicSize.w);
            return;
        }
        auto *data = (MediaAi::AiInfoClassifyData *) header.data;
        int maxArea = 0;
        int maxAreaIdx = 0;
        int isHaveCorss = false;
        bool isHavePerson = false;
        for (int i = 0; i < header.size / (int) sizeof(MediaAi::AiInfoClassifyData); i++) {
            emxlogt("Sub TargetType[%d]->Env TargetType[%d]\n", (int)data[i].type, (int)m_ctx->env.alertarea.type);
            if (data[i].type == MediaAi::ClassifyTypeE::Person) {
                isHavePerson = true;
            }
            if (!isSupportedTargetType(data[i].type)) {
                continue;
            }
            if (AlarmInZone(header.srcPicSize, data[i].rect, m_ctx->env.alertarea.zone)) {
                isHaveCorss = true;
                if (data[i].rect.Area() > maxArea) {
                    maxArea = data[i].rect.Area();
                    maxAreaIdx = i;
                }
            }
        }
        if (!isHaveCorss) {
            if (isHavePerson && (m_ctx->env.cfg.nightvision_detect_mode == EnvCfg::NightvisionDetectMode::person)) {
                IntelligentLight(&m_ctx->env.alertarea.on, 5);
            }
            return;
        }
        m_checkAlgStatusTimer.Stop();
        m_checkAlgStatusTimer.Start(2000, 0, [this]() {
            //note: 2s内未检测到目标输出，则认为目标离开，逗留时间清零, 目标id重置
            m_cacheTargetTime = Time::GetS();
            m_cacheTargetId = -1;
        });
        int currentTime = Time::GetS();
        emxlogt("targetid[%d];lastTargetid[%d]\n", data[maxAreaIdx].id, m_cacheTargetId);
        if ((int)data[maxAreaIdx].id != m_cacheTargetId) {
            //note: 目标切换, 逗留时间清零
            m_cacheTargetTime = currentTime;
        }
        m_cacheTargetId = data[maxAreaIdx].id;
        // 灵敏度判断（同人形检测阈值转换保持一致）: 无需校验灵敏度
        // int targetArea;
        // if (m_ctx->env.person.sensitivity <= 20) {
        //     targetArea = m_ctx->env.person.map[0];
        // } else if (m_ctx->env.person.sensitivity <= 40) {
        //     targetArea = m_ctx->env.person.map[1];
        // } else if (m_ctx->env.person.sensitivity <= 60) {
        //     targetArea = m_ctx->env.person.map[2];
        // } else if (m_ctx->env.person.sensitivity <= 80) {
        //     targetArea = m_ctx->env.person.map[3];
        // } else {
        //     targetArea = m_ctx->env.person.map[4];
        // }
        // double currentTraget = (double)(maxArea) / (double)(header.srcPicSize.w * header.srcPicSize.h) * 1000;
        // emxlogt("currentTarget[%.2f]->targetArea[%d]\n", currentTraget, targetArea);
        bool isAlarm = false;
        int duringTime = currentTime - m_cacheTargetTime;
        emxlogt("during time[%d]->staymodeTime[%d]\n", duringTime, m_ctx->env.alertarea.staymodeTime);
        if (duringTime >= m_ctx->env.alertarea.staymodeTime) {
            if (m_ctx->env.alertarea.staymodeTime == 0) {
                Alarm(currentTime, data[maxAreaIdx].id, isHavePerson);
                isAlarm = true;
            } else {
                // // 同一个目标
                // if ((int)data[maxAreaIdx].id == m_lastAlarmTargetId) {
                //     Alarm(currentTime, data[maxAreaIdx].id);
                // }
                //note: 区域入侵暂时不用检验是否为同一个目标
                Alarm(currentTime, data[maxAreaIdx].id, isHavePerson);
                isAlarm = true;
            }
            
        }
        if (!isAlarm && isHavePerson && (m_ctx->env.cfg.nightvision_detect_mode == EnvCfg::NightvisionDetectMode::person)) {
            IntelligentLight(&m_ctx->env.alertarea.on, 5);
        }
    });
    m_checkAlgStatusTimer.Create(m_ctx->loop);
    return ErrCodeE::Success;
}

void AlarmAlertarea::Destroy() {
    m_checkAlgStatusTimer.Destroy();
    m_ai->Stop();
    delete m_ai;
}

bool AlarmAlertarea::isSupportedTargetType(const MediaAi::ClassifyTypeE type) {
    if (type == MediaAi::ClassifyTypeE::Person && 
        (m_ctx->env.alertarea.type == EnvAlarmAlertarea::TargetType::person ||
        m_ctx->env.alertarea.type == EnvAlarmAlertarea::TargetType::personAndmotor ||
        m_ctx->env.alertarea.type == EnvAlarmAlertarea::TargetType::personAndnonmotor ||
        m_ctx->env.alertarea.type == EnvAlarmAlertarea::TargetType::personAndmotorAndnonmotor)) {
        return true;
    }
    if (type == MediaAi::ClassifyTypeE::MotorVehicle && 
        (m_ctx->env.alertarea.type == EnvAlarmAlertarea::TargetType::motorvehicle ||
        m_ctx->env.alertarea.type == EnvAlarmAlertarea::TargetType::personAndmotor ||
        m_ctx->env.alertarea.type == EnvAlarmAlertarea::TargetType::motorAndnonmotor ||
        m_ctx->env.alertarea.type == EnvAlarmAlertarea::TargetType::personAndmotorAndnonmotor)) {
        return true;
    }
    if (type == MediaAi::ClassifyTypeE::NonMotorVehicle && 
        (m_ctx->env.alertarea.type == EnvAlarmAlertarea::TargetType::nonmotorvehicle ||
        m_ctx->env.alertarea.type == EnvAlarmAlertarea::TargetType::personAndnonmotor ||
        m_ctx->env.alertarea.type == EnvAlarmAlertarea::TargetType::motorAndnonmotor ||
        m_ctx->env.alertarea.type == EnvAlarmAlertarea::TargetType::personAndmotorAndnonmotor)) {
        return true;
    }
    return false;
}

void AlarmAlertarea::Alarm(int time, int id, bool isHavePerson) {
    bool intelligent = false;
    if (isHavePerson && m_ctx->env.cfg.nightvision_detect_mode == EnvCfg::NightvisionDetectMode::person) {
        intelligent = true;
    }
    bool started = m_started;
    AlarmTrigger();
    if (m_started && !started) {
        BreakLastSoundAndLightAlarm();
        Strategy(&m_ctx->env.alertarea.strategy, &m_ctx->env.alertarea.on, "alertareaAlarmVoice", intelligent);
    } else {
        emxlogt("alarm IntelligentLight:[%d:%d]\n", m_started, !started);
        IntelligentLight(&m_ctx->env.alertarea.on, 5);
    }
}

void AlarmAlertarea::HandleIntelligentLight(MediaAi::AiInfoHeader &header) {
    if (m_ctx->env.alertarea.on) {
        return;
    }
    // 不依赖alertarea开关
    if (header.type != MediaAi::AiTypeE::ClassifyDetection) {
        return;
    }   
    if (m_ai->GetDataByInfo(header) != ErrCodeE::Success) {
        return;
    }
    auto *data = (MediaAi::AiInfoClassifyData *) header.data;
    bool isHavePerson = false;
    for (int i = 0; i < header.size / (int) sizeof(MediaAi::AiInfoClassifyData); i++) {
        emxlogt("type[%d]\n", (int)data[i].type);
        if (data[i].type == MediaAi::ClassifyTypeE::Person) {
            isHavePerson = true;
            break;
        }
    }
    if (isHavePerson) {
        IntelligentLight(&m_ctx->env.alertarea.on, 5);
    }
}
    