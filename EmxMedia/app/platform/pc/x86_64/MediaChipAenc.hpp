//
// Created by xiong on 2020/11/19.
//

#ifndef EMX_MediaChipAenc_HPP
#define EMX_MediaChipAenc_HPP

#include "MediaServerAenc.hpp"

namespace Emx {
    class MediaChipAenc : public MediaServerAenc {
    public:
        MediaChipAenc(int32_t chn, MediaServerResource &res) :
                MediaServerAenc(chn, res) {}

        virtual ~MediaChipAenc() {};

        ErrCodeE Start() override { return ErrCodeE::Success; }

        ErrCodeE Stop() override { return ErrCodeE::Success; }

        ErrCodeE SetParam(const Param &param) override { return ErrCodeE::Success; }

        ErrCodeE GetParam(Param &param) override { return ErrCodeE::Success; }

        ErrCodeE SetVolume(int32_t volume) override { return ErrCodeE::Success; }

        ErrCodeE Mute(bool ena) override { return ErrCodeE::Success; }
    };
}

#endif //EMX_MediaChipAenc_HPP
