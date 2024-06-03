#include "AlarmRegionalPeople.hpp"
#include "cmath"

using namespace Emx;

ErrCodeE AlarmRegionalPeople::Create() {
    m_ctx->soundAlarmControl.Update(
        SoundAlarmControl::Type::RegionalPeople, SoundAlarmControl::Status::Off);
    m_alarmTimer.Create(m_ctx->loop);
    m_gatPostTimer.Create(m_ctx->loop);
    m_gatCheckTimer.Create(m_ctx->loop);
    ManageGatPost();
    m_ai = new MediaClientAiDataAsync(m_ctx->loop);
    m_ai->Start(nullptr, [this](MediaAi::AiInfoHeader &header) {
        if (!m_ctx->env.regionalPeople.gatReportOn) {
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
        int inPeopleCount = 0;
        for (int i = 0; i < header.size / (int) sizeof(MediaAi::AiInfoClassifyData); i++) {
            if (!isSupportedTargetType(data[i].type)) {
                continue;
            }
            if (AlarmInZone(header.srcPicSize, data[i].rect, m_ctx->env.regionalPeople.zone)) {
                inPeopleCount++;
            }
        }
        m_alarmTimer.Start(2500, 0, [this]() {
            //note: 2s内未检测到目标输出，则认为目标离开,重置状态
            m_lastStatus = normal;
            m_inPeopleCount = 0;
            if (m_ctx->env.regionalPeople.osdStatus) {
                ShowOsd();
            }
        });
        emxlogd("inPeopleCount[%d]\n", inPeopleCount);
        int intervalTime = Time::GetS() - m_lastAlarmTime; //s
        if (m_inPeopleCount != inPeopleCount) {
            m_inPeopleCount = inPeopleCount;
            if (m_ctx->env.regionalPeople.osdStatus) {
                ShowOsd();
            }
        }
        RegionStatus curStatus = unknown;
        if (inPeopleCount > m_ctx->env.regionalPeople.regionalPeopleCount) {
            curStatus = alert;
        } else {
            curStatus = normal;
        }

        // todo 此处应该是更为合理的需求逻辑，我们自己其他产品这样处理实现 
        // bool isAlarm = false;
        // if (m_lastStatus != curStatus && curStatus == alert) {
        //     Alarm(Time::GetS());
        //     isAlarm = true;
        // }
        // m_lastStatus = curStatus;

        // if (isAlarm) {
        //     return;
        // }

        // if (inPeopleCount > m_ctx->env.regionalPeople.regionalPeopleCount
        //     && intervalTime > m_ctx->env.regionalPeople.alarmReportDuration * 60) {
        //     Alarm(Time::GetS());
        // }

        // todo 以下是按清微的白盒需求实现
        bool isAlarm = false;
        if (m_lastStatus == unknown && curStatus == alert) {
            Alarm(Time::GetS());
            isAlarm = true;
        }
        m_lastStatus = curStatus;
        if (isAlarm) {
            return;
        }
        if (inPeopleCount > m_ctx->env.regionalPeople.regionalPeopleCount
            && intervalTime > m_ctx->env.regionalPeople.alarmReportDuration * 60) {
            Alarm(Time::GetS());
            m_lastStatus = unknown;
        }
    });
    if (m_ctx->env.regionalPeople.osdStatus) {
        ShowOsd();
    }
    m_gatCheckTimer.Start(0, 1000, [this]() {
        if (m_ctx->env.regionalPeople.isChangedGatReport) {
            m_ctx->env.regionalPeople.isChangedGatReport = false;
            // 重置检测定时器状态
            ManageGatPost();
        }
    });
    return ErrCodeE::Success;
}

void AlarmRegionalPeople::ManageGatPost() {
    if (!m_ctx->env.regionalPeople.gatReportOn) {
        return;
    }
    m_gatPostTimer.Start(0, m_ctx->env.regionalPeople.gatReportDuration * 60 * 1000, [this]() {
        if (m_ctx->env.regionalPeople.gatReportOn) {
            auto *param = new Gat1400Util::UploadRegionData();
            PackUploadRegionData(*param);
            Gat1400Client::Instance()->Upload(param);
            delete param;
            param = nullptr;
        }
    });
}

void AlarmRegionalPeople::Destroy() {
    m_alarmTimer.Destroy();
    m_gatPostTimer.Destroy();
    m_gatCheckTimer.Destroy();
    m_ai->Stop();
    delete m_ai;
}

bool AlarmRegionalPeople::isSupportedTargetType(const MediaAi::ClassifyTypeE type) {
    if (type == MediaAi::ClassifyTypeE::Person) {
        return true;
    }
    return false;
}

void AlarmRegionalPeople::Alarm(int time) {
    m_lastAlarmTime = time;

    // sdk 告警
    bool started = m_started;
    AlarmTrigger();
    if (m_started && !started) {
        BreakLastSoundAndLightAlarm();
        // 声光控制告警
        Strategy(&m_ctx->env.regionalPeople.strategy, &m_ctx->env.regionalPeople.gatReportOn, "regionalPeopleAlarmVoice");
    }
}

void AlarmRegionalPeople::PackUploadRegionData(Gat1400Util::UploadRegionData &param) {
    param.type = Gat1400Util::UploadType::Region;
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
    param.regionID = param.SourceID;
    param.scheduledTime = param.DisAppearTime;
    param.peopleCount = m_inPeopleCount;
    param.cordon = 2;
    param.typeE = 0;
#if 1
    param.img.Type = "102";
    param.img.StoragePath = "";
    param.img.ImageID = Gat1400Client::Instance()->GetGetSourceId(param.deviceId, param.AppearTime);
    param.img.EventSort = 102;
    param.img.FileFormat = "jpg";
    param.img.ShotTime = Gat1400Util::GetTimeStr(Gat1400Util::TimePrecision::S);
    param.img.Width = 0; 
    param.img.Height = 0;
    param.img.Data = "";
    param.img.FileSize = 0;

#if 0
    // todo: 预留整图抓拍数据上传
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
            std::string inData = std::string((char*)frame.data);
            Base64::Encode(inData, param.img.Data);
            param.img.FileSize = frame.size;
        }
        snap.Close();
    } else {
        emxloge("get snap failed\n");
    }
#endif
#endif
}

void AlarmRegionalPeople::ShowOsd() {
    m_ctx->env.regionalPeople.inPeopleCount = m_inPeopleCount;
    MediaClientOSD osd(0);
    std::vector<std::shared_ptr<MediaOSD::Param>> osdParam;
    osdParam.clear();
    if (m_ctx->env.offDuty.gatReportOn) {
        emxlogd("offDuty on\n");
        return;
    }
    if (!m_ctx->env.regionalPeople.gatReportOn || !m_ctx->env.regionalPeople.osdStatus) {
        emxlogd("regionalPeople off\n");
        return;
    }
    if (osd.GetParam(osdParam) != ErrCodeE::Success) {
        emxloge("osd param get failed\n");
        return;
    }
    int fontsize = 0;
    int thickness = 0;
    MediaOSD::Margin margin;
    for (auto item : osdParam) {
        if (item->type == MediaOSD::TypeE::Time) {
            fontsize = item->time.size;
            thickness = item->time.thickness;
            margin = item->time.margin;
        }
    }

    auto paramIn = std::make_shared<MediaOSD::Param>();
    memset(paramIn.get(), 0, sizeof(MediaOSD::Param));
    paramIn->type = MediaOSD::TypeE::Text;
    paramIn->text.ena = true;
    memcpy(&paramIn->text.margin, &margin, sizeof(MediaOSD::Margin));
    paramIn->text.margin.rateY += fontsize + 5;
    paramIn->text.thickness = thickness;
    paramIn->text.size = fontsize;
    strncpy(paramIn->text.font, "font.ttf", sizeof(paramIn->text.font));
    snprintf(paramIn->text.content, sizeof(paramIn->text.content) - 1, "区域：%d", m_inPeopleCount);
    osdParam.push_back(paramIn);


    emxlogd("SetTempParam.\n");
    osd.SetTempParam(osdParam);
}