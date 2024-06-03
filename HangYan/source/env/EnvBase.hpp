//
// Created by xiong on 2022/7/9.
//

#ifndef EAPIL_EnvBase_HPP
#define EAPIL_EnvBase_HPP
#include "EmxCore.hpp"

namespace Emx {

    class EnvBase : public Param {
    public:
        EnvBase(const std::string &name) : Param("ovd/" + name) {}

        virtual ~EnvBase() {}

        virtual ErrCodeE Load() = 0;

        virtual ErrCodeE Save() = 0;
    };

}

#endif //EAPIL_EnvBase_HPP
