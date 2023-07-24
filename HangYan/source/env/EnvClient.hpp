//
// Created by xiong on 2022/7/9.
//

#ifndef EAPIL_EnvClient_HPP
#define EAPIL_EnvClient_HPP

#include "EnvBase.hpp"
#include "OVD_define.h"

namespace Emx {

    class EnvClient : public EnvBase {
    public:
        EnvClient() : EnvBase("client"), param() {}

        ErrCodeE Load() override;

        ErrCodeE Save() override { return ErrCodeE::Success; }

    public:
        OVDClientParam param;
    };
}

#endif //EAPIL_EnvClient_HPP
