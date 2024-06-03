/**
 * @file AlarmOffduty.hpp
 * @author hening
 * @brief 离岗检测
 * @date 2024-5-9
 */


#ifndef EAPIL_AlarmOffduty_HPP
#define EAPIL_AlarmOffduty_HPP

#include "AlarmBase.hpp"
#include "Gat1400Client.hpp"
namespace Emx {
    class AlarmOffduty : public AlarmBase {
    public:
        AlarmOffduty(OVDAlarmType type, OvdCtx *ctx,
                       std::list<AlarmBase *> *alarmList, OnAlarmUploadReqCallBack cb) :
                AlarmBase(type, ctx, alarmList, cb) 
                , m_lastStatus(Gat1400Util::OnLeaveStatus::Unknown)
                , m_offDutyCount(0)
                , m_onDutyCount(0) { }

        ErrCodeE Create() override;

        void Destroy() override;

        bool isSupportedTargetType(const MediaAi::ClassifyTypeE type);
        void Alarm(int startTime = 0);
        void PackUploadOffdutyData(Gat1400Util::UploadOnLeaveData &param);

    private:
        MediaClientAiDataAsync *m_ai;
        Gat1400Util::OnLeaveStatus m_lastStatus;
        EuvTimer m_offdutyDurationTimer;
        EuvTimer m_checkTimer;
        int m_offDutyCount; //离岗人数
        int m_onDutyCount;  //在岗人数
    };
};

#endif //EAPIL_AlarmOffduty_HPP
