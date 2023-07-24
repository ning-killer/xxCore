/*
* @Author: xiong
* @Date: 2023/2/28
*/

#include "RecordStdRecord.hpp"
#include "RecordStdInner.hpp"

using namespace Emx;

void RecordStdRecord::Create(RecordStdResource *res) {
    if (!m_created) {
        m_res = res;
        m_recordParam = &res->param->record;
        m_file.CreateIndex(m_res->param->path);
        m_stream = new MediaClientStreamAsync(m_res->loop);
        m_created = true;
        Start();
    }
}

void RecordStdRecord::Destroy() {
    if (m_created) {
        Stop();
        delete m_stream;
        m_file.DestroyIndex();
        m_created = false;
    }
}

void RecordStdRecord::Start() {
    if (!m_created)
        return;
    switch (m_recordParam->mode) {
        case RecordStd::Param::ModeE::Schedule:
            return EnterScheduleMode();
        case RecordStd::Param::ModeE::Manual:
            return EnterManualMode();
        case RecordStd::Param::ModeE::Event:
            return EnterEventMode();
        default:
            emxloge("unknown mode %d\n", m_recordParam->mode);
            return;
    }
}

void RecordStdRecord::Stop() {
    if (!m_created)
        return;
    switch (m_recordParam->mode) {
        case RecordStd::Param::ModeE::Schedule:
            return LeaveScheduleMode();
        case RecordStd::Param::ModeE::Manual:
            return LeaveManualMode();
        case RecordStd::Param::ModeE::Event:
            return LeaveEventMode();
        default:
            emxloge("unknown mode %d\n", m_recordParam->mode);
            return;
    }
}

ErrCodeE RecordStdRecord::OnRecvReq(uint32_t module, uint32_t method,
                                    const char *data, int32_t size, std::string &out) {
    if (!m_created)
        return ErrCodeE::Failure;
    switch ((RecordStdRecordMethodE) method) {
        case RecordStdRecordMethodE::GetManualRecord: {
            bool ena;
            auto e = GetManualRecord(ena);
            out.append((char *) &ena, sizeof(ena));
            return e;
        }
        case RecordStdRecordMethodE::SetManualRecord: {
            if (m_recordParam->mode != RecordStd::Param::ModeE::Manual)
                return ErrCodeE::Failure;
            bool ena;
            memcpy(&ena, data, sizeof(ena));
            return SetManualRecord(ena);
        }
        case RecordStdRecordMethodE::GetRecordList: {
            uint32_t startUtc;
            uint32_t endUtc;
            bool merge;
            int off = 0;
            memcpy(&startUtc, data + off, sizeof(startUtc));
            off += sizeof(startUtc);
            memcpy(&endUtc, data + off, sizeof(endUtc));
            off += sizeof(endUtc);
            memcpy(&merge, data + off, sizeof(merge));
            off += sizeof(merge);
            std::vector<RecordStd::RecFile> list;
            GetRecordList(startUtc, endUtc, merge, list);
            int num = (int) list.size();
            out.append(num, sizeof(num));
            for (auto &elem : list) {
                out.append((char *) &elem, sizeof(elem));
            }
            return ErrCodeE::Success;
        }
        default:
            return ErrCodeE::IllegalParam;
    }
}


void RecordStdRecord::EnterScheduleMode() {
    m_scheduleInited = false;
    m_scheduleEna = false;
    m_schedule.Create(m_res->loop, "RecordStdRecord",
                      m_recordParam->schedule.schedule,
                      std::bind(&RecordStdRecord::OnSchedule, this, ph_1, ph_2));

}

void RecordStdRecord::LeaveScheduleMode() {
    OnSchedule("RecordStdRecord", false);
    m_schedule.Destroy();
}

void RecordStdRecord::OnSchedule(const char *name, bool ena) {
    //用于去重
    if (m_scheduleInited && ena == m_scheduleEna)
        return;
    if (!m_scheduleInited) {
        m_scheduleInited = true;
    }

    if (ena) {
        m_splitTimeout = true;
        m_paramChange = true;
        m_splitTimer.Create(m_res->loop);
        MediaClientStream::Channel channel;
        if (m_res->param->record.vencChn >= 0)
            channel.Add(MediaFrame::TypeE::Venc, m_recordParam->vencChn);
        if (m_res->param->record.aencChn >= 0)
            channel.Add(MediaFrame::TypeE::Aenc, m_recordParam->aencChn);
        m_stream->Start(channel, std::bind(&RecordStdRecord::OnScheduleGetFrame, this, ph_1, ph_2));
    } else {
        m_stream->Stop();
        m_splitTimer.Destroy();
        m_file.CompleteFile(Time::GetS());
    }
}

void RecordStdRecord::OnScheduleGetFrame(ErrCodeE e, MediaFrame &frame) {
    if (e != ErrCodeE::Success)
        return;

    //设想中venc的frame.paramChange和frame.isKeyFrame应该同时出现，这里加打印看看会不会有异常情况
    if (frame.type == MediaFrame::TypeE::Venc && frame.paramChange && !frame.isKeyFrame)
        emxlogc("chn %d venc param changed but is not key frame\n", frame.chn);

    bool createNewFile = false;
    //参数变化了需要等待I帧重新录制文件，期间的所有数据丢弃
    if (frame.paramChange)
        m_paramChange = true;
    if (m_paramChange) {
        if (frame.type == MediaFrame::TypeE::Venc && frame.isKeyFrame) {
            m_paramChange = false;
            createNewFile = true;
        } else {
            return;
        }
    }
    if (m_splitTimeout && frame.type == MediaFrame::TypeE::Venc && frame.isKeyFrame) {
        createNewFile = true;
        m_splitTimeout = false;
    }
    if (createNewFile) {
        uint32_t curUtc = Time::GetS();
        m_file.CompleteFile(curUtc);
        if (m_file.FreeSpace(m_res->sdcardChn, m_res->param->reservedSpace) != ErrCodeE::Success) {
            emxloge("free space failed\n");
            return;
        }
        m_file.CreateNewFile(curUtc, m_recordParam->vencChn, m_recordParam->aencChn);
        m_splitTimer.Start(m_recordParam->splitTimeS * 1000, 0,
                           [this]() { m_splitTimeout = true; });
    }
    m_file.WriteFile(frame);
}

void RecordStdRecord::EnterManualMode() {
    m_manualStarted = false;
    m_paramChange = true;
}

void RecordStdRecord::LeaveManualMode() {
    SetManualRecord(false);
}

ErrCodeE RecordStdRecord::SetManualRecord(bool ena) {
    if (m_manualStarted == ena)
        return ErrCodeE::Success;
    if (ena) {
        if (m_file.FreeSpace(m_res->sdcardChn, m_res->param->reservedSpace) != ErrCodeE::Success) {
            emxloge("free space failed\n");
            return ErrCodeE::ResNotAvailable;
        }
        MediaClientStream::Channel channel;
        if (m_res->param->record.vencChn >= 0)
            channel.Add(MediaFrame::TypeE::Venc, m_recordParam->vencChn);
        if (m_res->param->record.aencChn >= 0)
            channel.Add(MediaFrame::TypeE::Aenc, m_recordParam->aencChn);
        m_stream->Start(channel, std::bind(&RecordStdRecord::OnManualGetFrame, this, ph_1, ph_2));
        m_paramChange = true;
    } else {
        m_stream->Stop();
        m_splitTimer.Destroy();
        m_file.CompleteFile(Time::GetS());
    }
    m_manualStarted = ena;
    return ErrCodeE::Success;
}

ErrCodeE RecordStdRecord::GetManualRecord(bool &ena) {
    ena = m_manualStarted;
    return ErrCodeE::Success;
}

void RecordStdRecord::OnManualGetFrame(ErrCodeE e, MediaFrame &frame) {
    if (e != ErrCodeE::Success)
        return;

    //设想中venc的frame.paramChange和frame.isKeyFrame应该同时出现，这里加打印看看会不会有异常情况
    if (frame.type == MediaFrame::TypeE::Venc && frame.paramChange && !frame.isKeyFrame)
        emxlogc("chn %d venc param changed but is not key frame\n", frame.chn);

    bool createNewFile = false;
    //参数变化了需要等待I帧重新录制文件，期间的所有数据丢弃
    if (frame.paramChange)
        m_paramChange = true;
    if (m_paramChange) {
        if (frame.type == MediaFrame::TypeE::Venc && frame.isKeyFrame) {
            m_paramChange = false;
            createNewFile = true;
        } else {
            return;
        }
    }

    if (createNewFile) {
        uint32_t curUtc = Time::GetS();
        m_file.CompleteFile(curUtc);
        if (m_file.FreeSpace(m_res->sdcardChn, m_res->param->reservedSpace) != ErrCodeE::Success) {
            emxloge("free space failed\n");
            return;
        }
        m_file.CreateNewFile(curUtc, m_recordParam->vencChn, m_recordParam->aencChn);
    }
    m_file.WriteFile(frame);
}

void RecordStdRecord::EnterEventMode() {
    m_eventInfo.isStart = false;
    m_eventListen.Start(m_res->loop, m_res->buffer, sizeof(m_res->buffer),
                        std::bind(&RecordStdRecord::OnGetEvent, this, ph_1));
    MediaClientStream::Channel channel;
    if (m_res->param->record.vencChn >= 0)
        channel.Add(MediaFrame::TypeE::Venc, m_recordParam->vencChn);
    if (m_res->param->record.aencChn >= 0)
        channel.Add(MediaFrame::TypeE::Aenc, m_recordParam->aencChn);
    m_stream->Start(channel, nullptr, std::bind(&RecordStdRecord::OnEventGetFrameInfo, this, ph_1));
}

void RecordStdRecord::LeaveEventMode() {
    m_eventListen.Stop();
    m_stream->Stop();
    m_eventFrameList.clear();
    m_file.CompleteFile(Time::GetS());
}

void RecordStdRecord::OnGetEvent(Event::Info &info) {
    bool find = false;
    for (auto &event : m_res->param->record.events) {
        if (info.type == event.type && info.chn == event.chn) {
            find = true;
            break;
        }
    }
    if (!find)
        return;
    if (!m_eventInfo.isStart && info.isStart) {
        m_eventInfo = info;
        m_paramChange = true;
    } else if (m_eventInfo.isStart &&
               !info.isStart &&
               m_eventInfo.type == info.type &&
               m_eventInfo.chn == info.chn) {
        m_file.CompleteFile(Time::GetS());
        m_eventInfo = info;
    }
}

void RecordStdRecord::OnEventGetFrameInfo(MediaFrame &frame) {
    // 没有告警的时候，预录preRecTimeS秒录像
    if (!m_eventInfo.isStart) {
        if (frame.paramChange) {
            //如果参数改变了，则清空之前的预录
            m_eventFrameList.clear();
        } else if (!m_eventFrameList.empty() &&
                   m_eventFrameList.begin()->tsInUs > frame.tsInUs + m_recordParam->preRecTimeS * 1000000) {
            m_eventFrameList.erase(m_eventFrameList.begin());
        }
        m_eventFrameList.emplace_back(frame);
        return;
    }
    while (!m_eventFrameList.empty()) {
        EventWriteFrame(*m_eventFrameList.begin());
        m_eventFrameList.erase(m_eventFrameList.begin());
    }
    EventWriteFrame(frame);
}

void RecordStdRecord::EventWriteFrame(MediaFrame &frame) {
    auto e = m_stream->GetFrameByInfo(frame);
    if (e != ErrCodeE::Success)
        return;
    //设想中venc的frame.paramChange和frame.isKeyFrame应该同时出现，这里加打印看看会不会有异常情况
    if (frame.type == MediaFrame::TypeE::Venc && frame.paramChange && !frame.isKeyFrame)
        emxlogc("chn %d venc param changed but is not key frame\n", frame.chn);

    bool createNewFile = false;
    //参数变化了需要等待I帧重新录制文件，期间的所有数据丢弃
    if (frame.paramChange)
        m_paramChange = true;
    if (m_paramChange) {
        if (frame.type == MediaFrame::TypeE::Venc && frame.isKeyFrame) {
            m_paramChange = false;
            createNewFile = true;
        } else {
            return;
        }
    }

    if (createNewFile) {
        //如果还有预录的视频，则文件的时间需要计算一下预录偏移
        uint32_t utc = m_eventFrameList.empty() ? Time::GetS() :
                       Time::GetS() -
                       (uint32_t) ((m_eventFrameList.end()->tsInUs - m_eventFrameList.begin()->tsInUs) / 1000000);
        m_file.CompleteFile(utc);
        if (m_file.FreeSpace(m_res->sdcardChn, m_res->param->reservedSpace) != ErrCodeE::Success) {
            emxloge("free space failed\n");
            return;
        }
        m_file.CreateNewFile(utc, m_recordParam->vencChn, m_recordParam->aencChn);
    }
    m_file.WriteFile(frame);
}

void RecordStdRecord::GetRecordList(uint32_t startUtc, uint32_t endUtc, bool merge,
                                    std::vector<RecordStd::RecFile> &list) {
    m_file.GetRecordList(startUtc, endUtc, merge, list);
}





