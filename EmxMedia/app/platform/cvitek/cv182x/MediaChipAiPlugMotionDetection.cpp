//
// Created by xiong on 2022/5/11.
//

#include "MediaChipAiPlugMotionDetection.hpp"
#include "MediaInner.hpp"

using namespace Emx;

void MediaChipAiPlugMotionDetection::Create(Json::Value &cfg) {
    if (!m_param.motionDetection.ena)
        return;
    auto &json = cfg["motionDetection"];
    CallBack cb(json["vpssGrp"].asInt(),
                json["vpssChn"].asInt(),
                [this](VIDEO_FRAME_INFO_S &frame) { OnGetFrame(frame); },
                [this]() { OnComplete(); });
    m_callBackList.push_back(cb);

    m_interval = json["interval"].asInt();
    m_threshold = json["threshold"].asInt();
    m_minArea = json["minArea"].asInt();
    m_count = m_interval - 1;
    m_got = false;
    emxlogi("Ai Plug MotionDetection Done\n");
}

void MediaChipAiPlugMotionDetection::OnGetFrame(VIDEO_FRAME_INFO_S &frame) {
    CVI_S32 s32Ret;
    m_count++;
    if (m_count == m_interval) {
        m_count = 0;
        s32Ret = CVI_AI_Set_MotionDetection_Background(m_handle->m_aiHandle, &frame);
        if (s32Ret != CVI_SUCCESS) {
            emxloge("chn %d update background failed with %#x\n", m_chn, s32Ret);
        }
    }
    // Detect moving objects. All moving objects are store in obj_meta.
    // CVI_AI_MotionDetection(m_handle->m_aiHandle, &frame, &m_obj, m_threshold, m_minArea);
    // emxlogd("motion detect threshold: %d\n", m_param.motionDetection.threshold);
    CVI_AI_MotionDetection(m_handle->m_aiHandle, &frame, &m_obj, m_param.motionDetection.threshold, m_minArea);
    m_timestampMs = Time::GetMs64();
    m_srcPicSize.w = (int) frame.stVFrame.u32Width;
    m_srcPicSize.h = (int) frame.stVFrame.u32Height;
    m_got = true;
}

void MediaChipAiPlugMotionDetection::OnComplete() {
    if (!m_got)
        return;
    if (m_obj.size) {
        char *data;
        MediaAi::AiInfoHeader info = {};
        info.chn = m_chn;
        info.type = MediaAi::AiTypeE::MotionDetection;
        info.timestampMs = m_timestampMs;
        info.srcPicSize = m_srcPicSize;
        info.size = (int) m_obj.size * (int) sizeof(MediaAi::AiInfoMotionDetectionData);
        info.off = m_res.ring.GetFreeBuffer(info.size, info.timestampMs, &data);
        if (info.off > 0) {
            uint32_t off = 0;
            for (int i = 0; i < (int) m_obj.size; i++) {
                auto &bbox = m_obj.info[i].bbox;
                MediaAi::AiInfoMotionDetectionData motion((int) bbox.x1,
                                                          (int) bbox.y1,
                                                          (int) (bbox.x2 - bbox.x1),
                                                          (int) (bbox.y2 - bbox.y1));
                memcpy(&data[off], &motion, sizeof(motion));
                off += sizeof(motion);
            }
            m_res.pub.Publish(MEDIA_TOPIC_AI, (char *) &info, sizeof(info));
        }
    }
    CVI_AI_Free(&m_obj);
}
