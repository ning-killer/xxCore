#include <core/EmxLog.hpp>
#include <core/EmxParam.hpp>
#include "MediaChipDouLensSystem.hpp"
using namespace Emx;

ErrCodeE MediaChipDouLensSystem::Start() {

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
        emxlogc("Init Sensor failed : %d\n", e);
        return e;
    }

    if (json["vpss"].type() == Json::nullValue) {
        StartPost(json);
        m_started = true;
        emxlogi("normal start system done\n");
        return ErrCodeE::Success;
    }

    e = InitVpss(json["vpss"]);
    if (e != ErrCodeE::Success) {
        DeInitVi(json["vi"]);
        DeInitVb(json["vb"]);
        emxlogc("InitVpss failed : %d\n", e);
        return e;
    }

    e = InitExpandMode(json["expandMode"]);
    if (e != ErrCodeE::Success) {
        DeInitVi(json["vi"]);
        DeInitVb(json["vb"]);
        DeInitVpss(json["vpss"]);
        emxlogc("InitExpandMode failed : %d\n", e);
        return e;
    }
    StartPost(json);
    m_started = true;
    emxlogi("Start system done\n");
    return ErrCodeE::Success;
}

ErrCodeE MediaChipDouLensSystem::Stop() {
    if (!m_started)
        return ErrCodeE::Success;
    m_started = false;
    Json::Value json;
    m_res.configs.GetCfg("system", json);
    StopPre(json);
    DeInitVpss(json["vpss"]);
    DeInitVi(json["vi"]);
    DeInitVb(json["vb"]);
    DeInitExpandMode(json["expandMode"]);
    StopPost(json);
    emxlogi("Stop system done\n");
    return ErrCodeE::Success;
}

ErrCodeE MediaChipDouLensSystem::CreateVpssGroup(const Json::Value &json)
{
    CVI_S32 s32Ret;
    VPSS_GRP VpssGrp;
    VPSS_GRP_ATTR_S stVpssGrpAttr = {};
    CVI_BOOL abChnEnable[VPSS_MAX_PHY_CHN_NUM] = {};
    VPSS_CHN_ATTR_S astVpssChnAttr[VPSS_MAX_PHY_CHN_NUM] = {};

    VpssGrp = (VPSS_GRP) json["vpssGrp"].asInt();


    auto &groupAttr = json["attr"];
    stVpssGrpAttr.stFrameRate.s32SrcFrameRate = groupAttr["s32SrcFrameRate"].asInt();
    stVpssGrpAttr.stFrameRate.s32DstFrameRate = groupAttr["s32DstFrameRate"].asInt();
    stVpssGrpAttr.enPixelFormat = (PIXEL_FORMAT_E) groupAttr["enPixelFormat"].asInt();
    if (groupAttr["u32MaxW"].asInt() == 0 || groupAttr["u32MaxH"].asInt()) {
        Json::Value jsonMode;
        Param param("media/stitchMode");
        param.Get(jsonMode);
        for (auto &jsonAttr: jsonMode["attr"]) {
            if (jsonAttr["mode"].asInt() == jsonMode["mode"].asInt()) {
                stVpssGrpAttr.u32MaxW = jsonAttr["u32MaxW"].asInt();
                stVpssGrpAttr.u32MaxH = jsonAttr["u32MaxH"].asInt();
            }
        }
    } else {
        stVpssGrpAttr.u32MaxW = groupAttr["u32MaxW"].asInt();
        stVpssGrpAttr.u32MaxH = groupAttr["u32MaxH"].asInt();
    }
    stVpssGrpAttr.u8VpssDev = groupAttr["u8VpssDev"].asInt();

    for (int i = 0; i < (int) json["chn"].size(); i++) {
        auto &chn = json["chn"][i];
        if (chn["u32Width"].asInt() == 0 || chn["u32Height"].asInt() == 0) {
            Json::Value jsonMode;
            Param param("media/stitchMode");
            param.Get(jsonMode);
            for (auto &jsonAttr: jsonMode["attr"]) {
                if (jsonAttr["mode"].asInt() == jsonMode["mode"].asInt()) {
                    astVpssChnAttr[i].u32Width = jsonAttr["u32MaxW"].asInt();
                    astVpssChnAttr[i].u32Height = jsonAttr["u32MaxH"].asInt();
                }
            }
        } else {
            astVpssChnAttr[i].u32Width = chn["u32Width"].asInt();
            astVpssChnAttr[i].u32Height = chn["u32Height"].asInt();
        }
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

    s32Ret = SAMPLE_COMM_VPSS_Init(VpssGrp, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
    if (s32Ret != CVI_SUCCESS) {
        emxloge("init vpss group failed. s32Ret: 0x%x !\n", s32Ret);
        return ErrCodeE::Failure;;
    }

    for (auto &e: json["attachPool"]) {
        s32Ret = CVI_VPSS_AttachVbPool(VpssGrp, e["chn"].asInt(), e["pool"].asInt());
        if (s32Ret != CVI_SUCCESS) {
            emxloge("attach vpss group %d chn %d pool %d failed. s32Ret: 0x%x !\n",
                    VpssGrp, e["chn"].asInt(), e["pool"].asInt(), s32Ret);
            return ErrCodeE::Failure;
        }
    }

    s32Ret = SAMPLE_COMM_VPSS_Start(VpssGrp, abChnEnable, &stVpssGrpAttr, astVpssChnAttr);
    if (s32Ret != CVI_SUCCESS) {
        emxloge("start vpss group failed. s32Ret: 0x%x !\n", s32Ret);
        return ErrCodeE::Failure;;
    }

    emxlogt("vpss group %d created\n", VpssGrp);
    return ErrCodeE::Success;
}

ErrCodeE MediaChipDouLensSystem::InitVb(const Json::Value &json) {

    VB_CONFIG_S stVbConf = {};
    stVbConf.u32MaxPoolCnt = json.size();
    int width = 0,height = 0;

    for (uint32_t i = 0; i < stVbConf.u32MaxPoolCnt; i++) {
        if (json[i]["width"].asInt() == 0|| json[i]["height"].asInt() == 0){
            Json::Value jsonMode;
            Param param("media/stitchMode");
            param.Get(jsonMode);
            for (auto &jsonAttr: jsonMode["attr"]) {
                if (jsonAttr["mode"].asInt() == jsonMode["mode"].asInt()) {
                    width = jsonAttr["u32MaxW"].asInt();
                    height = jsonAttr["u32MaxH"].asInt();
                }
            }
        }
        else{
            width = json[i]["width"].asInt();
            height = json[i]["height"].asInt();
        }

        CVI_U32 u32BlkSize = COMMON_GetPicBufferSize(width,height,
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
