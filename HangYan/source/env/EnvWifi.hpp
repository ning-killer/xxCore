//
// Created by xiong on 2022/7/9.
//

#ifndef EAPIL_EnvWifi_HPP
#define EAPIL_EnvWifi_HPP

#include "EnvBase.hpp"
#include "Net.hpp"

namespace Emx {

    class EnvWifi : public EnvBase {
    public:
        EnvWifi() : EnvBase("wifi") {}

        ErrCodeE Load() override;

        ErrCodeE Save() override;

    public:
        bool valid;
        char ssid[Wifi::MaxSsidSize];
        char psk[Wifi::MaxPskSize];
    };
}

#endif //EAPIL_EnvWifi_HPP
