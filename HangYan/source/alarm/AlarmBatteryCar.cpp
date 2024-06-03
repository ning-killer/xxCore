#include "AlarmBatteryCar.hpp"

using namespace Emx;

ErrCodeE AlarmBatteryCar::Create() {
    m_ctx->soundAlarmControl.Update(
        SoundAlarmControl::Type::VEHICLE_MOTOR, SoundAlarmControl::Status::Off);
    m_ai = new MediaClientAiDataAsync(m_ctx->loop);
    m_ai->Start(nullptr, [this](MediaAi::AiInfoHeader &header) {
        if (header.type != MediaAi::AiTypeE::BatteryCarDetection) {
            return;
        }
        if (!m_ctx->env.batteryCar.on) {
            return;
        }
        if (m_ai->GetDataByInfo(header) != ErrCodeE::Success) {
            return;
        }
        if (header.data == nullptr) {
            return;
        }
        if (!CheckHeadSrcSize(header.srcPicSize)) {
            emxloge("header.srcPicSize.h[%d];header.srcPicSize.w[%d]\n", header.srcPicSize.h, header.srcPicSize.w);
            return;
        }
        MediaAi::AiInfoBatteryCarDetectionData batteryCarData;
        if (!MediaAiUtils::UnPackBatteryCarDetectionData(header.data, batteryCarData, header.size)) {
            emxloge("unpack batteryCar detection data failed.\n");
            for (auto &img : batteryCarData.imgs) {
                if (img.data != nullptr) {
                    free(img.data);
                    img.data = nullptr;
                }
            }
            return;
        }
        bool isTrigger = false;
        bool isMachedTarget = false;
        for (auto batterycar : batteryCarData.imgs) {
            if (AlarmInZone(header.srcPicSize, batterycar.rect, m_ctx->env.batteryCar.zone)) {
                isTrigger = true;
            }
            if (isMatchedTarget(batterycar.batCartype)) {
                isMachedTarget = true;
            }
        }
        if (isTrigger && isMachedTarget) {
            // ovd告警
            bool started = m_started;
            AlarmTrigger();
            // 深拷贝出数据用于gat1400数据上传
            auto *param  = new Gat1400Util::UploadNonMotorVehiclesData();
            Clone(*param, batteryCarData);
            // gat1400上传
            Gat1400Client::Instance()->Upload(param);
            delete param;
            param = nullptr;
            if (m_started && !started) {
                BreakLastSoundAndLightAlarm();
                Strategy(&m_ctx->env.batteryCar.strategy, &m_ctx->env.batteryCar.on, "nonmotorAlarmVoice");
            }    
        }
        // 手动释放深拷贝的数据
        // MediaAiUtils::FreeBatteryCarDetectionData(&batteryCarData);
        for (auto &img : batteryCarData.imgs) {
            if (img.data != nullptr) {
                free(img.data);
                img.data = nullptr;
            }
        }
    });
    return ErrCodeE::Success;
}

void AlarmBatteryCar::Destroy() {
    if (m_ai != nullptr) {
        m_ai->Stop();
        delete m_ai;
        m_ai = nullptr;
    }
}

void AlarmBatteryCar::Clone(Gat1400Util::UploadNonMotorVehiclesData &param, 
    const MediaAi::AiInfoBatteryCarDetectionData& BcData) {
    param.type = Gat1400Util::UploadType::NonMotorVehicles;
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
    param.HasPlate = false;
    param.PlateClass = "01";
    param.PlateNo = "1122";
    param.PlateColor = "1";
    param.VehicleColor = "1";
    for (auto sub : BcData.imgs) {
        Gat1400Util::SubImgDataParam bcImg;
        bcImg.Type = "12";
        bcImg.StoragePath = "";
        bcImg.ImageID = Gat1400Client::Instance()->GetGetSourceId(param.deviceId, param.AppearTime);
        bcImg.EventSort = 10;
        bcImg.FileFormat = Gat1400Util::GetImgType((Gat1400Util::ImgType)sub.encode);
        bcImg.ShotTime = Gat1400Util::GetTimeStr(Gat1400Util::TimePrecision::S);
        bcImg.Width = sub.rect.w; 
        bcImg.Height = sub.rect.h;
        std::string inData = std::string((char*)sub.data , sub.size);
        Base64::Encode(inData, bcImg.Data);
        bcImg.FileSize = sub.size;
        param.subImgList.push_back(bcImg);
#if 1  
        if (File::Exist("/mnt/sdcard/algtest")) {
            char currentTime[64] = { 0 };
            int timeNow = Time::GetS();
            Gat1400Util::GetTimeStr(timeNow, currentTime,sizeof(currentTime));
            emxlogd("check BatteryCar img\n");
            char path[128] = { 0 };
            snprintf(path, sizeof(path) - 1
                        , "/mnt/sdcard/BatteryCar_%s_%d_%d.jpg"
                        , currentTime
                        , (int)sub.type
                        , (int)sub.size);
            FILE* fp = fopen(path, "w");
            if (fp != nullptr) {
                fwrite(sub.data, sub.size, 1, fp);
                fflush(fp);
                fsync(fileno(fp));
                fclose(fp);
                fp = nullptr;
            }
        }
#endif
    }
}

bool AlarmBatteryCar::isMatchedTarget(MediaAi::BatteryCarType type) {
    emxlogt("target type[%d]\n", type);
    if (type == MediaAi::BatteryCarType::electromobile 
        && (m_ctx->env.batteryCar.detectModelist == "electromobile" 
        || m_ctx->env.batteryCar.detectModelist == "electromobile,bike")) {
        return true;
    }
    if (type == MediaAi::BatteryCarType::bike 
        && (m_ctx->env.batteryCar.detectModelist == "bike" 
        || m_ctx->env.batteryCar.detectModelist == "electromobile,bike")) {
        return true;
    }
    return false;
}