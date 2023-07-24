//
// Created by xiong on 2021/6/25.
//

#ifndef EMX_MediaChipQRScan_HPP
#define EMX_MediaChipQRScan_HPP

#include "core/EmxUV.hpp"
#include "MediaServerQRScan.hpp"

namespace Emx {

    class MediaChipQRScan : public MediaServerQRScan {
    public:
        MediaChipQRScan(int32_t chn, MediaServerResource &res)
                : MediaServerQRScan(chn, res), m_started(false), m_mem(-1) {}

        ErrCodeE Start() override;

        ErrCodeE Stop() override;

        virtual ~MediaChipQRScan() {}
    private:
        bool m_started;
        int m_mem;
    };

}

#endif //EMX_MediaChipQRScan_HPP
