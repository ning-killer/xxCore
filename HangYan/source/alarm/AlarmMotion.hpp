/*
 * @Author: xiong
 * @Date: 2022/10/25
*/

#ifndef EAPIL_AlarmMotion_HPP
#define EAPIL_AlarmMotion_HPP

#include "AlarmBase.hpp"
#include "NightVisionClient.hpp"
namespace Emx {
    class AlarmMotion : public AlarmBase {
    public:
        AlarmMotion(OVDAlarmType type, OvdCtx *ctx,
                       std::list<AlarmBase *> *alarmList, OnAlarmUploadReqCallBack cb) :
                AlarmBase(type, ctx, alarmList, cb), m_delay(false), m_trigger(false) {}

        ErrCodeE Create() override;

        void Destroy() override;

    protected:
        void AlarmEnd() override;
    
    private:
        void CalibrationDetectThreshold();

    private:
        MediaClientAiDataAsync *m_ai;
        NightVisionClientInfo m_night;
        NightVision::Status m_nightStatus;
        //移动侦测触发后需要延迟1s，判断是否有人行或是警戒已经上报
        //如果支持人行和警戒且任意一个有上报，则不进行移动侦测上报
        EuvTimer m_motionDetectDelayTimer;
        bool m_delay;
        bool m_trigger;
    };
};

#endif //EAPIL_AlarmMotion_HPP
