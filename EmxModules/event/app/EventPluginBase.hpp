/*
* @Author: xiong
* @Date: 2023/2/27
*/

#ifndef EMX_EventPluginBase_HPP
#define EMX_EventPluginBase_HPP

#include "Event.hpp"
#include "EventResource.hpp"
#include "EmxMedia.hpp"
#include "Zone.hpp"

namespace Emx {

    class EventPluginBase {
    public:
        EventPluginBase(const char *name, Event::TypeE type, int chn, EventResource &res) :
                m_name(name), m_type(type), m_pluginChn(chn), m_res(res), m_started(false) {
            m_endJudgeTimer.Create(res.loop);
            m_continueMinTimer.Create(res.loop);
            m_continueMaxTimer.Create(res.loop);
        }

        virtual ~EventPluginBase() {
            m_endJudgeTimer.Destroy();
            m_continueMinTimer.Destroy();
            m_continueMaxTimer.Destroy();
        }

        /**
         * @brief 获取事件名称
         * @return 事件名称
         */
        const char *GetName() { return m_name.c_str(); }

        /**
         * @brief 获取事件类型
         * @return 事件类型
         */
        Event::TypeE GetType() { return m_type; }

        /**
         * @brief 获取事件通道号
         * @return 事件通道号
         */
        int GetChn() { return m_pluginChn; }

        /**
         * @brief 获取事件是否正在进行
         * @return 是否正在进行
         */
        bool IsStarted() { return m_started; }

        static bool EventInZone(const Size &srcImgSize, Rect &rect, const Zone &zone);

    protected:

        virtual void EventEnd();

        virtual void EventTrigger();

        void PublishEvent();

    protected:
        std::string m_name;
        Event::TypeE m_type;
        int m_pluginChn;
        EventResource &m_res;
        bool m_started;
        bool m_eventReachEndJudge;//事件结束
        bool m_eventReachContinueMin;//到达事件最小持续时间
        EuvTimer m_endJudgeTimer;//用于判断事件结束
        EuvTimer m_continueMinTimer;//用于判断事件达到最小持续时间要求
        EuvTimer m_continueMaxTimer;//用于判断事件达到最大持续时间要求
    };
}

#endif //EMX_EventPluginBase_HPP
