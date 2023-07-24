/*
* @Author: xiong
* @Date: 2022/10/11
*/

#ifndef EAPIL_EnvAlarmInvasion_HPP
#define EAPIL_EnvAlarmInvasion_HPP

#include "EnvBase.hpp"
#include "EnvSchedule.hpp"
#include "EnvZone.hpp"
#include "EnvStrategy.hpp"

namespace Emx {

    class EnvAlarmInvasion : public EnvBase {
    public:
        EnvAlarmInvasion() : EnvBase("alarmInvasion") {}

        ErrCodeE Load() override;

        ErrCodeE Save() override;

        bool ena;
        int sensitivity;
        EnvSchedule schedule;
        EnvZone zone;
        EnvStrategy strategy;
    };

}

#endif //EAPIL_EnvAlarmInvasion_HPP
