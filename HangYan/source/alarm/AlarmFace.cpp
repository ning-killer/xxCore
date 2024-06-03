#include "AlarmFace.hpp"

using namespace Emx;

ErrCodeE AlarmFace::Create() {
    m_ai = new MediaClientAiDataAsync(m_ctx->loop);
    m_ai->Start(nullptr, [this](MediaAi::AiInfoHeader &header) {
        if (header.type != MediaAi::AiTypeE::FaceDetection) {
            return;
        }
        if (!m_ctx->env.face.ena) {
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
        MediaAi::AiInfoFaceDetectionData faceData;
        if (!MediaAiUtils::UnPackFaceDetectionData(header.data, faceData, header.size)) {
            emxloge("unpack face detection data failed.\n");
            for (auto &img : faceData.imgs) {
                if (img.data != nullptr) {
                    free(img.data);
                    img.data = nullptr;
                }
            }
            return;
        }
        bool isTrigger = false;
        bool isMasked = true; //是否存在戴口罩的人员
        for (auto face : faceData.imgs) {
            if (face.type == MediaAi::ImgTypeE::Panorama) {
                emxlogd("check face img is panorama\n");
                // 全景图跳过
                continue;
            }
            if (AlarmInZone(header.srcPicSize, face.rect, m_ctx->env.face.zone)) {
                isTrigger = true;
                isMasked = face.isMask;
                if (!isMasked) {
                    // 只要检测到未戴口罩的人员就跳出
                    break;
                }
            }
        }
        emxlogt("check face isTrigger[%d],is masked[%d]\n", isTrigger, isMasked);
        // m_type = OVD_FACE;
        if (isTrigger) {
            // if (m_ctx->env.cap.info.ovdAICapInfo.AIface.mask_detection 
            //     && m_ctx->env.face.mask_detection) {
            //     // 未戴口罩需要告警
            //     if (!isMasked) {
            //         bool started = m_started;
            //         m_type = OVD_FACE_MASK;
            //         AlarmTrigger();
            //         if (m_started && !started) {
            //             Strategy(&m_ctx->env.face.strategy, &m_ctx->env.face.mask_detection, "maskAlarmVoice");
            //         }
            //     }
            // } else {
            //     AlarmTrigger();
            // }
            AlarmTrigger();
            // 深拷贝出数据用于gat1400数据上传
            auto *param = new Gat1400Util::UploadFaceData();
            if (Clone(*param, faceData)) {
                // gat1400上传
                Gat1400Client::Instance()->Upload(param);
            }
            delete param;
            param = nullptr;
        }
        // 手动释放深拷贝的数据
        // MediaAiUtils::FreeFaceDetectionData(&faceData);
        for (auto &img : faceData.imgs) {
            if (img.data != nullptr) {
                free(img.data);
                img.data = nullptr;
            }
        }
    });
    return ErrCodeE::Success;
}

void AlarmFace::Destroy() {
    if (m_ai != nullptr) {
        m_ai->Stop();
        delete m_ai;
        m_ai = nullptr;
    }
}

bool AlarmFace::Clone(Gat1400Util::UploadFaceData &param, 
    const MediaAi::AiInfoFaceDetectionData& faceData) {
    param.type = Gat1400Util::UploadType::Faces;
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
    param.IsSuspectedTerrorist = 2;
    param.IsCriminalInvolved = 2;
    param.IsDetainees = 2;
    param.IsVictim = 2;
    param.IsSuspiciousPerson = 2;
    param.RespiratorColor = "5";
    param.isHaveFullImg = faceData.isHavePanorama;

    bool isNeedCheckFullImg = true;
    if (!param.isHaveFullImg) {
        isNeedCheckFullImg = false;
    }
    for (auto sub : faceData.imgs) {
        Gat1400Util::SubImgDataParam faceImg;
        if (isNeedCheckFullImg) {
            emxlogd("this face detect exit full img, check firt sub img!\n");
            isNeedCheckFullImg = false;
            if (sub.type != MediaAi::ImgTypeE::Panorama) {
                return false;
            }
            faceImg.Type = "11";
            faceImg.StoragePath = "";
            faceImg.ImageID = Gat1400Client::Instance()->GetGetSourceId(param.deviceId, param.AppearTime);
            faceImg.EventSort = 10;
            faceImg.FileFormat = Gat1400Util::GetImgType((Gat1400Util::ImgType)sub.encode);
            faceImg.ShotTime = Gat1400Util::GetTimeStr(Gat1400Util::TimePrecision::S);
            faceImg.Width = sub.rect.w; 
            faceImg.Height = sub.rect.h;
            std::string inData = std::string((char*)sub.data , sub.size);
            Base64::Encode(inData, faceImg.Data);
            faceImg.FileSize = sub.size;
            param.subImgList.push_back(faceImg);
        }
        faceImg.Type = "11";
        faceImg.StoragePath = "";
        faceImg.ImageID = Gat1400Client::Instance()->GetGetSourceId(param.deviceId, param.AppearTime);
        faceImg.EventSort = 10;
        faceImg.FileFormat = Gat1400Util::GetImgType((Gat1400Util::ImgType)sub.encode);
        faceImg.ShotTime = Gat1400Util::GetTimeStr(Gat1400Util::TimePrecision::S);
        faceImg.Width = sub.rect.w; 
        faceImg.Height = sub.rect.h;
        std::string inData = std::string((char*)sub.data , sub.size);
        Base64::Encode(inData, faceImg.Data);
        faceImg.FileSize = sub.size;
        param.subImgList.push_back(faceImg);
#if 1
        if (File::Exist("/mnt/sdcard/algtest")) {
            char currentTime[64] = { 0 };
            int timeNow = Time::GetS();
            Gat1400Util::GetTimeStr(timeNow, currentTime,sizeof(currentTime));
            emxlogd("check face img\n");
            char path[128] = { 0 };
            snprintf(path, sizeof(path) - 1
                        , "/mnt/sdcard/face_%s_%d_%d.jpg"
                        , currentTime
                        , (int)sub.type
                        , (int)sub.size);
            FILE* fp = fopen(path, "w");
            emxlogd("path:%s\n", path);
            if (fp != nullptr) {
                fwrite(sub.data, sub.size, 1, fp);
                fflush(fp);
                fsync(fileno(fp));
                fclose(fp);
                fp = nullptr;
            }  else {
                emxloge("1111111111\n");
            }
        } else {
            emxlogd("no flag\n");
        }
#endif
    }

    return true;
}