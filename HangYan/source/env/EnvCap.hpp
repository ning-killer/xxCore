//
// Created by xiong on 2022/7/9.
//

#ifndef EAPIL_EnvCap_HPP
#define EAPIL_EnvCap_HPP

#include "EnvBase.hpp"
#include "OVD_define.h"

namespace Emx {

    class EnvCap : public EnvBase {
    public:
        EnvCap() : EnvBase("cap"), info() {}

        ErrCodeE Load() override;

        ErrCodeE Save() override { return ErrCodeE::Success; }

    public:
        OVDCapInfo info;
    };

}

#endif //EAPIL_EnvCap_HPP
