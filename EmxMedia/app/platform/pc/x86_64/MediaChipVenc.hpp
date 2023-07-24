//
// Created by xiong on 2020/10/27.
//

#ifndef EMX_MediaChipVenc_HPP
#define EMX_MediaChipVenc_HPP

#include "MediaServerVenc.hpp"

namespace Emx {

    class MediaChipVenc : public MediaServerVenc {
    public:
        MediaChipVenc(int32_t chn, MediaServerResource &res)
                : MediaServerVenc(chn, res) {}

        virtual ~MediaChipVenc() {};

        ErrCodeE Start() override { return ErrCodeE::Success; }

        ErrCodeE Stop() override { return ErrCodeE::Success; }

        ErrCodeE FlushIDR() override { return ErrCodeE::Success; }

        ErrCodeE SetParam(const Param &param) override { return ErrCodeE::Success; }

        ErrCodeE GetParam(Param &param) override { return ErrCodeE::Success; }
    };
}// namespace Emx

#endif// EMX_MediaChipVenc_HPP
