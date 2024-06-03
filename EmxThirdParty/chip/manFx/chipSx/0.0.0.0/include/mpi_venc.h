/*
 * Copyright © 2019 TSING MICRO.com.cn All Rights Reserved
 * Description: mpi_venc.h
 * Author:TsingMicro multimedia software group
 * Create: 2022/06/02
 */
#ifndef __MPI_VENC_H__
#define __MPI_VENC_H__

#include "ts_comm_venc.h"

TS_S32 TS_MPI_VENC_CreateChn(VENC_CHN VeChn, const VENC_CHN_ATTR_S * pstAttr);
TS_S32 TS_MPI_VENC_DestroyChn(VENC_CHN VeChn);
TS_S32 TS_MPI_VENC_StartRecvFrame(VENC_CHN VeChn, const VENC_RECV_PIC_PARAM_S *pstRecvParam);
TS_S32 TS_MPI_VENC_StopRecvFrame(VENC_CHN VeChn);
TS_S32 TS_MPI_VENC_QueryStatus(VENC_CHN VeChn, VENC_CHN_STATUS_S *pstStatus);
ts_s32 TS_MPI_VENC_GetFd(VENC_CHN venc_chn);
int TS_MPI_VENC_SELECT(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
ts_s32 TS_MPI_VENC_GetStream(VENC_CHN venc_chn, VENC_STREAM_S *stream, ts_s32 milli_sec);
ts_s32 TS_MPI_VENC_SetChnAttr(VENC_CHN venc_chn, const VENC_CHN_ATTR_S *pstAttr);

TS_S32 TS_MPI_VENC_GetChnAttr(VENC_CHN VeChn, VENC_CHN_ATTR_S* pstChnAttr);
//TS_S32 TS_MPI_VENC_SetRcParam(VENC_CHN VeChn, const VENC_RC_PARAM_S *pstRcParam);
//TS_S32 TS_MPI_VENC_GetRcParam(VENC_CHN VeChn, VENC_RC_PARAM_S *pstRcParam);
TS_S32 TS_MPI_VENC_SetSmartParam(VENC_CHN venc_chn, const uint8_t *is_enable);
TS_S32 TS_MPI_VENC_GetSmartParam(VENC_CHN venc_chn, uint8_t *is_enable);
TS_S32 TS_MPI_VENC_SetMotionDetectParam(VENC_CHN venc_chn, const VENC_MOTION_DETECT_RESULT_S *motiondetect_param);

TS_S32 TS_MPI_VENC_ReleaseStream(VENC_CHN VeChn, VENC_STREAM_S *pstStream);
TS_S32 TS_MPI_VENC_SendFrame(VENC_CHN VeChn, const VIDEO_FRAME_INFO_S *pstFrame, TS_S32 s32MilliSec);

TS_S32 TS_MPI_VENC_SetRoiAttr (VENC_CHN VeChn, const VENC_ROI_ATTR_S *pstRoiAttr);
TS_S32 TS_MPI_VENC_GetRoiAttr(VENC_CHN VeChn, TS_U32 u32Index, VENC_ROI_ATTR_S *pstRoiAttr);

TS_S32 TS_MPI_VENC_RequestIDR(VENC_CHN VeChn, TS_BOOL bInstant);

#endif /* __MPI_VENC_H__ */
