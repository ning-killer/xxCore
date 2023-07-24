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
                : MediaServerQRScan(chn, res) {}

        virtual ~MediaChipQRScan() {}

        ErrCodeE Start() override { return ErrCodeE::Success; }

        ErrCodeE Stop() override { return ErrCodeE::Success; }
    };

}

#endif //EMX_MediaChipQRScan_HPP
