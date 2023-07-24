//
// Created by xiong on 2020/11/23.
//

#ifndef EMX_MediaChipAdec_HPP
#define EMX_MediaChipAdec_HPP

#include "MediaServerAdec.hpp"
#include "EmxCore.hpp"

namespace Emx {
    class MediaChipAdec : public MediaServerAdec {
    public:
        MediaChipAdec(int32_t chn, MediaServerResource &res) :
                MediaServerAdec(chn, res) {}

        virtual ~MediaChipAdec() {};

        ErrCodeE Start() override { return ErrCodeE::Success; }

        ErrCodeE Stop() override { return ErrCodeE::Success; }

        ErrCodeE SetParam(const Param &param) override { return ErrCodeE::Success; }

        ErrCodeE GetParam(Param &param) override { return ErrCodeE::Success; }

        ErrCodeE SetVolume(int32_t volume) override { return ErrCodeE::Success; }

        ErrCodeE SetVolumeTemp(int32_t volume) override { return ErrCodeE::Success; }

        PlayStatusE GetPlayStatus() override { return PlayStatusE::Unknown; }
    };
}
#endif //EMX_MediaChipAdec_HPP
