//
// Created by xiong on 2020/10/27.
//

#ifndef EMX_MediaChipSystem_HPP
#define EMX_MediaChipSystem_HPP

#include "MediaServerSystem.hpp"

namespace Emx {

    class MediaChipSystem : public MediaServerSystem {
    public:
        MediaChipSystem(MediaServerResource &res) : MediaServerSystem(res) {}

        virtual ~MediaChipSystem() {}

        ErrCodeE Start() override { return ErrCodeE::Success; }

        ErrCodeE Stop() override { return ErrCodeE::Success; }
    };
}


#endif //EMX_MediaChipSystem_HPP
