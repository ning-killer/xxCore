//
// Created by xiong on 2021/8/10.
//

#ifndef EMX_MediaChipOSD_HPP
#define EMX_MediaChipOSD_HPP

#include "MediaServerOSD.hpp"

namespace Emx {
    class MediaChipOSD : public MediaServerOSD {
    public:
        friend class MediaChipVenc;

        MediaChipOSD(int32_t chn, MediaServerResource &res) :
                MediaServerOSD(chn, res) {}

        virtual ~MediaChipOSD() {};
    public:

        ErrCodeE Start() override { return ErrCodeE::Success; }

        ErrCodeE Stop() override { return ErrCodeE::Success; }

        ErrCodeE SetParam(const std::vector<std::shared_ptr<Param>> &param) override { return ErrCodeE::Success; }

        ErrCodeE GetParam(std::vector<std::shared_ptr<Param>> &param) override { return ErrCodeE::Success; }
    };
}


#endif //EMX_MediaChipOSD_HPP
