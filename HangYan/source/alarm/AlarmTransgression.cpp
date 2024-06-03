#include "AlarmTransgression.hpp"
#include "cmath"

using namespace Emx;

ErrCodeE AlarmTransgression::Create() {
    m_ctx->soundAlarmControl.Update(
        SoundAlarmControl::Type::TRANSGRESSION, SoundAlarmControl::Status::Off);
    m_checkAlgStatusTimer.Create(m_ctx->loop);
    m_ai = new MediaClientAiDataAsync(m_ctx->loop);
    m_ai->Start(nullptr, [this](MediaAi::AiInfoHeader &header) {
        if (!m_ctx->env.transgression.on 
            || m_ctx->env.transgression.type == EnvAlarmTransgression::notsupport) {
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
        m_checkAlgStatusTimer.Stop();
        m_checkAlgStatusTimer.Start(2500, 0, [this]() {
            //note: 2s内未检测到目标输出，则认为目标离开,清空缓存
            m_cacheTargets.clear();
        });
        auto *data = (MediaAi::AiInfoClassifyData *) header.data;
        std::vector<MediaAi::AiInfoClassifyData> corssTargets;
        for (int i = 0; i < header.size / (int) sizeof(MediaAi::AiInfoClassifyData); i++) {
            emxlogt("Sub TargetType[%d]->Env TargetType[%d]\n", (int)data[i].type, (int)m_ctx->env.alertarea.type);
            if (!isSupportedTargetType(data[i].type)) {
                continue;
            }
            // 拌线检测
            if (m_ctx->env.transgression.line.IsCross(data[i].rect, header.srcPicSize)) {
                corssTargets.emplace_back(data[i]);
                emxlogt("current obejct have corss\n");
            } else {
                emxlogt("obejct id: %d\n", data[i].id);
                for (auto it = m_cacheTargets.begin(); it != m_cacheTargets.end(); ) {
                    if (it->id == data[i].id) {
                        // 删除重复id缓存
                        it = m_cacheTargets.erase(it);
                    } else {
                        ++it;
                    }
                }
                // 缓存未拌线目标
                if ((int)m_cacheTargets.size() < m_maxCacheSize) {
                    m_cacheTargets.emplace_back(data[i]);
                } else {
                    // 更新缓存
                    m_cacheTargets.erase(m_cacheTargets.begin());
                    m_cacheTargets.shrink_to_fit();
                    m_cacheTargets.emplace_back(data[i]);
                }
            }
        }
        if (corssTargets.size() == 0) {
            return;
        }
        std::vector<MediaAi::AiInfoClassifyData> noAlarmTargets;
        emxlogt("corssTargets size[%d],m_cacheTargets[%d]\n", corssTargets.size(), m_cacheTargets.size());
        EnvAlarmTransgression::Direction direction = EnvAlarmTransgression::BottomToTop;
        bool isAlarm = false;
        for (auto corss : corssTargets) {
            for (auto iter = m_cacheTargets.begin(); iter != m_cacheTargets.end(); ++iter) {
                emxlogt("corss id[%d],cache_id[%d]\n", corss.id, iter->id);
                if (corss.id == iter->id) {
                    // 判断越界方向
                    m_ctx->env.transgression.line.CheckCrossDirection(direction, iter->rect, corss.rect, header.srcPicSize);
                    emxlogt("env style[%d],cross style[%d]\n", m_ctx->env.transgression.statisticsModeDirection, direction);
                    if (m_ctx->env.transgression.statisticsModeStyle == EnvAlarmTransgression::Twoway) {
                        isAlarm = true;
                        m_cacheTargets.erase(iter);
                        break;
                    }
                    // 判断是否满足配置越界方向
                    if (direction == m_ctx->env.transgression.statisticsModeDirection) {
                        isAlarm = true;
                        m_cacheTargets.erase(iter);
                        break;
                    }
                }
            }
            emxlogt("isAlarm[%d]\n", isAlarm);
            if (isAlarm) { continue; }
            noAlarmTargets.emplace_back(corss);
        }
        if (!isAlarm) {
            return;
        }
        bool started = m_started;
        AlarmTrigger();
        if (m_started && !started) {
            BreakLastSoundAndLightAlarm();
            Strategy(&m_ctx->env.transgression.strategy, &m_ctx->env.transgression.on, "transgressionAlarmVoice");
        }
    });
    return ErrCodeE::Success;
}

void AlarmTransgression::Destroy() {
    m_checkAlgStatusTimer.Destroy();
    m_ai->Stop();
    delete m_ai;
}

bool AlarmTransgression::isSupportedTargetType(const MediaAi::ClassifyTypeE type) {
    if (type == MediaAi::ClassifyTypeE::Person && 
        (m_ctx->env.transgression.type == EnvAlarmTransgression::TargetType::person ||
        m_ctx->env.transgression.type == EnvAlarmTransgression::TargetType::personAndmotor ||
        m_ctx->env.transgression.type == EnvAlarmTransgression::TargetType::personAndnonmotor ||
        m_ctx->env.transgression.type == EnvAlarmTransgression::TargetType::personAndmotorAndnonmotor)) {
        return true;
    }
    if (type == MediaAi::ClassifyTypeE::MotorVehicle && 
        (m_ctx->env.transgression.type == EnvAlarmTransgression::TargetType::motorvehicle ||
        m_ctx->env.transgression.type == EnvAlarmTransgression::TargetType::personAndmotor ||
        m_ctx->env.transgression.type == EnvAlarmTransgression::TargetType::motorAndnonmotor ||
        m_ctx->env.transgression.type == EnvAlarmTransgression::TargetType::personAndmotorAndnonmotor)) {
        return true;
    }
    if (type == MediaAi::ClassifyTypeE::NonMotorVehicle && 
        (m_ctx->env.transgression.type == EnvAlarmTransgression::TargetType::nonmotorvehicle ||
        m_ctx->env.transgression.type == EnvAlarmTransgression::TargetType::personAndnonmotor ||
        m_ctx->env.transgression.type == EnvAlarmTransgression::TargetType::motorAndnonmotor ||
        m_ctx->env.transgression.type == EnvAlarmTransgression::TargetType::personAndmotorAndnonmotor)) {
        return true;
    }
    return false;
}

void AlarmTransgression::Alarm(int time, int id) {
    m_lastAlarmTime = time;
    m_lastAlarmTargetId = id;
    bool started = m_started;
    AlarmTrigger();
    if (m_started && !started) {
        Strategy(&m_ctx->env.alertarea.strategy, &m_ctx->env.alertarea.on, "transgressionAlarmVoice");
    }
}
