#include <valarray>
#include "ScreenExposure.hpp"
#include "MediaChipAiPlugFdPdDetection.hpp"
#include "Raw2Jpge.hpp"
#include "MediaInner.hpp"
#include "cviai/core/cviai_core.h"


using namespace Emx;

void MediaChipAiPlugFdPdDetection::Create(Json::Value &cfg) {
    if (!m_param.facePersonDetection.ena)
        return;
    auto &json = cfg["facePersonDetection"];
    CallBack cb(json["vpssGrp"].asInt(),
                json["vpssChn"].asInt(),
                [this](VIDEO_FRAME_INFO_S &frame) { OnGetFrame(frame); },
                [this]() { OnComplete(); });
    m_callBackList.push_back(cb);

    m_isStartAe = false;
    m_frameInterval = 0;
    m_got = false;
    m_fps = json["fps"].asInt();
    m_maxFaceNum = json["maxFaceNum"].asInt();
    m_imgQuality = json["imgQuality"].asInt();
    m_FDModelId = (CVI_AI_SUPPORTED_MODEL_E) json["FDModelId"].asInt();
    InitFaceCaptureConfig();

    int Ret = CVI_AI_APP_CreateHandle(&m_aiAppHandle, m_handle->m_aiHandle);
    Ret |= CVI_AI_APP_FaceCapture_Init(m_aiAppHandle, m_maxFaceNum);/*一次最多抓m_maxFaceNum个人的图*/
    if (Ret != CVI_SUCCESS) {
        emxloge("set FaceCapture failed with %#x!\n", Ret);
        return;
    }

    char modelFdPath[EMX_MAX_PATH_SIZE] = {};
    char modelFdFeaturePath[EMX_MAX_PATH_SIZE] = {};
    snprintf(modelFdPath, sizeof(modelFdPath), "%s/model/%s", m_res.configs.GetDir(),
             json["modelFdPath"].asCString());
    snprintf(modelFdFeaturePath, sizeof(modelFdFeaturePath), "%s/model/%s", m_res.configs.GetDir(),
             json["modelFdFeaturePath"].asCString());
    /*跟踪算法初始化*/
    Ret = CVI_AI_APP_FaceCapture_QuickSetUp(m_aiAppHandle,
                                            (CVI_AI_SUPPORTED_MODEL_E) json["FDModelId"].asInt(),
                                            (CVI_AI_SUPPORTED_MODEL_E) json["FeatureModelId"].asInt(), modelFdPath,
                                            NULL, NULL, modelFdFeaturePath);

    char modelPdPath[EMX_MAX_PATH_SIZE] = {};
    snprintf(modelPdPath, sizeof(modelPdPath), "%s/model/%s", m_res.configs.GetDir(),
             json["modelPdPath"].asCString());
    Ret |= CVI_AI_APP_FaceCapture_FusePedSetup(m_aiAppHandle,
                                               (CVI_AI_SUPPORTED_MODEL_E) json["PDModelId"].asInt(), modelPdPath);
    if (Ret != CVI_SUCCESS) {
        emxloge("Model initialization failed  with %#x!\n", Ret);
        return;
    }

    // Ret = CVI_AI_SetModelThreshold(m_handle->m_aiHandle, m_FDModelId, 0.5);
    SetFaceParam(aIParamType::SetSensitivity, m_param.facePersonDetection.captureSensitivity);
    Ret |= CVI_AI_SetModelThreshold(m_handle->m_aiHandle, (CVI_AI_SUPPORTED_MODEL_E) json["PDModelId"].asInt(),
                                    json["PDThreshold"].asInt());/*人形灵敏度可以固定死*/
    if (Ret != CVI_SUCCESS) {
        emxloge("Model sensitivity setting failed with %#x!\n", Ret);
        return;
    }

    Ret = CVI_AI_SetVpssTimeout(m_handle->m_aiHandle, 4000);
    if (Ret != CVI_SUCCESS) {
        emxloge("set vpss timeout failed with %#x!\n", Ret);
        return;
    }

    CVI_AI_SetVBPool(m_handle->m_aiHandle, 0, json["vbPool"].asInt());

    //CVI_AI_APP_FaceCapture_SetMode(m_handle->m_aiFaceCaptureHandle, AUTO);
    SetFaceParam(aIParamType::SetCaptureMode, m_param.facePersonDetection.captureMode);

    CVI_AI_APP_FaceCapture_SetConfig(m_aiAppHandle, &m_faceCaptureConfig);

    emxlogi("Ai Plug FaceDetection Done\n");
}

void MediaChipAiPlugFdPdDetection::Destroy() {

    CVI_AI_Service_DestroyHandle(m_aiAppHandle);
    m_callBackList.clear();
}

void MediaChipAiPlugFdPdDetection::InitFaceCaptureConfig() {
    m_faceCaptureConfig.thr_size_min = 32;        /*face size less than it, quality = 0*/
    m_faceCaptureConfig.thr_size_max = 1024;        /*face size more than it, quality = 0*/
    m_faceCaptureConfig.qa_method = 0;        /*use fq?*/
    m_faceCaptureConfig.thr_quality = 0.1;            /*capture new face quality difference*/
    m_faceCaptureConfig.thr_yaw = 0.75;            /*angle more than it, quality = 0*/
    m_faceCaptureConfig.thr_pitch = 0.75;            /*angle more than it, quality = 0*/
    m_faceCaptureConfig.thr_roll = 0.75;            /*angle more than it, quality = 0*/
    m_faceCaptureConfig.thr_laplacian = 20;
    m_faceCaptureConfig.miss_time_limit = 30;
    m_faceCaptureConfig.m_interval = m_param.facePersonDetection.captureInterval * m_fps;
    m_faceCaptureConfig.m_capture_num = m_param.facePersonDetection.captureNum;
    m_faceCaptureConfig.auto_m_fast_cap = 1;
    m_faceCaptureConfig.store_feature = 0;
    m_faceCaptureConfig.img_capture_flag = 0;
    m_faceCaptureConfig.eye_dist_thresh = m_param.facePersonDetection.captureQuality;
    m_faceCaptureConfig.landmark_score_thresh = 0.5;//用来过滤侧脸和头部的，人脸特征点得分的阈值
}

template<class ...variableParam>
int MediaChipAiPlugFdPdDetection::SetFaceParam(aIParamType paramType, variableParam... param) {

    return SetFaceCaptureProcess(paramType, param...);
}

template<typename T, typename... Rest>
int MediaChipAiPlugFdPdDetection::SetFaceCaptureProcess(aIParamType paramType, T value, Rest... rest) {

    int Ret = 0;
    switch (paramType) {
        case aIParamType::SetSensitivity: {
            //sensitivity值越大，越难检测到
            Ret = CVI_AI_SetModelThreshold(m_handle->m_aiHandle, m_FDModelId, value);
            emxlogi("face SetSensitivity:%f\n", value);
        }
            break;

        case aIParamType::SetCaptureNum : {
            m_faceCaptureConfig.m_capture_num = value;
            Ret = CVI_AI_APP_FaceCapture_SetConfig(m_aiAppHandle, &m_faceCaptureConfig);
            emxlogi("face SetCaptureNum:%d\n", value);
        }
            break;

        case aIParamType::SetCaptureMode : {
            if (value == 0) {
                Ret = CVI_AI_APP_FaceCapture_SetMode(m_aiAppHandle, AUTO);
            } else if (value == 1) {
                Ret = CVI_AI_APP_FaceCapture_SetMode(m_aiAppHandle, FAST);
            }
            emxlogi("face SetCaptureMode:%d\n", value);
        }
            break;

        case aIParamType::SetCaptureInterval : {
            m_faceCaptureConfig.m_interval = value * m_fps;
            Ret = CVI_AI_APP_FaceCapture_SetConfig(m_aiAppHandle, &m_faceCaptureConfig);
            emxlogi("face SetCaptureInterval:%d\n", value);
        }
            break;

        case aIParamType::SetCaptureQuality: {
            m_faceCaptureConfig.eye_dist_thresh = value;
            Ret = CVI_AI_APP_FaceCapture_SetConfig(m_aiAppHandle, &m_faceCaptureConfig);
            emxlogi("face SetCaptureQuality->eye_dist_thresh:%d\n", value);
        }
            break;

        case aIParamType::SetSmartAe: {
            if (value == false)
                ScreenExposure::GetInst()->EndScreenExposure(0);
        }
            break;
        default:
            emxloge("Command not supported!\n");
            break;
    }

    if (Ret != CVI_SUCCESS) {
        emxloge("face SetFaceCaptureProcess  fail,error:%d\n", Ret);
    }
    return SetFaceCaptureProcess(paramType, rest...);
}

void MediaChipAiPlugFdPdDetection::ConfigAiParam(const MediaAi::Param &param) {
    if (m_param.facePersonDetection.captureSensitivity != param.facePersonDetection.captureSensitivity) {
        SetFaceParam(aIParamType::SetSensitivity, param.facePersonDetection.captureSensitivity);
    } else if (m_param.facePersonDetection.captureNum != param.facePersonDetection.captureNum) {
        SetFaceParam(aIParamType::SetCaptureNum, param.facePersonDetection.captureMode);
    } else if (m_param.facePersonDetection.captureMode != param.facePersonDetection.captureNum) {
        SetFaceParam(aIParamType::SetCaptureMode, param.facePersonDetection.captureMode);
    } else if (m_param.facePersonDetection.captureInterval != param.facePersonDetection.captureInterval) {
        SetFaceParam(aIParamType::SetCaptureInterval, param.facePersonDetection.captureInterval);
    } else if (m_param.facePersonDetection.captureQuality != param.facePersonDetection.captureQuality) {
        SetFaceParam(aIParamType::SetCaptureQuality, param.facePersonDetection.captureQuality);
    } else if (!param.facePersonDetection.aeEna) {
        SetFaceParam(aIParamType::SetSmartAe, param.facePersonDetection.aeEna);
    }
}

void MediaChipAiPlugFdPdDetection::OnGetFrame(VIDEO_FRAME_INFO_S &frame) {

    CVI_S32 s32Ret;
    RECT_S rect[10] = {};

    s32Ret = CVI_AI_APP_FaceCapture_Run(m_aiAppHandle, &frame);
    if (s32Ret != CVI_SUCCESS) {
        emxlogd("CVI_AI_APP_FaceCapture_Run fail with %#x \n", s32Ret);
        return;
    }

    if (m_param.facePersonDetection.aeEna || m_param.facePersonDetection.faceDraw) {
        //
        if (m_aiAppHandle->face_cpt_info->last_faces.size > 0) {

            for (uint32_t i = 0; i < m_aiAppHandle->face_cpt_info->last_faces.size; ++i) {
                rect[i].s32X = m_aiAppHandle->face_cpt_info->last_faces.info->bbox.x1;
                rect[i].s32Y = m_aiAppHandle->face_cpt_info->last_faces.info->bbox.y1;
                rect[i].u32Width =
                        m_aiAppHandle->face_cpt_info->last_faces.info->bbox.x2 -
                        m_aiAppHandle->face_cpt_info->last_faces.info->bbox.x1;
                rect[i].u32Height =
                        m_aiAppHandle->face_cpt_info->last_faces.info->bbox.y2 -
                        m_aiAppHandle->face_cpt_info->last_faces.info->bbox.y1;

                if (i == (uint32_t) m_maxFaceNum)
                    break;
            }

            if (m_param.facePersonDetection.aeEna && !m_isStartAe) {
                m_frameInterval++;
                if (m_frameInterval >= m_fps) {
                    m_frameInterval = 0;
                    m_isStartAe = true;
                    ScreenExposure::GetInst()->StartScreenExposure(&frame, rect,
                                                                   m_aiAppHandle->face_cpt_info->last_faces.size,
                                                                   m_param.facePersonDetection.aeValue, 0);
                }
            }

            if (m_param.facePersonDetection.faceDraw) {
                ///***画框
            }
        } else if (m_aiAppHandle->face_cpt_info->last_faces.size <= 0) {
            if (m_param.facePersonDetection.aeEna && m_isStartAe) {
                m_frameInterval++;
                if (m_frameInterval >= m_fps) {
                    m_frameInterval = 0;
                    m_isStartAe = false;
                    ScreenExposure::GetInst()->EndScreenExposure(0);
                }
            }
        }
    }

    m_timestampMs = Time::GetMs64();
    m_srcPicSize.w = (int) frame.stVFrame.u32Width;
    m_srcPicSize.h = (int) frame.stVFrame.u32Height;
    m_got = true;
}

void media_fd_jpeg_save(char *filename, unsigned char *data, int len) {
    char name[128] = {0};
    if (access("/mnt/sdcard/pic", F_OK)) {
        system("mkdir -p /mnt/sdcard/pic");
    }
    sprintf(name, "/mnt/sdcard/pic/%s", filename);
    FILE *pFile = fopen(name, "wb");
    if (pFile) {
        fwrite(data, len, 1, pFile);
        fflush(pFile);
        fclose(pFile);
        pFile = NULL;
    }
    return;
}

void MediaChipAiPlugFdPdDetection::OnComplete() {
    if (!m_got || !m_aiAppHandle->face_cpt_info)
        return;
    m_got = false;

    if (m_param.facePersonDetection.personDraw) {

    }

    if (m_aiAppHandle->face_cpt_info->last_faces.size > 0) {
        char *data;
        MediaAi::AiInfoHeader info = {};
        info.chn = m_chn;
        info.type = MediaAi::AiTypeE::PersonDetection;
        info.timestampMs = m_timestampMs;
        info.srcPicSize = m_srcPicSize;
        info.size = (int) m_aiAppHandle->face_cpt_info->last_objects.size *
                    (int) sizeof(MediaAi::AiInfoPersonDetectionData);
        info.off = m_res.ring.GetFreeBuffer(info.size, info.timestampMs, &data);
        if (info.off > 0) {
            uint32_t off = 0;
            for (int i = 0; i < (int) m_aiAppHandle->face_cpt_info->last_objects.size; i++) {
                auto &bbox = m_aiAppHandle->face_cpt_info->last_objects.info->bbox;
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

    MediaAi::AiInfoFaceDetectionData face = {0};
    unsigned long piclen = 0;
    for (uint32_t i = 0; i < m_aiAppHandle->face_cpt_info->size; ++i) {
        if (!m_aiAppHandle->face_cpt_info->_output[i])
            continue;

        Raw2JpegInfo rawInfo = {0};
        rawInfo.width = m_aiAppHandle->face_cpt_info->data[i].image.width;
        rawInfo.height = m_aiAppHandle->face_cpt_info->data[i].image.height;
        rawInfo.stride = m_aiAppHandle->face_cpt_info->data[i].image.width * 3;
        rawInfo.pixel_format = RAW2JPEG_MPDE_RGB;
        rawInfo.jpeg_quality = m_imgQuality;
        rawInfo.src_size = m_aiAppHandle->face_cpt_info->data[i].image.length[0];
        rawInfo.src_buffer = m_aiAppHandle->face_cpt_info->data[i].image.pix[0];
        Raw2Jpeg::RawToJpeg(&rawInfo, &face.faceMsg[face.toTalPic].img, &face.faceMsg[face.toTalPic].imgSize);


        piclen += face.faceMsg[face.toTalPic].imgSize;
        face.faceMsg[face.toTalPic].id = m_aiAppHandle->face_cpt_info->data[i].info.unique_id;
        face.faceMsg[face.toTalPic].capture_time = m_aiAppHandle->face_cpt_info->data[i]._timestamp;
        face.faceMsg[face.toTalPic].start_time = m_aiAppHandle->face_cpt_info->data[i].cap_timestamp;
        face.faceMsg[face.toTalPic].end_time = m_aiAppHandle->face_cpt_info->data[i].last_cap_timestamp;
        face.faceMsg[face.toTalPic].face_quality = m_aiAppHandle->face_cpt_info->data[i].info.face_quality;
        face.faceMsg[face.toTalPic].face_sharpness = m_aiAppHandle->face_cpt_info->data[i].info.sharpness_score;
        face.faceMsg[face.toTalPic].rect.x =(int)m_aiAppHandle->face_cpt_info->data[i].info.bbox.x1;
        face.faceMsg[face.toTalPic].rect.y =(int)m_aiAppHandle->face_cpt_info->data[i].info.bbox.y1;
        face.faceMsg[face.toTalPic].rect.h =(int)m_aiAppHandle->face_cpt_info->data[i].info.bbox.x2-(int)m_aiAppHandle->face_cpt_info->data[i].info.bbox.x1;
        face.faceMsg[face.toTalPic].rect.w =(int)m_aiAppHandle->face_cpt_info->data[i].info.bbox.y2-(int)m_aiAppHandle->face_cpt_info->data[i].info.bbox.y1;
        face.toTalPic++;

    }

    if (face.toTalPic > 0) {
        int bufLen = sizeof(MediaAi::AiInfoFaceDetectionData) + piclen;
        char *data;
        MediaAi::AiInfoHeader info = {};
        info.chn = m_chn;
        info.type = MediaAi::AiTypeE::FaceSnap;
        info.timestampMs = m_timestampMs;
        info.srcPicSize = m_srcPicSize;
        info.size = bufLen;
        info.off = m_res.ring.GetFreeBuffer(info.size, info.timestampMs, &data);
        memcpy(data, &face, sizeof(MediaAi::AiInfoFaceDetectionData));
        unsigned long copyPicLen = sizeof(MediaAi::AiInfoFaceDetectionData);
        for (int i = 0; i < face.toTalPic; ++i) {
            memcpy(data + copyPicLen, face.faceMsg[i].img,
                   face.faceMsg[i].imgSize);
            free(face.faceMsg[i].img);
            copyPicLen += face.faceMsg[i].imgSize;
        }
        m_res.pub.Publish(MEDIA_TOPIC_AI, (char *) &info, sizeof(info));
    }
}





