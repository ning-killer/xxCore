/*
* @Author: xiong
* @Date: 2023/3/1
*/

#ifndef EMX_RecordStdSnap_HPP
#define EMX_RecordStdSnap_HPP

#include "RecordStdResource.hpp"
#include "RecordStdSnapFile.hpp"
#include "EventClientInfo.hpp"

namespace Emx {

    class RecordStdSnap {
    public:
        RecordStdSnap() : m_created(false) {}

        void Create(RecordStdResource *res);

        void Destroy();

        void Start();

        void Stop();

        ErrCodeE OnRecvReq(uint32_t module, uint32_t method,
                           const char *data, int32_t size, std::string &out);

    private:
        void GetSnapList(uint32_t startUtc, uint32_t endUtc, std::vector<RecordStd::RecFile> &list);

    private:
        void OnGetFrame(ErrCodeE e, MediaFrame &frame);

    private:
        void EnterScheduleMode();

        void LeaveScheduleMode();

        void OnSchedule(const char *name, bool ena);

    private:

        void EnterEventMode();

        void LeaveEventMode();

        void OnGetEvent(Event::Info &info);

    private:
        void EnterManualMode() {}

        void LeaveManualMode() {}

    private:
        RecordStdResource *m_res;
        RecordStd::Param::Snap *m_snapParam;
        RecordStdSnapFile m_file;
        bool m_created;
        Schedule m_schedule;
        bool m_scheduleEna;
        bool m_scheduleInited;
        MediaClientStreamAsync *m_stream;
        EventClientInfo m_eventListen;
        bool m_snapping;
        EuvTimer m_snapInvTimer;
    };

}

#endif //EMX_RecordStdSnap_HPP
