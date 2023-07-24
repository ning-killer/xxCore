//
// Created by xiong on 2022/6/29.
//

#ifndef EMX_BspIr_HPP
#define EMX_BspIr_HPP

#include "EmxBsp.hpp"
#include "BspBaseGpio.hpp"

namespace Emx {
    class BspIrLight : public Bsp::IrLight {
    public:
        BspIrLight() : Bsp::IrLight(), m_baseGpio("irLight") {}

        virtual ~BspIrLight() {}

        ErrCodeE SetEna(bool ena, int chn) override {
            return m_baseGpio.Set(ena, chn);
        }

        bool GetEna(int chn) override {
            return m_baseGpio.Get(chn);
        }

        ErrCodeE SetLuma(int luma, int chn) override {
            return SetEna(luma > 0, chn);
        }

        int GetLuma(int chn) override {
            return GetEna(chn) ? 100 : 0;
        }

    private:
        BspBaseGpio m_baseGpio;
    };
}
#endif //EMX_BspIr_HPP
