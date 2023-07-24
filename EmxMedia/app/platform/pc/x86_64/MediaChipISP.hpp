//
// Created by xiong on 2021/4/30.
//

#ifndef EMX_MediaChipISP_HPP
#define EMX_MediaChipISP_HPP

#include "MediaServerISP.hpp"

namespace Emx {
    class MediaChipISP : public MediaServerISP {
    public:
        MediaChipISP(int32_t chn, MediaServerResource &res) :
                MediaServerISP(chn, res) {}

        virtual ~MediaChipISP() {};
    public:

        ErrCodeE Start() override { return ErrCodeE::Success; }

        ErrCodeE Stop() override { return ErrCodeE::Success; }

        ErrCodeE SetParam(const Param &param) override { return ErrCodeE::Success; }

        ErrCodeE GetParam(Param &param) override { return ErrCodeE::Success; }

        ErrCodeE SetRunMode(RunModeE mode) override { return ErrCodeE::Success; }

        ErrCodeE GetRunMode(RunModeE &mode) override { return ErrCodeE::Success; }
    };
}

#endif //EMX_MediaChipISP_HPP
