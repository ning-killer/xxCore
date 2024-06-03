//
// Created by xiong on 2022/7/9.
//

#ifndef EAPIL_EnvModelId_HPP
#define EAPIL_EnvModelId_HPP
#include "EnvBase.hpp"
namespace Emx {

    class EnvModelId : public EnvBase {
    public:
        EnvModelId() : EnvBase("modelId") {}

        ErrCodeE Load() override;

        ErrCodeE Save() override { return ErrCodeE::Success; }

    public:
        char id[64];
    };

}

#endif //EAPIL_EnvModelId_HPP
