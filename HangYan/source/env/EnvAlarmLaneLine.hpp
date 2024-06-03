#ifndef EAPIL_EnvAlarmLaneLine_HPP
#define EAPIL_EnvAlarmLaneLine_HPP

#include "EnvBase.hpp"
#include "EnvZone.hpp"

namespace Emx {

    class EnvAlarmLaneLine : public EnvBase {
    public:
        EnvAlarmLaneLine() : EnvBase("alarmLaneLine") {}

        ErrCodeE Load() override;

        ErrCodeE Save() override;

        bool on;
        int laneLineNum;
        EnvZone zone;
    };

}

#endif //EAPIL_EnvAlarmLaneLine_HPP
