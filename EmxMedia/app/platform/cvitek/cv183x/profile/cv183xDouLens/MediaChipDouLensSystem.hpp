#ifndef EMX_MediaChipDouLensSystem_HPP
#define EMX_MediaChipDouLensSystem_HPP

#include <MediaChipSystem.hpp>
#include "sample_comm.h"
#include "MediaServerSystem.hpp"

namespace Emx {

    class MediaChipDouLensSystem : public MediaChipSystem {
    public:
        MediaChipDouLensSystem(MediaServerResource &res) : MediaChipSystem(res){}

        virtual ~MediaChipDouLensSystem() {}

    public:
        ErrCodeE InitVb(const Json::Value &json) override;
        ErrCodeE Start() override;
        ErrCodeE Stop() override;
        ErrCodeE CreateVpssGroup(const Json::Value &json) override;
    };
}


#endif //EMX_MediaChipSystem_HPP
