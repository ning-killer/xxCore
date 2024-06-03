/*
* @Author: xiong
* @Date: 2022/10/11
*/

#ifndef EAPIL_EnvSleep_HPP
#define EAPIL_EnvSleep_HPP

#include "EnvBase.hpp"
#include "EnvSchedule.hpp"
namespace Emx {

    class EnvSleep : public EnvBase {
    public:
        EnvSleep() : EnvBase("sleep") {}

        ErrCodeE Load() override;

        ErrCodeE Save() override;

        EnvSchedule schedule;
    };

}

#endif //EAPIL_EnvSleep_HPP
