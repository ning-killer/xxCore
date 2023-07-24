//
// Created by xiong on 2021/5/28.
//

#ifndef EMX_MediaSnap_HPP
#define EMX_MediaSnap_HPP

#include <memory.h>
#include "MediaTypedef.hpp"

namespace Emx {

    //!@brief 缩略图
    class MediaSnap {
    public:
        explicit MediaSnap(int32_t chn) : m_chn(chn) {};

        virtual ~MediaSnap() {};

        /**
         * @brief 获取通道号
         * @return 通道号
         */
        int32_t GetChn() { return m_chn; }

    protected:
        int32_t m_chn;
    };

}
#endif //EMX_MediaSnap_HPP

