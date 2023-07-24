/*
* @Author: xiong
* @Date: 2022/9/27
*/

#ifndef EMX_BspWifi_HPP
#define EMX_BspWifi_HPP

#include "EmxBsp.hpp"
#include "BspBaseGpio.hpp"

namespace Emx {

    class BspWifi : public Bsp::Wifi {
    public:
        BspWifi() : Bsp::Wifi(), m_baseGpio("wifi") {}

        ErrCodeE Set(bool ena) override {
            return m_baseGpio.Set(ena, -1);
        }

        bool Get() override { return m_baseGpio.Get(-1); }

    private:
        BspBaseGpio m_baseGpio;
    };

}
#endif //EMX_BspWifi_HPP
