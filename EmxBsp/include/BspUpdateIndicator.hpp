/*
* @Author: xiong
* @Date: 2022/9/27
*/

#ifndef EMX_BspUpdateIndicator_HPP
#define EMX_BspUpdateIndicator_HPP

#include "EmxBsp.hpp"
#include "BspBaseGpio.hpp"

namespace Emx {

    class BspUpdateIndicator : public Bsp::UpdateIndicator {
    public:
        BspUpdateIndicator();

        ErrCodeE Start() override;

        ErrCodeE Stop() override;

    private:
        bool m_started;
        EuvLoop m_loop;
        EuvTimer m_timer;
        int m_idx;
        int m_invMs;
        std::vector<Gpio *> m_allChannels;
        std::vector<int> m_channelsCycleIdx;
    };

}

#endif //EMX_BspUpdateIndicator_HPP
