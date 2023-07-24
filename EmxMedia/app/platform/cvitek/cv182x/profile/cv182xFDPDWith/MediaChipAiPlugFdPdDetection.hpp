//
// Created by cx on 2023/6/20.
//

#ifndef EMX_MediaChipAiPlugFdPdDetection_HPP
#define EMX_MediaChipAiPlugFdPdDetection_HPP


#include "app/cviai_app.h"
#include "MediaChipAiPlug.hpp"

namespace Emx {
    enum class aIParamType : int32_t {
        SetSensitivity = 0, //!  设定人脸抓拍灵敏度
        SetCaptureNum = 1, //!  设置同一个人脸抓拍的张数
        SetCaptureMode = 2, //!   设置算法模式
        SetCaptureInterval = 3, //!  设置抓拍间隔（超时多少s就不抓了）
        SetCaptureQuality = 4, //!设置人脸瞳距（杭研用瞳距来控制质量）
        SetSmartAe = 5,
    };

    class MediaChipAiPlugFdPdDetection : public MediaChipAiPlug {
    public:

        MediaChipAiPlugFdPdDetection(int chn, MediaChipAiHandle *handle,
                                     MediaServerResource &res,
                                     MediaAi::Param &param) :
                MediaChipAiPlug(chn, handle, res, param) {}

        virtual ~MediaChipAiPlugFdPdDetection() {}

        void Create(Json::Value &cfg) override;

        void Destroy() override;

        void ConfigAiParam(const MediaAi::Param &param) override;

    private:
        void OnGetFrame(VIDEO_FRAME_INFO_S &frame);

        void OnComplete();

        void InitFaceCaptureConfig();

        template<class ...variableParam>
        int SetFaceParam(aIParamType paramType, variableParam... param);

        template<typename T, typename... Rest>
        int SetFaceCaptureProcess(aIParamType paramType, T value, Rest... rest);

        int SetFaceCaptureProcess(aIParamType paramType) {
            return 0;
        }

    public:
        int m_maxFaceNum;
    private:
        bool m_got;
        bool m_isStartAe;
        int m_imgQuality;
        int m_fps;
        int m_frameInterval;

        uint64_t m_timestampMs;
        Size m_srcPicSize;
        cviai_app_handle_t m_aiAppHandle;
        CVI_AI_SUPPORTED_MODEL_E m_FDModelId;
        face_capture_config_t m_faceCaptureConfig;
    };

}
#endif //EMX_MediaChipAiPlugFdPdDetection_HPP
