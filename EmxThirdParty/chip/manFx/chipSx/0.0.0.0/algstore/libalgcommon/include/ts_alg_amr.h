/*
 * Copyright (c) Beijing Tsing Micro Co.,Ltd. All rights reserved.
 * Description: base type definitions of alg
 * Author: Tsing Micro solution-application group
 * Create: 2022/08/18
 */
#ifndef _TS_ALG_AMR_H_
#define _TS_ALG_AMR_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#include "ts_alg_type.h"

typedef enum tsALG_RGB_TYPE_E {
	ALG_RGB_TYPE_RGB24,
	ALG_RGB_TYPE_RGBA32,
} ALG_RGB_TYPE_E;


typedef TS_S32 (*TS_ALG_ResizeFunc)(const TS_U8 *pSrc, TS_U8 *pDst, TS_S32 s32SrcW, TS_S32 s32SrcH,
											TS_S32 s32DstW, TS_S32 s32DstH, TS_S32 s32Channels);

TS_S32 TS_ALG_ResizeCbk_Register(TS_ALG_ResizeFunc fCbk);

TS_VOID TS_ALG_ResizeCbk_Unregister(TS_VOID);

TS_S32 TS_ALG_Resize(const TS_U8 *pSrc, TS_U8 *pDst, TS_S32 s32SrcW, TS_S32 s32SrcH, TS_S32 s32DstW, TS_S32 s32DstH, TS_S32 s32Channels);

// #if (-march==armv7-a)
#if 1
// u32ImgW must multiply of 4
TS_VOID TS_ALG_YUV2RGB(TS_U8 *pY, TS_U8 *pUv, TS_U8 *pRgb, TS_U32 u32SrcW, TS_U32 u32SrcH, TS_U32 u32ImgW, TS_U32 u32ImgH, ALG_RGB_TYPE_E eRgb);
#else
TS_VOID TS_ALG_YUV2RGB(TS_U8 *pYuv, TS_U8 *pRgb, TS_U32 u32ImgW, TS_U32 u32ImgH);
#endif

extern TS_S32 TS_MPI_TRP_CVE_Resize(TS_U64 srcPhy, TS_VOID *src, TS_U64 dstPhy, TS_VOID *dst,
										TS_S32 srcW, TS_S32 srcH, TS_S32 subW, TS_S32 subH,
										TS_S32 dstW, TS_S32 dstH, TS_S32 channels);

extern TS_S32 TS_MPI_TRP_CVE_Init(TS_VOID);

extern TS_S32 TS_MPI_TRP_CVE_Exit(TS_VOID);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* _TS_ALG_AMR__H_ */
