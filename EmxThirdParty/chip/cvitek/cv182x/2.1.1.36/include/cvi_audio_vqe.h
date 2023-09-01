/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: include/cvi_audio_vqe.h
 * Description: audio vqe function define
 */
#ifndef __CVI_AUDIO_VQE_H__
#define __CVI_AUDIO_VQE_H__
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "cvi_type.h"
#include "cvi_comm_aio.h"
/* Extern variable for cvi_audio_interface.c */
typedef struct _AI_CHANNEL_VQE_CONFIG {
	AI_RECORDVQE_CONFIG_S reocrdvqe;
	AI_TALKVQE_CONFIG_S   talkvqe;
	CVI_BOOL benablevqe;
} AI_CHANNEL_VQE_CONFIG;


typedef struct _AO_VQE_CONFIG {
	AI_TALKVQE_CONFIG_S talkvqe;
	AI_RECORDVQE_CONFIG_S reocrdvqe;
	AO_VQE_CONFIG_S  stVqeConf;
	CVI_BOOL bAoVqeEnable;
	CVI_S32 s32BytesPerSample;
} AO_VQE_CONFIG;


typedef struct _AI_VQE_CONFIG {
	/* AI_CHANNEL_VQE_CONFIG  chnvqe[MAX_CHANNEL_NUM]; */
	AI_TALKVQE_CONFIG_S talkvqe;
	AI_RECORDVQE_CONFIG_S reocrdvqe;
	CVI_BOOL bEnableRefAEC;
	CVI_BOOL bAiVqeEnable;
	CVI_S32 s32BytesPerSample;
} AI_VQE_CONFIG;


extern AUDIO_VQE_REGISTER_S gstVqeReg;
//extern AI_VQE_CONFIG gstAiVQE;
extern AO_VQE_CONFIG gstAoVQE;
extern CVI_S32 s32CurrAiSampleRate;
extern CVI_S32 s32CurrAoutSampleRate;


CVI_S32 CVI_AudIn_AlgoInit(const AI_TALKVQE_CONFIG_S *pstVqeConfig);
CVI_S32 CVI_AudIn_AlgoProcess_AnrAgc(CVI_CHAR *datain, CVI_CHAR *dataout,
	 CVI_S32 s32SizeInBytes,
	 CVI_S32 *s32SizeOutBytes);
CVI_S32 CVI_AudIn_AlgoProcess_ExtAEC(CVI_CHAR *datain, CVI_CHAR *dataout,
	CVI_S32 s32SizeInBytes,
	CVI_S32 *s32SizeOutBytes);
CVI_S32 CVI_AudIn_AlgoDeInit(void);

CVI_S32 CVI_AudOut_AlgoInit(const AI_TALKVQE_CONFIG_S *pstVqeConfig);
CVI_S32 CVI_AudOut_AlgoProcess(CVI_CHAR *datain, CVI_CHAR *dataout,
	 CVI_S32 s32SizeInBytes,
	 CVI_S32 *s32SizeOutBytes);
CVI_S32 CVI_AudOut_AlgoDeInit(void);
CVI_BOOL CVI_AI_VQECheckEnable(AUDIO_DEV AiDevId, AI_CHN AiChn);
CVI_S32 CVI_AI_VQECheckFlag(AUDIO_DEV AiDevId, AI_CHN AiChn);
CVI_BOOL CVI_AO_VQECheckEnable(AUDIO_DEV AoDevId, AO_CHN AoChn);

#ifdef __cplusplus
}
#endif
#endif

