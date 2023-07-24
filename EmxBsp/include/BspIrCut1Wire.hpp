//
// Created by xiong on 2022/6/29.
//

#ifndef EMX_BspIrCut1Wire_HPP
#define EMX_BspIrCut1Wire_HPP

#include "EmxBsp.hpp"
#include "BspBaseGpio.hpp"

namespace Emx {

    class BspIrCut1Wire : public Bsp::IrCut {
    public:
        BspIrCut1Wire() : Bsp::IrCut(), m_baseGpio("irCut1Wire") {
            for (auto &item:m_baseGpio.m_channels) {
                item->SetValue(Gpio::ValueE::Disable);
            }
        }

        ErrCodeE SwitchToNight(bool ena, int chn) override {
            return m_baseGpio.Set(ena, chn);
        }

    private:
        BspBaseGpio m_baseGpio;
    };

}
#endif //EMX_BspIrCut1Wire_HPP
