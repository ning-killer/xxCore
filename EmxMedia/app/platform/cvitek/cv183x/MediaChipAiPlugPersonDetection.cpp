#include "MediaChipAiPlugPersonDetection.hpp"
#include "MediaInner.hpp"

using namespace Emx;

void MediaChipAiPlugPersonDetection::Create(Json::Value &cfg) {
    if (!m_param.personDetection.ena)
        return;

    auto &json = cfg["personDetection"];
    m_vpssGrp = json["vpssGrp"].asInt();
    m_vpssChn = json["vpssChn"].asInt();
    m_modelId = (CVI_AI_SUPPORTED_MODEL_E) json["modelId"].asInt();

    char modelPath[EMX_MAX_PATH_SIZE] = {0};
    snprintf(modelPath,sizeof (modelPath),"%s/model/%s",m_res.configs.GetDir(), json["modelPath"].asCString());
    int ret = CVI_AI_SetModelPath(m_handle->m_aiHandle, m_modelId, modelPath);
    if (ret != CVI_SUCCESS) {
        emxloge("CVI_AI_SetModelPath %d: %s: failed with %#x!\n", m_modelId, modelPath, ret);
        return;
    }

    ret = CVI_AI_SetModelThreshold(m_handle->m_aiHandle, m_modelId, json["threshold"].asFloat());
    if (ret != CVI_SUCCESS) {
        emxloge("CVI_AI_SetModelThreshold failed with %#x!\n", ret);
        return;
    }

    CVI_AI_SetSkipVpssPreprocess(m_handle->m_aiHandle, m_modelId, CVI_TRUE);
    VPSS_GRP_ATTR_S stGrpAttr = {};
    CVI_VPSS_GetGrpAttr(m_vpssGrp, &stGrpAttr);
    cvai_vpssconfig_t vpssConfig = {};
    ret = CVI_AI_GetVpssChnConfig(m_handle->m_aiHandle, m_modelId,
                                  stGrpAttr.u32MaxW, stGrpAttr.u32MaxH,
                                  0,
                                  &vpssConfig);
    if (ret != CVI_SUCCESS) {
        emxloge("CVI_AI_GetVpssChnConfig failed with %#x!\n", ret);
        return;
    }

    emxlogd("chn_coeff = %d\n", vpssConfig.chn_coeff);
    emxlogd("chn_attr.stAspectRatio.enMode=%d\n", vpssConfig.chn_attr.stAspectRatio.enMode);

    ret = CVI_VPSS_SetChnScaleCoefLevel(m_vpssGrp, m_vpssChn, vpssConfig.chn_coeff);
    if (ret != CVI_SUCCESS) {
        emxloge("CVI_VPSS_SetChnScaleCoefLevel failed with %#x!\n", ret);
        return;
    }
    ret = CVI_VPSS_SetChnAttr(m_vpssGrp, m_vpssChn, &vpssConfig.chn_attr);
    if (ret != CVI_SUCCESS) {
        emxloge("CVI_VPSS_SetChnAttr failed with %#x!\n", ret);
        return;
    }
    ret = CVI_VPSS_EnableChn(m_vpssGrp, m_vpssChn);
    if (ret != CVI_SUCCESS) {
        emxloge("CVI_VPSS_EnableChn failed with %#x!\n", ret);
        return;
    }

    CVI_AI_SelectDetectClass(m_handle->m_aiHandle, m_modelId, 6, CVI_AI_DET_TYPE_PERSON,
                             CVI_AI_DET_TYPE_BICYCLE,
                             CVI_AI_DET_TYPE_CAR,
                             CVI_AI_DET_TYPE_MOTORBIKE,
                             CVI_AI_DET_TYPE_TRUCK,
                             CVI_AI_DET_TYPE_BUS);

    ret = CVI_AI_OpenModel(m_handle->m_aiHandle, m_modelId);
    if (ret != CVI_SUCCESS) {
        emxloge("CVI_AI_OpenModel failed with %#x!\n", ret);
        return;
    }
    ret = CVI_AI_SetVpssTimeout(m_handle->m_aiHandle, 2000);
    if (ret != CVI_SUCCESS) {
        emxloge("CVI_AI_SetVpssTimeout failed with %#x!\n", ret);
        return;
    }

    CallBack cb(m_vpssGrp,
                m_vpssChn,
                [this](VIDEO_FRAME_INFO_S &frame) { OnGetFrame(frame); },
                [this]() { OnComplete(); });
    m_callBackList.push_back(cb);
    m_got = false;
    emxlogi("Ai Plug PersonDetection Done\n");
}

void MediaChipAiPlugPersonDetection::OnGetFrame(VIDEO_FRAME_INFO_S &frame) {
    CVI_AI_MobileDetV2_Pedestrian_D0(m_handle->m_aiHandle, &frame, &m_obj);
    m_timestampMs = Time::GetMs64();
    m_srcPicSize.w = (int) frame.stVFrame.u32Width;
    m_srcPicSize.h = (int) frame.stVFrame.u32Height;
    m_got = true;
}

void MediaChipAiPlugPersonDetection::OnComplete() {
    if (!m_got)
        return;
    m_got = false;
    if (m_obj.size) {
        char *data;
        MediaAi::AiInfoHeader info = {};
        info.chn = m_chn;
        info.type = MediaAi::AiTypeE::PersonDetection;
        info.timestampMs = m_timestampMs;
        info.srcPicSize = m_srcPicSize;
        info.size = (int) m_obj.size * (int) sizeof(MediaAi::AiInfoPersonDetectionData);
        info.off = m_res.ring.GetFreeBuffer(info.size, info.timestampMs, &data);
        if (info.off > 0) {
            uint32_t off = 0;
            for (int i = 0; i < (int) m_obj.size; i++) {
                auto &bbox = m_obj.info[i].bbox;
                MediaAi::AiInfoPersonDetectionData person((int) bbox.x1,
                                                          (int) bbox.y1,
                                                          (int) (bbox.x2 - bbox.x1),
                                                          (int) (bbox.y2 - bbox.y1));
                memcpy(&data[off], &person, sizeof(person));
                off += sizeof(person);
            }
            m_res.pub.Publish(MEDIA_TOPIC_AI, (char *) &info, sizeof(info));
        }

    }
    CVI_AI_Free(&m_obj);
}