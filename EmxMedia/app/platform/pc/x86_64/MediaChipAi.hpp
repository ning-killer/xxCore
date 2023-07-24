//
// Created by xiong on 2022/4/27.
//

#ifndef EMX_MediaChipAi_HPP
#define EMX_MediaChipAi_HPP


#include "MediaServerAi.hpp"
#include "MediaChipAiPlug.hpp"

namespace Emx {

    class MediaChipAi : public MediaServerAi {
    public:
        MediaChipAi(int32_t chn, MediaServerResource &res) :
                MediaServerAi(chn, res) {}

        virtual ~MediaChipAi() {};

        ErrCodeE Start() override { return ErrCodeE::Success; }

        ErrCodeE Stop() override { return ErrCodeE::Success; }

        ErrCodeE SetParam(const Param &param) override { return ErrCodeE::Success; }

        ErrCodeE GetParam(Param &param) override { return ErrCodeE::Success; }
    };
}

#endif //EMX_MediaChipAi_HPP
