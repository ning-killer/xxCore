//
// Created by xiong on 2022/6/29.
//

#ifndef EMX_BspWhiteLightTx_HPP
#define EMX_BspWhiteLightTx_HPP

#include "EmxBsp.hpp"

namespace Emx {

    class BspWhiteLightTx : public Bsp::WhiteLight {
    public:
        BspWhiteLightTx();

        virtual ~BspWhiteLightTx() override;

        ErrCodeE SetEna(bool ena, int chn) override;

        bool GetEna(int chn) override;

        ErrCodeE SetLuma(int luma, int chn) override;

        int GetLuma(int chn) override;

    private:
        ErrCodeE DoSetLuma(int luma, int chn);

    private:
        bool m_created;
        int m_defaultChnIdx;
        struct Channel {
            int luma;
            int chipNum;
            int pwmNum;
            int period;
            int dutyMin;
            int dutyMax;
        };
        std::vector<Channel> m_channels;
    };
}

#endif //EMX_BspWhiteLightTx_HPP
