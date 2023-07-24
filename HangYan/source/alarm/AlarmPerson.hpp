/*
 * @Author: xiong
 * @Date: 2022/10/25
*/

#ifndef EAPIL_AlarmPerson_HPP
#define EAPIL_AlarmPerson_HPP

#include "AlarmBase.hpp"

namespace Emx {
    class AlarmPerson : public AlarmBase {
    public:
        AlarmPerson(OVDAlarmType type, OvdCtx *ctx,
                       std::list<AlarmBase *> *alarmList, OnAlarmUploadReqCallBack cb) :
                AlarmBase(type, ctx, alarmList, cb) {}

        ErrCodeE Create() override;

        void Destroy() override;

        bool IsStaticPerson(Size &size, MediaAi::AiInfoPersonDetectionData &data);

    private:
        MediaClientAiDataAsync *m_ai;
        MediaAi::AiInfoPersonDetectionData m_cache[5];
        int m_cacheIdx;
    };
};

#endif //EAPIL_AlarmPerson_HPP
