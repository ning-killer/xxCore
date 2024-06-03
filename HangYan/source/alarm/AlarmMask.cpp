#include "AlarmMask.hpp"

using namespace Emx;

ErrCodeE AlarmMask::Create() {
    m_ctx->soundAlarmControl.Update(
        SoundAlarmControl::Type::FACE_MASK, SoundAlarmControl::Status::Off);
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
        if (isTrigger) {
            if (m_ctx->env.cap.info.ovdAICapInfo.AIface.mask_detection 
                && m_ctx->env.face.mask_detection) {
                // 未戴口罩需要告警
                if (!isMasked) {
                    bool started = m_started;
                    AlarmTrigger();
                    if (m_started && !started) {
                        BreakLastSoundAndLightAlarm();
                        Strategy(&m_ctx->env.face.strategy, &m_ctx->env.face.mask_detection, "maskAlarmVoice");
                    }
                }
            }
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

void AlarmMask::Destroy() {
    if (m_ai != nullptr) {
        m_ai->Stop();
        delete m_ai;
        m_ai = nullptr;
    }
}