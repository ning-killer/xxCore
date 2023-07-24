//
// Created by xiong on 2022/6/28.
//

#ifndef EMX_BspSpk_HPP
#define EMX_BspSpk_HPP

#include "EmxBsp.hpp"
#include "BspBaseGpio.hpp"

namespace Emx {
    class BspSpk : public Bsp::Spk {
    public:
        BspSpk() : Bsp::Spk(), m_baseGpio("spk") {}

        ErrCodeE Set(bool ena) override {
            return m_baseGpio.Set(ena, -1);
        }

        bool Get() override { return m_baseGpio.Get(-1); }

    private:
        BspBaseGpio m_baseGpio;
    };
}
#endif //EMX_BspSpk_HPP
