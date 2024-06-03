/*
* @Author: xiong
* @Date: 2022/9/27
*/

#ifndef EMX_BspMobile_HPP
#define EMX_BspMobile_HPP

#include "EmxBsp.hpp"
#include "BspBaseGpio.hpp"

namespace Emx {

    class BspMobile : public Bsp::Mobile {
    public:
        BspMobile() : Bsp::Mobile(), m_baseGpio("mobile") {}

        ErrCodeE Set(bool ena) override {
            return m_baseGpio.Set(ena, -1);
        }

        bool Get() override { return m_baseGpio.Get(-1); }

    private:
        BspBaseGpio m_baseGpio;
    };

}
#endif //EMX_BspMobile_HPP
