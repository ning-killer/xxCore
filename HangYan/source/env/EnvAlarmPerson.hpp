//
// Created by xiong on 2022/7/9.
//

#ifndef EAPIL_EnvAlarmPerson_HPP
#define EAPIL_EnvAlarmPerson_HPP

#include "EnvBase.hpp"
#include "EnvZone.hpp"

namespace Emx {

    class EnvAlarmPerson : public EnvBase {
    public:
        EnvAlarmPerson() : EnvBase("alarmPerson") {}

        ErrCodeE Load() override;

        ErrCodeE Save() override;

        bool ena;
        int sensitivity;
        EnvZone zone;
        int map[5];
    };

}

#endif //EAPIL_EnvAlarmPerson_HPP
