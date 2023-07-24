//
// Created by xiong on 2022/7/5.
//

#ifndef EAPIL_LedRgb_HPP
#define EAPIL_LedRgb_HPP

#include "Led.hpp"

namespace Emx {

    class LedRgb : public Led {
    public:
        LedRgb(EuvLoop &loop, Bsp::Led *led) :
                Led(loop),
                m_led(led) {}

    private:
        void OnLedStatusChange() override;

        void SetOff();

        void SetRed();

        void SetGreen();

        void SetBlue();

    private:
        Bsp::Led *m_led;
    };

}

#endif //EAPIL_LedRgb_HPP
