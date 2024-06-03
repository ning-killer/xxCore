/**
 * @file AlarmPassengerFlow.hpp
 * @author hening
 * @brief 客流统计
 * @date 2024-5-9
 */


#ifndef EAPIL_AlarmPassengerFlow_HPP
#define EAPIL_AlarmPassengerFlow_HPP

#include "AlarmBase.hpp"
#include "Gat1400Client.hpp"

namespace Emx {
    class AlarmPassengerFlow : public AlarmBase {
    public:
        AlarmPassengerFlow(OVDAlarmType type, OvdCtx *ctx,
                       std::list<AlarmBase *> *alarmList, OnAlarmUploadReqCallBack cb) :
                AlarmBase(type, ctx, alarmList, cb)
                , m_maxCacheSize(20)
                , m_inCount(0) 
                , m_outCount(0) { }

        ErrCodeE Create() override;

        void Destroy() override;

        void ShowOsd();

        void ManageGatPost();

        void PackUploadTrafficData(Gat1400Util::UploadTrafficData &param);

    private:
        MediaClientAiDataAsync *m_ai;
        int m_maxCacheSize;
        std::vector<MediaAi::AiInfoPassengerFlowDetectionData> m_cacheTargets;
        EuvTimer m_checkAlgStatusTimer;
        EuvTimer m_checkGatPostTimer;
        int m_inCount;
        int m_outCount;
    };
};

#endif //EAPIL_AlarmPassengerFlow_HPP
