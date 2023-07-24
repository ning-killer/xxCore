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
        void Clone(Gat1400Util::UploadDataParam &param
                , const std::map<int/*img偏移量*/, MediaAi::facePicMsg> &match_data
                , const uint8_t *face_data);

      private:
        MediaClientAiDataAsync *m_ai;
    };
}

#endif //HANGYAN_ALARM_ALRAMFACE_HPP
