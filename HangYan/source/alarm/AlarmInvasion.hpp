/*
* @Author: xiong
* @Date: 2022/10/26
*/

#ifndef EAPIL_AlarmInvasion_HPP
#define EAPIL_AlarmInvasion_HPP

#include "AlarmBase.hpp"
#include "mutex"

namespace Emx {

    class AlarmInvasion : public AlarmBase {
    public:
        AlarmInvasion(OVDAlarmType type, OvdCtx *ctx,
                      std::list<AlarmBase *> *alarmList, OnAlarmUploadReqCallBack cb) :
                AlarmBase(type, ctx, alarmList, cb) {}
                // m_playStatusCount(0),
                // m_lastCheckTime(Time::GetS()),
                // m_cloudBroadcastEnable(false)

        ErrCodeE Create() override;

        void Destroy() override;

    private:
        // void Strategy();
        // bool IsStopAudio();
        // void CheckLightContorl();
        // void ExitLightContorl();
        // void BreakVoip();
        // void UpdatePlayStatus();
        // bool GetIsPlaying();
        // bool IsFlashingLight();
        // void IntelligentLight(const uint32_t frequency_time = 0);

    private:
        MediaClientAiDataAsync *m_ai;
        // EuvTimer m_lightDuration;
        // EuvTimer m_lightBlink;
        // EuvTimer m_playStatusUpate;
        // EuvTimer m_lightIntelligent;
        // std::mutex m_lightExit_mtx;
        // uint32_t m_playStatusCount;
        // bool m_isIntelligentNightVision;
        // uint32_t m_lastCheckTime;
        // bool m_cloudBroadcastEnable; // 闪灯前记录云广播开启状态
    };

}

#endif //EAPIL_AlarmInvasion_HPP
