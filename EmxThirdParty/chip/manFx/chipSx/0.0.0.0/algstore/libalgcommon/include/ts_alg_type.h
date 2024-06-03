/*
 * Copyright (c) Beijing Tsing Micro Co.,Ltd. All rights reserved.
 * Description: base type definitions of alg
 * Author: Tsing Micro solution-application group
 * Create: 2022/08/18
 */
#ifndef _TS_ALG_TYPE_H_
#define _TS_ALG_TYPE_H_

#ifdef __KERNEL__

#include <linux/types.h>
#else

#include <stdint.h>
#include <stdbool.h>

#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef unsigned char TS_UCHAR;
typedef unsigned char TS_U8;
typedef unsigned short TS_U16;
typedef unsigned int TS_U32;
typedef unsigned long TS_UL;
typedef TS_UL TS_ULONG;
typedef uintptr_t TS_UINTPTR_T;

typedef char TS_CHAR;
typedef signed char TS_S8;
typedef short TS_S16;
typedef int TS_S32;
typedef long TS_SL;

typedef float TS_FLOAT;
typedef double TS_DOUBLE;

#ifndef _M_IX86
typedef unsigned long long TS_U64;
typedef long long TS_S64;
#else
typedef unsigned __int64 TS_U64;
typedef __int64 TS_S64;
#endif

typedef unsigned long TS_SIZE_T;
typedef unsigned long TS_LENGTH_T;
typedef unsigned long int TS_PHYS_ADDR_T;

typedef unsigned int TS_HANDLE;
//typedef bool TS_BOOL;

#if __has_include("ts_type.h")

#else

#ifndef TS_FALSE
#define TS_FALSE 0
#endif

#ifndef TS_TRUE
#define TS_TRUE 1
#endif

#endif

#ifndef NULL
#define NULL 0L
#endif

#ifndef TS_NULL
#define TS_NULL 0L
#endif

#ifndef TS_SUCCESS
#define TS_SUCCESS 0
#endif

#ifndef TS_FAILURE
#define TS_FAILURE (-1)
#endif

#ifndef TS_VOID
#define TS_VOID void
#endif

#ifndef TS_UNUSED
#define TS_UNUSED(x) ((x) = (x))
#endif

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef ABS
#define ABS(a) ((a) >= (0) ? (a) : (-a))
#endif

#define ROUNDF(a) ((int)((a) + 0.5f))

#define ALG_FLT_EPSILON        1.192092896e-07F        // smallest such that 1.0+FLT_EPSILON != 1.0
#define ALG_INT_MIN            (-2147483647 - 1)
#define ALG_INT_MAX            2147483647
#define ALG_DBL_EPSILON        2.2204460492503131e-016
#define ALG_NAN                2.2204460492503131e+016
#define ALG_DBL_MIN            2.2250738585072014e-308 // min positive value
#define ALG_FLT_MIN            1.175494351e-38F        // min normalized positive value

TS_U32 getSysTime();
#define TIME_CACL_GET()    (getSysTime())
#define TIME_CACL_DELTA(msg, time)  printf("TIME_CACL_GET : %s  : %d ms\r\n", (msg), time)

TS_U32 getSysTimeUs(void);

typedef struct tsALG_FLOAT_POINT_S{
    TS_FLOAT f32X;
    TS_FLOAT f32Y;
}ALG_FLOAT_POINT_S;

typedef struct tsALG_INT_POINT_S{
    TS_S32 s32X;
    TS_S32 s32Y;
}ALG_INT_POINT_S;

typedef struct tsALG_SIZE_S{
    TS_S32 s32Width;
    TS_S32 s32Height;
}ALG_SIZE_S;

typedef struct tsALG_IMAGE_S{
    TS_S32 s32H;
    TS_S32 s32W;
    TS_S32 s32C;
    TS_VOID *pData;
#if (defined(TX521X) || defined(TX5112X) || defined(TX5215) || defined(TX5239))
    TS_U32 pDataPhy;
#endif    
#if (defined(TX5368X))
    TS_U64 pDataPhy;
#endif    
}ALG_IMAGE_S;

typedef struct tsALG_IMAGES_S{
    ALG_IMAGE_S images[2];
}ALG_IMAGES_S;

typedef struct tsALG_SUB_IMAGE_S{
    TS_S32 s32X;
    TS_S32 s32Y;
    TS_S32 s32W;
    TS_S32 s32H;
}ALG_SUB_IMAGE_S;

typedef enum tsALG_IMAGE_TYPE_E {
	ALG_IMAGE_TYPE_FLOAT32_HWC,
	ALG_IMAGE_TYPE_FLOAT32_CHW,
	ALG_IMAGE_TYPE_INT8_HWC,
	ALG_IMAGE_TYPE_INT8_CHW,
	ALG_IMAGE_TYPE_FORMAT_0RGB,
	ALG_IMAGE_TYPE_FORMAT_GREY
} ALG_IMAGE_TYPE_E;


typedef struct tsALG_MODEL_INIT_S{
    TS_U8   *pGraph;                       // mode graph address
    TS_U8   *pWeight;                      // mode weight address
// #if (defined(TX521X) || defined(TX5112X) || defined(TX5215) || defined(TX5239))
//     TS_U32  pGraphPhy;                     // mode graph Physical address
//     TS_U32  pWeightPhy;                    // mode weight Physical address
// #endif
// #if (defined(TX5368X))
//     TS_U64  pGraphPhy;                     // mode graph Physical address
//     TS_U64  pWeightPhy;                    // mode weight Physical address
// #endif
    TS_U32  u32GraphSize;				   // mode graph size
    TS_U32  u32WeightSize;				   // mode weight size
    ALG_IMAGE_TYPE_E eImageType;           // input image type (RGBA, gray, yuv ...)
    TS_VOID *vpSelfBuf;                    // blob mem
    TS_U8 bRneOff;					       // open or not open RNE device, revent multiple operations
} ALG_MODEL_INIT_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* _TS_ALG_TYPE__H_ */
