/**
 * @file AlarmLicensePlate.hpp
 * @author hening
 * @brief 车辆车牌检测
 * @date 2023-12-1
 */

#ifndef HANGYAN_ALARM_ALRAMLICENSEPLATE_HPP
#define HANGYAN_ALARM_ALRAMLICENSEPLATE_HPP

#include "AlarmBase.hpp"
#include "Gat1400Client.hpp"
#include "NightVisionClient.hpp"

namespace Emx {

    class AlarmLicensePlate : public AlarmBase {
      public:
        AlarmLicensePlate(OVDAlarmType type, OvdCtx *ctx, 
            std::list<AlarmBase *> *alarmList, OnAlarmUploadReqCallBack cb)
            : AlarmBase(type, ctx, alarmList, cb) { }
        ~AlarmLicensePlate() { };

      public:
        ErrCodeE Create() override;
        void Destroy() override;

      private:
        void Clone(Gat1400Util::UploadMotorVehiclesData &param, 
          const MediaAi::AiInfoVehicleLicensePlateDetectionData& VPData);

        void CalibrationAutoExposure();

      private:
        MediaClientAiDataAsync *m_ai;
        NightVisionClientInfo m_night;
        NightVision::Status m_nightStatus;
        EuvTimer m_montorNightTimer;
    };
}

#endif //HANGYAN_ALARM_ALRAMFACE_HPP
