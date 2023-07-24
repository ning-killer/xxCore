//
// Created by xiong on 2022/6/29.
//

#ifndef EMX_BspKeyReset_HPP
#define EMX_BspKeyReset_HPP

#include "EmxBsp.hpp"
#include "BspBaseGpio.hpp"

namespace Emx {
    class BspKeyReset : public Bsp::KeyReset {
    public:
        BspKeyReset() : Bsp::KeyReset(), m_baseGpio("keyReset") {}

        bool Get() override { return m_baseGpio.Get(-1); }

    private:
        BspBaseGpio m_baseGpio;
    };
}

#endif //EMX_BspKeyReset_HPP
