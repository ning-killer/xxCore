/**
 * @file AlarmFace.hpp
 * @author hening
 * @brief 人脸告警
 * @date 2023-6-1
 */

#ifndef HANGYAN_ALARM_ALRAMFACE_HPP
#define HANGYAN_ALARM_ALRAMFACE_HPP

#include "AlarmBase.hpp"
#include "Gat1400Client.hpp"

namespace Emx {

    class AlarmFace : public AlarmBase {
      public:
        AlarmFace(OVDAlarmType type, OvdCtx *ctx, 
            std::list<AlarmBase *> *alarmList, OnAlarmUploadReqCallBack cb)
            : AlarmBase(type, ctx, alarmList, cb) { }
        ~AlarmFace() { };

      public:
        ErrCodeE Create() override;
        void Destroy() override;

      private:
        bool Clone(Gat1400Util::UploadFaceData &param, 
          const MediaAi::AiInfoFaceDetectionData& faceData);

      private:
        MediaClientAiDataAsync *m_ai;
    };
}

#endif //HANGYAN_ALARM_ALRAMFACE_HPP
