/*
* @Author: xiong
* @Date: 2023/3/1
*/

#include "RecordStdSnap.hpp"
#include "RecordStdInner.hpp"

using namespace Emx;

void RecordStdSnap::Create(RecordStdResource *res) {
    if (!m_created) {
        m_res = res;
        m_snapParam = &res->param->snap;
        m_file.CreateIndex(m_res->param->path);
        m_stream = new MediaClientStreamAsync(m_res->loop);
        MediaClientStream::Channel channel;
        channel.Add(MediaFrame::TypeE::Snap, m_snapParam->chn);
        m_stream->Start(channel, std::bind(&RecordStdSnap::OnGetFrame, this, ph_1, ph_2));
        m_created = true;
        Start();
    }
}

void RecordStdSnap::Destroy() {
    if (m_created) {
        m_stream->Stop();
        Stop();
        delete m_stream;
        m_file.DestroyIndex();
        m_created = false;
    }
}

void RecordStdSnap::Start() {
    if (!m_created)
        return;
    switch (m_snapParam->mode) {
        case RecordStd::Param::ModeE::Schedule:
            return EnterScheduleMode();
        case RecordStd::Param::ModeE::Manual:
            return EnterManualMode();
        case RecordStd::Param::ModeE::Event:
            return EnterEventMode();
        default:
            emxloge("unknown mode %d\n", m_snapParam->mode);
            return;
    }
}

void RecordStdSnap::Stop() {
    if (!m_created)
        return;
    switch (m_snapParam->mode) {
        case RecordStd::Param::ModeE::Schedule:
            return LeaveScheduleMode();
        case RecordStd::Param::ModeE::Manual:
            return LeaveManualMode();
        case RecordStd::Param::ModeE::Event:
            return LeaveEventMode();
        default:
            emxloge("unknown mode %d\n", m_snapParam->mode);
            return;
    }
}

ErrCodeE RecordStdSnap::OnRecvReq(uint32_t module, uint32_t method,
                                  const char *data, int32_t size, std::string &out) {
    if (!m_created)
        return ErrCodeE::Failure;
    switch ((RecordStdSnapMethodE) method) {
        case RecordStdSnapMethodE::TriggerManualSnap : {
            if (m_snapParam->mode != RecordStd::Param::ModeE::Manual)
                return ErrCodeE::Failure;
            if (m_file.FreeSpace(m_res->sdcardChn, m_res->param->reservedSpace) != ErrCodeE::Success)
                return ErrCodeE::ResNotAvailable;
            auto e = MediaClientSnap::Trigger(m_snapParam->chn, 100);
            if (e == ErrCodeE::Success)
                m_snapping = true;
            return e;
        }
        case RecordStdSnapMethodE::GetSnapList : {
            uint32_t startUtc;
            uint32_t endUtc;
            int off = 0;
            memcpy(&startUtc, data + off, sizeof(startUtc));
            off += sizeof(startUtc);
            memcpy(&endUtc, data + off, sizeof(endUtc));
            off += sizeof(endUtc);
            std::vector <RecordStd::RecFile> list;
            GetSnapList(startUtc, endUtc, list);
            int num = (int) list.size();
            out.append((char *) &num, sizeof(num));
            for (auto &elem : list) {
                out.append((char *) &elem, sizeof(elem));
            }
            return ErrCodeE::Success;
        }
        default:
            return ErrCodeE::IllegalParam;
    }
}

void RecordStdSnap::OnGetFrame(ErrCodeE e, MediaFrame &frame) {
    if (e == ErrCodeE::Success && m_snapping && frame.chn == m_snapParam->chn) {
        m_file.WriteFile(Time::GetS(), frame);
        m_snapping = false;
    }
}


void RecordStdSnap::EnterScheduleMode() {
    m_snapInvTimer.Create(m_res->loop);
    m_scheduleInited = false;
    m_scheduleEna = false;
    m_schedule.Create(m_res->loop, "RecordStdSnap",
                      m_snapParam->schedule.schedule,
                      std::bind(&RecordStdSnap::OnSchedule, this, ph_1, ph_2));
}

void RecordStdSnap::LeaveScheduleMode() {
    m_snapInvTimer.Destroy();
    OnSchedule("RecordStdSnap", false);
    m_schedule.Destroy();
}

void RecordStdSnap::OnSchedule(const char *name, bool ena) {
    //用于去重
    if (m_scheduleInited && ena == m_scheduleEna)
        return;
    if (!m_scheduleInited) {
        m_scheduleInited = true;
    }

    if (ena) {
        m_snapInvTimer.Start(0, m_snapParam->snapInvS * 1000, [this]() {
            if (m_file.FreeSpace(m_res->sdcardChn, m_res->param->reservedSpace) != ErrCodeE::Success)
                return;
            auto e = MediaClientSnap::Trigger(m_snapParam->chn, 100);
            if (e == ErrCodeE::Success)
                m_snapping = true;
        });
    } else {
        m_snapInvTimer.Start(0, m_snapParam->snapInvS * 1000, [this]() {
            if (m_file.FreeSpace(m_res->sdcardChn, m_res->param->reservedSpace) != ErrCodeE::Success)
                return;
            auto e = MediaClientSnap::Trigger(m_snapParam->chn, 100);
            if (e == ErrCodeE::Success)
                m_snapping = true;
        });
        m_snapInvTimer.Stop();
    }
}

void RecordStdSnap::EnterEventMode() {
    m_eventListen.Start(m_res->loop, m_res->buffer, sizeof(m_res->buffer),
                        std::bind(&RecordStdSnap::OnGetEvent, this, ph_1));
}

void RecordStdSnap::LeaveEventMode() {
    m_eventListen.Stop();
}

void RecordStdSnap::OnGetEvent(Event::Info &info) {
    bool find = false;
    for (auto &event : m_res->param->record.events) {
        if (info.type == event.type && info.chn == event.chn) {
            find = true;
            break;
        }
    }
    if (!find)
        return;
    if (info.isStart) {
        if (m_file.FreeSpace(m_res->sdcardChn, m_res->param->reservedSpace) != ErrCodeE::Success)
            return;
        auto e = MediaClientSnap::Trigger(m_snapParam->chn, 100);
        if (e == ErrCodeE::Success)
            m_snapping = true;
    }

}

void RecordStdSnap::GetSnapList(uint32_t startUtc, uint32_t endUtc, std::vector <RecordStd::RecFile> &list) {
    m_file.GetSnapList(startUtc, endUtc, list);
}
