//
// Created by xiong on 2022/6/30.
//

#ifndef EMX_BspAdcCvitek_HPP
#define EMX_BspAdcCvitek_HPP

#include "EmxBsp.hpp"

namespace Emx {

    class BspAdcCvitek : public Bsp::Adc {
    public:
        BspAdcCvitek();

        ~BspAdcCvitek();

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

#endif //EMX_BspAdcCvitek_HPP
