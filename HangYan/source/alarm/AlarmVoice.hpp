/*
 * @Author: xiong
 * @Date: 2022/10/25
*/

#ifndef EAPIL_AlarmVoice_HPP
#define EAPIL_AlarmVoice_HPP

#include "AlarmBase.hpp"

namespace Emx {
    class AlarmVoice : public AlarmBase {
    public:
        AlarmVoice(OVDAlarmType type, OvdCtx *ctx,
                      std::list<AlarmBase *> *alarmList, OnAlarmUploadReqCallBack cb) :
                AlarmBase(type, ctx, alarmList, cb) {}

        ErrCodeE Create() override;

        void Destroy() override;

    private:
        MediaClientStreamAsync *m_voice;
    };
};

#endif //EAPIL_AlarmVoice_HPP
