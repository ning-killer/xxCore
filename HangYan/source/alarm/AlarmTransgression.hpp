/**
 * @file AlarmTransgression.hpp
 * @author hening
 * @brief 周界检测-越界检测
 * @date 2023-12-9
 */


#ifndef EAPIL_Transgression_HPP
#define EAPIL_Transgression_HPP

#include "AlarmBase.hpp"
namespace Emx {
    class AlarmTransgression : public AlarmBase {
    public:
        AlarmTransgression(OVDAlarmType type, OvdCtx *ctx,
                       std::list<AlarmBase *> *alarmList, OnAlarmUploadReqCallBack cb) :
                AlarmBase(type, ctx, alarmList, cb)
                , m_lastAlarmTargetId(-1)
                , m_lastAlarmTime(Time::GetS())
                , m_maxCacheSize(5)
                , m_isCrossed(false) { }

        ErrCodeE Create() override;

        void Destroy() override;

        bool isSupportedTargetType(const MediaAi::ClassifyTypeE type);
        void Alarm(int time, int id);

    private:
        MediaClientAiDataAsync *m_ai;
        int m_lastAlarmTargetId;
        int m_lastAlarmTime;
        int m_maxCacheSize;
        std::vector<MediaAi::AiInfoClassifyData> m_cacheTargets;
        bool m_isCrossed;
        EuvTimer m_checkAlgStatusTimer;
    };
};

#endif //EAPIL_Transgression_HPP
