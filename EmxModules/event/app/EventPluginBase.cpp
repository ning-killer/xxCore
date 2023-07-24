/*
* @Author: xiong
* @Date: 2023/2/27
*/

#include "EventPluginBase.hpp"
#include "EventInner.hpp"

using namespace Emx;


bool EventPluginBase::EventInZone(const Size &srcImgSize, Rect &rect, const Zone &zone) {
    Size dstSize(Zone::Width, Zone::Height);
    const auto dstRect = rect.Transfer((Size &) srcImgSize, dstSize);
    for (int y = dstRect.y; y < dstRect.y + dstRect.h; y++)
        for (int x = dstRect.x; x < dstRect.x + dstRect.w; x++)
            if (zone.map[dstSize.w * y + x])
                return true;
    return false;
}

void EventPluginBase::PublishEvent() {
    Event::Info info = {};
    info.type = m_type;
    info.chn = m_pluginChn;
    info.timestampMs = Time::GetMs64();
    info.isStart = m_started;
    m_res.publish.Publish(EVENT_TOPIC_ID, (char *) &info, sizeof(info));
}

void EventPluginBase::EventTrigger() {
    if (!m_started) {
        //事件首次到来
        m_started = true;
        PublishEvent();
        m_endJudgeTimer.Start(m_res.eventEndJudgeTimeMs, 0, [this]() {
            //检测到告警事件已经结束，如果此时达到了告警最小持续时间则触发告警结束回调
            m_eventReachEndJudge = true;
            if (m_eventReachContinueMin) EventEnd();
        });
        m_continueMinTimer.Start(m_res.eventContinueTimeMinMs, 0, [this]() {
            //已经达到告警最小持续时间，如果此时告警事件已经结束则触发告警结束回调
            m_eventReachContinueMin = true;
            if (m_eventReachEndJudge) EventEnd();
        });
        m_continueMaxTimer.Start(m_res.eventContinueTimeMaxMs, 0, [this]() {
            //已经达到告警最大持续时间，直接触发告警结束回调
            EventEnd();
        });

    } else {
        //事件开始后重复到来
        //更新告警事件结束timer，重新记时AlarmEndJudgeTime
        m_endJudgeTimer.Start(m_res.eventEndJudgeTimeMs, 0, [this]() {
            //检测到告警事件已经结束，如果此时达到了告警最小持续时间则触发告警结束回调
            //如果网络未连接
            m_eventReachEndJudge = true;
            if (m_eventReachContinueMin) EventEnd();
        });
    }
}

void EventPluginBase::EventEnd() {
    if (m_started) {
        m_started = false;
        PublishEvent();
    }
    m_eventReachEndJudge = false;
    m_eventReachContinueMin = false;
    m_endJudgeTimer.Stop();
    m_continueMinTimer.Stop();
    m_continueMaxTimer.Stop();
}
