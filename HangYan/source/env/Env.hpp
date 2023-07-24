//
// Created by xiong on 2022/6/24.
//

#ifndef EAPIL_Env_HPP
#define EAPIL_Env_HPP

#include "OVD_define.h"
#include "EmxCore.hpp"
#include "DevInfo.hpp"
#include "Mac.hpp"
#include "Sn.hpp"
#include "TimeZone.hpp"
#include "EnvBurn.hpp"
#include "EnvModelId.hpp"
#include "EnvCap.hpp"
#include "EnvCfg.hpp"
#include "EnvWifi.hpp"
#include "EnvClient.hpp"
#include "EnvSleep.hpp"
#include "EnvAlarmFace.hpp"
#include "EnvAlarmPerson.hpp"
#include "EnvAlarmMotion.hpp"
#include "EnvAlarmVoice.hpp"
#include "EnvAlarmInvasion.hpp"
#include "EnvManufacturers.hpp"
#include "EnvVoip.hpp"


namespace Emx {

    class Env {
    public:
        ErrCodeE LoadAll();

    public:
        DevInfo devInfo;
        Sn sn;
        Mac mac;
        TimeZone timeZone;
        EnvBurn burn;
        EnvModelId modelId;
        EnvCap cap;
        EnvCfg cfg;
        EnvWifi wifi;
        EnvClient client;
        EnvSleep sleep;
        EnvAlarmInvasion invasion;
        EnvAlarmFace face;
        EnvAlarmPerson person;
        EnvAlarmMotion motion;
        EnvAlarmVoice voice;
        EnvManufacturers manufacturers;
        EnvVoip voip;
    };
}

#endif //EAPIL_Env_HPP
