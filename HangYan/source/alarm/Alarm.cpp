/*
 * @Author: xiong
 * @Date: 2022/10/21
*/

#include "Alarm.hpp"
#include "AlarmVoice.hpp"
#include "AlarmMotion.hpp"
#include "AlarmPerson.hpp"
#include "AlarmInvasion.hpp"
#include "AlarmFace.hpp"

using namespace Emx;

ErrCodeE Alarm::Create(OvdCtx *ctx) {
    if (m_created)
        return ErrCodeE::Success;
    m_ctx = ctx;

    if (m_ctx->env.cap.info.ovdCapInfo_alarms.have_alarms_voice)
        m_alarmList.push_back(new AlarmVoice(OVD_VOICE, m_ctx, &m_alarmList,
                                             std::bind(&Alarm::OnAlarmUploadReqCallBack, this, ph_1, ph_2)));
    if (m_ctx->env.cap.info.ovdCapInfo_alarms.have_alarms_motion)
        m_alarmList.push_back(new AlarmMotion(OVD_MOTIOM, m_ctx, &m_alarmList,
                                              std::bind(&Alarm::OnAlarmUploadReqCallBack, this, ph_1, ph_2)));
    if (m_ctx->env.cap.info.ovdCapInfo_alarms.have_alarms_body)
        m_alarmList.push_back(new AlarmPerson(OVD_MAN, m_ctx, &m_alarmList,
                                              std::bind(&Alarm::OnAlarmUploadReqCallBack, this, ph_1, ph_2)));
    if (m_ctx->env.cap.info.ovdCapInfo_alarms.have_alarms_alertarea)
        m_alarmList.push_back(new AlarmInvasion(OVD_ALERTAREA, m_ctx, &m_alarmList,
                                                std::bind(&Alarm::OnAlarmUploadReqCallBack, this, ph_1, ph_2)));
    if (m_ctx->env.cap.info.ovdCapInfo_alarms.have_alarms_face)
        m_alarmList.push_back(new AlarmFace(OVD_FACE, m_ctx, &m_alarmList,
                                                std::bind(&Alarm::OnAlarmUploadReqCallBack, this, ph_1, ph_2)));

    for (auto alarm : m_alarmList) {
        alarm->Create();
    }

    //创建实际调用杭研接口上报的任务，用于在线程池中调用杭研接口，防止阻塞
    m_workUpload.Create(&m_ctx->loop, nullptr,
                        std::bind(&Alarm::OnUploadAlarm, this, ph_1),
                        std::bind(&Alarm::OnUploadComplete, this, ph_1, ph_2));
    m_created = true;
    emxlogi("Alarm created\n");
    return ErrCodeE::Success;
}

void Alarm::Destroy() {
    if (!m_created)
        return;
    for (auto alarm : m_alarmList) {
        alarm->Destroy();
        delete alarm;
    }
    m_alarmList.clear();
    m_workUpload.Destroy();
    for (auto upload : m_alarmUploadList) {
        delete upload;
    }
    m_alarmUploadList.clear();
    m_created = false;
    emxlogi("Alarm destroyed\n");
}

void Alarm::OnAlarmUploadReqCallBack(OVDAlarmType type, bool start) {
    auto *alarm = new AlarmUploadItem;
    alarm->type = type;
    alarm->flag = start ? AlarmUploadItem::FlagE::Start : AlarmUploadItem::FlagE::End;
    alarm->utc = Time::GetS();
    m_alarmUploadList.push_back(alarm);
    if (m_alarmUploadList.size() == 1) {
        m_workUpload.Run(alarm);
    }
}

void Alarm::OnUploadAlarm(void *arg) {
    int ret;
    auto *alarm = (AlarmUploadItem *) arg;
    if (alarm->flag == AlarmUploadItem::FlagE::End) {
        ret = OVD_AlarmInfoEnd(0, alarm->type, (long long) alarm->utc);
    } else {
        OVDUpLoadAlarmInfo alarmInfo = {};
        alarmInfo.channel = 0;
        alarmInfo.startTimeStamp = alarm->utc;
        alarmInfo.AlarmType = alarm->type;
        alarmInfo.desc = (char *) AlarmBase::GetAlarmName(alarm->type);
        MediaClientStream::Channel chn;
        chn.Add(MediaFrame::TypeE::Snap, 0);
        MediaClientStreamSync snap;
        MediaFrame frame = {};
        if (snap.Open(chn, 3000) == ErrCodeE::Success) {
            MediaClientSnap::Trigger(0, 3000);
            ErrCodeE e = snap.GetFrame(frame, 3000);
            if (e != ErrCodeE::Success) {
                emxloge("get snap frame failed\n");
            } else {
                alarmInfo.ImageInfo.buf = (OVD_char *) frame.data;
                alarmInfo.ImageInfo.size = frame.size;
            }
            snap.Close();
#if 0
            // note: 缩略图存储验证
            char path[128] = { 0 };
            snprintf(path, sizeof(path) - 1
                        , "/root/configs/normal/%d_snap.jpg"
                        , frame.size);
            FILE* fp = fopen(path, "w");
            if (fp != nullptr) {
                fwrite(frame.data, frame.size, 1, fp);
                fclose(fp);
                fp = nullptr;
            }
#endif
        } else {
            emxloge("get snap failed\n");
        }
#if 1
        if (alarm->type == OVD_FACE) {
            //note: 人脸告警处理
            alarmInfo.startTimeStamp -= 5;
        }
#endif
        ret = OVD_AlarmInfoStart(&alarmInfo);
    }

    emxlogi("upload alarm %s %s utc:%u, ret=%d\n", AlarmBase::GetAlarmName(alarm->type),
           alarm->flag == AlarmUploadItem::FlagE::Start ? "start" : "end", alarm->utc, ret);
}

void Alarm::OnUploadComplete(ErrCodeE e, void *arg) {
    auto *alarm = (AlarmUploadItem *) arg;
    delete alarm;
    m_alarmUploadList.erase(m_alarmUploadList.begin());
    if (!m_alarmUploadList.empty()) {
        m_workUpload.Run(m_alarmUploadList.front());
    }
}

void Alarm::SetEna(OVDAlarmType type, bool ena) {
    for (auto alarm : m_alarmList) {
        if (alarm->GetType() == type) {
            alarm->SetEna(ena);
            break;
        }
    }
}
