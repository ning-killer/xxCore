/*
* @Author: xiong
* @Date: 2022/9/27
*/

#ifndef EMX_BspLed_HPP
#define EMX_BspLed_HPP

#include "EmxBsp.hpp"
#include "BspBaseGpio.hpp"

namespace Emx {

    class BspLed : public Bsp::Led {
    public:
        BspLed() : Bsp::Led(), m_baseGpio("led") {}

        ErrCodeE Set(bool ena, int chn) override {
            return m_baseGpio.Set(ena, chn);
        }

        bool Get(int chn) override {
            return m_baseGpio.Get(chn);
        }

        int GetChnsNum() const override {
            return m_baseGpio.GetChnsNum();
        }

    private:
        BspBaseGpio m_baseGpio;
    };

}
#endif //EMX_BspLed_HPP
