//
// Created by xiong on 2022/6/29.
//

#ifndef EMX_BspSoundCheck_HPP
#define EMX_BspSoundCheck_HPP

#include "EmxBsp.hpp"
#include "BspBaseGpio.hpp"

namespace Emx {
    class BspSoundCheck : public Bsp::SoundCheck {
    public:
        BspSoundCheck() : Bsp::SoundCheck(), m_baseGpio("soundCheck") {}

        bool Get() override { return m_baseGpio.Get(-1); }

    private:
        BspBaseGpio m_baseGpio;
    };
}

#endif //EMX_BspKeyReset_HPP
