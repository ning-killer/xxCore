/*
* @Author: xiong
* @Date: 2023/2/13
*/

#ifndef EMX_EventStdResource_HPP
#define EMX_EventStdResource_HPP

#include "Event.hpp"

namespace Emx {
    class EventPluginBase;

    class EventResource {
    public:
        EuvLoop loop;
        char buffer[16 * 1024];
//        using PublishEvent = std::function<void(Emx::EventStd::EventE event, bool start)>;
//        PublishEvent publishEvent;
        MsgTopicPublisherAsync publish;
        std::vector<EventPluginBase *> pluginList;
        bool scheduleActive;
        int eventEndJudgeTimeMs;//连续(x)ms内没有再次出现此告警，则认为此告警结束
        int eventContinueTimeMinMs;//告警触发后最少需要持续(x)ms，才能上报告警结束
        int eventContinueTimeMaxMs;//告警触发后最长持续(x)ms，必须上报告警结束
    };

}
#endif //EMX_EventStdResource_HPP
