/**
 * @file AlarmMask.hpp
 * @author hening
 * @brief 人脸口罩检测
 * @date 2023-6-1
 */

#ifndef HANGYAN_ALARM_ALRAMMASK_HPP
#define HANGYAN_ALARM_ALRAMMASK_HPP

#include "AlarmBase.hpp"
#include "Gat1400Client.hpp"

namespace Emx {

    class AlarmMask : public AlarmBase {
      public:
        AlarmMask(OVDAlarmType type, OvdCtx *ctx, 
            std::list<AlarmBase *> *alarmList, OnAlarmUploadReqCallBack cb)
            : AlarmBase(type, ctx, alarmList, cb) { }
        ~AlarmMask() { };

      public:
        ErrCodeE Create() override;
        void Destroy() override;

      private:
        MediaClientAiDataAsync *m_ai;
    };
}

#endif //HANGYAN_ALARM_ALRAMMASK_HPP
