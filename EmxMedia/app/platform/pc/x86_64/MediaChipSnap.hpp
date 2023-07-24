//
// Created by xiong on 2021/5/28.
//

#ifndef EMX_MediaChipSnap_HPP
#define EMX_MediaChipSnap_HPP

#include "MediaServerSnap.hpp"

namespace Emx {

    class MediaChipSnap : public MediaServerSnap {
    public:
        MediaChipSnap(int32_t chn, MediaServerResource &res)
                : MediaServerSnap(chn, res) {}

        virtual ~MediaChipSnap() {};

        ErrCodeE Start() override { return ErrCodeE::Success; }

        ErrCodeE Stop() override { return ErrCodeE::Success; }

        ErrCodeE Trigger() override { return ErrCodeE::Success; }
    };

}

#endif //EMX_MediaChipSnap_HPP
