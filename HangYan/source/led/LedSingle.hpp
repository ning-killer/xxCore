//
// Created by xiong on 2022/7/5.
//

#ifndef EAPIL_LedSingle_HPP
#define EAPIL_LedSingle_HPP

#include "Led.hpp"
namespace Emx {

    class LedSingle : public Led {
    public:
        LedSingle(EuvLoop &loop, Bsp::Led *led) :
                Led(loop),
                m_led(led) {}

    private:
        void OnLedStatusChange() override;

    private:
        Bsp::Led *m_led;
    };

}

#endif //EAPIL_LedSingle_HPP
