#include "MediaChipDouLensVenc.hpp"
#include "DrawRect.hpp"
#include "MediaInner.hpp"
#include "core/EmxLog.hpp"
#include "MediaConfigs.hpp"
#include "sample_comm.h"
#include "cvi_comm_venc.h"
#include "MediaChipDouLensStitch.hpp"

using namespace Emx;

static CVI_S32 CreateNewVideoFrame(VFrBox *pVFBox, CVI_U32 u32Width, CVI_U32 u32Height) {
    SIZE_S stSize = {.u32Width = u32Width, .u32Height = u32Height};

    pVFBox->stSize.u32Width = u32Width;
    pVFBox->stSize.u32Height = u32Height;

    COMMON_GetPicBufferConfig(stSize.u32Width, stSize.u32Height, PIXEL_FORMAT_YUV_PLANAR_420, DATA_BITWIDTH_8,
                              COMPRESS_MODE_NONE, DEFAULT_ALIGN, &pVFBox->stVbCalConfig);

    pVFBox->stVideoFrame.stVFrame.enCompressMode = COMPRESS_MODE_NONE;
    pVFBox->stVideoFrame.stVFrame.enPixelFormat = PIXEL_FORMAT_YUV_PLANAR_420;
    pVFBox->stVideoFrame.stVFrame.enVideoFormat = VIDEO_FORMAT_LINEAR;
    pVFBox->stVideoFrame.stVFrame.enColorGamut = COLOR_GAMUT_BT709;
    pVFBox->stVideoFrame.stVFrame.u32Width = stSize.u32Width;
    pVFBox->stVideoFrame.stVFrame.u32Height = stSize.u32Height;
    pVFBox->stVideoFrame.stVFrame.u32Stride[0] = pVFBox->stVbCalConfig.u32MainStride;
    pVFBox->stVideoFrame.stVFrame.u32Stride[1] = pVFBox->stVbCalConfig.u32CStride;
    pVFBox->stVideoFrame.stVFrame.u32Stride[2] = pVFBox->stVbCalConfig.u32CStride;
    pVFBox->stVideoFrame.stVFrame.u32TimeRef = 0;
    pVFBox->stVideoFrame.stVFrame.u64PTS = 0;
    pVFBox->stVideoFrame.stVFrame.enDynamicRange = DYNAMIC_RANGE_SDR8;

    pVFBox->blk = CVI_VB_GetBlock(VB_INVALID_POOLID, pVFBox->stVbCalConfig.u32VBSize);
    if (pVFBox->blk == VB_INVALID_HANDLE) {
        SAMPLE_PRT("Can't acquire vb block\n");
        return -1;
    }

    pVFBox->stVideoFrame.u32PoolId = CVI_VB_Handle2PoolId(pVFBox->blk);
    pVFBox->stVideoFrame.stVFrame.u32Length[0] = pVFBox->stVbCalConfig.u32MainYSize;
    pVFBox->stVideoFrame.stVFrame.u32Length[1] = pVFBox->stVideoFrame.stVFrame.u32Length[2]
            = pVFBox->stVbCalConfig.u32MainCSize;
    pVFBox->stVideoFrame.stVFrame.u64PhyAddr[0] = CVI_VB_Handle2PhysAddr(pVFBox->blk);
    pVFBox->stVideoFrame.stVFrame.u64PhyAddr[1] = pVFBox->stVideoFrame.stVFrame.u64PhyAddr[0]
                                                  + ALIGN(pVFBox->stVbCalConfig.u32MainYSize,
                                                          pVFBox->stVbCalConfig.u16AddrAlign);
    pVFBox->stVideoFrame.stVFrame.u64PhyAddr[2] = pVFBox->stVideoFrame.stVFrame.u64PhyAddr[1]
                                                  + ALIGN(pVFBox->stVbCalConfig.u32MainCSize,
                                                          pVFBox->stVbCalConfig.u16AddrAlign);

    for (int i = 0; i < 3; i++) {
        pVFBox->stVideoFrame.stVFrame.pu8VirAddr[i]
                = (CVI_U8 *) CVI_SYS_Mmap(pVFBox->stVideoFrame.stVFrame.u64PhyAddr[i],
                                          pVFBox->stVideoFrame.stVFrame.u32Length[i]);
    }

    return 0;
}

static CVI_S32 ViGetDualFrame(VIDEO_FRAME_INFO_S *pstVideoFrameL, VIDEO_FRAME_INFO_S *pstVideoFrameR) {

    VI_CHN chn = -1;
    CVI_S32 diff = 0;
    do {
        if (chn == -1){
            if (CVI_VI_GetChnFrame(0, 0, pstVideoFrameL, 3000)) {//left
                emxloge("CVI_Vi_GetChnFrame left fail\n");
                return CVI_FAILURE;
            }

            if (CVI_VI_GetChnFrame(0, 1, pstVideoFrameR, 3000)) {//right
                emxloge("CVI_Vi_GetChnFrame right fail\n");
                CVI_VPSS_ReleaseChnFrame(1, 0, pstVideoFrameL);
                return CVI_FAILURE;
            }
        }
        else{
            if (chn)
                CVI_VI_GetChnFrame(0, chn, pstVideoFrameR,3000);
            else
                CVI_VI_GetChnFrame(0, chn, pstVideoFrameL,3000);
        }

        diff = pstVideoFrameR->stVFrame.u64PTS - pstVideoFrameL->stVFrame.u64PTS;
        if(labs(diff) > (10 * 1000)){
            emxloge("R_u64PTS = %lld, L_u64PTS = %lld\n",pstVideoFrameR->stVFrame.u64PTS, pstVideoFrameL->stVFrame.u64PTS);
            chn = 1;
            if(pstVideoFrameR->stVFrame.u64PTS > pstVideoFrameL->stVFrame.u64PTS){
                chn = 0;
            }
        }
        else{
            chn = -1;
        }
        if(chn != -1){
            if (chn)
              CVI_VI_ReleaseChnFrame(0, chn, pstVideoFrameR);
            else
                CVI_VI_ReleaseChnFrame(0, chn, pstVideoFrameL);
        }
    } while (chn != -1);


    for (int i = 0; i < 3; i++) {
        pstVideoFrameL->stVFrame.pu8VirAddr[i]
                = (CVI_U8 *) CVI_SYS_Mmap(pstVideoFrameL->stVFrame.u64PhyAddr[i],
                                          pstVideoFrameL->stVFrame.u32Length[i]);
    }
    for (int i = 0; i < 3; i++) {
        pstVideoFrameR->stVFrame.pu8VirAddr[i]
                = (CVI_U8 *) CVI_SYS_Mmap(pstVideoFrameR->stVFrame.u64PhyAddr[i],
                                          pstVideoFrameR->stVFrame.u32Length[i]);
    }
    return CVI_SUCCESS;
}

static CVI_S32 FreeVideoFrame(VFrBox *pVFBox) {
    for (int i = 0; i < 3; i++) {
        CVI_SYS_Munmap(pVFBox->stVideoFrame.stVFrame.pu8VirAddr[i], pVFBox->stVideoFrame.stVFrame.u32Length[i]);
    }
    CVI_VB_ReleaseBlock(pVFBox->blk);
    return 0;
}

static CVI_S32 VpssReleaseDualFrame(VIDEO_FRAME_INFO_S *pstVideoFrameL, VIDEO_FRAME_INFO_S *pstVideoFrameR) {
    for (int i = 0; i < 3; i++)
        CVI_SYS_Munmap(pstVideoFrameL->stVFrame.pu8VirAddr[i], pstVideoFrameL->stVFrame.u32Length[i]);
    for (int i = 0; i < 3; i++)
        CVI_SYS_Munmap(pstVideoFrameR->stVFrame.pu8VirAddr[i], pstVideoFrameR->stVFrame.u32Length[i]);

    CVI_VI_ReleaseChnFrame(0, 0, pstVideoFrameL);
    CVI_VI_ReleaseChnFrame(0, 1, pstVideoFrameR);
    return CVI_SUCCESS;
}


void MediaChipDouLensVenc::StartStream() {

    Json::Value root;
    m_res.configs.GetCfg("venc", root);
    if (root.size() == 1) {
        m_pthread = std::thread(&MediaChipDouLensVenc::Stitch2Venc, this);
        m_work.Create(
                &m_res.loop, nullptr,
                [this](void *arg) { OnStitchStream(arg); },
                [this](ErrCodeE e, void *arg) { OnStreamComplete(e, arg); });
    } else {
        m_work.Create(
                &m_res.loop, nullptr,
                [this](void *arg) { OnNormalStream(arg); },
                [this](ErrCodeE e, void *arg) { OnStreamComplete(e, arg); });
    }

    m_work.Run();
}

void MediaChipDouLensVenc::StopStream()
{
    Json::Value root;
    m_res.configs.GetCfg("venc", root);
    if (root.size() == 1) {
        if (m_bind) {
            m_pthread.join();
            SAMPLE_COMM_VPSS_UnBind_VENC(m_vpssGrp, m_vpssChn, m_vencChn);
        }
    } else {
        if (m_bind)
            SAMPLE_COMM_VI_UnBind_VENC(m_pipe, m_viChn, m_vencChn);
    }
    m_work.Destroy();
}


void MediaChipDouLensVenc::OnNormalStream(void *arg) {
    if (Vi2Venc() == ErrCodeE::Success)
        Venc2User();
}

void MediaChipDouLensVenc::OnStitchStream(void *arg) {
    Venc2User();
}

void MediaChipDouLensVenc::PushFrame() {
    unsigned char naltype;
    unsigned int len = 0;
    for (uint32_t i = 0; i < m_stream.u32PackCount; i++) {
        len += m_stream.pstPack[i].u32Len - m_stream.pstPack[i].u32Offset;
    }
    char *data;
    MediaFrame frame = {};
    frame.type = MediaFrame::TypeE::Venc;
    frame.chn = m_chn;
    frame.tsInUs = Time::GetMs64()*1000;
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

ErrCodeE MediaChipDouLensVenc::Vi2Venc() {
    VIDEO_FRAME_INFO_S frame = {};
    VI_CHN_ATTR_S stChnAttr = {};
    CVI_S32 s32Ret;

    s32Ret = CVI_VI_GetChnAttr(m_pipe, m_viChn, &stChnAttr);
    if (CVI_SUCCESS != s32Ret) {
        emxloge("chn %d CVI_VI_GetChnAttr(%d,%d) failed with #%x!\n", m_chn, m_pipe, m_viChn, s32Ret);
        return ErrCodeE::Failure;
    }

    s32Ret = CVI_VI_GetChnFrame(m_pipe, m_viChn, &frame, 1000);
    if (s32Ret != CVI_SUCCESS) {
        emxloge("chn %d CVI_VI_GetChnFrame failed with #%x!\n", m_viChn, s32Ret);
        CVI_VI_ReleaseChnFrame(m_pipe, m_viChn, &frame);
        return ErrCodeE::Failure;
    }

    s32Ret = CVI_VENC_SendFrame(m_vencChn, &frame, 6000);
    if (s32Ret != CVI_SUCCESS) {
        emxloge("chn %d CVI_VENC_SendFrame failed with #%x!\n", m_chn, s32Ret);
        CVI_VI_ReleaseChnFrame(m_pipe, m_viChn, &frame);
        return ErrCodeE::Failure;
    }

    s32Ret = CVI_VI_ReleaseChnFrame(m_pipe, m_viChn, &frame);
    if (s32Ret != CVI_SUCCESS) {
        emxloge("chn %d CVI_VI_ReleaseChnFrame failed with #%x!\n", m_chn, s32Ret);
        return ErrCodeE::Failure;
    }
    return ErrCodeE::Success;
}



ErrCodeE MediaChipDouLensVenc::Stitch2Venc() {

    int Ret;
    while (m_started) {
        Ret = ViGetDualFrame(&m_stVideoFrame0, &m_stVideoFrame1);
        if (Ret != CVI_SUCCESS) {
            emxloge("ViGetDualFrame fail,continue,with %#x\n",Ret);
            continue;
        }
        Ret = CreateNewVideoFrame(&m_vfboxOut, m_stVideoFrame0.stVFrame.u32Width, m_stVideoFrame0.stVFrame.u32Height);
        if (Ret != CVI_SUCCESS) {
            emxloge("CreateNewVideoFrame fail,with %#x\n",Ret);
            VpssReleaseDualFrame(&m_stVideoFrame0, &m_stVideoFrame1);
            continue;
        }
        Ret = MediaChipDouLensStitch::GetInst()->Stitch(&m_stVideoFrame0, &m_stVideoFrame1, &m_vfboxOut.stVideoFrame);
        if (Ret) {
            emxloge("CVI_STITCH fail\n");
            VpssReleaseDualFrame(&m_stVideoFrame0, &m_stVideoFrame1);
            FreeVideoFrame(&m_vfboxOut);
            continue;
        }

        //m_vfboxOut.stVideoFrame.stVFrame.u64PTS = m_stVideoFrame0.stVFrame.u64PTS;
        Ret = CVI_VPSS_SendFrame(0, &m_vfboxOut.stVideoFrame, 6000);
        if (Ret != CVI_SUCCESS) {
            emxloge("CVI_VPSS_SendFrame failed! with %#x\n", Ret);
            VpssReleaseDualFrame(&m_stVideoFrame0, &m_stVideoFrame1);
            FreeVideoFrame(&m_vfboxOut);
            continue;
        }

        VpssReleaseDualFrame(&m_stVideoFrame0, &m_stVideoFrame1);
        FreeVideoFrame(&m_vfboxOut);
    }

    return ErrCodeE::Success;
}