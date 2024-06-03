/*
 * @Author: xiong
 * @Date: 2022/10/25
*/

#ifndef EAPIL_AlarmBase_HPP
#define EAPIL_AlarmBase_HPP

#include "OvdCtx.hpp"
#include "EnvCommon.hpp"
#include "EmxMedia.hpp"
#include <iostream>
#include <fstream>

namespace Emx {
    class AlarmBase {
    public:
        using OnAlarmUploadReqCallBack = std::function<void(OVDAlarmType type, bool start)>;

        AlarmBase(OVDAlarmType type, OvdCtx *ctx,
                  std::list<AlarmBase *> *alarmList, OnAlarmUploadReqCallBack cb) :
                m_type(type),
                m_ctx(ctx),
                m_alarmList(alarmList),
                m_cb(std::move(cb)),
                m_started(false),
                m_alarmEnd(false),
                m_alarmContinueReachMin(false),
                m_ena(true),
                m_isProcessing(false) {
            m_endJudgeTimer.Create(m_ctx->loop);
            m_continueMinTimer.Create(m_ctx->loop);
            m_continueMaxTimer.Create(m_ctx->loop);

            m_lightDuration.Create(m_ctx->loop);
            m_lightBlink.Create(m_ctx->loop);
            m_playStatusUpate.Create(m_ctx->loop);
            m_lightIntelligent.Create(m_ctx->loop);
            m_playStatusCount = 0;
            m_lastCheckTime = Time::GetS();
            m_cloudBroadcastEnable = false;
        }

        virtual ~AlarmBase() {
            m_endJudgeTimer.Destroy();
            m_continueMinTimer.Destroy();
            m_continueMaxTimer.Destroy();

            m_lightDuration.Destroy();
            m_lightBlink.Destroy();
            m_playStatusUpate.Destroy();
            m_lightIntelligent.Destroy();
        }

        virtual ErrCodeE Create() = 0;

        virtual void Destroy() = 0;

        void SetEna(bool ena) { m_ena = ena; }

        bool IsProcessing() { return m_isProcessing; }

        static const char *GetAlarmName(OVDAlarmType type);

        OVDAlarmType GetType() { return m_type; }

        bool IsStarted() { return m_started; }

    protected:
        static bool AlarmInZone(Size &srcImgSize, Rect &rect, Zone &zone);

        virtual void AlarmEnd();

        virtual void AlarmTrigger();

        void Strategy(EnvStrategy *strategy, bool *ena, std::string alarmVoice, bool ContorlIntelligentLight = false);
        void BreakVoip();
        void IntelligentLight(bool *ena, const uint32_t frequency_time = 0);
        void UpdatePlayStatus();
        bool GetIsPlaying();
        bool IsStopAudio(bool *ena);
        bool IsFlashingLight();
        void ExitLightContorl(bool isResetDay = true);
        void CheckLightContorl(bool *ena, bool *light);
        bool CheckHeadSrcSize(const Size &srcImgSize);
        void BreakLastSoundAndLightAlarm();

    protected:
        OVDAlarmType m_type;
        OvdCtx *m_ctx;
        std::list<AlarmBase *> *m_alarmList;
        OnAlarmUploadReqCallBack m_cb;
        bool m_started;
        static const int AlarmEndJudgeTime = 5000;//连续(x)ms内没有再次出现此告警，则认为此告警结束
        EuvTimer m_endJudgeTimer;//用于判断告警结束
        static const int AlarmContinueTimeMin = 10100;//告警触发后最少需要持续(x)ms，才能上报告警结束
        EuvTimer m_continueMinTimer;//用于判断告警达到最小持续时间要求
        static const int AlarmContinueTimeMax = 3600000 - 1200;//告警触发后最少需要持续(x)ms，才能上报告警结束
        EuvTimer m_continueMaxTimer;//用于判断告警达到最大持续时间要求
        bool m_alarmEnd;
        bool m_alarmContinueReachMin;
        bool m_ena;
        bool m_isProcessing;

        bool m_isIntelligentNightVision;
        uint32_t m_lastCheckTime;
        EuvTimer m_lightIntelligent;
        EuvTimer m_playStatusUpate;
        uint32_t m_playStatusCount;
        EuvTimer m_lightDuration;
        EuvTimer m_lightBlink;
        std::mutex m_lightExit_mtx;
        bool m_cloudBroadcastEnable; // 闪灯前记录云广播开启状态
        bool *m_lightBlinkEnaFlag;
        bool *m_lightIntelligentEnaFlag;
        bool *m_enaFlag;
        SoundAlarmControl::Type m_strategyType;
    };
};

#endif //EAPIL_AlarmBase_HPP
