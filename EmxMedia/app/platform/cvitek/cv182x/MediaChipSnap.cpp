//
// Created by xiong on 2021/5/28.
//
#include "sample_comm.h"
#include "core/EmxLog.hpp"
#include "MediaChipSnap.hpp"
#include "MediaConfigs.hpp"
#include "MediaInner.hpp"

using namespace Emx;

ErrCodeE MediaChipSnap::Start() {
    if (m_started)
        return ErrCodeE::Success;
    CVI_S32 s32Ret;
    Json::Value root;
    m_res.configs.GetCfg("snap", root);
    auto &json = root[m_chn];
    m_vpssGrp = json["VpssGrp"].asInt();
    m_vpssChn = json["VpssChn"].asInt();
    m_vencChn = json["VencChn"].asInt();
    m_bind = json["bind"].asBool();
    VPSS_CHN_ATTR_S stVpssAttr = {};
    CVI_VPSS_GetChnAttr(m_vpssGrp, m_vpssChn, &stVpssAttr);
    VENC_CHN_ATTR_S stVencChnAttr = {};

    stVencChnAttr.stVencAttr.enType = PT_JPEG;
    stVencChnAttr.stVencAttr.u32MaxPicWidth = stVpssAttr.u32Width;
    stVencChnAttr.stVencAttr.u32MaxPicHeight = stVpssAttr.u32Height;
    stVencChnAttr.stVencAttr.u32PicWidth = json["width"].asInt();
    stVencChnAttr.stVencAttr.u32PicHeight = json["height"].asInt();
    stVencChnAttr.stVencAttr.u32BufSize = 0;
    stVencChnAttr.stVencAttr.bSingleCore = CVI_FALSE;
    stVencChnAttr.stVencAttr.u32Profile = 0;
    stVencChnAttr.stVencAttr.bByFrame = CVI_TRUE;
    stVencChnAttr.stGopAttr.enGopMode = VENC_GOPMODE_NORMALP;
    stVencChnAttr.stGopAttr.stNormalP.s32IPQpDelta = 0;

    VENC_ATTR_JPEG_S *pstJpegAttr = &stVencChnAttr.stVencAttr.stAttrJpege;
    pstJpegAttr->bSupportDCF = CVI_FALSE;
    pstJpegAttr->stMPFCfg.u8LargeThumbNailNum = 0;
    pstJpegAttr->enReceiveMode = VENC_PIC_RECEIVE_SINGLE;

    s32Ret = CVI_VENC_CreateChn(m_vencChn, &stVencChnAttr);
    if (s32Ret != CVI_SUCCESS) {
        emxloge("chn %d CVI_VENC_CreateChn [%d] failed with %#x\n", m_chn, m_vencChn, s32Ret);
        return ErrCodeE::Failure;
    }

    VENC_JPEG_PARAM_S stParamJpeg = {};
    s32Ret = CVI_VENC_GetJpegParam(m_vencChn, &stParamJpeg);
    if (CVI_SUCCESS != s32Ret) {
        emxloge("chn %d CVI_VENC_GetJpegParam [%d] failed with %#x\n", m_chn, m_vencChn, s32Ret);
    }
    stParamJpeg.u32Qfactor = (CVI_U32) json["u32Qfactor"].asInt();
    s32Ret = CVI_VENC_SetJpegParam(m_vencChn, &stParamJpeg);
    if (CVI_SUCCESS != s32Ret) {
        emxloge("chn %d CVI_VENC_SetJpegParam [%d] failed with %#x\n", m_chn, m_vencChn, s32Ret);
    }

    VENC_RECV_PIC_PARAM_S stRecvParam = {};
    stRecvParam.s32RecvPicNum = -1;
    s32Ret = CVI_VENC_StartRecvFrame(m_vencChn, &stRecvParam);
    if (CVI_SUCCESS != s32Ret) {
        emxloge("chn %d CVI_VENC_StartRecvFrame [%d] failed with %#x\n", m_chn, m_vencChn, s32Ret);
        CVI_VENC_DestroyChn(m_vencChn);
        return ErrCodeE::Failure;
    }
    m_work.Create(
            &m_res.loop, nullptr,
            [this](void *arg) { OnTrigger(arg); },
            [this](ErrCodeE e, void *arg) { OnTriggerComplete(e, arg); });
    m_started = true;
    m_isSnapping = false;
    emxlogi("chn %d done\n", m_chn);
    return ErrCodeE::Success;
}

ErrCodeE MediaChipSnap::Stop() {
    if (!m_started)
        return ErrCodeE::Success;
    m_work.Destroy();
    m_started = false;
    SAMPLE_COMM_VENC_Stop(m_vencChn);
    emxlogi("chn %d done\n", m_chn);
    return ErrCodeE::Success;
}

ErrCodeE MediaChipSnap::Trigger() {
    if (m_isSnapping) return ErrCodeE::Success;
    m_isSnapping = true;
    m_work.Run();
    return ErrCodeE::Success;
}

void MediaChipSnap::OnTrigger(void *arg) {
    if (Vpss2Venc() == ErrCodeE::Success)
        Venc2User(m_stream);
}

void MediaChipSnap::OnTriggerComplete(ErrCodeE e, void *arg) {
    if (m_stream.pstPack) {
        PushFrame(m_stream);
        CVI_VENC_ReleaseStream(m_vencChn, &m_stream);
        free(m_stream.pstPack);
    }
    m_isSnapping = false;
}


ErrCodeE MediaChipSnap::Vpss2Venc() {
    VPSS_CHN_ATTR_S stChnAttr = {};
    VIDEO_FRAME_INFO_S frame = {};
    CVI_S32 s32Ret;

    s32Ret = CVI_VPSS_GetChnAttr(m_vpssGrp, m_vpssChn, &stChnAttr);
    if (CVI_SUCCESS != s32Ret) {
        emxloge("chn %d CVI_VPSS_GetChnAttr(%d,%d) failed with #%x!\n", m_chn, m_vpssGrp, m_vpssChn, s32Ret);
        return ErrCodeE::Failure;
    }

    s32Ret = CVI_VPSS_GetChnFrame(m_vpssGrp, m_vpssChn, &frame, 1000);
    if (s32Ret != CVI_SUCCESS) {
        emxloge("chn %d CVI_VPSS_GetChnFrame(%d,%d) failed with #%x!\n", m_chn, m_vpssGrp, m_vpssChn, s32Ret);
        return ErrCodeE::Failure;
    }

    if (stChnAttr.u32Height != frame.stVFrame.u32Height) {
        emxlogw("chn %d vpss chn height %d != act height %d\n",
               m_chn, stChnAttr.u32Height, frame.stVFrame.u32Height);
        CVI_VPSS_ReleaseChnFrame(m_vpssGrp, m_vpssChn, &frame);
        return ErrCodeE::Failure;
    }

    s32Ret = CVI_VENC_SendFrame(m_vencChn, &frame, 1000);
    if (s32Ret != CVI_SUCCESS) {
        emxloge("chn %d CVI_VENC_SendFrame failed with #%x!\n", m_chn, s32Ret);
        CVI_VPSS_ReleaseChnFrame(m_vpssGrp, m_vpssChn, &frame);
        return ErrCodeE::Failure;
    }

    s32Ret = CVI_VPSS_ReleaseChnFrame(m_vpssGrp, m_vpssChn, &frame);
    if (s32Ret != CVI_SUCCESS) {
        emxloge("chn %d CVI_VPSS_ReleaseChnFrame failed with #%x!\n", m_chn, s32Ret);
        return ErrCodeE::Failure;
    }
    return ErrCodeE::Success;
}


ErrCodeE MediaChipSnap::Venc2User(VENC_STREAM_S &stream) {
    CVI_S32 s32Ret;
    VENC_CHN_STATUS_S stStat = {};

    s32Ret = CVI_VENC_QueryStatus(m_vencChn, &stStat);
    if (CVI_SUCCESS != s32Ret) {
        emxloge("chn %d CVI_VENC_QueryStatus failed with %#x!\n", m_chn, s32Ret);
        return ErrCodeE::Failure;
    }

    if (0 == stStat.u32CurPacks) {
        emxloge("chn %d NOTE: Current  frame is NULL!\n", m_chn);
        return ErrCodeE::Failure;
    }

    stream.pstPack = (VENC_PACK_S *) malloc(sizeof(VENC_PACK_S) * stStat.u32CurPacks);
    if (nullptr == stream.pstPack) {
        emxloge("chn %d malloc stream pack failed!\n", m_chn);
        return ErrCodeE::Failure;
    }

    s32Ret = CVI_VENC_GetStream(m_vencChn, &stream, 1000);
    if (CVI_SUCCESS != s32Ret) {
        free(stream.pstPack);
        stream.pstPack = nullptr;
        emxloge("chn %d HI_MPI_VENC_GetStream failed with %#x!\n", m_chn, s32Ret);
        return ErrCodeE::Failure;
    }
    return ErrCodeE::Success;
}

ErrCodeE MediaChipSnap::PushFrame(VENC_STREAM_S &stream) {
    unsigned int len = 0;

    for (uint32_t i = 0; i < stream.u32PackCount; i++) {
        len += stream.pstPack[i].u32Len - stream.pstPack[i].u32Offset;
    }

    if (0 == len) {
        emxloge("zero length\n");
        return ErrCodeE::Failure;
    }

    char *data;
    MediaFrame frame = {};
    frame.type = MediaFrame::TypeE::Snap;
    frame.chn = m_chn;
    frame.tsInUs = stream.pstPack[0].u64PTS;
    frame.size = (int) len;
    frame.off = m_res.ring.GetFreeBuffer((int) len, frame.tsInUs, &data);
    if (frame.off < 0) {
        emxloge("get free buffer failed\n");
        return ErrCodeE::Failure;
    }
    uint32_t off = 0;
    for (uint32_t i = 0; i < stream.u32PackCount; i++) {
        memcpy(&data[off], stream.pstPack[i].pu8Addr + stream.pstPack[i].u32Offset,
               stream.pstPack[i].u32Len - stream.pstPack[i].u32Offset);
        off += stream.pstPack[i].u32Len - stream.pstPack[i].u32Offset;
    }

    m_res.pub.Publish(MEDIA_TOPIC_SNAP + m_chn, (char *) &frame, sizeof(frame));
    return ErrCodeE::Success;
}

