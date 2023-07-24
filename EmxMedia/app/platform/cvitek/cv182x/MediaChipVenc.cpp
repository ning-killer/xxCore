//
// Created by xiong on 2020/10/27.
//
#include "MediaChipVenc.hpp"
#include "DrawRect.hpp"
#include "MediaInner.hpp"
#include "core/EmxLog.hpp"
#include "MediaConfigs.hpp"
#include "sample_comm.h"
#include "MediaChipOSD.hpp"

using namespace Emx;

ErrCodeE MediaChipVenc::Start() {
    if (m_started) return ErrCodeE::Success;
    if (LoadParam() != ErrCodeE::Success) {
        return ErrCodeE::Failure;
    }
    StartPre();
    if (Config() != ErrCodeE::Success) {
        return ErrCodeE::Failure;
    }
    StartPost();
    StartStream();
    m_paramChange = true;
    m_started = true;
    emxlogi("chn %d done\n", m_chn);
    return ErrCodeE::Success;
}

ErrCodeE MediaChipVenc::Stop() {
    if (!m_started) return ErrCodeE::Success;
    m_started = false;
    StopStream();
    StopPre();
    SAMPLE_COMM_VENC_Stop(m_vencChn);
    StopPost();
    emxlogd("chn %d done\n", m_chn);
    return ErrCodeE::Success;
}

ErrCodeE MediaChipVenc::Config() {
    cviGetMask();
    Json::Value root;
    m_res.configs.GetCfg("venc", root);
    auto &json = root[m_chn]["attr"];
    m_pipe = (VI_PIPE) json["ViPipe"].asInt();
    m_vpssGrp = (VPSS_GRP) json["VpssGrp"].asInt();
    m_vpssChn = (VPSS_CHN) json["VpssChn"].asInt();
    m_vencChn = (VENC_CHN) json["VencChn"].asInt();
    m_bind = json["bind"].asBool();

    if (SetSingleESBuffer(json["modSizeKb"].asInt() * 1024) != ErrCodeE::Success)
        emxloge("SetSingleESBuffer error\n");

    if (SetVpssChnAlignment() != ErrCodeE::Success)
        return ErrCodeE::Failure;

    if (CreateChn(json) != ErrCodeE::Success)
        return ErrCodeE::Failure;

    if (SetRcParam(json) != ErrCodeE::Success) {
        CVI_VENC_DestroyChn(m_vencChn);
        return ErrCodeE::Failure;
    }

    if (SetRefParam(json) != ErrCodeE::Success) {
        CVI_VENC_DestroyChn(m_vencChn);
        return ErrCodeE::Failure;
    }

    if (AttachVbPool(json) != ErrCodeE::Success) {
        CVI_VENC_DestroyChn(m_vencChn);
        return ErrCodeE::Failure;
    }
    if (SetVideoCodecParam(json) != ErrCodeE::Success) {
        CVI_VENC_DestroyChn(m_vencChn);
        return ErrCodeE::Failure;
    }

    CVI_S32 s32Ret;
    VENC_RECV_PIC_PARAM_S stRecvParam = {};
    stRecvParam.s32RecvPicNum = -1;
    s32Ret = CVI_VENC_StartRecvFrame(m_vencChn, &stRecvParam);
    if (CVI_SUCCESS != s32Ret) {
        emxloge("chn %d CVI_VENC_StartRecvFrame failed with %#x!\n", m_chn, s32Ret);
        CVI_VENC_DestroyChn(m_vencChn);
        return ErrCodeE::Failure;
    }
    return ErrCodeE::Success;
}

ErrCodeE MediaChipVenc::FlushIDR() {
    CVI_VENC_RequestIDR(m_vencChn, CVI_TRUE);
    return ErrCodeE::Success;
}

ErrCodeE MediaChipVenc::SetParam(const MediaVenc::Param &param) {
    if (!ParamChanged(param, m_param)) {
        return ErrCodeE::Success;
    }
    if (!ParamValid(param)) {
        return ErrCodeE::IllegalParam;
    }
    Stop();
    memcpy(&m_param, &param, sizeof(MediaVenc::Param));
    SaveParam();
    return Start();
}

ErrCodeE MediaChipVenc::GetParam(MediaVenc::Param &param) {
    memcpy(&param, &m_param, sizeof(MediaVenc::Param));
    return ErrCodeE::Success;
}

void MediaChipVenc::StartStream() {
    //vpss fd不能被poll，非绑定模式下venc fd也不能被poll
    m_work.Create(
            &m_res.loop, nullptr,
            [this](void *arg) { OnStream(arg); },
            [this](ErrCodeE e, void *arg) { OnStreamComplete(e, arg); });
    m_work.Run();
}

void MediaChipVenc::StopStream() {
    m_work.Destroy();
}

void MediaChipVenc::OnStream(void *arg) {
    if (Vpss2Venc() == ErrCodeE::Success)
        Venc2User();
}

void MediaChipVenc::OnStreamComplete(ErrCodeE e, void *arg) {
    if (m_stream.pstPack) {
        PushFrame();
        CVI_VENC_ReleaseStream(m_vencChn, &m_stream);
        free(m_stream.pstPack);
        m_stream.pstPack = nullptr;
    }
    if (e == ErrCodeE::Success) {
        m_work.Run();
    }
}

ErrCodeE MediaChipVenc::Vpss2Venc() {
    VPSS_CHN_ATTR_S stChnAttr = {};
    VIDEO_FRAME_INFO_S frame = {};
    CVI_S32 s32Ret;

    memset(&m_stream, 0, sizeof(m_stream));

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

    if (m_param.drawRect)
        DrawRectByJson(frame.stVFrame);

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


ErrCodeE MediaChipVenc::Venc2User() {
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

    m_stream.pstPack = (VENC_PACK_S *) malloc(sizeof(VENC_PACK_S) * stStat.u32CurPacks);
    if (nullptr == m_stream.pstPack) {
        emxloge("chn %d malloc stream pack failed!\n", m_chn);
        return ErrCodeE::Failure;
    }

    s32Ret = CVI_VENC_GetStream(m_vencChn, &m_stream, 1000);
    if (CVI_SUCCESS != s32Ret) {
        free(m_stream.pstPack);
        m_stream.pstPack = nullptr;
        emxloge("chn %d HI_MPI_VENC_GetStream failed with %#x!\n", m_chn, s32Ret);
        return ErrCodeE::Failure;
    }
    return ErrCodeE::Success;
}

void MediaChipVenc::PushFrame() {
    unsigned char naltype;
    unsigned int len = 0;
    for (uint32_t i = 0; i < m_stream.u32PackCount; i++) {
        len += m_stream.pstPack[i].u32Len - m_stream.pstPack[i].u32Offset;
    }
    char *data;
    MediaFrame frame = {};
    frame.type = MediaFrame::TypeE::Venc;
    frame.chn = m_chn;
    frame.tsInUs = m_stream.pstPack[0].u64PTS;
    frame.trace = Time::GetMonoClockUs();
    frame.seq = m_seq++;
    frame.isKeyFrame = false;
    frame.size = (int) len;
    frame.off = m_res.ring.GetFreeBuffer((int) len, frame.tsInUs, &data);
    if (frame.off < 0) {
        return;
    }
    uint32_t off = 0;
    for (uint32_t i = 0; i < m_stream.u32PackCount; i++) {
        memcpy(&data[off], m_stream.pstPack[i].pu8Addr + m_stream.pstPack[i].u32Offset,
               m_stream.pstPack[i].u32Len - m_stream.pstPack[i].u32Offset);
        naltype = data[off + 4];
        /*vps sps pps sei IDR*/
        if ((naltype == 0x67) || (naltype == 0x40)) {
            frame.isKeyFrame = true;
        }
        off += m_stream.pstPack[i].u32Len - m_stream.pstPack[i].u32Offset;
    }
    frame.paramChange = m_paramChange;
    if (m_paramChange) {
        m_paramChange = false;
    }
    m_res.pub.Publish(MEDIA_TOPIC_VENC_FRAME + m_chn, (char *) &frame, sizeof(frame));
}

ErrCodeE MediaChipVenc::CreateChn(Json::Value &json) {
    //    printf("%s:%d:codec=%d\n", __FUNCTION__, __LINE__, (int) m_param.codec);
    //    printf("%s:%d:width=%d\n", __FUNCTION__, __LINE__, (int) m_param.width);
    //    printf("%s:%d:height=%d\n", __FUNCTION__, __LINE__, (int) m_param.height);
    //    printf("%s:%d:fps=%d\n", __FUNCTION__, __LINE__, (int) m_param.fps);
    //    printf("%s:%d:iFrameInv=%d\n", __FUNCTION__, __LINE__, (int) m_param.iFrameInv);
    //    printf("%s:%d:bitRate=%d\n", __FUNCTION__, __LINE__, (int) m_param.bitRate);
    //    printf("%s:%d:rcMode=%d\n", __FUNCTION__, __LINE__, (int) m_param.rcMode);
    //    printf("%s:%d:flip=%d\n", __FUNCTION__, __LINE__, (int) m_param.flip);
    //    printf("%s:%d:mirror=%d\n", __FUNCTION__, __LINE__, (int) m_param.mirror);
    //    printf("%s:%d:drawRect=%d\n", __FUNCTION__, __LINE__, (int) m_param.drawRect);

    CVI_S32 s32Ret;
    VPSS_CHN_ATTR_S stChnAttr = {};
    s32Ret = CVI_VPSS_GetChnAttr(m_vpssGrp, m_vpssChn, &stChnAttr);
    if (CVI_SUCCESS != s32Ret) {
        emxloge("chn %d CVI_VPSS_GetChnAttr failed with #%x!\n", m_chn, s32Ret);
        return ErrCodeE::Failure;
    }

    VENC_CHN_ATTR_S stVencChnAttr = {};

    stVencChnAttr.stVencAttr.u32MaxPicWidth = stChnAttr.u32Width;
    stVencChnAttr.stVencAttr.u32MaxPicHeight = stChnAttr.u32Height;
    stVencChnAttr.stVencAttr.u32PicWidth = m_param.width;             /*the picture width*/
    stVencChnAttr.stVencAttr.u32PicHeight = m_param.height;           /*the picture height*/
    stVencChnAttr.stVencAttr.u32BufSize = json["u32BufSize"].asInt(); /*stream buffer size*/
    stVencChnAttr.stVencAttr.bSingleCore = json["bSingleCore"].asBool();
    stVencChnAttr.stVencAttr.bByFrame = json["bByFrame"].asBool(); /*get stream mode is slice mode or frame mode?*/
    auto enGopMode = (VENC_GOP_MODE_E) json["gopMode"].asInt();
    s32Ret = SAMPLE_COMM_VENC_GetGopAttr(enGopMode, &stVencChnAttr.stGopAttr);
    if (s32Ret != CVI_SUCCESS) {
        emxloge("chn %d SAMPLE_COMM_VENC_GetGopAttr err for %#x!\n", m_chn, s32Ret);
        return ErrCodeE::Failure;
    }
    CVI_U32 u32StatTime = -1;
    if (enGopMode == VENC_GOPMODE_NORMALP) {
        stVencChnAttr.stGopAttr.stNormalP.s32IPQpDelta = json["s32IPQpDelta"].asInt();
        u32StatTime = 2;
    } else if (enGopMode == VENC_GOPMODE_SMARTP) {
        stVencChnAttr.stGopAttr.stSmartP.u32BgInterval = json["u32BgInterval"].asInt();
        u32StatTime = stVencChnAttr.stGopAttr.stSmartP.u32BgInterval / (m_param.iFrameInv * m_param.fps);
    }
    int srcFps;
    ISP_PUB_ATTR_S pubAttr = {};
    s32Ret = CVI_ISP_GetPubAttr(m_pipe, &pubAttr);
    if (s32Ret == CVI_SUCCESS) {
        srcFps = (int) pubAttr.f32FrameRate;
    } else {
        srcFps = json["defFps"].asInt();
        emxloge("chn %d CVI_ISP_GetPubAttr %d failed with %#x!\n", m_chn, m_pipe, s32Ret);
    }

    switch (m_param.codec) {
        case VideoCodecE::H264: {
            stVencChnAttr.stVencAttr.u32Profile = 0;
            stVencChnAttr.stVencAttr.enType = PT_H264;
            if (RcModeE::CBR == m_param.rcMode) {
                VENC_H264_CBR_S *pstH264Cbr = &stVencChnAttr.stRcAttr.stH264Cbr;
                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264CBR;
                pstH264Cbr->u32Gop = m_param.iFrameInv * m_param.fps; /*the interval of IFrame*/
                pstH264Cbr->u32StatTime = u32StatTime;                /* stream rate statics time(s) */
                pstH264Cbr->u32SrcFrameRate = srcFps;                 /* input (vi) frame rate */
                pstH264Cbr->fr32DstFrameRate = m_param.fps;           /* target frame rate */
                pstH264Cbr->u32BitRate = m_param.bitRate;
                pstH264Cbr->bVariFpsEn = CVI_FALSE;
            } else if (RcModeE::VBR == m_param.rcMode) {
                VENC_H264_VBR_S *pstH264Vbr = &stVencChnAttr.stRcAttr.stH264Vbr;
                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264VBR;
                pstH264Vbr->u32Gop = m_param.iFrameInv * m_param.fps;
                pstH264Vbr->u32StatTime = u32StatTime;
                pstH264Vbr->u32SrcFrameRate = srcFps;
                pstH264Vbr->fr32DstFrameRate = m_param.fps;
                pstH264Vbr->u32MaxBitRate = m_param.bitRate * 5 / 4;
                pstH264Vbr->bVariFpsEn = CVI_FALSE;
            } else if (RcModeE::AVBR == m_param.rcMode) {
                VENC_H264_AVBR_S *pstH264AVbr = &stVencChnAttr.stRcAttr.stH264AVbr;
                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264AVBR;
                pstH264AVbr->u32Gop = m_param.iFrameInv * m_param.fps;
                pstH264AVbr->u32StatTime = u32StatTime;
                pstH264AVbr->u32SrcFrameRate = srcFps;
                pstH264AVbr->fr32DstFrameRate = m_param.fps;
                pstH264AVbr->u32MaxBitRate = m_param.bitRate * 5 / 4;
                pstH264AVbr->bVariFpsEn = CVI_FALSE;
            } else {
                emxloge("chn %d unknown rc mode %d\n", m_chn, (int) m_param.rcMode);
                return ErrCodeE::Failure;
            }
            stVencChnAttr.stVencAttr.stAttrH264e.bRcnRefShareBuf = json["bRcnRefShareBuf"].asBool();
            stVencChnAttr.stVencAttr.stAttrH264e.bSingleLumaBuf = CVI_FALSE;
            break;
        }
        case VideoCodecE::H265: {
            stVencChnAttr.stVencAttr.u32Profile = 0;
            stVencChnAttr.stVencAttr.enType = PT_H265;
            if (RcModeE::CBR == m_param.rcMode) {
                VENC_H265_CBR_S *pstH265Cbr = &stVencChnAttr.stRcAttr.stH265Cbr;
                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265CBR;
                pstH265Cbr->u32Gop = m_param.iFrameInv * m_param.fps;
                pstH265Cbr->u32StatTime = u32StatTime;      /* stream rate statics time(s) */
                pstH265Cbr->u32SrcFrameRate = srcFps;       /* input (vi) frame rate */
                pstH265Cbr->fr32DstFrameRate = m_param.fps; /* target frame rate */
                pstH265Cbr->u32BitRate = m_param.bitRate;
                pstH265Cbr->bVariFpsEn = CVI_FALSE;
            } else if (RcModeE::VBR == m_param.rcMode) {
                VENC_H265_VBR_S *pstH265Vbr = &stVencChnAttr.stRcAttr.stH265Vbr;
                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265VBR;
                pstH265Vbr->u32Gop = m_param.iFrameInv * m_param.fps;
                pstH265Vbr->u32StatTime = u32StatTime;      /* stream rate statics time(s) */
                pstH265Vbr->u32SrcFrameRate = srcFps;       /* input (vi) frame rate */
                pstH265Vbr->fr32DstFrameRate = m_param.fps; /* target frame rate */
                pstH265Vbr->u32MaxBitRate = m_param.bitRate * 5 / 4;
                pstH265Vbr->bVariFpsEn = CVI_FALSE;
            } else if (RcModeE::AVBR == m_param.rcMode) {
                VENC_H265_AVBR_S *pstH265AVbr = &stVencChnAttr.stRcAttr.stH265AVbr;
                stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265AVBR;
                pstH265AVbr->u32Gop = m_param.iFrameInv * m_param.fps;
                pstH265AVbr->u32StatTime = u32StatTime;      /* stream rate statics time(s) */
                pstH265AVbr->u32SrcFrameRate = srcFps;       /* input (vi) frame rate */
                pstH265AVbr->fr32DstFrameRate = m_param.fps; /* target frame rate */
                pstH265AVbr->u32MaxBitRate = m_param.bitRate * 5 / 4;
                pstH265AVbr->bVariFpsEn = CVI_FALSE;
            } else {
                emxloge("chn %d unknown rc mode %d\n", m_chn, (int) m_param.rcMode);
                return ErrCodeE::Failure;
            }
            stVencChnAttr.stVencAttr.stAttrH265e.bRcnRefShareBuf = json["bRcnRefShareBuf"].asBool();
            break;
        }
        default:
            emxloge("chn %d codec %d not support\n", m_chn, m_param.codec);
            return ErrCodeE::IllegalParam;
    }

//    printf("%s:%d-stVencChnAttr.stVencAttr.enType=%d\n", __FUNCTION__, __LINE__, stVencChnAttr.stVencAttr.enType);
//    printf("%s:%d-stVencChnAttr.stVencAttr.u32MaxPicWidth=%d\n", __FUNCTION__, __LINE__,
//           stVencChnAttr.stVencAttr.u32MaxPicWidth);
//    printf("%s:%d-stVencChnAttr.stVencAttr.u32MaxPicHeight=%d\n", __FUNCTION__, __LINE__,
//           stVencChnAttr.stVencAttr.u32MaxPicHeight);
//    printf("%s:%d-stVencChnAttr.stVencAttr.u32BufSize=%d\n", __FUNCTION__, __LINE__,
//           stVencChnAttr.stVencAttr.u32BufSize);
//    printf("%s:%d-stVencChnAttr.stVencAttr.u32Profile=%d\n", __FUNCTION__, __LINE__,
//           stVencChnAttr.stVencAttr.u32Profile);
//    printf("%s:%d-stVencChnAttr.stVencAttr.bByFrame=%d\n", __FUNCTION__, __LINE__, stVencChnAttr.stVencAttr.bByFrame);
//    printf("%s:%d-stVencChnAttr.stVencAttr.u32PicWidth=%d\n", __FUNCTION__, __LINE__,
//           stVencChnAttr.stVencAttr.u32PicWidth);
//    printf("%s:%d-stVencChnAttr.stVencAttr.u32PicHeight=%d\n", __FUNCTION__, __LINE__,
//           stVencChnAttr.stVencAttr.u32PicHeight);
//    printf("%s:%d-stVencChnAttr.stVencAttr.bSingleCore=%d\n", __FUNCTION__, __LINE__,
//           stVencChnAttr.stVencAttr.bSingleCore);
//    printf("%s:%d-stVencChnAttr.stVencAttr.stAttrH265e.bRcnRefShareBuf=%d\n", __FUNCTION__, __LINE__,
//           stVencChnAttr.stVencAttr.stAttrH265e.bRcnRefShareBuf);
//
//    printf("%s:%d-stVencChnAttr.stRcAttr.enRcMode=%d\n", __FUNCTION__, __LINE__, stVencChnAttr.stRcAttr.enRcMode);
//    printf("%s:%d-stVencChnAttr.stRcAttr.stH265AVbr.u32Gop=%d\n", __FUNCTION__, __LINE__,
//           stVencChnAttr.stRcAttr.stH265AVbr.u32Gop);
//    printf("%s:%d-stVencChnAttr.stRcAttr.stH265AVbr.u32StatTime=%d\n", __FUNCTION__, __LINE__,
//           stVencChnAttr.stRcAttr.stH265AVbr.u32StatTime);
//    printf("%s:%d-stVencChnAttr.stRcAttr.stH265AVbr.u32SrcFrameRate=%d\n", __FUNCTION__, __LINE__,
//           stVencChnAttr.stRcAttr.stH265AVbr.u32SrcFrameRate);
//    printf("%s:%d-stVencChnAttr.stRcAttr.stH265AVbr.fr32DstFrameRate=%d\n", __FUNCTION__, __LINE__,
//           stVencChnAttr.stRcAttr.stH265AVbr.fr32DstFrameRate);
//    printf("%s:%d-stVencChnAttr.stRcAttr.stH265AVbr.u32MaxBitRate=%d\n", __FUNCTION__, __LINE__,
//           stVencChnAttr.stRcAttr.stH265AVbr.u32MaxBitRate);
//    printf("%s:%d-stVencChnAttr.stRcAttr.stH265AVbr.bVariFpsEn=%d\n", __FUNCTION__, __LINE__,
//           stVencChnAttr.stRcAttr.stH265AVbr.bVariFpsEn);
//
//    printf("%s:%d-stVencChnAttr.stGopAttr.enGopMode=%d\n", __FUNCTION__, __LINE__, stVencChnAttr.stGopAttr.enGopMode);
//    printf("%s:%d-stVencChnAttr.stGopAttr.stNormalP.s32IPQpDelta=%d\n", __FUNCTION__, __LINE__,
//           stVencChnAttr.stGopAttr.stNormalP.s32IPQpDelta);

    s32Ret = CVI_VENC_CreateChn(m_vencChn, &stVencChnAttr);
    if (CVI_SUCCESS != s32Ret) {
        emxloge("chn %d HI_MPI_VENC_CreateChn [%d] faild with %#x!\n", m_chn, m_vencChn, s32Ret);
        return ErrCodeE::Failure;
    }

    return ErrCodeE::Success;
}

ErrCodeE MediaChipVenc::SetSingleESBuffer(int size) {
    CVI_S32 s32Ret;
    for (int type = (int) MODTYPE_H264E; type <= (int) MODTYPE_JPEGE; type++) {
        VENC_PARAM_MOD_S stModParam = {};

        stModParam.enVencModType = (VENC_MODTYPE_E) type;
        s32Ret = CVI_VENC_GetModParam(&stModParam);
        if (s32Ret != CVI_SUCCESS) {
            emxloge("chn %d CVI_VENC_GetModParam type %d failed with #%x\n", m_chn, type, s32Ret);
            return ErrCodeE::Failure;
        }

        switch (stModParam.enVencModType) {
            case MODTYPE_H264E:
                stModParam.stH264eModParam.bSingleEsBuf = true;
                stModParam.stH264eModParam.u32SingleEsBufSize = size;
                break;
            case MODTYPE_H265E:
                stModParam.stH265eModParam.bSingleEsBuf = true;
                stModParam.stH265eModParam.u32SingleEsBufSize = size;
                break;
            case MODTYPE_JPEGE:
                stModParam.stJpegeModParam.bSingleEsBuf = true;
                stModParam.stJpegeModParam.u32SingleEsBufSize = size;
                break;
            default:
                emxloge("chn %d invalid type %d failure\n", m_chn, type);
                return ErrCodeE::Failure;
        }

        s32Ret = CVI_VENC_SetModParam(&stModParam);
        if (s32Ret != CVI_SUCCESS) {
            emxloge("chn %d CVI_VENC_SetModParam type %d size %d failed width #%x\n", m_chn, type, size, s32Ret);
            return ErrCodeE::Failure;
        }
    }
    return ErrCodeE::Success;
}

// 晶视芯片264编码如果不是16像素对其会出绿边
ErrCodeE MediaChipVenc::SetVpssChnAlignment() {
    CVI_S32 s32Ret;
    VPSS_CHN_ATTR_S stChnAttr = {};
    s32Ret = CVI_VPSS_GetChnAttr(m_vpssGrp, m_vpssChn, &stChnAttr);
    if (CVI_SUCCESS != s32Ret) {
        emxloge("chn %d CVI_VPSS_GetChnAttr failed with #%x!\n", m_chn, s32Ret);
        return ErrCodeE::Failure;
    }
    if (stChnAttr.u32Width == m_param.width && stChnAttr.u32Height == m_param.height)
        return ErrCodeE::Success;
    StopOsd();
    stChnAttr.u32Width = m_param.width;
    stChnAttr.u32Height = m_param.height;
    if (m_param.codec == VideoCodecE::H264) {
        int modH = m_param.height % 16;
        int modW = m_param.width % 16;
        if (modH || modW) {
            if (modH)
                stChnAttr.u32Height = m_param.height + 16 - modH;
            if (modW)
                stChnAttr.u32Width = m_param.width + 16 - modW;
            stChnAttr.stAspectRatio.enMode = ASPECT_RATIO_MANUAL;
            stChnAttr.stAspectRatio.bEnableBgColor = CVI_TRUE;
            stChnAttr.stAspectRatio.u32BgColor = 0;
            stChnAttr.stAspectRatio.stVideoRect.s32X = 0;
            stChnAttr.stAspectRatio.stVideoRect.s32Y = 0;
            stChnAttr.stAspectRatio.stVideoRect.u32Width = m_param.width;
            stChnAttr.stAspectRatio.stVideoRect.u32Height = m_param.height;
        } else {
            stChnAttr.stAspectRatio.enMode = ASPECT_RATIO_AUTO;
        }
    }
    s32Ret = CVI_VPSS_SetChnAttr(m_vpssGrp, m_vpssChn, &stChnAttr);
    if (CVI_SUCCESS != s32Ret) {
        emxloge("chn %d CVI_VPSS_SetChnAttr(%d,%d) failed!\n", m_chn, m_vpssGrp, m_vpssChn);
        return ErrCodeE::Failure;
    }
    StartOsd();
    return ErrCodeE::Success;
}

ErrCodeE MediaChipVenc::SetRcParam(Json::Value &json) {
    CVI_S32 s32Ret;
    VENC_RC_PARAM_S stRcParam = {};
    VENC_CHN_ATTR_S stChnAttr = {};

    s32Ret = CVI_VENC_GetChnAttr(m_vencChn, &stChnAttr);
    if (CVI_SUCCESS != s32Ret) {
        emxloge("chn %d GetChnAttr failed!\n", m_chn);
        return ErrCodeE::Failure;
    }

    s32Ret = CVI_VENC_GetRcParam(m_vencChn, &stRcParam);
    if (CVI_SUCCESS != s32Ret) {
        emxloge("chn %d CVI_VENC_GetRcParam failed!\n", m_chn);
        return ErrCodeE::Failure;;
    }
    stRcParam.u32ThrdLv = 2;
    stRcParam.bBgEnhanceEn = CVI_FALSE;
    stRcParam.s32BgDeltaQp = 0;
    stRcParam.u32RowQpDelta = CVI_H26X_ROW_QP_DELTA_DEFAULT;
    stRcParam.s32FirstFrameStartQp = 31;
    stRcParam.s32InitialDelay = 1000;

    int u32MinQp = json["u32MinQp"].asInt();
    int u32MaxQp = json["u32MaxQp"].asInt();
    int u32MinIQp = json["u32MinIQp"].asInt();
    int u32MaxIQp = json["u32MaxIQp"].asInt();
    int u32MaxIprop = json["u32MaxIprop"].asInt();
    int u32MinIprop = json["u32MinIprop"].asInt();
    int s32MaxReEncodeTimes = json["s32MaxReEncodeTimes"].asInt();

    if (VENC_RC_MODE_H264CBR == stChnAttr.stRcAttr.enRcMode) {
        stRcParam.stParamH264Cbr.u32MaxIprop = u32MaxIprop;
        stRcParam.stParamH264Cbr.u32MinIprop = u32MinIprop;
        stRcParam.stParamH264Cbr.u32MinQp = u32MinQp;
        stRcParam.stParamH264Cbr.u32MaxQp = u32MaxQp;
        stRcParam.stParamH264Cbr.u32MinIQp = u32MinIQp;
        stRcParam.stParamH264Cbr.u32MaxIQp = u32MaxIQp;
        stRcParam.stParamH264Cbr.s32MaxReEncodeTimes = s32MaxReEncodeTimes;
    } else if (VENC_RC_MODE_H264VBR == stChnAttr.stRcAttr.enRcMode) {
        stRcParam.stParamH264Vbr.u32MaxIprop = u32MaxIprop;
        stRcParam.stParamH264Vbr.u32MinIprop = u32MinIprop;
        stRcParam.stParamH264Vbr.u32MinQp = u32MinQp;
        stRcParam.stParamH264Vbr.u32MaxQp = u32MaxQp;
        stRcParam.stParamH264Vbr.u32MinIQp = u32MinIQp;
        stRcParam.stParamH264Vbr.u32MaxIQp = u32MaxIQp;
        stRcParam.stParamH264Vbr.s32ChangePos = 90;
        stRcParam.stParamH264Vbr.s32MaxReEncodeTimes = s32MaxReEncodeTimes;
    } else if (VENC_RC_MODE_H264AVBR == stChnAttr.stRcAttr.enRcMode) {
        stRcParam.stParamH264AVbr.u32MaxIprop = u32MaxIprop;
        stRcParam.stParamH264AVbr.u32MinIprop = u32MinIprop;
        stRcParam.stParamH264AVbr.u32MinQp = u32MinQp;
        stRcParam.stParamH264AVbr.u32MaxQp = u32MaxQp;
        stRcParam.stParamH264AVbr.u32MinIQp = u32MinIQp;
        stRcParam.stParamH264AVbr.u32MaxIQp = u32MaxIQp;
        stRcParam.stParamH264AVbr.s32ChangePos = 90;
        stRcParam.stParamH264AVbr.s32MinStillPercent = 30;
        stRcParam.stParamH264AVbr.u32MaxStillQP = 38;
        stRcParam.stParamH264AVbr.u32MotionSensitivity = 20;
        stRcParam.stParamH264AVbr.s32AvbrFrmLostOpen = 0;
        stRcParam.stParamH264AVbr.s32AvbrFrmGap = 1;
        stRcParam.stParamH264AVbr.s32AvbrPureStillThr = 4;
        stRcParam.stParamH264AVbr.s32MaxReEncodeTimes = s32MaxReEncodeTimes;
    } else if (VENC_RC_MODE_H265CBR == stChnAttr.stRcAttr.enRcMode) {
        stRcParam.stParamH264Cbr.u32MaxIprop = u32MaxIprop;
        stRcParam.stParamH264Cbr.u32MinIprop = u32MinIprop;
        stRcParam.stParamH265Cbr.u32MinQp = u32MinQp;
        stRcParam.stParamH265Cbr.u32MaxQp = u32MaxQp;
        stRcParam.stParamH265Cbr.u32MinIQp = u32MinIQp;
        stRcParam.stParamH265Cbr.u32MaxIQp = u32MaxIQp;
        stRcParam.stParamH265Cbr.s32MaxReEncodeTimes = s32MaxReEncodeTimes;
    } else if (VENC_RC_MODE_H265VBR == stChnAttr.stRcAttr.enRcMode) {
        stRcParam.stParamH265Vbr.u32MaxIprop = u32MaxIprop;
        stRcParam.stParamH265Vbr.u32MinIprop = u32MinIprop;
        stRcParam.stParamH265Vbr.u32MinQp = u32MinQp;
        stRcParam.stParamH265Vbr.u32MaxQp = u32MaxQp;
        stRcParam.stParamH265Vbr.u32MinIQp = u32MinIQp;
        stRcParam.stParamH265Vbr.u32MaxIQp = u32MaxIQp;
        stRcParam.stParamH265Vbr.s32ChangePos = 90;
        stRcParam.stParamH265Vbr.s32MaxReEncodeTimes = s32MaxReEncodeTimes;
    } else if (VENC_RC_MODE_H265AVBR == stChnAttr.stRcAttr.enRcMode) {
        stRcParam.stParamH265AVbr.u32MaxIprop = u32MaxIprop;
        stRcParam.stParamH265AVbr.u32MinIprop = u32MinIprop;
        stRcParam.stParamH265AVbr.u32MinQp = u32MinQp;
        stRcParam.stParamH265AVbr.u32MaxQp = u32MaxQp;
        stRcParam.stParamH265AVbr.u32MinIQp = u32MinIQp;
        stRcParam.stParamH265AVbr.u32MaxIQp = u32MaxIQp;
        stRcParam.stParamH265AVbr.s32ChangePos = 90;
        stRcParam.stParamH265AVbr.s32MinStillPercent = 30;
        stRcParam.stParamH265AVbr.u32MaxStillQP = 38;
        stRcParam.stParamH265AVbr.u32MotionSensitivity = 20;
        stRcParam.stParamH265AVbr.s32AvbrFrmLostOpen = 0;
        stRcParam.stParamH265AVbr.s32AvbrFrmGap = 1;
        stRcParam.stParamH265AVbr.s32AvbrPureStillThr = 4;
        stRcParam.stParamH265AVbr.s32MaxReEncodeTimes = s32MaxReEncodeTimes;
    } else {
        emxloge("chn %d unknown rc mode %d\n", m_chn, stChnAttr.stRcAttr.enRcMode);
        return ErrCodeE::Failure;
    }
    s32Ret = CVI_VENC_SetRcParam(m_vencChn, &stRcParam);
    if (CVI_SUCCESS != s32Ret) {
        emxloge("chn %d SetRcParam failed with #%x!\n", m_chn, s32Ret);
        return ErrCodeE::Failure;
    }
    return ErrCodeE::Success;
}

ErrCodeE MediaChipVenc::AttachVbPool(Json::Value &json) {
    VB_SOURCE_E eVbSource = {};
    VENC_PARAM_MOD_S stModParam = {};
    CVI_S32 s32Ret;

    if (m_param.codec != VideoCodecE::H264 && m_param.codec != VideoCodecE::H265)
        return ErrCodeE::Success;

    if (m_param.codec == VideoCodecE::H264)
        stModParam.enVencModType = MODTYPE_H264E;
    else
        stModParam.enVencModType = MODTYPE_H265E;

    s32Ret = CVI_VENC_GetModParam(&stModParam);
    if (CVI_SUCCESS != s32Ret) {
        emxloge("chn %d CVI_VENC_GetModParam failed with%#x!\n", m_chn, s32Ret);
        return ErrCodeE::Failure;
    }

    if (m_param.codec == VideoCodecE::H264)
        eVbSource = stModParam.stH264eModParam.enH264eVBSource;
    else
        eVbSource = stModParam.stH265eModParam.enH265eVBSource;

    if (eVbSource == VB_SOURCE_USER) {
        VENC_CHN_POOL_S stPool = {};
        stPool.hPicVbPool = m_vencChn;
        stPool.hPicInfoVbPool = m_vencChn;
        s32Ret = CVI_VENC_AttachVbPool(m_vencChn, &stPool);
        if (s32Ret != CVI_SUCCESS) {
            emxloge("chn %d CVI_VENC_AttachVbPool failed with #%x\n", m_chn, s32Ret);
            return ErrCodeE::Failure;
        }
    }
    return ErrCodeE::Success;
}

ErrCodeE MediaChipVenc::SetVideoCodecParam(Json::Value &json) {
    if (m_param.codec != VideoCodecE::H264 && m_param.codec != VideoCodecE::H265)
        return ErrCodeE::Success;

    CVI_S32 s32Ret;
    if (m_param.codec == VideoCodecE::H264) {
        VENC_H264_ENTROPY_S h264Entropy = {};
        auto h264EntropyMode = (H264E_ENTROPY) json["h264EntropyMode"].asInt();
        h264Entropy.u32EntropyEncModeI = h264EntropyMode;
        h264Entropy.u32EntropyEncModeP = h264EntropyMode;
        h264Entropy.u32EntropyEncModeB = h264EntropyMode;
        s32Ret = CVI_VENC_SetH264Entropy(m_vencChn, &h264Entropy);
        if (s32Ret != CVI_SUCCESS) {
            emxloge("chn %d CVI_VENC_SetH264Entropy failed with %#x\n", m_chn, s32Ret);
            return ErrCodeE::Failure;
        }

        VENC_H264_TRANS_S h264Trans = {};
        s32Ret = CVI_VENC_GetH264Trans(m_vencChn, &h264Trans);
        if (s32Ret != CVI_SUCCESS) {
            emxloge("chn %d CVI_VENC_GetH264Trans failed with #%x\n", m_chn, s32Ret);
            return ErrCodeE::Failure;
        }
        h264Trans.chroma_qp_index_offset = json["h264ChromaQpOffset"].asInt();
        s32Ret = CVI_VENC_SetH264Trans(m_vencChn, &h264Trans);
        if (s32Ret != CVI_SUCCESS) {
            emxloge("chn %d CVI_VENC_SetH264Trans failed with #%x\n", m_chn, s32Ret);
            return ErrCodeE::Failure;
        }

        VENC_H264_VUI_S h264Vui = {};
        s32Ret = CVI_VENC_GetH264Vui(m_vencChn, &h264Vui);
        if (s32Ret != CVI_SUCCESS) {
            emxloge("chn %d CVI_VENC_GetH264Vui failed with #%x\n", m_chn, s32Ret);
            return ErrCodeE::Failure;
        }
        h264Vui.stVuiTimeInfo.timing_info_present_flag = 1;
        if (h264Vui.stVuiTimeInfo.timing_info_present_flag) {
            h264Vui.stVuiTimeInfo.fixed_frame_rate_flag = 1;
            h264Vui.stVuiTimeInfo.num_units_in_tick = 1;
            h264Vui.stVuiTimeInfo.time_scale = m_param.fps * 2;
        }
        s32Ret = CVI_VENC_SetH264Vui(m_vencChn, &h264Vui);
        if (s32Ret != CVI_SUCCESS) {
            emxloge("chn %d CVI_VENC_SetH264Vui failed with #%x\n", m_chn, s32Ret);
            return ErrCodeE::Failure;
        }

        if (m_param.height % 16) {
            VENC_CHN_PARAM_S venc_chn_param = {};
            s32Ret = CVI_VENC_GetChnParam(m_vencChn, &venc_chn_param);
            if (s32Ret != CVI_SUCCESS) {
                emxloge("chn %d CVI_VENC_GetChnParam failed with #%x\n", m_chn, s32Ret);
                return ErrCodeE::Failure;
            }
            venc_chn_param.stCropCfg.bEnable = false;
            venc_chn_param.stCropCfg.stRect.s32X = 0;
            venc_chn_param.stCropCfg.stRect.s32Y = m_param.height % 16;
            venc_chn_param.stCropCfg.stRect.u32Width = m_param.width;
            venc_chn_param.stCropCfg.stRect.u32Height = m_param.height;
            s32Ret = CVI_VENC_SetChnParam(m_vencChn, &venc_chn_param);
            if (s32Ret != CVI_SUCCESS) {
                emxloge("chn %d CVI_VENC_SetChnParam failed with #%x\n", m_chn, s32Ret);
                return ErrCodeE::Failure;
            }
        }
    } else {
        VENC_H265_TRANS_S h265Trans = {};
        s32Ret = CVI_VENC_GetH265Trans(m_vencChn, &h265Trans);
        if (s32Ret != CVI_SUCCESS) {
            emxloge("chn %d CVI_VENC_GetH265Trans failed with #%x\n", m_chn, s32Ret);
            return ErrCodeE::Failure;
        }
        h265Trans.cb_qp_offset = json["h265CbQpOffset"].asInt();
        h265Trans.cr_qp_offset = json["h265CrQpOffset"].asInt();
        s32Ret = CVI_VENC_SetH265Trans(m_vencChn, &h265Trans);
        if (s32Ret != CVI_SUCCESS) {
            emxloge("chn %d CVI_VENC_SetH265Trans failed with #%x\n", m_chn, s32Ret);
            return ErrCodeE::Failure;
        }

        VENC_H265_VUI_S h265Vui = {};
        s32Ret = CVI_VENC_GetH265Vui(m_vencChn, &h265Vui);
        if (s32Ret != CVI_SUCCESS) {
            emxloge("chn %d CVI_VENC_GetH265Vui failed with #%x\n", m_chn, s32Ret);
            return ErrCodeE::Failure;
        }
        h265Vui.stVuiTimeInfo.timing_info_present_flag = 1;
        if (h265Vui.stVuiTimeInfo.timing_info_present_flag) {
            h265Vui.stVuiTimeInfo.num_units_in_tick = 1;
            h265Vui.stVuiTimeInfo.time_scale = m_param.fps;
        }
        s32Ret = CVI_VENC_SetH265Vui(m_vencChn, &h265Vui);
        if (s32Ret != CVI_SUCCESS) {
            emxloge("chn %d CVI_VENC_SetH265Vui failed with #%x\n", m_chn, s32Ret);
            return ErrCodeE::Failure;
        }
    }
    return ErrCodeE::Success;
}

void MediaChipVenc::DrawMsgRecv(uint32_t topic, const char *data, int32_t size) {
    Json::Value json;
    if (EasyJson::Parse(data, json) != ErrCodeE::Success)
        return;
    if (json["pipe"].asInt() != m_pipe)
        return;
    //todo:还没想好数据的清空使用定时器超时的方式还是pub端不停发送空的rect方式or...
    m_drawMsgMap[json["id"].asString()] = json;
}

//todo:单独剥离出来
void MediaChipVenc::DrawRectByJson(VIDEO_FRAME_S &stVFrame) {
    size_t image_size = stVFrame.u32Length[0] + stVFrame.u32Length[1] + stVFrame.u32Length[2];
    CVI_U32 u32LumaSize = stVFrame.u32Stride[0] * stVFrame.u32Height;
    //    CVI_U32 u32ChromaSize = stVFrame.u32Stride[1] * stVFrame.u32Height / 2;
    CVI_VOID *vir_addr = CVI_SYS_Mmap(stVFrame.u64PhyAddr[0], image_size);

    if (!vir_addr)
        return;
    int stride = (int) stVFrame.u32Stride[0];
    auto y = (uint8_t *) vir_addr;
    auto uv = y + u32LumaSize;

    Size dstSize((int) stVFrame.u32Width, (int) stVFrame.u32Height);

    for (auto &mapId: m_drawMsgMap) {
        auto &json = mapId.second;
        Size srcSize(json["picWidth"].asInt(), json["picHeight"].asInt());
        int lineWidth = json["lineWidth"].asInt();
        auto color = (DrawRect::ColorE) json["color"].asInt();
        for (auto &e: json["rect"]) {
            Rect srcRect(e["x"].asInt(), e["y"].asInt(), e["w"].asInt(), e["h"].asInt());
            Rect dstRect = srcRect.Transfer(srcSize, dstSize);
            DrawRect::Nv21(color, y, uv, dstRect, stride, lineWidth);
        }
    }
    CVI_SYS_Munmap(vir_addr, image_size);
}

ErrCodeE MediaChipVenc::SetRefParam(Json::Value &json) {
    CVI_S32 s32Ret;
    VENC_REF_PARAM_S stRefParam = {};

    s32Ret = CVI_VENC_GetRefParam(m_vencChn, &stRefParam);
    if (s32Ret != CVI_SUCCESS) {
        emxloge("chn %d GetRefParam failed!\n", m_chn);
        return ErrCodeE::Failure;
    }
    int tempLayer = 0;
    if (tempLayer == 2) {
        stRefParam.u32Base = 1;
        stRefParam.u32Enhance = 1;
        stRefParam.bEnablePred = CVI_TRUE;
    } else if (tempLayer == 3) {
        stRefParam.u32Base = 2;
        stRefParam.u32Enhance = 1;
        stRefParam.bEnablePred = CVI_TRUE;
    } else {
        stRefParam.u32Base = 0;
        stRefParam.u32Enhance = 0;
        stRefParam.bEnablePred = CVI_TRUE;
    }
    s32Ret = CVI_VENC_SetRefParam(m_vencChn, &stRefParam);
    if (s32Ret != CVI_SUCCESS) {
        emxloge("chn %d CVI_VENC_SetRefParam failed!\n", m_chn);
        return ErrCodeE::Failure;
    }

    VENC_CU_PREDICTION_S stCuPrediction = {};
    s32Ret = CVI_VENC_GetCuPrediction(m_vencChn, &stCuPrediction);
    if (s32Ret != CVI_SUCCESS) {
        emxloge("chn %d CVI_VENC_GetCuPrediction failed!\n", m_chn);
        return ErrCodeE::Failure;
    }
    stCuPrediction.u32IntraCost = 0;
    s32Ret = CVI_VENC_SetCuPrediction(m_vencChn, &stCuPrediction);
    if (s32Ret != CVI_SUCCESS) {
        emxloge("chn %d CVI_VENC_SetCuPrediction failed!\n", m_chn);
        return ErrCodeE::Failure;
    }

    VENC_FRAMELOST_S stFL = {};
    s32Ret = CVI_VENC_GetFrameLostStrategy(m_vencChn, &stFL);
    if (s32Ret != CVI_SUCCESS) {
        emxloge("chn %d CVI_VENC_GetFrameLostStrategy failed!\n", m_chn);
        return ErrCodeE::Failure;
    }

    stFL.bFrmLostOpen = CVI_FALSE;
    stFL.enFrmLostMode = FRMLOST_PSKIP;

    s32Ret = CVI_VENC_SetFrameLostStrategy(m_vencChn, &stFL);
    if (s32Ret != CVI_SUCCESS) {
        emxloge("chn %d CVI_VENC_SetFrameLostStrategy failed!\n", m_chn);
        return ErrCodeE::Failure;
    }
    return ErrCodeE::Success;
}

void MediaChipVenc::StartOsd() {
    for (auto serverOsd:MediaChipOSD::m_array) {
        auto platOsd = (MediaChipOSD *) serverOsd;
        if (platOsd->m_vpssGrp == m_vpssGrp && platOsd->m_vpssChn == m_vpssChn) {
            platOsd->Start();
        }
    }
}

void MediaChipVenc::StopOsd() {
    for (auto serverOsd:MediaChipOSD::m_array) {
        auto platOsd = (MediaChipOSD *) serverOsd;
        if (platOsd->m_vpssGrp == m_vpssGrp && platOsd->m_vpssChn == m_vpssChn) {
            platOsd->Stop();
        }
    }
}
