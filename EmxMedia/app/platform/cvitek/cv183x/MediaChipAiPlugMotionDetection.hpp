#ifndef EMX_MediaChipAiPlugMotionDetection_HPP
#define EMX_MediaChipAiPlugMotionDetection_HPP

#include "MediaChipAiPlug.hpp"
#include "core/object/cvai_object_types.h"

namespace Emx {

    class MediaChipAiPlugMotionDetection : public MediaChipAiPlug {
    public:
        MediaChipAiPlugMotionDetection(int chn, MediaChipAiHandle *handle,
                                       MediaServerResource &res,
                                       MediaAi::Param &param) :
                MediaChipAiPlug(chn, handle, res, param) {}

        virtual ~MediaChipAiPlugMotionDetection() {}

        void Create(Json::Value &cfg) override;

        void Destroy() override {
            m_callBackList.clear();
        }

    protected:
        void OnGetFrame(VIDEO_FRAME_INFO_S &frame);

        void OnComplete();


    protected:
        int m_interval;
        int m_count;
        int m_threshold;
        int m_minArea;
        bool m_got;
        uint64_t m_timestampMs;
        Size m_srcPicSize;
        cvai_object_t m_obj;
    };

}

#endif //EMX_MediaChipAiPlugMotionDetection_HPP
