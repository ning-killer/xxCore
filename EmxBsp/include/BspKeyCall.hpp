//
// Created by xiong on 2022/6/29.
//

#ifndef EMX_BspKeyCall_HPP
#define EMX_BspKeyCall_HPP

#include "EmxBsp.hpp"
#include "BspBaseGpio.hpp"

namespace Emx {
    class BspKeyCall : public Bsp::KeyCall {
    public:
        BspKeyCall() : Bsp::KeyCall(), m_baseGpio("keyCall") {}

        bool Get() override { return m_baseGpio.Get(-1); }

    private:
        BspBaseGpio m_baseGpio;
    };
}
#endif //EMX_BspKeyCall_HPP
