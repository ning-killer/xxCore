#ifndef EAPIL_EnvAlarmOffduty_HPP
#define EAPIL_EnvAlarmOffduty_HPP

#include "EnvBase.hpp"
#include "EnvSchedule.hpp"
#include "EnvZone.hpp"
#include "EnvStrategy.hpp"

namespace Emx {

    class EnvAlarmOffduty : public EnvBase {
    public:
        EnvAlarmOffduty() : EnvBase("alarmOffduty") {}

        ErrCodeE Load() override;

        ErrCodeE Save() override;

        bool gatReportOn;
        bool on;
        int regionalId;
        std::string regionalName;
        int onDutyCount;
        int offdutyDuration;
        EnvSchedule schedule;
        EnvZone zone;
        int isChangedoffdutyDuration = false; //临时存储状态，管理告警间隔配置发生变化
    };

}

#endif //EAPIL_EnvAlarmOffduty_HPP
