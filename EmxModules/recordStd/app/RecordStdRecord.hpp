/*
* @Author: xiong
* @Date: 2023/2/28
*/

#ifndef EMX_RecordStdRecord_HPP
#define EMX_RecordStdRecord_HPP

#include "RecordStdRecordFile.hpp"
#include "RecordStdResource.hpp"
#include "EventClientInfo.hpp"

namespace Emx {

    class RecordStdRecord {
    public:
        RecordStdRecord() : m_created(false) {}

        void Create(RecordStdResource *res);

        void Destroy();

        void Start();

        void Stop();

        ErrCodeE OnRecvReq(uint32_t module, uint32_t method,
                           const char *data, int32_t size, std::string &out);

    private:
        void GetRecordList(uint32_t startUtc, uint32_t endUtc, bool merge, std::vector<RecordStd::RecFile> &list);

    private:
        void EnterScheduleMode();

        void LeaveScheduleMode();

        void OnSchedule(const char *name, bool ena);

        void OnScheduleGetFrame(ErrCodeE e, MediaFrame &frame);

    private:

        void EnterEventMode();

        void LeaveEventMode();

        void OnGetEvent(Event::Info &info);

        void OnEventGetFrameInfo(MediaFrame &frame);

        void EventWriteFrame(MediaFrame &frame);

    private:
        void EnterManualMode();

        void LeaveManualMode();

        ErrCodeE SetManualRecord(bool ena);

        ErrCodeE GetManualRecord(bool &ena);

        void OnManualGetFrame(ErrCodeE e, MediaFrame &frame);

    private:
        RecordStdResource *m_res;
        RecordStd::Param::Record *m_recordParam;
        RecordStdRecordFile m_file;
        bool m_created;
        Schedule m_schedule;
        bool m_scheduleEna;
        bool m_scheduleInited;
        EuvTimer m_splitTimer;
        bool m_splitTimeout;
        MediaClientStreamAsync *m_stream;
        bool m_paramChange;
        bool m_manualStarted;
        EventClientInfo m_eventListen;
        std::list<MediaFrame> m_eventFrameList;
        Event::Info m_eventInfo;
    };
}

#endif //EMX_RecordStdRecord_HPP
