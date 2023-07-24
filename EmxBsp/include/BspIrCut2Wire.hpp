//
// Created by xiong on 2022/6/29.
//

#ifndef EMX_BspIrCut2Wire_HPP
#define EMX_BspIrCut2Wire_HPP

#include "EmxBsp.hpp"
#include "BspBaseGpio.hpp"

namespace Emx {

    class BspIrCut2Wire : public Bsp::IrCut {
    public:
        BspIrCut2Wire();

        virtual ~BspIrCut2Wire();

        ErrCodeE SwitchToNight(bool ena, int chn) override;

    protected:
        bool m_created;
        int m_defaultChnIdx;
        struct Channel {
            Gpio *wire0;
            Gpio *wire1;
            int delayMs;
        };
        std::vector<Channel> m_channels;
    };
}

#endif //EMX_BspIrCut2Wire_HPP
