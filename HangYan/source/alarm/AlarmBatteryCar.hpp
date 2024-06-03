/**
 * @file AlarmBatteryCar.hpp
 * @author hening
 * @brief 电瓶车告警
 * @date 2023-12-1
 */

#ifndef HANGYAN_ALARM_ALRAMBATTERYCAR_HPP
#define HANGYAN_ALARM_ALRAMBATTERYCAR_HPP

#include "AlarmBase.hpp"
#include "Gat1400Client.hpp"

namespace Emx {

    class AlarmBatteryCar : public AlarmBase {
      public:
        AlarmBatteryCar(OVDAlarmType type, OvdCtx *ctx, 
            std::list<AlarmBase *> *alarmList, OnAlarmUploadReqCallBack cb)
            : AlarmBase(type, ctx, alarmList, cb) { }
        ~AlarmBatteryCar() { };

      public:
        ErrCodeE Create() override;
        void Destroy() override;

      private:
        void Clone(Gat1400Util::UploadNonMotorVehiclesData &param, 
          const MediaAi::AiInfoBatteryCarDetectionData& BcData);
        bool isMatchedTarget(MediaAi::BatteryCarType type);

      private:
        MediaClientAiDataAsync *m_ai;
    };
}

#endif //HANGYAN_ALARM_ALRAMBATTERYCAR_HPP
