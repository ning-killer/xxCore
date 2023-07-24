//
// Created by xiong on 2022/7/9.
//

#ifndef EAPIL_EnvZone_HPP
#define EAPIL_EnvZone_HPP

#include "Zone.hpp"

namespace Emx {

    class EnvZone : public Zone {
    public:
        EnvZone() : Zone() {}

        ErrCodeE SetOvdZone(const Json::Value &json);

        Json::Value ovdZone;

    };
}

#endif //EAPIL_EnvZone_HPP
