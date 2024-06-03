//
// Created by xiong on 2022/6/30.
//

#ifndef EMX_BspAdcTx_HPP
#define EMX_BspAdcTx_HPP

#include "EmxBsp.hpp"

namespace Emx {

    class BspAdcTx : public Bsp::Adc {
    public:
        BspAdcTx();

        ~BspAdcTx();

        virtual int GetLumaValue(int chn) override;

        virtual int GetAdcValue(int chn) override;

    private:
        bool m_created;
        int m_defaultChnIdx;
        struct Channel {
            int chnNum;
            int reference;
            int adcValueAtMaxLuma;
            int adcValueAtMinLuma;
        };
        std::vector<Channel> m_channels;
    };
}

#endif //EMX_BspAdcTx_HPP
