//
// Created by xiong on 2021/4/30.
//
#include <isp/cv182x/cvi_ae.h>
#include <isp/cv182x/cvi_awb.h>
#include "sample_comm.h"
#include "core/EmxLog.hpp"
#include "MediaConfigs.hpp"
#include "MediaChipISP.hpp"
#include "MediaInner.hpp"

using namespace Emx;

ErrCodeE MediaChipISP::Start() {
    if (m_started)
        return ErrCodeE::Success;
    if (LoadParam() != ErrCodeE::Success) {
        return ErrCodeE::Failure;
    }
    Json::Value root;
    m_res.configs.GetCfg("isp", root);
    auto &json = root[m_chn];
    m_pipe = json["pipe"].asInt();
    m_infoInvMs = json["infoInvMs"].asInt();
    m_wdrMode = (WDR_MODE_E) json["wdrMode"].asInt();
    m_timer.Create(m_res.loop);
    SetRunMode(RunModeE::Day);
    StartCollection();
    m_started = true;
    return ErrCodeE::Success;
}

ErrCodeE MediaChipISP::Stop() {
    if (!m_started)
        return ErrCodeE::Success;
    m_started = false;
    StopCollection();
    m_timer.Destroy();
    return ErrCodeE::Success;
}

ErrCodeE MediaChipISP::SetParam(const MediaISP::Param &param) {
    if (!ParamChanged(param, m_param)) {
        return ErrCodeE::Success;
    }
    if (!ParamValid(param)) {
        return ErrCodeE::IllegalParam;
    }
    StopCollection();
    memcpy(&m_param, &param, sizeof(MediaISP::Param));
    SaveParam();
    StartCollection();
    return ErrCodeE::Success;
}

ErrCodeE MediaChipISP::GetParam(MediaISP::Param &param) {
    memcpy(&param, &m_param, sizeof(MediaISP::Param));
    return ErrCodeE::Success;
}

ErrCodeE MediaChipISP::SetRunMode(MediaISP::RunModeE mode) {
    if (m_mode == mode) {
        return ErrCodeE::Success;
    }
    Json::Value root;
    m_res.configs.GetCfg("isp", root);
    auto &json = root[m_chn];
    const char *name;
    int fps;
    switch (mode) {
        case RunModeE::Day:
            name = "day";
            fps = json["fps"]["day"].asInt();
            break;
        case RunModeE::Night:
            name = "night";
            fps = json["fps"]["night"].asInt();
            break;
        case RunModeE::NightWithLight:
            name = "nightWithLight";
            fps = json["fps"]["night"].asInt();
            break;
        default:
            emxloge("unknown mode %d\n", (int) mode);
            return ErrCodeE::IllegalParam;
    }

    char path[EMX_MAX_PATH_SIZE];
    snprintf(path, sizeof(path), "%s/isp/%s", m_res.configs.GetDir(), json["pq"][name].asCString());
    CVI_S32 s32Ret = CVI_SUCCESS;
    CVI_BIN_SetBinName(m_wdrMode, path);
    CVI_CHAR binName[BIN_FILE_LENGTH];
    s32Ret = CVI_BIN_GetBinName(binName);
    if (s32Ret != CVI_SUCCESS) {
        emxloge("GetBinName(%s) fail\n", binName);
        return ErrCodeE::Failure;
    }
    std::string buffer;
    if (File::Read(path, buffer) != ErrCodeE::Success) {
        emxloge("read %s failed\n", path);
        return ErrCodeE::Failure;
    }
    s32Ret = CVI_BIN_ImportBinData((uint8_t *) buffer.data(), (uint32_t) buffer.size());
    if (s32Ret != CVI_SUCCESS) {
        emxloge("PQBin file(%s), buffer size(%d)\n", path, buffer.size());
        emxloge("CVI_BIN_ImportBinData failed with %#x!\n", s32Ret);
    }

    ISP_PUB_ATTR_S pubAttr = {};
    if (CVI_ISP_GetPubAttr(m_pipe, &pubAttr) == CVI_SUCCESS) {
        if (pubAttr.f32FrameRate != (float) fps) {
            pubAttr.f32FrameRate = (float) fps;
            if (CVI_ISP_SetPubAttr(m_pipe, &pubAttr) == CVI_SUCCESS) {
                SetAntiFlicker(m_param.antiFlickerHz, m_param.forceAntiFlickerEna);
                Json::Value jsonFps;
                jsonFps["chn"] = m_pipe;
                jsonFps["fps"] = fps;
                std::string data;
                EasyJson::Serialize(json, data);
                m_res.pub.Publish(MEDIA_TOPIC_ISP_FPS_CHANGE, data.data(), (int) data.size() + 1);
                //todo:vpss and venc change fps src
            } else {
                emxloge("CVI_ISP_SetPubAttr %d failed with %#x!\n", m_pipe, s32Ret);
            }
        }
    } else {
        emxloge("CVI_ISP_GetPubAttr %d failed with %#x!\n", m_pipe, s32Ret);
    }
    m_mode = mode;

    // struct IspControlArg {
    //     CVI_S32 s32Ret = CVI_SUCCESS;
    //     int fps;
    //     MediaISP::RunModeE mode;
    //     char path[EMX_MAX_PATH_SIZE] = { 0 };
    //     Json::Value json;
    //     EuvWork work;
    // };
    // auto *handle = new IspControlArg;
    // snprintf(handle->path, sizeof(handle->path)
    //         , "%s/isp/%s", m_res.configs.GetDir(), json["pq"][name].asCString());
    // handle->fps = fps;
    // handle->mode = mode;
    // handle->json = json;
    // handle->work.Create(&m_res.loop, handle,
    //                     [this](void *arg) {
    //                         auto *handle = (IspControlArg*)arg;
    //                         if (handle == nullptr) {
    //                             emxloge("isp arg is null");
    //                             return;
    //                         }
    //                         CVI_BIN_SetBinName(m_wdrMode, handle->path);
    //                         CVI_CHAR binName[BIN_FILE_LENGTH];
    //                         handle->s32Ret = CVI_BIN_GetBinName(binName);
    //                         if (handle->s32Ret != CVI_SUCCESS) {
    //                             emxloge("GetBinName(%s) fail\n", binName);
    //                             return;
    //                         }
    //                         std::string buffer;
    //                         if (File::Read(handle->path, buffer) != ErrCodeE::Success) {
    //                             emxloge("read %s failed\n", handle->path);
    //                             handle->s32Ret = (CVI_S32)ErrCodeE::Failure;
    //                             return;
    //                         }
    //                         handle->s32Ret = CVI_BIN_ImportBinData((uint8_t *) buffer.data(), (uint32_t) buffer.size());
    //                         if (handle->s32Ret != CVI_SUCCESS) {
    //                             emxloge("PQBin file(%s), buffer size(%d)\n", handle->path, buffer.size());
    //                             emxloge("CVI_BIN_ImportBinData failed with %#x!\n", handle->s32Ret);
    //                         } else {
    //                             emxlogd("CVI_BIN_ImportBinData Ok!\n");
    //                         }
    //                     },
    //                     [this](ErrCodeE e, void *arg) {
    //                         auto *handle = (IspControlArg*)arg;
    //                         do {
    //                             if (handle == nullptr) {
    //                                 emxloge("isp arg is null");
    //                                 break;
    //                             }
    //                             if (handle->s32Ret != CVI_SUCCESS) {
    //                                 break;
    //                             }
    //                             m_mode = handle->mode;
    //                             emxlogd("SetRunMode(%d) Success\n", m_mode);
    //                             ISP_PUB_ATTR_S pubAttr = {};
    //                             if (CVI_ISP_GetPubAttr(m_pipe, &pubAttr) != CVI_SUCCESS) {
    //                                 emxloge("CVI_ISP_GetPubAttr %d failed with %#x!\n", m_pipe, handle->s32Ret);
    //                                 break; 
    //                             }
    //                             if (pubAttr.f32FrameRate == (float) handle->fps) {
    //                                 emxlogd("f32FrameRate no changed\n");
    //                                 break;
    //                             }
    //                             pubAttr.f32FrameRate = (float) handle->fps;
    //                             if (CVI_ISP_SetPubAttr(m_pipe, &pubAttr) != CVI_SUCCESS) {
    //                                 emxloge("CVI_ISP_SetPubAttr %d failed with %#x!\n", m_pipe, handle->s32Ret);
    //                                 break;
    //                             }
    //                             SetAntiFlicker(m_param.antiFlickerHz, m_param.forceAntiFlickerEna);
    //                             Json::Value jsonFps;
    //                             jsonFps["chn"] = m_pipe;
    //                             jsonFps["fps"] = handle->fps;
    //                             std::string data;
    //                             EasyJson::Serialize(handle->json, data);
    //                             m_res.pub.Publish(MEDIA_TOPIC_ISP_FPS_CHANGE, data.data(), (int) data.size() + 1);
    //                         } while(false);
                            
    //                         if (handle != nullptr) {
    //                             handle->work.Destroy();
    //                             delete handle;
    //                             handle = nullptr;
    //                         }
    //                     });
    // handle->work.Run();
    return ErrCodeE::Success;
}

void MediaChipISP::StartCollection() {
    m_timer.Start(3000, m_infoInvMs, [this]() {
        Collection();
    });
}

void MediaChipISP::StopCollection() {
    m_timer.Stop();
}

void MediaChipISP::Collection() {
    ISP_EXP_INFO_S stExpInfo = {};
    if (CVI_ISP_QueryExposureInfo((VI_PIPE) m_pipe, &stExpInfo) != CVI_SUCCESS)
        return;
    CVI_U16 redGain = 0;
    CVI_U16 blueGain = 0;
    if (m_mode == RunModeE::Night) {
        if (CVI_ISP_GetGrayWorldAwbInfo((VI_PIPE) m_pipe, &redGain, &blueGain) != CVI_SUCCESS)
            return;
    } else {
        ISP_WB_INFO_S stWBInfo = {};
        if (CVI_ISP_QueryWBInfo((VI_PIPE) m_pipe, &stWBInfo) != CVI_SUCCESS)
            return;
        redGain = stWBInfo.u16Rgain;
        blueGain = stWBInfo.u16Bgain;
    }
    Info info = {};
    info.chn = m_chn;
    info.iso = (int) stExpInfo.u32ISO;
    info.redGain = redGain;
    info.blueGain = blueGain;
    info.luma = (int) stExpInfo.u8AveLum;
    m_res.pub.Publish(MEDIA_TOPIC_ISP_INFO, (char *) &info, (int) sizeof(info));
}

ErrCodeE MediaChipISP::SetAntiFlicker(uint8_t antiFlicker, bool forceAntiFlickerEna) {
    CVI_S32 s32Ret = CVI_SUCCESS;
    ISP_EXPOSURE_ATTR_S stExpAttr = {};
    s32Ret = CVI_ISP_GetExposureAttr(m_pipe, &stExpAttr);
    if (s32Ret != CVI_SUCCESS) {
        emxloge("pipe %d CVI_ISP_GetExposureAttr err,s32Ret=%#x\n", m_pipe, s32Ret);
        return ErrCodeE::Failure;
    }
    stExpAttr.stAuto.stAntiflicker.bEnable = CVI_TRUE;
    stExpAttr.stAuto.stAntiflicker.enMode = ISP_ANTIFLICKER_AUTO_MODE;
    stExpAttr.stAuto.stAntiflicker.enFrequency =
            antiFlicker == 50 ? AE_FREQUENCE_50HZ : AE_FREQUENCE_60HZ;
    s32Ret = CVI_ISP_SetExposureAttr(m_pipe, &stExpAttr);
    if (s32Ret != CVI_SUCCESS) {
        emxloge("pipe %d CVI_ISP_GetExposureAttr err,s32Ret=%#x\n", m_pipe, s32Ret);
        return ErrCodeE::Failure;
    }
    return ErrCodeE::Success;
}


