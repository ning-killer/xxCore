//
// Created by xiong on 2022/5/11.
//

#ifndef EMX_MediaChipAiPlugMotionDetection_HPP
#define EMX_MediaChipAiPlugMotionDetection_HPP

#include "MediaChipAiPlug.hpp"

namespace Emx {

    class MediaPlatAiPlugMotionDetection : public MediaPlatAiPlug {
    public:
        MediaPlatAiPlugMotionDetection(int chn, MediaPlatAiHandle *handle,
                                       MediaServerResource &res,
                                       MediaAi::Param &param) :
                MediaPlatAiPlug(chn, handle, res, param) {}

        virtual ~MediaPlatAiPlugMotionDetection() {}

        void Create(Json::Value &cfg) override {}

        void Destroy() override {}
    };

}

#endif //EMX_MediaChipAiPlugMotionDetection_HPP
