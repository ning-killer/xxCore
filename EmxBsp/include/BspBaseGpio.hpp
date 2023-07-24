//
// Created by xiong on 2022/6/29.
//

#ifndef EMX_BspBaseGpio_HPP
#define EMX_BspBaseGpio_HPP

#include "EmxCore.hpp"

namespace Emx {

    class BspBaseGpio {
    public:
        BspBaseGpio(const char *name);

        virtual ~BspBaseGpio();

        virtual ErrCodeE Set(bool ena, int chn);

        virtual bool Get(int chn);

        virtual int GetChnsNum() const {
            return m_channels.size();
        }

        bool m_created;
        int m_defaultChnIdx;
        std::vector<Gpio *> m_channels;
    };

}

#endif //EMX_BspBaseGpio_HPP
