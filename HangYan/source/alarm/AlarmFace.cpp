#include "AlarmFace.hpp"

using namespace Emx;

ErrCodeE AlarmFace::Create() {
    m_ai = new MediaClientAiDataAsync(m_ctx->loop);
    m_ai->Start(nullptr, [this](MediaAi::AiInfoHeader &header) {
        if (header.type != MediaAi::AiTypeE::FaceSnap) {
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
        auto *data = reinterpret_cast<MediaAi::AiInfoFaceDetectionData*> (header.data);
        int headerLen = sizeof(MediaAi::AiInfoFaceDetectionData);
        int imgsLen = header.size - headerLen;
        // 数据正确性校验
        int checkImgsLen = 0;
        for (auto img : data->faceMsg) {
            checkImgsLen += img.imgSize;
        }
        if (checkImgsLen !=  imgsLen) {
            emxloge("facedata is error, imgslen(%d) != checklen(%d)", imgsLen, checkImgsLen);
            return;
        }
        // 区域检查，筛选出匹配区域内人脸信息。这里只做浅拷贝赋值(减少拷贝次数)
        std::map<int/*img偏移量*/, MediaAi::facePicMsg> match_data; 
        bool isTrigger = false;
        int offLen = headerLen;
        for (int i = 0; i < data->toTalPic; i++) {
            if (!data->faceMsg[i].isFullPic && AlarmInZone(header.srcPicSize, data->faceMsg[i].rect, m_ctx->env.face.zone)) {
                isTrigger = true;
                match_data[offLen] = data->faceMsg[i];
            }
            offLen += data->faceMsg[i].imgSize;
        }
        if (isTrigger) {
            // ovd告警
            bool started = m_started;
            AlarmTrigger();
            if (m_started && !started) {
                // 深拷贝出数据用于gat1400数据上传
                Gat1400Util::UploadDataParam param;
                Clone(param, match_data, header.data);
                // gat1400上传
                Gat1400Client::Instance()->Upload(param);
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

void AlarmFace::Clone(Gat1400Util::UploadDataParam &param
                , const std::map<int/*img偏移量*/, MediaAi::facePicMsg> &match_data
                , const uint8_t *face_data) {
    param.type = Gat1400Util::UploadType::Faces;
    param.InfoKind = 1;
    param.LeftTopX = 0;
    param.LeftTopY = 0;
    param.RightBtmX = 0;
    param.RightBtmY = 0;
    param.deviceId = Gat1400Client::Instance()->GetDeviceId();
    param.LocationMarkTime = Gat1400Util::GetTimeStr(Gat1400Util::TimePrecision::MS);
    param.AppearTime = Gat1400Util::GetTimeStr(Gat1400Util::TimePrecision::MS);
    param.DisAppearTime = Gat1400Util::GetTimeStr(Gat1400Util::TimePrecision::MS);
    param.SourceID = Gat1400Client::Instance()->GetGetSourceId(param.deviceId, param.AppearTime);
    param.Id = Gat1400Client::Instance()->GetFaceId(param.SourceID);
    for (auto sub : match_data) {
        Gat1400Util::SubImgDataParam faceImg;
        faceImg.isfullImg = sub.second.isFullPic;
        faceImg.ImageID = Gat1400Client::Instance()->GetGetSourceId(param.deviceId, param.AppearTime);
        faceImg.EventSort = 10;
        faceImg.FileFormat = Gat1400Util::GetImgType((Gat1400Util::ImgType)sub.second.imgType);
        faceImg.ShotTime = Gat1400Util::GetTimeStr(Gat1400Util::TimePrecision::S);
        faceImg.Width = sub.second.Width; 
        faceImg.Height = sub.second.Height;
        emxloge("headersize(%d)\n", sizeof(MediaAi::AiInfoFaceDetectionData));
        emxloge("off(%d)\n", sub.first);
        std::string inData = std::string((char*)(face_data + sub.first), sub.second.imgSize);
        Base64::Encode(inData, faceImg.Data);
        param.subImgList.push_back(faceImg);
#if 0
        char path[128] = { 0 };
        snprintf(path, sizeof(path) - 1
                    , "/root/configs/normal/face_%d_%d.jpg"
                    , (int)sub.second.isFullPic
                    , (int)sub.second.imgSize);
        FILE* fp = fopen(path, "w");
        if (fp != nullptr) {
            fwrite(face_data + sub.first, sub.second.imgSize, 1, fp);
            fclose(fp);
            fp = nullptr;
        }
#endif
    }
}