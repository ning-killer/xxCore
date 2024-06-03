/**
 * @file AlarmAlertarea.hpp
 * @author hening
 * @brief 周界检测-区域入侵
 * @date 2023-12-9
 */

#ifndef EAPIL_AlarmAlertarea_HPP
#define EAPIL_AlarmAlertarea_HPP

#include "AlarmBase.hpp"

namespace Emx {
    class AlarmAlertarea : public AlarmBase {
    public:
        AlarmAlertarea(OVDAlarmType type, OvdCtx *ctx,
                       std::list<AlarmBase *> *alarmList, OnAlarmUploadReqCallBack cb) :
                AlarmBase(type, ctx, alarmList, cb)
                , m_cacheTargetId(-1)
                , m_cacheTargetTime(Time::GetS()) { }

        ErrCodeE Create() override;

        void Destroy() override;

        bool isSupportedTargetType(const MediaAi::ClassifyTypeE type);
        void Alarm(int time, int id, bool isHavePerson);
        void HandleIntelligentLight(MediaAi::AiInfoHeader &header);

    private:
        MediaClientAiDataAsync *m_ai;
        int m_cacheTargetId;
        int m_cacheTargetTime;
        EuvTimer m_checkAlgStatusTimer;
    };
};

#endif //EAPIL_AlarmAlertarea_HPP
