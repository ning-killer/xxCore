/*
* @Author: xiong
* @Date: 2022/12/7
*/

#ifndef EAPIL_EnvVoip_HPP
#define EAPIL_EnvVoip_HPP

#include "EnvBase.hpp"

namespace Emx {

    class EnvVoip: public EnvBase {
    public:
        EnvVoip() : EnvBase("voip") {}

        ErrCodeE Load() override;

        ErrCodeE Save() override;

    public:
        char appKey[128];
        char appSecret[128];
        bool bind;
    };

}

#endif //EAPIL_EnvVoip_HPP
