//
// Created by xiong on 2022/5/12.
//

#ifndef EMX_MediaChipAiPlugPersonDetection_HPP
#define EMX_MediaChipAiPlugPersonDetection_HPP

#include "MediaChipAiPlug.hpp"

namespace Emx {

    class MediaPlatAiPlugPersonDetection : public MediaPlatAiPlug {
    public:
        MediaPlatAiPlugPersonDetection(int chn, MediaPlatAiHandle *handle,
                                       MediaServerResource &res,
                                       MediaAi::Param &param) :
                MediaPlatAiPlug(chn, handle, res, param) {}

        virtual ~MediaPlatAiPlugPersonDetection() {}

        void Create(Json::Value &cfg) override {}

        void Destroy() override  {}
    };
}

#endif //EMX_MediaChipAiPlugPersonDetection_HPP
