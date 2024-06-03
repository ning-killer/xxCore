#include "AlarmOffduty.hpp"
#include "cmath"

using namespace Emx;

ErrCodeE AlarmOffduty::Create() {
    m_offdutyDurationTimer.Create(m_ctx->loop);
    m_checkTimer.Create(m_ctx->loop);
    m_ai = new MediaClientAiDataAsync(m_ctx->loop);
    m_ai->Start(nullptr, [this](MediaAi::AiInfoHeader &header) {
        if (!m_ctx->env.offDuty.gatReportOn) {
            return;
        }
        if (header.type != MediaAi::AiTypeE::ClassifyDetection) {
            return;
        }   
        if (m_ai->GetDataByInfo(header) != ErrCodeE::Success) {
            return;
        }
        if (!CheckHeadSrcSize(header.srcPicSize)) {
            emxloge("header.srcPicSize.h[%d];header.srcPicSize.w[%d]\n", header.srcPicSize.h, header.srcPicSize.w);
            return;
        }
        auto *data = (MediaAi::AiInfoClassifyData *) header.data;
        int ondutyCount = 0;
        Gat1400Util::OnLeaveStatus curStatus = Gat1400Util::OnLeaveStatus::Unknown;
        for (int i = 0; i < header.size / (int) sizeof(MediaAi::AiInfoClassifyData); i++) {
            if (!isSupportedTargetType(data[i].type)) {
                continue;
            }
            if (AlarmInZone(header.srcPicSize, data[i].rect, m_ctx->env.offDuty.zone)) {
                ondutyCount++;
            }
        }
        if (ondutyCount >= m_ctx->env.offDuty.onDutyCount) {
            // 为在岗状态
            curStatus = Gat1400Util::OnLeaveStatus::OnDuty;
            m_onDutyCount = m_ctx->env.offDuty.onDutyCount;
            m_offDutyCount = 0;
        } else {
            // 为离岗状态
            curStatus = Gat1400Util::OnLeaveStatus::OffDuty;
            m_onDutyCount = ondutyCount;
            m_offDutyCount = m_ctx->env.offDuty.onDutyCount - ondutyCount;
        }
        emxlogd("onDutyCount[%d];offDutyCount[%d]\n", m_onDutyCount, m_offDutyCount);

        if (curStatus != m_lastStatus && curStatus == Gat1400Util::OnLeaveStatus::OffDuty) {
            // 1.状态发生切换且当前为离岗状态，offdutyDuration时间片准备进行告警。
            // 2.连续的离岗状态，offdutyDuration时间不会被重置掉。
            // 3.无人状态(无ai数据回调的状态)也是离岗状态，offdutyDuration也在计时中。
            m_offdutyDurationTimer.Start(m_ctx->env.offDuty.offdutyDuration * 60 * 1000, 0, [this]() {
                Alarm();
            });
        }
        if (curStatus == Gat1400Util::OnLeaveStatus::OnDuty) {
            // 只要当前状态为在岗，重置定时器，并也可监听无人状态(无ai数据回调的状态：离岗状态)进行离岗告警
            m_offdutyDurationTimer.Start(m_ctx->env.offDuty.offdutyDuration * 60 * 1000, 0, [this]() {
                Alarm();
            });
        }
        m_lastStatus = curStatus;

    });

    // 启动服务时，监听无人状态离岗告警触发
    m_offdutyDurationTimer.Start(m_ctx->env.offDuty.offdutyDuration * 60 * 1000, 0, [this]() {
        Alarm();
    });

    // 监控间隔时间配置切换发生改变
    m_checkTimer.Start(0, 1000, [this]() {
       if (m_ctx->env.offDuty.isChangedoffdutyDuration) {
            m_ctx->env.offDuty.isChangedoffdutyDuration = false;
            // 重置检测定时器状态
            Alarm(m_ctx->env.offDuty.offdutyDuration * 60 * 1000);
        }
    });
    return ErrCodeE::Success;
}

void AlarmOffduty::Destroy() {
    m_offdutyDurationTimer.Destroy();
    m_checkTimer.Destroy();
    m_ai->Stop();
    delete m_ai;
}

bool AlarmOffduty::isSupportedTargetType(const MediaAi::ClassifyTypeE type) {
    if (type == MediaAi::ClassifyTypeE::Person) {
        return true;
    }
    return false;
}

void AlarmOffduty::Alarm(int startTime) {
    // 告警调用立刻触发，并且之后起offdutyDuration时间片进行轮询，保证无人状态触发的离岗监听事件。
    m_offdutyDurationTimer.Start(startTime, m_ctx->env.offDuty.offdutyDuration * 60 * 1000, [this]() {
        if (!m_ctx->env.offDuty.gatReportOn) {
            return;
        }
        AlarmTrigger();
        m_lastStatus = Gat1400Util::OnLeaveStatus::OffDuty;
        if (m_ctx->env.offDuty.gatReportOn) {
            auto *param = new Gat1400Util::UploadOnLeaveData();
            PackUploadOffdutyData(*param);
            Gat1400Client::Instance()->Upload(param);
            delete param;
            param = nullptr;
        }
    });
}

void AlarmOffduty::PackUploadOffdutyData(Gat1400Util::UploadOnLeaveData &param) {
    param.type = Gat1400Util::UploadType::OnLeave;
    param.InfoKind = 1;
    param.LeftTopX = 0;
    param.LeftTopY = 0;
    param.RightBtmX = 0;
    param.RightBtmY = 0;
    param.deviceId = Gat1400Client::Instance()->GetDeviceId();
    param.LocationMarkTime = Gat1400Util::GetTimeStr(Gat1400Util::TimePrecision::S);
    param.AppearTime = Gat1400Util::GetTimeStr(Gat1400Util::TimePrecision::S);
    param.DisAppearTime = Gat1400Util::GetTimeStr(Gat1400Util::TimePrecision::S);
    param.SourceID = Gat1400Client::Instance()->GetGetSourceId(param.deviceId, param.AppearTime);
    param.Id = Gat1400Client::Instance()->GetFaceId(param.SourceID);
    param.onLeaveID = param.SourceID;
    param.endTime = param.DisAppearTime;
    param.eventType = Gat1400Util::OnLeaveStatus::OffDuty;
    param.onDutyCount = m_onDutyCount;
    param.offDutyDuration = m_offDutyCount;
    Json::Value alarmZone;
    alarmZone["AlarmZone"] = m_ctx->env.offDuty.zone.ovdZone;
    param.alarmZone = alarmZone;
    Json::Value alertTime;
    alertTime["AlertTime"]["starttime"] = std::string(m_ctx->env.offDuty.schedule.startTime);
    alertTime["AlertTime"]["endtime"] = std::string(m_ctx->env.offDuty.schedule.endTime);
    param.alertTime = alertTime;
}
