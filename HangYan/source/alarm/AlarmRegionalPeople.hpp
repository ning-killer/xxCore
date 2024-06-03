/**
 * @file AlarmRegionalPeople.hpp
 * @author hening
 * @brief 区域人数统计
 * @date 2024-5-9
 */


#ifndef EAPIL_AlarmRegionalPeople_HPP
#define EAPIL_AlarmRegionalPeople_HPP

#include "AlarmBase.hpp"
#include "Gat1400Client.hpp"
namespace Emx {
    class AlarmRegionalPeople : public AlarmBase {
    public:
        AlarmRegionalPeople(OVDAlarmType type, OvdCtx *ctx,
                       std::list<AlarmBase *> *alarmList, OnAlarmUploadReqCallBack cb) :
                AlarmBase(type, ctx, alarmList, cb)
                , m_inPeopleCount(0)
                , m_lastStatus(unknown) { }

        ErrCodeE Create() override;

        void Destroy() override;

        bool isSupportedTargetType(const MediaAi::ClassifyTypeE type);
        void ShowOsd();
        void Alarm(int time);
        void PackUploadRegionData(Gat1400Util::UploadRegionData &param);
        void ManageGatPost();

        enum RegionStatus {
            unknown = 0,
            normal,
            alert,
        };

    private:
        MediaClientAiDataAsync *m_ai;
        int m_inPeopleCount;
        EuvTimer m_alarmTimer;
        EuvTimer m_gatPostTimer;
        EuvTimer m_gatCheckTimer;
        int m_lastAlarmTime;
        RegionStatus m_lastStatus;
    };
};

#endif //EAPIL_AlarmRegionalPeople_HPP
