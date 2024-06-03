#ifndef EAPIL_EnvAlarmRegionalPeople_HPP
#define EAPIL_EnvAlarmRegionalPeople_HPP

#include "EnvBase.hpp"
#include "EnvSchedule.hpp"
#include "EnvZone.hpp"
#include "EnvStrategy.hpp"

namespace Emx {

    class EnvAlarmRegionalPeople : public EnvBase {
    public:
        EnvAlarmRegionalPeople() : EnvBase("alarmRegionalPeople") {}

        ErrCodeE Load() override;

        ErrCodeE Save() override;

        bool gatReportOn;
        bool on;
        int regionalId;
        std::string regionalName;
        int regionalPeopleCount;
        int alarmReportDuration;
        int gatReportDuration;
        bool osdStatus;
        EnvSchedule schedule;
        EnvStrategy strategy;
        EnvZone zone;
        int inPeopleCount = 0;          //临时计数
        int isChangedGatReport = false; //临时存储状态，管理上报开关发生变化以及间隔配置发生变化
    };

}

#endif //EAPIL_EnvAlarmRegionalPeople_HPP
