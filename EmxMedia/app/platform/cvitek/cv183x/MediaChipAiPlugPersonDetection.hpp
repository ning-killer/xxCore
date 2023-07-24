#ifndef EMX_MediaChipAiPlugPersonDetection_HPP
#define EMX_MediaChipAiPlugPersonDetection_HPP

#include "MediaChipAiPlug.hpp"

namespace Emx {

    class MediaChipAiPlugPersonDetection : public MediaChipAiPlug {
    public:
        MediaChipAiPlugPersonDetection(int chn,MediaChipAiHandle *hadnle,
                                       MediaServerResource &res,
                                       MediaAi::Param &param):
                MediaChipAiPlug(chn,hadnle,res,param){}

    virtual ~MediaChipAiPlugPersonDetection() {}

    void Create(Json::Value &cfg) override;

    void Destroy() override {
         m_callBackList.clear();
    }

    protected:
        void OnGetFrame(VIDEO_FRAME_INFO_S &frame);

        void OnComplete();

    protected:
        bool m_got;
        int m_vpssGrp;
        int m_vpssChn;
        CVI_AI_SUPPORTED_MODEL_E m_modelId;
        uint64_t m_timestampMs;
        Size m_srcPicSize;
        cvai_object_t m_obj;
    };
}

#endif //EMX_MediaChipAiPlugPersonDetection_HPP
