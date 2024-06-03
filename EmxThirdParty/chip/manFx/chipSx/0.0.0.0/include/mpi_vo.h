/*
 * Copyright © 2019 TSING MICRO.com.cn All Rights Reserved
 * Description: mpi_vo.h
 * Author:TsingMicro multimedia software group
 * Create: 2022/06/02
 */
#ifndef __MPI_VO_H__
#define __MPI_VO_H__

#include "ts_comm_vo.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* Device Relative Settings */
TS_S32 TS_MPI_VO_SetPubAttr(VO_DEV VoDev, const VO_PUB_ATTR_S *pstPubAttr);
TS_S32 TS_MPI_VO_GetPubAttr(VO_DEV VoDev, VO_PUB_ATTR_S *pstPubAttr);

TS_S32 TS_MPI_VO_Enable(VO_DEV VoDev);
TS_S32 TS_MPI_VO_Disable(VO_DEV VoDev);

/*
 *TS_S32 TS_MPI_VO_CloseFd(TS_VOID);
 *TS_S32 TS_MPI_VO_SetUserIntfSyncInfo(VO_DEV VoDev, VO_USER_INTFSYNC_INFO_S *pstUserInfo);
 */

/* Video Relative Settings */
TS_S32 TS_MPI_VO_SetVideoLayerAttr(VO_LAYER VoLayer, const VO_VIDEO_LAYER_ATTR_S *pstLayerAttr);
TS_S32 TS_MPI_VO_GetVideoLayerAttr(VO_LAYER VoLayer, VO_VIDEO_LAYER_ATTR_S *pstLayerAttr);

TS_S32 TS_MPI_VO_EnableVideoLayer(VO_LAYER VoLayer);
TS_S32 TS_MPI_VO_DisableVideoLayer(VO_LAYER VoLayer);

/*
 *TS_S32 TS_MPI_VO_BindVideoLayer(VO_LAYER VoLayer, VO_DEV VoDev);
 *TS_S32 TS_MPI_VO_UnBindVideoLayer(VO_LAYER VoLayer, VO_DEV VoDev);
 *
 *TS_S32 TS_MPI_VO_SetVideoLayerPriority(VO_LAYER VoLayer, TS_U32 u32Priority);
 *TS_S32 TS_MPI_VO_GetVideoLayerPriority(VO_LAYER VoLayer, TS_U32 *pu32Priority);
 */

TS_S32 TS_MPI_VO_SetVideoLayerCSC(VO_LAYER VoLayer, const VO_CSC_S *pstVideoCSC);
TS_S32 TS_MPI_VO_GetVideoLayerCSC(VO_LAYER VoLayer, VO_CSC_S *pstVideoCSC);
TS_S32 TS_MPI_VO_SetVideoLayerCrop(VO_LAYER VoLayer, const CROP_INFO_S *pstCropInfo);
TS_S32 TS_MPI_VO_GetVideoLayerCrop(VO_LAYER VoLayer, CROP_INFO_S *pstCropInfo);

/*
 *TS_S32 TS_MPI_VO_SetVideoLayerPartitionMode(VO_LAYER VoLayer, VO_PART_MODE_E enPartMode);
 *TS_S32 TS_MPI_VO_GetVideoLayerPartitionMode(VO_LAYER VoLayer, VO_PART_MODE_E *penPartMode);
 *
 *TS_S32 TS_MPI_VO_BatchBegin(VO_LAYER VoLayer);
 *TS_S32 TS_MPI_VO_BatchEnd(VO_LAYER VoLayer);
 *
 *TS_S32 TS_MPI_VO_SetVideoLayerBoundary(VO_LAYER VoLayer, const VO_LAYER_BOUNDARY_S *pstLayerBoundary);
 *TS_S32 TS_MPI_VO_GetVideoLayerBoundary(VO_LAYER VoLayer, VO_LAYER_BOUNDARY_S *pstLayerBoundary);
 *
 *TS_S32 TS_MPI_VO_SetVideoLayerParam(VO_LAYER VoLayer, const VO_LAYER_PARAM_S *pstLayerParam);
 *TS_S32 TS_MPI_VO_GetVideoLayerParam(VO_LAYER VoLayer, VO_LAYER_PARAM_S *pstLayerParam);
 *
 *TS_S32 TS_MPI_VO_SetVideoLayerDecompress(VO_LAYER VoLayer, TS_BOOL bSupportDecompress);
 *TS_S32 TS_MPI_VO_GetVideoLayerDecompress(VO_LAYER VoLayer, TS_BOOL *pbSupportDecompress);
 *
 *TS_S32 TS_MPI_VO_SetVideoLayerCrop(VO_LAYER VoLayer, const CROP_INFO_S *pstCropInfo);
 *TS_S32 TS_MPI_VO_GetVideoLayerCrop(VO_LAYER VoLayer, CROP_INFO_S *pstCropInfo);
 */

/* Display relative operations */
/*
 *TS_S32 TS_MPI_VO_SetPlayToleration(VO_LAYER VoLayer, TS_U32 u32Toleration);
 *TS_S32 TS_MPI_VO_GetPlayToleration(VO_LAYER VoLayer, TS_U32 *pu32Toleration);
 *
 *TS_S32 TS_MPI_VO_GetScreenFrame(VO_LAYER VoLayer, VIDEO_FRAME_INFO_S *pstVFrame, TS_S32 s32MilliSec);
 *TS_S32 TS_MPI_VO_ReleaseScreenFrame(VO_LAYER VoLayer, const VIDEO_FRAME_INFO_S *pstVFrame);
 */

TS_S32 TS_MPI_VO_SetDisplayBufLen(VO_LAYER VoLayer, TS_U32 u32BufLen);
TS_S32 TS_MPI_VO_GetDisplayBufLen(VO_LAYER VoLayer, TS_U32 *pu32BufLen);

/* Channel Relative Operations */
TS_S32 TS_MPI_VO_SetChnAttr(VO_LAYER VoLayer, VO_CHN VoChn, const VO_CHN_ATTR_S *pstChnAttr);
TS_S32 TS_MPI_VO_GetChnAttr(VO_LAYER VoLayer, VO_CHN VoChn, VO_CHN_ATTR_S *pstChnAttr);

TS_S32 TS_MPI_VO_EnableChn(VO_LAYER VoLayer, VO_CHN VoChn);
TS_S32 TS_MPI_VO_DisableChn(VO_LAYER VoLayer, VO_CHN VoChn);
void TS_MPI_VO_ACTIVE();

/*
 *TS_S32 TS_MPI_VO_SetChnParam(VO_LAYER VoLayer, VO_CHN VoChn, const VO_CHN_PARAM_S *pstChnParam);
 *TS_S32 TS_MPI_VO_GetChnParam(VO_LAYER VoLayer, VO_CHN VoChn, VO_CHN_PARAM_S *pstChnParam);
 *
 *TS_S32 TS_MPI_VO_SetChnDisplayPosition(VO_LAYER VoLayer, VO_CHN VoChn, const POINT_S *pstDispPos);
 *TS_S32 TS_MPI_VO_GetChnDisplayPosition(VO_LAYER VoLayer, VO_CHN VoChn, POINT_S *pstDispPos);
 *
 *TS_S32 TS_MPI_VO_SetChnFrameRate(VO_LAYER VoLayer, VO_CHN VoChn, TS_S32 s32ChnFrmRate);
 *TS_S32 TS_MPI_VO_GetChnFrameRate(VO_LAYER VoLayer, VO_CHN VoChn, TS_S32 *ps32ChnFrmRate);
 *
 *TS_S32 TS_MPI_VO_GetChnFrame(VO_LAYER VoLayer, VO_CHN VoChn, VIDEO_FRAME_INFO_S *pstFrame, TS_S32 s32MilliSec);
 *TS_S32 TS_MPI_VO_ReleaseChnFrame(VO_LAYER VoLayer, VO_CHN VoChn, const VIDEO_FRAME_INFO_S *pstFrame);
 *
 *TS_S32 TS_MPI_VO_PauseChn(VO_LAYER VoLayer, VO_CHN VoChn);
 *TS_S32 TS_MPI_VO_ResumeChn(VO_LAYER VoLayer, VO_CHN VoChn);
 *TS_S32 TS_MPI_VO_StepChn(VO_LAYER VoLayer, VO_CHN VoChn);
 *
 *TS_S32 TS_MPI_VO_RefreshChn(VO_LAYER VoLayer, VO_CHN VoChn);
 *
 *TS_S32 TS_MPI_VO_ShowChn(VO_LAYER VoLayer, VO_CHN VoChn);
 *TS_S32 TS_MPI_VO_HideChn(VO_LAYER VoLayer, VO_CHN VoChn);
 *
 *TS_S32 TS_MPI_VO_SetZoomInWindow(VO_LAYER VoLayer, VO_CHN VoChn, const VO_ZOOM_ATTR_S *pstZoomAttr);
 *TS_S32 TS_MPI_VO_GetZoomInWindow(VO_LAYER VoLayer, VO_CHN VoChn, VO_ZOOM_ATTR_S *pstZoomAttr);
 *
 *TS_S32 TS_MPI_VO_GetChnPTS(VO_LAYER VoLayer, VO_CHN VoChn, TS_U64 *pu64ChnPTS);
 *TS_S32 TS_MPI_VO_QueryChnStatus(VO_LAYER VoLayer, VO_CHN VoChn, VO_QUERY_STATUS_S *pstStatus);
 *
 *TS_S32 TS_MPI_VO_SendFrame(VO_LAYER VoLayer, VO_CHN VoChn, VIDEO_FRAME_INFO_S *pstVFrame, TS_S32 s32MilliSec);
 *
 *TS_S32 TS_MPI_VO_ClearChnBuf(VO_LAYER VoLayer, VO_CHN VoChn, TS_BOOL bClrAll);
 *
 *TS_S32 TS_MPI_VO_SetChnBorder(VO_LAYER VoLayer, VO_CHN VoChn, const VO_BORDER_S *pstBorder);
 *TS_S32 TS_MPI_VO_GetChnBorder(VO_LAYER VoLayer, VO_CHN VoChn, VO_BORDER_S *pstBorder);
 *
 *TS_S32 TS_MPI_VO_SetChnBoundary(VO_LAYER VoLayer, VO_CHN VoChn, const VO_CHN_BOUNDARY_S *pstChnBoundary);
 *TS_S32 TS_MPI_VO_GetChnBoundary(VO_LAYER VoLayer, VO_CHN VoChn, VO_CHN_BOUNDARY_S *pstChnBoundary);
 *
 *TS_S32 TS_MPI_VO_SetChnRecvThreshold(VO_LAYER VoLayer, VO_CHN VoChn, TS_U32 u32Threshold);
 *TS_S32 TS_MPI_VO_GetChnRecvThreshold(VO_LAYER VoLayer, VO_CHN VoChn, TS_U32 *pu32Threshold);
 *
 *TS_S32 TS_MPI_VO_SetChnRotation(VO_LAYER VoLayer, VO_CHN VoChn, ROTATION_E enRotation);
 *TS_S32 TS_MPI_VO_GetChnRotation(VO_LAYER VoLayer, VO_CHN VoChn, ROTATION_E *penRotation);
 *
 *TS_S32 TS_MPI_VO_GetChnRegionLuma(VO_LAYER VoLayer, VO_CHN VoChn, VO_REGION_INFO_S *pstRegionInfo,
 *                                  TS_U64 *pu64LumaData, TS_S32 s32MilliSec);
 */

/* WBC(Write Back Control) Relative Settings */
/*
 *TS_S32 TS_MPI_VO_SetWBCSource(VO_WBC VoWBC, const VO_WBC_SOURCE_S *pstWBCSource);
 *TS_S32 TS_MPI_VO_GetWBCSource(VO_WBC VoWBC, VO_WBC_SOURCE_S *pstWBCSources);
 *
 *TS_S32 TS_MPI_VO_SetWBCAttr(VO_WBC VoWBC, const VO_WBC_ATTR_S *pstWBCAttr);
 *TS_S32 TS_MPI_VO_GetWBCAttr(VO_WBC VoWBC, VO_WBC_ATTR_S *pstWBCAttr);
 *
 *TS_S32 TS_MPI_VO_EnableWBC(VO_WBC VoWBC);
 *TS_S32 TS_MPI_VO_DisableWBC(VO_WBC VoWBC);
 *
 *TS_S32 TS_MPI_VO_SetWBCMode(VO_WBC VoWBC, VO_WBC_MODE_E enWBCMode);
 *TS_S32 TS_MPI_VO_GetWBCMode(VO_WBC VoWBC, VO_WBC_MODE_E *penWBCMode);
 *
 *TS_S32 TS_MPI_VO_SetWBCDepth(VO_WBC VoWBC, TS_U32 u32Depth);
 *TS_S32 TS_MPI_VO_GetWBCDepth(VO_WBC VoWBC, TS_U32 *pu32Depth);
 *
 *TS_S32 TS_MPI_VO_GetWBCFrame(VO_WBC VoWBC, VIDEO_FRAME_INFO_S *pstVFrame, TS_S32 s32MilliSec);
 *TS_S32 TS_MPI_VO_ReleaseWBCFrame(VO_WBC VoWBC, const VIDEO_FRAME_INFO_S *pstVFrame);
 */

/* Graphic Relative Settings */
/*
 *TS_S32 TS_MPI_VO_BindGraphicLayer(GRAPHIC_LAYER GraphicLayer, VO_DEV VoDev);
 *TS_S32 TS_MPI_VO_UnBindGraphicLayer(GRAPHIC_LAYER GraphicLayer, VO_DEV VoDev);
 *
 *TS_S32 TS_MPI_VO_SetGraphicLayerCSC(GRAPHIC_LAYER GraphicLayer, const VO_CSC_S *pstCSC);
 *TS_S32 TS_MPI_VO_GetGraphicLayerCSC(GRAPHIC_LAYER GraphicLayer, VO_CSC_S *pstCSC);
 *
 *TS_S32 TS_MPI_VO_SetDevFrameRate(VO_DEV VoDev, TS_U32 u32FrameRate);
 *TS_S32 TS_MPI_VO_GetDevFrameRate(VO_DEV VoDev, TS_U32 *pu32FrameRate);
 */

/* Module Parameter Settings */
/*
 *TS_S32 TS_MPI_VO_SetModParam(const VO_MOD_PARAM_S *pstModParam);
 *TS_S32 TS_MPI_VO_GetModParam(VO_MOD_PARAM_S *pstModParam);
 *TS_S32 TS_MPI_VO_SetVtth(VO_DEV VoDev, TS_U32 u32Vtth);
 *TS_S32 TS_MPI_VO_GetVtth(VO_DEV VoDev, TS_U32 *pu32Vtth);
 *TS_S32 TS_MPI_VO_SetVtth2(VO_DEV VoDev, TS_U32 u32Vtth);
 *TS_S32 TS_MPI_VO_GetVtth2(VO_DEV VoDev, TS_U32 *pu32Vtth);
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __MPI_VO_H__ */


