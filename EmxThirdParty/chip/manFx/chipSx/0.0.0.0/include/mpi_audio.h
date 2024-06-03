/*
 * Copyright © 2019 TSING MICRO.com.cn All Rights Reserved
 * Description: mpi_audio.h
 * Author:TsingMicro multimedia software group
 * Create: 2022/06/02
 */
#ifndef __MPI_AUDIO_H__
#define __MPI_AUDIO_H__

#include "ts_type.h"
#include "ts_common.h"
#include "ts_comm_aio.h"
#include "ts_comm_aenc.h"
#include "ts_comm_adec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

#define AENC_ADAPT_MAGIC 0Xfcfcfcfc


/* AI function api. */
TS_S32 TS_MPI_AI_SetPubAttr(AUDIO_DEV AiDevId, const AIO_ATTR_S *pstAttr);
TS_S32 TS_MPI_AI_GetPubAttr(AUDIO_DEV AiDevId, AIO_ATTR_S *pstAttr);

TS_S32 TS_MPI_AI_Enable(AUDIO_DEV AiDevId);
TS_S32 TS_MPI_AI_Disable(AUDIO_DEV AiDevId);

TS_S32 TS_MPI_AI_EnableChn(AUDIO_DEV AiDevId, AI_CHN AiChn);
TS_S32 TS_MPI_AI_DisableChn(AUDIO_DEV AiDevId, AI_CHN AiChn);

TS_S32 TS_MPI_AI_GetFrame(AUDIO_DEV AiDevId, AI_CHN AiChn, AUDIO_FRAME_S *pstFrm, AEC_FRAME_S *pstAecFrm, TS_S32 s32MilliSec);
TS_S32 TS_MPI_AI_ReleaseFrame(AUDIO_DEV AiDevId, AI_CHN AiChn, const AUDIO_FRAME_S *pstFrm, const AEC_FRAME_S *pstAecFrm);

TS_S32 TS_MPI_AI_SetTalkVqeAttr(AUDIO_DEV AiDevId, AI_CHN AiChn, AUDIO_DEV AoDevId, AO_CHN AoChn, const AI_TALKVQE_CONFIG_S *pstVqeConfig);
TS_S32 TS_MPI_AI_GetTalkVqeAttr(AUDIO_DEV AiDevId, AI_CHN AiChn, AI_TALKVQE_CONFIG_S *pstVqeConfig);

TS_S32 TS_MPI_AI_EnableVqe(AUDIO_DEV AiDevId, AI_CHN AiChn);
TS_S32 TS_MPI_AI_DisableVqe(AUDIO_DEV AiDevId, AI_CHN AiChn);
TS_S32 TS_MPI_AI_EnableReSmp(AUDIO_DEV AiDevId, AI_CHN AiChn, AUDIO_SAMPLE_RATE_E enOutSampleRate);
TS_S32 TS_MPI_AI_DisableReSmp(AUDIO_DEV AiDevId, AI_CHN AiChn);

TS_S32 TS_MPI_AI_GetFd(AUDIO_DEV AiDevId, AI_CHN AiChn);
TS_S32 TS_MPI_AI_SELECT(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);

TS_S32 TS_MPI_AI_SetVqeVolume(AUDIO_DEV AiDevId, AI_CHN AiChn, TS_S32 s32VolumeDb);
TS_S32 TS_MPI_AI_GetVqeVolume(AUDIO_DEV AiDevId, AI_CHN AiChn, TS_S32 *ps32VolumeDb);


/* AO function api. */
TS_S32 TS_MPI_AO_SetPubAttr(AUDIO_DEV AoDevId, const AIO_ATTR_S *pstAttr);
TS_S32 TS_MPI_AO_GetPubAttr(AUDIO_DEV AoDevId, AIO_ATTR_S *pstAttr);

TS_S32 TS_MPI_AO_Enable(AUDIO_DEV AoDevId);
TS_S32 TS_MPI_AO_Disable(AUDIO_DEV AoDevId);

TS_S32 TS_MPI_AO_EnableChn(AUDIO_DEV AoDevId, AO_CHN AoChn);
TS_S32 TS_MPI_AO_DisableChn(AUDIO_DEV AoDevId, AO_CHN AoChn);
TS_S32 TS_MPI_AO_EnableReSmp(AUDIO_DEV AoDevId, AO_CHN AoChn, AUDIO_SAMPLE_RATE_E enInSampleRate);
TS_S32 TS_MPI_AO_DisableReSmp(AUDIO_DEV AoDevId, AO_CHN AoChn);

TS_S32 TS_MPI_AO_SendFrame(AUDIO_DEV AoDevId, AO_CHN AoChn, const AUDIO_FRAME_S *pstData, TS_S32 s32MilliSec);

TS_S32 TS_MPI_AO_SetVolume(AUDIO_DEV AoDevId, TS_S32 s32VolumeDb);
TS_S32 TS_MPI_AO_GetVolume(AUDIO_DEV AoDevId, TS_S32 *ps32VolumeDb);

TS_S32 TS_MPI_AO_SetMute(AUDIO_DEV AoDevId, TS_BOOL bEnable, const AUDIO_FADE_S *pstFade);
TS_S32 TS_MPI_AO_GetMute(AUDIO_DEV AoDevId, TS_BOOL *pbEnable, AUDIO_FADE_S *pstFade);


/* AENC function api. */
TS_S32 TS_MPI_AENC_CreateChn(AENC_CHN AeChn, const AENC_CHN_ATTR_S *pstAttr);
TS_S32 TS_MPI_AENC_DestroyChn(AENC_CHN AeChn);

TS_S32 TS_MPI_AENC_GetStream(AENC_CHN AeChn, AUDIO_STREAM_S *pstStream, TS_S32 s32MilliSec);
TS_S32 TS_MPI_AENC_ReleaseStream(AENC_CHN AeChn, const AUDIO_STREAM_S *pstStream);

TS_S32 TS_MPI_AENC_GetFd(AENC_CHN AeChn);

TS_S32 TS_MPI_AENC_SELECT(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
TS_S32 TS_MPI_AENC_SendFrame(AENC_CHN AeChn, const AUDIO_FRAME_S *pstFrm, const AEC_FRAME_S *pstAecFrm);

/* ADEC function api. */
TS_S32 TS_MPI_ADEC_CreateChn(ADEC_CHN AdChn, const ADEC_CHN_ATTR_S *pstAttr);
TS_S32 TS_MPI_ADEC_DestroyChn(ADEC_CHN AdChn);

TS_S32 TS_MPI_ADEC_SendStream(ADEC_CHN AdChn, const AUDIO_STREAM_S *pstStream, TS_BOOL bBlock);
TS_S32 TS_MPI_ADEC_SendEndOfStream(ADEC_CHN AdChn, TS_BOOL bInstant);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __MPI_AI_H__ */

