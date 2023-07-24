/*
 * @Author: xiong
 * @Date: 2022/10/25
*/

#include "AlarmMotion.hpp"

using namespace Emx;

void AlarmMotion::CalibrationDetectThreshold() {
    MediaClientAi ai_client(0);
    MediaAi::Param ai_param;
    if (ai_client.GetParam(ai_param) != ErrCodeE::Success) {
        return;
    }
    emxlogd("night switch, current night mode[%d]; motion detect threshold[%d]\n"
                , m_nightStatus.mode, m_ctx->env.motion.threshold[(int)m_nightStatus.mode]);
    ai_param.motionDetection.threshold = m_ctx->env.motion.threshold[(int)m_nightStatus.mode];
    ai_client.SetParam(ai_param);
}

ErrCodeE AlarmMotion::Create() {
    m_motionDetectDelayTimer.Create(m_ctx->loop);
    m_night.Start(m_ctx->loop, m_ctx->buffer, m_ctx->bufferSize, [this](NightVision::Status &status) {
        m_nightStatus = status;
        CalibrationDetectThreshold();
    });
    m_ai = new MediaClientAiDataAsync(m_ctx->loop);
    //启动监听AI数据，这里只需要MD数据，为了防止不必要的数据拷贝，没有使用
    //OnGetDataCallback而是使用OnGetInfoCallback加GetDataByInfo的组合获取数据
    m_ai->Start(nullptr, [this](MediaAi::AiInfoHeader &header) {
        if (!m_ctx->env.motion.ena)
            return;
        if (header.type != MediaAi::AiTypeE::MotionDetection)
            return;
        if (m_ai->GetDataByInfo(header) != ErrCodeE::Success)
            return;
        //如果警戒区域正在执行闪灯等策略，则不进行移动侦测
        for (auto alarm : *m_alarmList) {
            if (alarm->GetType() == OVD_ALERTAREA && alarm->IsProcessing())
                return;
        }

        auto *data = (MediaAi::AiInfoMotionDetectionData *) header.data;
        int maxArea = 0;
        for (int i = 0; i < header.size / (int) sizeof(MediaAi::AiInfoMotionDetectionData); i++) {
            if (AlarmInZone(header.srcPicSize, data[i], m_ctx->env.motion.zone)) {
                if (data[i].Area() > maxArea)
                    maxArea = data[i].Area();
            }
        }
        /*如果检测到运动面积大于90%，就不认为是移动物体*/
        if (maxArea > header.srcPicSize.w * header.srcPicSize.h * 90 / 100)
            return;
        int level = m_ctx->env.motion.sensitivity / 20;
        int targetArea;
        if (level <= 1) {
            targetArea = m_ctx->env.motion.map[2];
        } else if (level <= 2) {
            targetArea = m_ctx->env.motion.map[3];
        } else if (level <= 3) {
            targetArea = m_ctx->env.motion.map[5];
        } else if (level <= 4) {
            targetArea = m_ctx->env.motion.map[6];
        } else {
            targetArea = m_ctx->env.motion.map[8];
        }

        if (((maxArea * 1000) / (header.srcPicSize.w * header.srcPicSize.h)) > targetArea) {
            if (m_trigger) {
                AlarmTrigger();
            }
            if (!m_delay) {
                m_delay = true;
                m_motionDetectDelayTimer.Start(1000, 0, [this]() {
                    //如果夜视正在切换中，则不进行移动侦测
                    if (m_nightStatus.switching) {
                        m_delay = false;
                        return;
                    }
                    //如果人行或警戒已经触发则不进行移动告警
                    for (auto alarm : *m_alarmList) {
                        if (alarm->IsStarted() &&
                            (alarm->GetType() == OVD_MAN ||
                             alarm->GetType() == OVD_ALERTAREA)) {
                            m_delay = false;
                            return;
                        }
                    }
                    m_trigger = true;
                    AlarmTrigger();
                });
            }
        }
    });
    return ErrCodeE::Success;
}

void AlarmMotion::Destroy() {
    m_night.Stop();
    m_motionDetectDelayTimer.Destroy();
    m_ai->Stop();
    delete m_ai;
}

void AlarmMotion::AlarmEnd() {
    m_started = false;
    m_cb(m_type, false);
    m_alarmEnd = false;
    m_alarmContinueReachMin = false;
    m_endJudgeTimer.Stop();
    m_continueMinTimer.Stop();
    m_continueMaxTimer.Stop();
    m_delay = false;
    m_trigger = false;
}
