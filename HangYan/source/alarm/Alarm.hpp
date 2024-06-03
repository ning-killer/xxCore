/*
 * @Author: xiong
 * @Date: 2022/10/21
*/

#ifndef EAPIL_Alarm_HPP
#define EAPIL_Alarm_HPP

#include "OvdCtx.hpp"
#include "AlarmBase.hpp"

namespace Emx {
    class Alarm {
    public:
        Alarm() : m_created(false) {}

        ErrCodeE Create(OvdCtx *ctx);

        void Destroy();

        void SetEna(OVDAlarmType type, bool ena);

    private:
        void OnAlarmUploadReqCallBack(OVDAlarmType type, bool start);

        void OnUploadAlarm(void *arg);

        void OnUploadComplete(ErrCodeE e, void *arg);

    private:

        struct AlarmUploadItem {
            OVDAlarmType type;
            enum class FlagE {
                Start,
                End
            } flag;
            uint32_t utc;
        };
    private:
        bool m_created;
        OvdCtx *m_ctx;
        EuvWork m_workUpload;
        std::list<AlarmBase *> m_alarmList;
        std::list<AlarmUploadItem *> m_alarmUploadList;
    };
};

#endif //EAPIL_Alarm_HPP
