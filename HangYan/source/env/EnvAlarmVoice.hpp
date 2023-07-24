//
// Created by xiong on 2022/7/9.
//

#ifndef EAPIL_EnvAlarmVoice_HPP
#define EAPIL_EnvAlarmVoice_HPP

#include "EnvBase.hpp"

namespace Emx {

    class EnvAlarmVoice : public EnvBase {
    public:
        EnvAlarmVoice() : EnvBase("alarmVoice") {}

        ErrCodeE Load() override;

        ErrCodeE Save() override;

        bool ena;
        int sensitivity;
        int map[5];
    };

}

#endif //EAPIL_EnvAlarmVoice_HPP
