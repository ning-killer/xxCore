//
// Created by xiong on 2020/10/27.
//
#include "MediaChipSystem.hpp"
#include "core/EmxLog.hpp"
#include "core/EmxParam.hpp"
#include "MediaConfigs.hpp"
#include "sample_comm.h"
#include "json/json.h"

using namespace Emx;

ErrCodeE MediaChipSystem::Start() {
    if (m_started)
        return ErrCodeE::Success;
    Json::Value json;
    m_res.configs.GetCfg("system", json);
    StartPre(json);
    ErrCodeE e;
    e = InitVb(json["vb"]);
    if (e != ErrCodeE::Success) {
        emxlogc("Init vb failed : %d\n", e);
        return e;
    }
    e = InitVi(json["vi"]);
    if (e != ErrCodeE::Success) {
        DeInitVb(json["vb"]);
        emxlogc("InitSensor failed : %d\n", e);
        return e;
    }
    e = InitVpss(json["vpss"]);
    if (e != ErrCodeE::Success) {
        DeInitVi(json["vi"]);
        DeInitVb(json["vb"]);
        emxlogc("InitVpss failed : %d\n", e);
        return e;
    }
    StartPost(json);
    m_started = true;
    emxlogi("Start system done\n");
    return ErrCodeE::Success;
}

ErrCodeE MediaChipSystem::Stop() {
    if (!m_started)
        return ErrCodeE::Success;
    m_started = false;
    Json::Value json;
    m_res.configs.GetCfg("system", json);
    StopPre(json);
    DeInitVpss(json["vpss"]);
    DeInitVi(json["vi"]);
    DeInitVb(json["vb"]);
    StopPost(json);
    emxlogi("Stop system done\n");
    return ErrCodeE::Success;
}

ErrCodeE MediaChipSystem::InitVb(const Json::Value &json) {
    VB_CONFIG_S stVbConf = {};
    stVbConf.u32MaxPoolCnt = json.size();
    for (uint32_t i = 0; i < stVbConf.u32MaxPoolCnt; i++) {
        CVI_U32 u32BlkSize = COMMON_GetPicBufferSize(
                json[i]["width"].asInt(),
                json[i]["height"].asInt(),
                (PIXEL_FORMAT_E) json[i]["format"].asInt(),
                (DATA_BITWIDTH_E) json[i]["bitWidth"].asInt(),
                (COMPRESS_MODE_E) json[i]["compress"].asInt(),
                json[i]["align"].asInt());
        stVbConf.astCommPool[i].u32BlkSize = u32BlkSize;//2592000
        stVbConf.astCommPool[i].u32BlkCnt = json[i]["cnt"].asInt();
        emxlogd("pool[%d] BlkSize = %u,BlkCnt=%u\n", i,
               stVbConf.astCommPool[i].u32BlkSize,
               stVbConf.astCommPool[i].u32BlkCnt);
    }
    CVI_S32 s32Ret;
    s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
    if (CVI_SUCCESS != s32Ret) {
        emxlogc("system init failed with %d!\n", s32Ret);
        return ErrCodeE::Failure;
    }
    emxlogt("vb init done\n");
    return ErrCodeE::Success;
}

void MediaChipSystem::DeInitVb(const Json::Value &json) {
    SAMPLE_COMM_SYS_Exit();
    emxlogt("vb deinit done\n");
}
static CVI_S32 media_isp_ProcInfo_Open(CVI_U32 ProcLogLev)
{
    CVI_S32 s32Ret = CVI_SUCCESS;

    if (ProcLogLev == 0) {
        printf("isp proc log not enable\n");
    } else {
        ISP_CTRL_PARAM_S setParam;
        memset(&setParam, 0, sizeof(ISP_CTRL_PARAM_S));

        setParam.u32ProcLevel = ProcLogLev;	// proc printf level (level =0,disable; =3,log max)
        setParam.u32ProcParam = 15;		// isp info frequency of collection (unit:frame; rang:(0,0xffffffff])
        setParam.u32AEStatIntvl = 1;	// AE info update frequency (unit:frame; rang:(0,0xffffffff])
        setParam.u32AWBStatIntvl = 6;	// AW info update frequency (unit:frame; rang:(0,0xffffffff])
        setParam.u32AFStatIntvl = 0;	// AF info update frequency (unit:frame; rang:(0,0xffffffff])
        setParam.u32UpdatePos = 0;		// Now, only support before sensor cfg; default 0
        setParam.u32IntTimeOut = 0;		// interrupt timeout; unit:ms; not used now
        setParam.u32PwmNumber = 0;		// PWM Num ID; Not used now
        setParam.u32PortIntDelay = 0;	// Port interrupt delay time

        s32Ret = CVI_ISP_SetCtrlParam(0, &setParam);
        if (s32Ret != CVI_SUCCESS) {
            printf("CVI_ISP_SetCtrlParam failed with %#x\n", s32Ret);
            return s32Ret;
        }
    }

    return s32Ret;
}
ErrCodeE MediaChipSystem::InitVi(const Json::Value &json) {
    CVI_S32 s32Ret;
    std::vector<VI_DEV> vViDev;//for rollback destroy of vi devs, when sth wrong during process

    //配置vi-vpss模式
    if (SetViVpssMode(json["ViVpssMode"]) != ErrCodeE::Success)
        return ErrCodeE::Failure;

    /************************************************
     * step1:  Config VI
     ************************************************/
    //设置sensor配置文件路径
    char path[EMX_MAX_PATH_SIZE] = {};
    snprintf(path, sizeof(path),
             "%s/sensor/%s",
             m_res.configs.GetDir(), json["sensorCfg"].asCString());
    SAMPLE_COMM_VI_SetIniPath(path);

    //加载并解析sensor配置文件
    SAMPLE_INI_CFG_S stIniCfg = {};
    //    stIniCfg.enSource  = VI_PIPE_FRAME_SOURCE_DEV;
    //    stIniCfg.devNum    = 1;
    //    stIniCfg.enSnsType = SONY_IMX307_MIPI_2M_30FPS_12BIT;
    //    stIniCfg.enWDRMode = WDR_MODE_NONE;
    //    stIniCfg.s32BusId  = 2;
    //    stIniCfg.s32SnsI2cAddr = -1;
    //    stIniCfg.MipiDev   = 0xFF;
    //    stIniCfg.u8UseDualSns = 0;
    //    stIniCfg.enSns2Type = SONY_IMX307_SLAVE_MIPI_2M_30FPS_12BIT;
    //    stIniCfg.s32Sns2BusId = 0;
    //    stIniCfg.s32Sns2I2cAddr = -1;
    //    stIniCfg.Sns2MipiDev = 0xFF;

    if (!SAMPLE_COMM_VI_ParseIni(&stIniCfg)) {
        emxlogc("SAMPLE_COMM_VI_ParseIni failed!\n");
        return ErrCodeE::Failure;
    }
    //    stIniCfg.enWDRMode = WDR_MODE_NONE;
    //    stIniCfg.u8Orien = 0;
    //    CVI_VI_SetDevNum(stIniCfg.devNum);
    //将配置文件数据转换为vi配置数据
    s32Ret = SAMPLE_COMM_VI_IniToViCfg(&stIniCfg, &m_viCfg);
    if (s32Ret != CVI_SUCCESS) {
        emxlogc("SAMPLE_COMM_VI_IniToViCfg failed with %#x\n", s32Ret);
        return ErrCodeE::Failure;
    }

    /************************************************
    * step2:  Init VI ISP
    ************************************************/
    //todo:不确定这部操作是否需要某个对应的回滚stop操作
    s32Ret = SAMPLE_COMM_VI_StartSensor(&m_viCfg);
    if (s32Ret != CVI_SUCCESS) {
        emxlogc("system start sensor failed with %#x\n", s32Ret);
        return ErrCodeE::Failure;
    }

    for (VI_DEV ViDev = 0; ViDev < m_viCfg.s32WorkingViNum; ViDev++) {
        s32Ret = SAMPLE_COMM_VI_StartDev(&m_viCfg.astViInfo[ViDev]);
        if (s32Ret != CVI_SUCCESS) {
            emxlogc("VI_StartDev[%d] failed with %#x\n", ViDev, s32Ret);
            goto EXIT_DESTROY_DEV;
        }
        //for rollback destroy
        vViDev.push_back(ViDev);
    }

    //todo:不确定这部操作是否需要某个对应的回滚stop操作
    s32Ret = SAMPLE_COMM_VI_StartMIPI(&m_viCfg);
    if (s32Ret != CVI_SUCCESS) {
        emxlogc("system start MIPI failed with %#x\n", s32Ret);
        goto EXIT_DESTROY_DEV;
    }

    if (CreatePipe(json["pipeAttr"]) != ErrCodeE::Success)
        goto EXIT_DESTROY_DEV;

    for (auto &item: json["defIsp"]) {
        snprintf(path, sizeof(path), "%s/isp/%s",
                 m_res.configs.GetDir(), item["bin"].asCString());
        s32Ret = CVI_BIN_SetBinName((WDR_MODE_E) item["mode"].asInt(), path);
        if (s32Ret != CVI_SUCCESS) {
            emxloge("CVI_BIN_SetBinName %s failed with %#x!\n", path, s32Ret);
        }
    }

    s32Ret = SAMPLE_COMM_VI_CreateIsp(&m_viCfg);
    if (s32Ret != CVI_SUCCESS) {
        emxlogc("VI_CreateIsp failed with %#x!\n", s32Ret);
        goto EXIT_DESTROY_PIPE;
    }

    s32Ret = SAMPLE_COMM_VI_StartViChn(&m_viCfg);
    if (s32Ret != CVI_SUCCESS) {
        emxlogc("StartViChn failed with %#x!\n", s32Ret);
        goto EXIT_DESTROY_ISP;
    }

    for (auto &item: json["frameRate"]) {
        ISP_PUB_ATTR_S pubAttr = {};
        s32Ret = CVI_ISP_GetPubAttr(item["pipe"].asInt(), &pubAttr);
        if (s32Ret != CVI_SUCCESS) {
            emxloge("CVI_ISP_GetPubAttr failed with %#x!\n", s32Ret);
        } else {
            pubAttr.f32FrameRate = (CVI_FLOAT) item["rate"].asInt();
            s32Ret = CVI_ISP_SetPubAttr(item["pipe"].asInt(), &pubAttr);
            if (s32Ret != CVI_SUCCESS) {
                emxloge("CVI_ISP_SetPubAttr failed with %#x!\n", s32Ret);
            }
        }
    }

    for (auto &item: json["rotate"]) {
        s32Ret = CVI_VI_SetChnFlipMirror(item["pipe"].asInt(),
                                         item["chn"].asInt(),
                                         (CVI_BOOL) item["flip"].asBool(),
                                         (CVI_BOOL) item["mirror"].asBool());
        if (s32Ret != CVI_SUCCESS) {
            emxloge("CVI_VI_SetChnFlipMirror failed with %#x!\n", s32Ret);
        }
    }

    for (auto &item: json["bind"]) {
        VI_PIPE ViPipe = item["pipe"].asInt();
        VI_CHN ViChn = item["viChn"].asInt();
        VPSS_GRP VpssGrp = item["vpssGrp"].asInt();
        s32Ret = SAMPLE_COMM_VI_Bind_VPSS(ViPipe, ViChn, VpssGrp);
        if (s32Ret != CVI_SUCCESS) {
            emxloge("SAMPLE_COMM_VI_Bind_VPSS(%d,%d,%d) failed with %#x!\n", ViPipe, ViChn, VpssGrp, s32Ret);
        }
    }
    media_isp_ProcInfo_Open(3);
    emxlogt("vi init done\n");
    return ErrCodeE::Success;
EXIT_DESTROY_ISP:
    SAMPLE_COMM_VI_DestroyIsp(&m_viCfg);
EXIT_DESTROY_PIPE:
    DestroyPipe(json["pipeAttr"]);
EXIT_DESTROY_DEV:
    for (auto &dev: vViDev) {
        if ((s32Ret = SAMPLE_COMM_VI_StopDev(&m_viCfg.astViInfo[dev])) != CVI_SUCCESS)
            emxlogw("SAMPLE_COMM_VI_StopDev failed with %#x\n", s32Ret);
    }
    return ErrCodeE::Failure;
}

void MediaChipSystem::DeInitVi(const Json::Value &json) {
    CVI_S32 s32Ret;
    for (CVI_S32 i = 0; i < m_viCfg.s32WorkingViNum; i++) {
        CVI_S32 s32ViNum = m_viCfg.as32WorkingViId[i];
        if ((s32Ret = SAMPLE_COMM_VI_StopViChn(&m_viCfg.astViInfo[s32ViNum])) != CVI_SUCCESS)
            emxlogw("SAMPLE_COMM_VI_StopViChn %d failed with %#x\n", s32ViNum, s32Ret);
    }
    SAMPLE_COMM_VI_DestroyIsp(&m_viCfg);
    DestroyPipe(json["pipeAttr"]);
    for (VI_DEV ViDev = 0; ViDev < m_viCfg.s32WorkingViNum; ViDev++) {
        s32Ret = SAMPLE_COMM_VI_StopDev(&m_viCfg.astViInfo[ViDev]);
        if (s32Ret != CVI_SUCCESS) {
            emxlogw("VI_StartDev[%d] failed with %#x\n", ViDev, s32Ret);
        }
    }
    emxlogt("vi deinit done\n");
}

//todo:rollback
ErrCodeE MediaChipSystem::InitVpss(const Json::Value &json) {
    CVI_S32 s32Ret;
    if ((VI_VPSS_MODE_E) json["ViVpssMode"].asInt() != VI_OFFLINE_VPSS_OFFLINE) {
        VPSS_MODE_S stVPSSMode = {};
        stVPSSMode.enMode = (VPSS_MODE_E) json["enMode"].asInt();
        for (int i = 0; i < VPSS_IP_NUM && i < (int) json["aenInput"].size(); i++)
            stVPSSMode.aenInput[i] = (VPSS_INPUT_E) json["aenInput"][i].asInt();

        for (int i = 0; i < VPSS_IP_NUM && i < (int) json["ViPipe"].size(); i++)
            stVPSSMode.ViPipe[i] = (VI_PIPE) json["ViPipe"][i].asInt();
        s32Ret = CVI_SYS_SetVPSSModeEx(&stVPSSMode);
        if (s32Ret != CVI_SUCCESS) {
            emxlogc("CVI_SYS_SetVPSSModeEx failed with %#x\n", s32Ret);
            return ErrCodeE::Failure;
        }
    } else {
        s32Ret = CVI_SYS_SetVPSSMode(VPSS_MODE_DUAL);
        if (s32Ret != CVI_SUCCESS) {
            emxlogc("CVI_SYS_SetVPSSMode failed with %#x\n", s32Ret);
            return ErrCodeE::Failure;
        }
    }
    for (auto &e: json["group"]) {
        if (CreateVpssGroup(e) != ErrCodeE::Success) {
            emxlogc("vpss group create failed\n");
            return ErrCodeE::Failure;
        }
    }
    for (auto &e: json["bindViVpss"]) {
        s32Ret = SAMPLE_COMM_VI_Bind_VPSS(e["pipe"].asInt(),
                                          e["chn"].asInt(),
                                          e["group"].asInt());
        if (s32Ret != CVI_SUCCESS) {
            emxloge("vi pipe %d chn %d bind vpss %d failed. s32Ret: 0x%x !\n",
                   e["pipe"].asInt(), e["chn"].asInt(), e["group"].asInt(), s32Ret);
            return ErrCodeE::Failure;
        }
    }
    for (auto &e: json["bindVpssVpss"]) {
        s32Ret = SAMPLE_COMM_VPSS_Bind_VPSS(e["srcGrp"].asInt(),
                                            e["srcChn"].asInt(),
                                            e["dstGrp"].asInt());
        if (s32Ret != CVI_SUCCESS) {
            emxloge("vpss %d chn %d bind vpss %d failed. s32Ret: 0x%x !\n",
                   e["srcGrp"].asInt(), e["srcChn"].asInt(), e["dstGrp"].asInt(), s32Ret);
            return ErrCodeE::Failure;
        }
    }
    for (auto &e: json["ScaleCoefLevel"]) {
        s32Ret = CVI_VPSS_SetChnScaleCoefLevel(e["group"].asInt(),
                                               e["chn"].asInt(),
                                               (VPSS_SCALE_COEF_E) e["scaleCoef"].asInt());
        if (s32Ret != CVI_SUCCESS) {
            emxloge("set vpss group %d chn %d scaleCoef %d failed. s32Ret: 0x%x !\n",
                   e["group"].asInt(), e["chn"].asInt(), e["scaleCoef"].asInt(), s32Ret);
            return ErrCodeE::Failure;
        }
    }
    emxlogt("vpss init done\n");
    return ErrCodeE::Success;
}

void MediaChipSystem::DeInitVpss(const Json::Value &json) {
    CVI_S32 s32Ret;
    for (auto &e: json["bindVpssVpss"]) {
        s32Ret = SAMPLE_COMM_VPSS_UnBind_VPSS(e["srcGrp"].asInt(),
                                              e["srcChn"].asInt(),
                                              e["dstGrp"].asInt());
        if (s32Ret != CVI_SUCCESS) {
            emxlogw("vpss %d chn %d unbind vpss %d failed. s32Ret: 0x%x !\n",
                   e["srcGrp"].asInt(), e["srcChn"].asInt(), e["dstGrp"].asInt(), s32Ret);
        }
    }

    for (auto &e: json["bindViVpss"]) {
        s32Ret = SAMPLE_COMM_VI_UnBind_VPSS(e["pipe"].asInt(),
                                            e["chn"].asInt(),
                                            e["group"].asInt());
        if (s32Ret != CVI_SUCCESS) {
            emxlogw("vi pipe %d chn %d unbind vpss %d failed. s32Ret: 0x%x !\n",
                   e["pipe"].asInt(), e["chn"].asInt(), e["group"].asInt(), s32Ret);
        }
    }

    for (auto &e: json["group"])
        DestroyVpssGroup(e);

    emxlogt("vpss deinit done\n");
}

ErrCodeE MediaChipSystem::SetViVpssMode(const Json::Value &json) {
    VI_VPSS_MODE_S stVIVPSSMode = {};
    if (json.size() > VI_MAX_PIPE_NUM) {
        emxlogc("ViVpssMode number:%d > VI_MAX_PIPE_NUM:%d\n", json["ViVpssMode"].size(), VI_MAX_PIPE_NUM);
        return ErrCodeE::IllegalParam;
    }
    for (int i = 0; i < (int) json.size(); i++) {
        if (i >= VI_MAX_PIPE_NUM) {
            emxlogw("ViVpssMode json array size:%d larger than VI_MAX_PIPE_NUM:%d\n",
                   (int) json.size(), VI_MAX_PIPE_NUM);
            break;
        }
        stVIVPSSMode.aenMode[i] = (VI_VPSS_MODE_E) json[i].asInt();
    }
    CVI_S32 s32Ret = CVI_SYS_SetVIVPSSMode(&stVIVPSSMode);
    if (s32Ret != CVI_SUCCESS) {
        emxlogc("CVI_SYS_SetVIVPSSMode failed with %#x\n", s32Ret);
        return ErrCodeE::Failure;
    }
    return ErrCodeE::Success;
}

ErrCodeE MediaChipSystem::CreatePipe(const Json::Value &json) {
    CVI_S32 s32Ret;
    std::vector<VI_PIPE> vPipe;//for rollback destroy of pipe, when sth wrong during process
    for (auto &pipe: json) {
        VI_PIPE ViPipe = pipe["number"].asInt();
        VI_PIPE_ATTR_S stPipeAttr = {};
        stPipeAttr.bYuvSkip = pipe["bYuvSkip"].asBool();
        stPipeAttr.u32MaxW = pipe["u32MaxW"].asInt();
        stPipeAttr.u32MaxH = pipe["u32MaxH"].asInt();
        stPipeAttr.enPixFmt = (PIXEL_FORMAT_E) pipe["enPixFmt"].asInt();
        stPipeAttr.enBitWidth = (DATA_BITWIDTH_E) pipe["bitWidth"].asInt();
        stPipeAttr.stFrameRate.s32SrcFrameRate = pipe["s32SrcFrameRate"].asInt();
        stPipeAttr.stFrameRate.s32DstFrameRate = pipe["s32DstFrameRate"].asInt();
        stPipeAttr.bNrEn = pipe["NrEn"].asBool();
        stPipeAttr.bYuvBypassPath = pipe["bYuvBypassPath"].asBool();
        stPipeAttr.enCompressMode = (COMPRESS_MODE_E) pipe["enCompressMode"].asInt();
        s32Ret = CVI_VI_CreatePipe(ViPipe, &stPipeAttr);
        if (s32Ret != CVI_SUCCESS) {
            emxloge("CVI_VI_CreatePipe[%d] failed with %#x!\n", ViPipe, s32Ret);
            goto EXIT;
        }
        s32Ret = CVI_VI_StartPipe(ViPipe);
        if (s32Ret != CVI_SUCCESS) {
            emxloge("CVI_VI_StartPipe[%d] failed with %#x!\n", ViPipe, s32Ret);
            CVI_VI_DestroyPipe(ViPipe);
            goto EXIT;
        }
        s32Ret = CVI_VI_GetPipeAttr(ViPipe, &stPipeAttr);
        if (s32Ret != CVI_SUCCESS) {
            emxloge("CVI_VI_GetPipeAttr[%d] failed with %#x!\n", ViPipe, s32Ret);
            CVI_VI_StopPipe(ViPipe);
            CVI_VI_DestroyPipe(ViPipe);
            goto EXIT;
        }
        vPipe.push_back(ViPipe);
    }
    return ErrCodeE::Success;
EXIT:
    for (auto &pipe: vPipe) {
        CVI_VI_StopPipe(pipe);
        CVI_VI_DestroyPipe(pipe);
    }
    return ErrCodeE::Failure;
}

void MediaChipSystem::DestroyPipe(const Json::Value &json) {
    for (auto &pipe: json) {
        VI_PIPE ViPipe = pipe["number"].asInt();
        CVI_VI_StopPipe(ViPipe);
        CVI_VI_DestroyPipe(ViPipe);
    }
}

//todo: rollback
ErrCodeE MediaChipSystem::CreateVpssGroup(const Json::Value &json) {
    CVI_S32 s32Ret;
    VPSS_GRP VpssGrp;
    CVI_BOOL abChnEnable[VPSS_MAX_PHY_CHN_NUM] = {};
    VPSS_GRP_ATTR_S stVpssGrpAttr = {};
    VPSS_CHN_ATTR_S astVpssChnAttr[VPSS_MAX_PHY_CHN_NUM] = {};

    VpssGrp = (VPSS_GRP) json["vpssGrp"].asInt();
    auto &groupAttr = json["attr"];
    stVpssGrpAttr.stFrameRate.s32SrcFrameRate = groupAttr["s32SrcFrameRate"].asInt();
    stVpssGrpAttr.stFrameRate.s32DstFrameRate = groupAttr["s32DstFrameRate"].asInt();
    stVpssGrpAttr.enPixelFormat = (PIXEL_FORMAT_E) groupAttr["enPixelFormat"].asInt();
    stVpssGrpAttr.u32MaxW = groupAttr["u32MaxW"].asInt();
    stVpssGrpAttr.u32MaxH = groupAttr["u32MaxH"].asInt();
    stVpssGrpAttr.u8VpssDev = groupAttr["u8VpssDev"].asInt();

    for (int i = 0; i < (int) json["chn"].size(); i++) {
        auto &chn = json["chn"][i];
        astVpssChnAttr[i].u32Width = chn["u32Width"].asInt();
        astVpssChnAttr[i].u32Height = chn["u32Height"].asInt();
        astVpssChnAttr[i].enVideoFormat = (VIDEO_FORMAT_E) chn["enVideoFormat"].asInt();
        astVpssChnAttr[i].enPixelFormat = (PIXEL_FORMAT_E) chn["enPixelFormat"].asInt();
        astVpssChnAttr[i].stFrameRate.s32SrcFrameRate = chn["s32SrcFrameRate"].asInt();
        astVpssChnAttr[i].stFrameRate.s32DstFrameRate = chn["s32DstFrameRate"].asInt();
        astVpssChnAttr[i].bMirror = (CVI_BOOL) chn["bMirror"].asBool();
        astVpssChnAttr[i].bFlip = (CVI_BOOL) chn["bFlip"].asInt();
        astVpssChnAttr[i].u32Depth = chn["u32Depth"].asInt();
        auto &normalize = chn["normalize"];
        astVpssChnAttr[i].stNormalize.bEnable = (CVI_BOOL) normalize["bEnable"].asBool();
        for (int j = 0; j < (int) normalize["factor"].size(); j++)
            astVpssChnAttr[i].stNormalize.factor[j] = normalize["factor"][j].asFloat();
        for (int j = 0; j < (int) normalize["mean"].size(); j++)
            astVpssChnAttr[i].stNormalize.mean[j] = normalize["mean"][j].asFloat();
        astVpssChnAttr[i].stNormalize.rounding = (VPSS_ROUNDING_E) normalize["rounding"].asInt();
        auto &aspect = chn["aspect"];
        astVpssChnAttr[i].stAspectRatio.enMode = (ASPECT_RATIO_E) aspect["enMode"].asInt();
        astVpssChnAttr[i].stAspectRatio.bEnableBgColor = (CVI_BOOL) aspect["bEnableBgColor"].asInt();
        astVpssChnAttr[i].stAspectRatio.u32BgColor = aspect["u32BgColor"].asInt();
        astVpssChnAttr[i].stAspectRatio.stVideoRect.s32X = aspect["s32X"].asInt();
        astVpssChnAttr[i].stAspectRatio.stVideoRect.s32Y = aspect["s32Y"].asInt();
        astVpssChnAttr[i].stAspectRatio.stVideoRect.u32Width = aspect["u32Width"].asInt();
        astVpssChnAttr[i].stAspectRatio.stVideoRect.u32Height = aspect["u32Height"].asInt();
        abChnEnable[i] = CVI_TRUE;
    }
    s32Ret = SAMPLE_COMM_VPSS_Init(VpssGrp, abChnEnable,
                                   &stVpssGrpAttr, astVpssChnAttr);
    if (s32Ret != CVI_SUCCESS) {
        emxlogc("init vpss group %d failed. s32Ret: 0x%x !\n", VpssGrp, s32Ret);
        return ErrCodeE::Failure;
    }
    for (auto &e: json["attachPool"]) {
        s32Ret = CVI_VPSS_AttachVbPool(VpssGrp, e["chn"].asInt(), e["pool"].asInt());
        if (s32Ret != CVI_SUCCESS) {
            emxlogc("attach vpss group %d chn %d pool %d failed. s32Ret: 0x%x !\n",
                   VpssGrp, e["chn"].asInt(), e["pool"].asInt(), s32Ret);
            return ErrCodeE::Failure;
        }
    }
    s32Ret = SAMPLE_COMM_VPSS_Start(VpssGrp, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
    if (s32Ret != CVI_SUCCESS) {
        emxloge("start vpss group failed. s32Ret: 0x%x !\n", s32Ret);
        return ErrCodeE::Failure;
    }
    for (auto &e: json["bind"]) {
        s32Ret = SAMPLE_COMM_VPSS_Bind_VPSS(e["grp"].asInt(), e["chn"].asInt(), VpssGrp);
        if (s32Ret != CVI_SUCCESS) {
            emxlogc("bind vpss group %d chn %d to %d failed. s32Ret: 0x%x !\n",
                   e["group"].asInt(), e["chn"].asInt(), VpssGrp, s32Ret);
            return ErrCodeE::Failure;
        }
    }
    emxlogt("vpss group %d created\n", VpssGrp);
    return ErrCodeE::Success;
}

void MediaChipSystem::DestroyVpssGroup(const Json::Value &json) {
    CVI_S32 s32Ret;
    VPSS_GRP VpssGrp;

    VpssGrp = (VPSS_GRP) json["vpssGrp"].asInt();
    for (auto &e: json["bind"]) {
        s32Ret = SAMPLE_COMM_VPSS_UnBind_VPSS(e["grp"].asInt(), e["chn"].asInt(), VpssGrp);
        if (s32Ret != CVI_SUCCESS) {
            emxlogw("unbind vpss group %d chn %d to %d failed. s32Ret: 0x%x !\n",
                   e["grp"].asInt(), e["chn"].asInt(), VpssGrp, s32Ret);
        }
    }

    CVI_BOOL abChnEnable[VPSS_MAX_PHY_CHN_NUM] = {};
    for (int i = 0; i < (int) json["chn"].size(); i++) {
        abChnEnable[i] = CVI_TRUE;
    }
    s32Ret = SAMPLE_COMM_VPSS_Stop(VpssGrp, abChnEnable);
    if (s32Ret != CVI_SUCCESS) {
        emxlogw("stop vpss group failed. s32Ret: 0x%x !\n", s32Ret);
    }

    for (auto &e: json["attachPool"]) {
        s32Ret = CVI_VPSS_DetachVbPool(VpssGrp, (VPSS_CHN) e["chn"].asInt());
        if (s32Ret != CVI_SUCCESS) {
            emxlogw("detach vpss group %d chn %d pool %d failed. s32Ret: 0x%x !\n",
                   VpssGrp, e["chn"].asInt(), e["pool"].asInt(), s32Ret);
        }
    }
    for (int i = 0; i < (int) json["chn"].size(); i++) {
        s32Ret = CVI_VPSS_DisableChn(VpssGrp, i);
        if (s32Ret != CVI_SUCCESS) {
            emxlogw("disable vpss group %d chn %d failed. s32Ret: 0x%x !\n", VpssGrp, i, s32Ret);
        }
    }

    s32Ret = CVI_VPSS_DestroyGrp(VpssGrp);
    if (s32Ret != CVI_SUCCESS) {
        emxlogw("destroy vpss group %d failed. s32Ret: 0x%x !\n", VpssGrp, s32Ret);
    }
}
