/*
 * Copyright © 2019 TSING MICRO.com.cn All Rights Reserved
 * Description: mpi_vi.h
 * Author:TsingMicro multimedia software group
 * Create: 2022/06/02
 */

#ifndef __MPI_VI_H__
#define __MPI_VI_H__

#include "ts_comm_vi.h"
#include "ts_comm_dis.h"
#include "ts_comm_gdc.h"
#include "ts_comm_vb.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

TS_S32 TS_MPI_VI_CreatePipe(VI_PIPE ViPipe, const VI_PIPE_ATTR_S *pstPipeAttr);
TS_S32 TS_MPI_VI_DestroyPipe(VI_PIPE ViPipe);

TS_S32 TS_MPI_VI_SetPipeAttr(VI_PIPE ViPipe, const VI_PIPE_ATTR_S *pstPipeAttr);
TS_S32 TS_MPI_VI_GetPipeAttr(VI_PIPE ViPipe, VI_PIPE_ATTR_S *pstPipeAttr);

TS_S32 TS_MPI_VI_StartPipe(VI_PIPE ViPipe);
TS_S32 TS_MPI_VI_StopPipe(VI_PIPE ViPipe);

TS_S32 TS_MPI_VI_QueryPipeStatus(VI_PIPE ViPipe, VI_PIPE_STATUS_S *pstStatus);

TS_S32 TS_MPI_VI_SetChnAttr(VI_PIPE ViPipe, VI_CHN ViChn, const VI_CHN_ATTR_S *pstChnAttr);
TS_S32 TS_MPI_VI_GetChnAttr(VI_PIPE ViPipe, VI_CHN ViChn, VI_CHN_ATTR_S *pstChnAttr);

TS_S32 TS_MPI_VI_EnableChn(VI_PIPE ViPipe, VI_CHN ViChn);
TS_S32 TS_MPI_VI_DisableChn(VI_PIPE ViPipe, VI_CHN ViChn);

TS_S32 TS_MPI_VI_GetChnFrame(VI_PIPE ViPipe, VI_CHN ViChn, VIDEO_FRAME_INFO_S *pstFrameInfo, TS_S32 s32MilliSec);
TS_S32 TS_MPI_VI_ReleaseChnFrame(VI_PIPE ViPipe, VI_CHN ViChn, const VIDEO_FRAME_INFO_S *pstFrameInfo);

TS_S32 TS_MPI_VI_QueryChnStatus(VI_PIPE ViPipe, VI_CHN ViChn, VI_CHN_STATUS_S *pstChnStatus);

/*
非标接口
mode ：0:day  1:night
*/
TS_S32 TS_MPI_VI_SetDayNight(VI_PIPE ViPipe, TS_S32 mode);
TS_S32 TS_MPI_VI_GetDayNight(VI_PIPE ViPipe, TS_S32 * pMode);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __MPI_VI_H__ */


