//
// Created by xiong on 2022/7/9.
//

#ifndef EAPIL_EnvAlarmMotion_HPP
#define EAPIL_EnvAlarmMotion_HPP

#include "EnvBase.hpp"
#include "EnvZone.hpp"

namespace Emx {

    class EnvAlarmMotion : public EnvBase {
    public:
        EnvAlarmMotion() : EnvBase("alarmMotion") {}

        ErrCodeE Load() override;

        ErrCodeE Save() override;

        bool ena;
        int sensitivity;
        EnvZone zone;
        int map[10];
        int threshold[3];
    };

}

#endif //EAPIL_EnvAlarmMotion_HPP
