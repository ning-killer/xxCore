//
// Created by cx on 2023/6/29.
//

#include "core/EmxLog.hpp"
#include "cvi_comm_isp.h"
#include "cvi_isp.h"
#include "cvi_sys.h"
#include "cvi_ae.h"
#include "ScreenExposure.hpp"

using namespace Emx;

int ScreenExposure::StartScreenExposure(VIDEO_FRAME_INFO_S *pstFrame, RECT_S *pstRegion, int regionNum, int targetLuma,
                                        VI_PIPE chn) {

    RECT_S maxRegion = {0};
    for (int i = 0; i < regionNum; ++i) {
        if (pstRegion[i].u32Width * pstRegion[i].u32Width >
            maxRegion.u32Width * maxRegion.u32Height) {
            memcpy(&maxRegion, &pstRegion[i], sizeof(RECT_S));
        }
    }

    positionMsg pos = {0};
    pos.x1 = maxRegion.s32X;
    pos.y1 = maxRegion.s32Y;
    pos.x2 = maxRegion.s32X + maxRegion.u32Width;
    pos.y2 = maxRegion.s32Y + maxRegion.u32Height;
    if ((pos.x1 >= pos.x2) || (pos.y1 >= pos.y2) ||
        (pos.x1 > pstFrame->stVFrame.u32Width) ||
        (pos.x2 > pstFrame->stVFrame.u32Width) ||
        (pos.y1 > pstFrame->stVFrame.u32Height) ||
        (pos.y1 > pstFrame->stVFrame.u32Height)) {
        emxloge("pararm is invalid,tMax.x1 %d,tMax.x2 %d,tMax.y1 %d,tMax.y2 %d\n", pos.x1, pos.x2, pos.y1, pos.y2);
        return -1;
    }

    int iValue = GetRegionAE(pos, pstFrame->stVFrame.u32Width, pstFrame->stVFrame.u32Height);
    emxlogi("Screen Exposure get AE value = %d\n", iValue);
    if (iValue == 0) {
        iValue = GetRegionLuma(pos, pstFrame);
        emxlogi("Screen Exposure get Luma value = %d\n", iValue);
    }
    if (iValue > 0) {
        int aeRangeMax = targetLuma + 20;
        int aeRangeMin = targetLuma - 10;
        emxlogi("aeRangeMax=%d,aeRangeMin=%d\n", aeRangeMax, aeRangeMin);
        if ((iValue >= aeRangeMax) || (iValue <= aeRangeMin)) {
            ISP_EXPOSURE_ATTR_S stExpAttr;
            memset(&stExpAttr, 0, sizeof(stExpAttr));
            int ret = CVI_ISP_GetExposureAttr(chn, &stExpAttr);
            if (ret != CVI_SUCCESS) {
                emxloge("CVI_ISP_GetExposureAttr failed with %#x\n", ret);
            }

            stExpAttr.stAuto.u16EVBias = (targetLuma * 1024) / iValue;
            if (stExpAttr.stAuto.u16EVBias < FACE_TARGET_EVBIAS_L_RANGE)
                stExpAttr.stAuto.u16EVBias = FACE_TARGET_EVBIAS_L_RANGE;

            if (stExpAttr.stAuto.u16EVBias > FACE_TARGET_EVBIAS_H_RANGE)
                stExpAttr.stAuto.u16EVBias = FACE_TARGET_EVBIAS_H_RANGE;

            emxlogi("stExpAttr.stAuto.u16EVBias:%d\n", stExpAttr.stAuto.u16EVBias);
            ret = CVI_ISP_SetExposureAttr(chn, &stExpAttr);
            if (ret != CVI_SUCCESS) {
                emxloge("CVI_ISP_SetAeInfo failed with %#x\n", ret);
            }
        }
    }
    return 0;
}

void ScreenExposure::EndScreenExposure(VI_PIPE chn) {

    int ret = 0;
    ISP_EXPOSURE_ATTR_S stExpAttr;
    memset(&stExpAttr, 0, sizeof(stExpAttr));
    CVI_ISP_GetExposureAttr(0, &stExpAttr);
    stExpAttr.stAuto.u16EVBias = m_defaultEVBias;
    ret = CVI_ISP_SetExposureAttr(chn, &stExpAttr);
    if (ret != CVI_SUCCESS) {
        emxloge("CVI_ISP_SetFaceAeInfo failed with %#x\n", ret);
    }
}

int ScreenExposure::GetRegionAE(positionMsg pos, CVI_U32 Width, CVI_U32 Height) {

    int ret = 0;
    CVI_U8 i = 0, j = 0;
    CVI_U16 gridPerSizeX = 0, gridPerSizeY = 0;
    CVI_U16 fdPosX = 0, fdPosY = 0, fdWidth = 0, fdHeight = 0;
    CVI_U8 rowStartIdx = 0, columnStartIdx = 0, rowEndIdx = 0, columnEndIdx = 0;
    CVI_U16 RValue = 0, GValue = 0, BValue = 0, maxValue = 0;
    CVI_U32 frameLumaSum = 0;
    CVI_U32 frameAvgLuma = 0;
    CVI_U32 aeWinCount = 0;


    gridPerSizeX = (Width / AE_GRID_COLUMN) ? Width / AE_GRID_COLUMN : 1;
    gridPerSizeY = (Height / AE_GRID_ROW) ? Height / AE_GRID_ROW : 1;

    fdPosX = pos.x1 + (pos.x2 - pos.x1) / 4;
    fdPosY = pos.y1 + (pos.y2 - pos.y1) / 4;
    fdWidth = (pos.x2 - pos.x1) / 2;
    fdHeight = (pos.y2 - pos.y1) / 2;

    rowStartIdx = fdPosY / gridPerSizeY;
    columnStartIdx = fdPosX / gridPerSizeX;

    rowEndIdx = (fdPosY + fdHeight) / gridPerSizeY;
    columnEndIdx = (fdPosX + fdWidth) / gridPerSizeX;

    ISP_AE_STATISTICS_S stAeStat;
    memset(&stAeStat, 0, sizeof(ISP_AE_STATISTICS_S));
    ret = CVI_ISP_GetAEStatistics(0, &stAeStat);
    if (ret != CVI_SUCCESS) {
        emxloge("CVI_ISP_GetAEStatistics error!!ret=%#x!\n", ret);
        return CVI_FALSE;
    }

    for (i = rowStartIdx; i < rowEndIdx; i++) {
        for (j = columnStartIdx; j < columnEndIdx; j++) {
            RValue = stAeStat.au16FEZoneAvg[0][0][i][j][AE_CHANNEL_R];
            BValue = stAeStat.au16FEZoneAvg[0][0][i][j][AE_CHANNEL_B];
            GValue = stAeStat.au16FEZoneAvg[0][0][i][j][AE_CHANNEL_GR] +
                     stAeStat.au16FEZoneAvg[0][0][i][j][AE_CHANNEL_GB];
            GValue = GValue / 2;

            maxValue = RValue > GValue ? RValue : GValue;
            maxValue = maxValue > BValue ? maxValue : BValue;

            frameLumaSum += maxValue;
            aeWinCount++;
        }
    }

    if (aeWinCount > FACE_AE_MIN_CNT) {
        frameAvgLuma = (frameLumaSum / aeWinCount) / 4;
    }
    return frameAvgLuma;
}

int ScreenExposure::GetRegionLuma(positionMsg pos, VIDEO_FRAME_INFO_S *pstFrame) {
    CVI_U32 i = 0, j = 0;
    CVI_U32 MaxValue = 0, StartOffset = 0, EndOffset = 0, iAvgValue = 0;
    CVI_U64 iTotalValue = 0, iTotalPixel = 0;

    pstFrame->stVFrame.pu8VirAddr[0] = (CVI_U8 *) CVI_SYS_Mmap(pstFrame->stVFrame.u64PhyAddr[0],
                                                               pstFrame->stVFrame.u32Length[0]);
    if (pstFrame->stVFrame.pu8VirAddr[0] == NULL) {
        emxloge("pstFrame->stVFrame.pu8VirAddr[0] is NULL\n");
        return 0;
    }
    for (i = pos.y1; i < pos.y2; i++) {
        StartOffset = (pstFrame->stVFrame.u32Stride[0] * i) + pos.x1;
        EndOffset = (pstFrame->stVFrame.u32Stride[0] * i) + pos.x2;
        if (EndOffset > pstFrame->stVFrame.u32Length[0]) {
            emxloge("EndOffset:%d > u32Length:%d\n", EndOffset, pstFrame->stVFrame.u32Length[0]);
            return iAvgValue;
        }
        for (j = StartOffset; j < EndOffset; j++) {
            MaxValue = *(pstFrame->stVFrame.pu8VirAddr[0] + j);
            iTotalValue += MaxValue;
            iTotalPixel++;
        }
    }
    CVI_SYS_Munmap(pstFrame->stVFrame.pu8VirAddr[0], pstFrame->stVFrame.u32Length[0]);
    pstFrame->stVFrame.pu8VirAddr[0] = NULL;

    if (iTotalPixel > 0) {
        iAvgValue = iTotalValue / iTotalPixel;
    }

    return iAvgValue;
}

ScreenExposure *ScreenExposure::GetInst() {
    static ScreenExposure *_instance = NULL;
    if (_instance == NULL) {
        _instance = new ScreenExposure();
    }
    return _instance;
}

ScreenExposure::ScreenExposure() {
    ISP_EXPOSURE_ATTR_S stExpAttr;
    memset(&stExpAttr, 0, sizeof(stExpAttr));
    int ret = CVI_ISP_GetExposureAttr(0, &stExpAttr);
    if (ret != CVI_SUCCESS) {
        emxloge("CVI_ISP_GetExposureAttr failed with %#x\n", ret);
        m_defaultEVBias = 1024;
    }else
        m_defaultEVBias = stExpAttr.stAuto.u16EVBias;
}

