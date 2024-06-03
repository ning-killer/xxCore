#include "AlarmLicensePlate.hpp"

using namespace Emx;

void AlarmLicensePlate::CalibrationAutoExposure() {
    MediaClientAi ai_client(0);
    MediaAi::Param ai_param;
    if (ai_client.GetParam(ai_param) != ErrCodeE::Success) {
        return;
    }
    NightVisionClient nightVision;
    NightVision::Param param = {};
    if (nightVision.GetParam(param) != ErrCodeE::Success) {
        return;
    }
    if (param.manual) {
        emxlogd("current nightVision is manual\n");
        return;
    }
    bool autoExposure = 
        (m_nightStatus.mode == NightVision::ModeE::IrNightVision || m_nightStatus.mode == NightVision::ModeE::ColorNightVision) ? true : false;
    emxlogt("last status[%d], current status[%d]\n", ai_param.vehicleLicensePlateDetection.autoExposure, autoExposure);
    if (ai_param.vehicleLicensePlateDetection.autoExposure != autoExposure) {
        ai_param.vehicleLicensePlateDetection.autoExposure = autoExposure;
        emxlogd("night switch, current night[%s]\n", ai_param.vehicleLicensePlateDetection.autoExposure ? "true" : "false");
        ai_client.SetParam(ai_param);
    }
}

ErrCodeE AlarmLicensePlate::Create() {
    m_nightStatus.mode = NightVision::ModeE::Day;
    m_ctx->soundAlarmControl.Update(
        SoundAlarmControl::Type::VEHICLE_GEN, SoundAlarmControl::Status::Off);
    m_ai = new MediaClientAiDataAsync(m_ctx->loop);
    m_montorNightTimer.Create(m_ctx->loop);
    m_night.Start(m_ctx->loop, m_ctx->buffer, m_ctx->bufferSize, [this](NightVision::Status &status) {
        // if (m_nightStatus.mode == status.mode) {
        //     return;
        // }
        m_nightStatus = status;
        if (m_ctx->env.licensePlate.on) {
            CalibrationAutoExposure();
        }
    });
    m_ai->Start(nullptr, [this](MediaAi::AiInfoHeader &header) {
        if (header.type != MediaAi::AiTypeE::VehicleLicensePlateDetection) {
            return;
        }
        if (!m_ctx->env.licensePlate.on) {
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
        MediaAi::AiInfoVehicleLicensePlateDetectionData licensePlateData;
        if (!MediaAiUtils::UnPackVehicleLicensePlateDetectionData(header.data, licensePlateData, header.size)) {
            emxloge("unpack licensePlate detection data failed.\n");
            for (auto &img : licensePlateData.imgs) {
                if (img.vehicleImg.data != nullptr) {
                    free(img.vehicleImg.data);
                    img.vehicleImg.data = nullptr;
                }
                if (img.licensePlateImg.data != nullptr) {
                    free(img.licensePlateImg.data);
                    img.licensePlateImg.data = nullptr;
                }
            }
            return;
        }
        bool isTrigger = false;
        for (auto data : licensePlateData.imgs) {
            if (AlarmInZone(header.srcPicSize, data.vehicleImg.rect, m_ctx->env.licensePlate.zone)) {
                isTrigger = true;
            }
        }
        if (isTrigger) {
            // ovd告警
            bool started = m_started;
            AlarmTrigger();
            // 深拷贝出数据用于gat1400数据上传
            auto *param = new Gat1400Util::UploadMotorVehiclesData();
            Clone(*param, licensePlateData);
            // gat1400上传
            Gat1400Client::Instance()->Upload(param);
            delete param;
            param = nullptr;
            if (m_started && !started) {
                BreakLastSoundAndLightAlarm();
                Strategy(&m_ctx->env.licensePlate.strategy, &m_ctx->env.licensePlate.on, "vehicleAlarmVoice");
            }  
        }
        // 手动释放深拷贝的数据
        // MediaAiUtils::FreeVehicleLicensePlateDetectionData(&licensePlateData);
        for (auto &img : licensePlateData.imgs) {
            if (img.vehicleImg.data != nullptr) {
                free(img.vehicleImg.data);
                img.vehicleImg.data = nullptr;
            }
            if (img.licensePlateImg.data != nullptr) {
                free(img.licensePlateImg.data);
                img.licensePlateImg.data = nullptr;
            }
        }
    });
    return ErrCodeE::Success;
}

void AlarmLicensePlate::Destroy() {
    m_night.Stop();
    m_montorNightTimer.Destroy();
    if (m_ai != nullptr) {
        m_ai->Stop();
        delete m_ai;
        m_ai = nullptr;
    }
}

void AlarmLicensePlate::Clone(Gat1400Util::UploadMotorVehiclesData &param, 
    const MediaAi::AiInfoVehicleLicensePlateDetectionData& VPData) {
    // 公共信息
    param.type = Gat1400Util::UploadType::MotorVehicles;
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
    for (auto sub : VPData.imgs) {
        Gat1400Util::VehiclePlateNo vpImg;
        // 基础信息
        vpImg.LaneNo = 0;
        vpImg.HasPlate = "";
        vpImg.PlateClass = "";
        vpImg.PlateColor = "";
        vpImg.VehicleClass = "";
        vpImg.Direction = "0";
        vpImg.VehicleColor = "";
        vpImg.PassTime = param.LocationMarkTime;
        vpImg.PlateNo = std::string(sub.licensePlate);
        // 车辆img
        vpImg.Vehicle.Type = "01";
        vpImg.Vehicle.StoragePath = "";
        vpImg.Vehicle.ImageID = Gat1400Client::Instance()->GetGetSourceId(param.deviceId, param.AppearTime);
        vpImg.Vehicle.EventSort = 10;
        vpImg.Vehicle.FileFormat = Gat1400Util::GetImgType((Gat1400Util::ImgType)sub.vehicleImg.encode);
        vpImg.Vehicle.ShotTime = Gat1400Util::GetTimeStr(Gat1400Util::TimePrecision::S);
        vpImg.Vehicle.Width = sub.vehicleImg.rect.w; 
        vpImg.Vehicle.Height = sub.vehicleImg.rect.h;
        std::string VhinData = std::string((char*)sub.vehicleImg.data , sub.vehicleImg.size);
        Base64::Encode(VhinData, vpImg.Vehicle.Data);
        vpImg.Vehicle.FileSize = sub.vehicleImg.size;
        // 车牌img
        vpImg.Pl.Type = "06";
        vpImg.Pl.StoragePath = "";
        vpImg.Pl.ImageID = Gat1400Client::Instance()->GetGetSourceId(param.deviceId, param.AppearTime);
        vpImg.Pl.EventSort = 10;
        vpImg.Pl.FileFormat = Gat1400Util::GetImgType((Gat1400Util::ImgType)sub.licensePlateImg.encode);
        vpImg.Pl.ShotTime = Gat1400Util::GetTimeStr(Gat1400Util::TimePrecision::S);
        vpImg.Pl.Width = sub.licensePlateImg.rect.w; 
        vpImg.Pl.Height = sub.licensePlateImg.rect.h;
        std::string PlinData = std::string((char*)sub.licensePlateImg.data , sub.licensePlateImg.size);
        Base64::Encode(PlinData, vpImg.Pl.Data);
        vpImg.Pl.FileSize = sub.licensePlateImg.size;
        param.subImgList.push_back(vpImg);
#if 1   
        if (File::Exist("/mnt/sdcard/algtest")) {
            char currentTime[64] = { 0 };
            int timeNow = Time::GetS();
            Gat1400Util::GetTimeStr(timeNow, currentTime,sizeof(currentTime));
            emxlogd("check Vehicle img\n");
            char path1[128] = { 0 };
            snprintf(path1, sizeof(path1) - 1
                        , "/mnt/sdcard/Vehicle_%s_%d.jpg"
                        , currentTime, (int)sub.vehicleImg.size);
            FILE* fp1 = fopen(path1, "w");
            if (fp1 != nullptr) {
                fwrite(sub.vehicleImg.data, sub.vehicleImg.size, 1, fp1);
                fflush(fp1);
                fsync(fileno(fp1));
                fclose(fp1);
                fp1 = nullptr;
            }

            emxlogd("check license img\n");
            char path[128] = { 0 };
            snprintf(path, sizeof(path) - 1
                        , "/mnt/sdcard/VehicleLicense_%s_%d.jpg"
                        , currentTime, (int)sub.licensePlateImg.size);
            FILE* fp = fopen(path, "w");
            if (fp != nullptr) {
                fwrite(sub.licensePlateImg.data, sub.licensePlateImg.size, 1, fp);
                fflush(fp);
                fsync(fileno(fp));
                fclose(fp);
                fp = nullptr;
            }
        }
#endif
    }
}