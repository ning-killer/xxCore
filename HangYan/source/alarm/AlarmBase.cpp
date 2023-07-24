/*
 * @Author: xiong
 * @Date: 2022/11/4
*/

#include "AlarmBase.hpp"

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
