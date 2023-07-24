//
// Created by xiong on 2022/7/5.
//

#ifndef EAPIL_LedDouble_HPP
#define EAPIL_LedDouble_HPP

#include "Led.hpp"

namespace Emx {

    class LedDouble : public Led {
    public:
        LedDouble(EuvLoop &loop, Bsp::Led *led) :
                Led(loop),
                m_led(led) {}

    private:
        void OnLedStatusChange() override;

        void SetOff();

        void SetNo1();

        void SetNo2();

    private:
        Bsp::Led *m_led;
    };

}

#endif //EAPIL_LedDouble_HPP
