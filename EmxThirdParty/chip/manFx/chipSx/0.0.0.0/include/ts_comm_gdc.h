/*
 * Copyright © 2019 TSING MICRO.com.cn All Rights Reserved
 * Description: ts_comm_gdc.h
 * Author:TsingMicro multimedia software group
 * Create: 2022/06/02
 */

#include "ts_type.h"
#include "ts_common.h"
#include "ts_errno.h"
#include "ts_comm_video.h"

#ifndef __TS_COMM_GDC_H__
#define __TS_COMM_GDC_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* failure caused by malloc buffer */
#define TS_ERR_GDC_NOBUF         TS_DEF_ERR(TS_ID_GDC, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
#define TS_ERR_GDC_BUF_EMPTY     TS_DEF_ERR(TS_ID_GDC, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
#define TS_ERR_GDC_NULL_PTR      TS_DEF_ERR(TS_ID_GDC, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
#define TS_ERR_GDC_ILLEGAL_PARAM TS_DEF_ERR(TS_ID_GDC, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
#define TS_ERR_GDC_BUF_FULL      TS_DEF_ERR(TS_ID_GDC, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_FULL)
#define TS_ERR_GDC_SYS_NOTREADY  TS_DEF_ERR(TS_ID_GDC, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
#define TS_ERR_GDC_NOT_SUPPORT   TS_DEF_ERR(TS_ID_GDC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
#define TS_ERR_GDC_NOT_PERMITTED TS_DEF_ERR(TS_ID_GDC, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
#define TS_ERR_GDC_BUSY          TS_DEF_ERR(TS_ID_GDC, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
#define TS_ERR_GDC_INVALID_CHNID TS_DEF_ERR(TS_ID_GDC, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
#define TS_ERR_GDC_CHN_UNEXIST   TS_DEF_ERR(TS_ID_GDC, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)

#define FISHEYE_MAX_REGION_NUM   4
#define FISHEYE_LMFCOEF_NUM      128
#define GDC_PMFCOEF_NUM          9

typedef TS_S32 GDC_HANDLE;

typedef struct tsGDC_TASK_ATTR_S {
    VIDEO_FRAME_INFO_S stImgIn; /* Input picture */
    VIDEO_FRAME_INFO_S stImgOut; /* Output picture */
    TS_U64 au64privateData[4]; /* RW; Private data of task */
    TS_U64 reserved; /* RW; Debug information,state of current picture */
} GDC_TASK_ATTR_S;

/* Mount mode of device */
typedef enum tsFISHEYE_MOUNT_MODE_E {
    FISHEYE_DESKTOP_MOUNT = 0, /* Desktop mount mode */
    FISHEYE_CEILING_MOUNT = 1, /* Ceiling mount mode */
    FISHEYE_WALL_MOUNT = 2, /* wall mount mode */

    FISHEYE_MOUNT_MODE_BUTT
} FISHEYE_MOUNT_MODE_E;

/* View mode of client */
typedef enum tsFISHEYE_VIEW_MODE_E {
    FISHEYE_VIEW_360_PANORAMA = 0, /* 360 panorama mode of gdc correction */
    FISHEYE_VIEW_180_PANORAMA = 1, /* 180 panorama mode of gdc correction */
    FISHEYE_VIEW_NORMAL = 2, /* normal mode of gdc correction */
    FISHEYE_NO_TRANSFORMATION = 3, /* no gdc correction */

    FISHEYE_VIEW_MODE_BUTT
} FISHEYE_VIEW_MODE_E;

/* Fisheye region correction attribute */
typedef struct tsFISHEYE_REGION_ATTR_S {
    FISHEYE_VIEW_MODE_E enViewMode; /* RW; Range: [0, 3];gdc view mode */
    TS_U32 u32InRadius; /* RW; inner radius of gdc correction region */
    TS_U32 u32OutRadius; /* RW; out radius of gdc correction region */
    TS_U32 u32Pan; /* RW; Range: [0, 360] */
    TS_U32 u32Tilt; /* RW; Range: [0, 360] */
    TS_U32 u32HorZoom; /* RW; Range: [1, 4095] */
    TS_U32 u32VerZoom; /* RW; Range: [1, 4095] */
    RECT_S stOutRect; /* RW; out Imge rectangle attribute */
} FISHEYE_REGION_ATTR_S;

typedef struct tsFISHEYE_REGION_ATTR_EX_S {
    FISHEYE_VIEW_MODE_E enViewMode; /* RW; Range: [0, 3];gdc view mode */
    TS_U32 u32InRadius; /* RW; inner radius of gdc correction region */
    TS_U32 u32OutRadius; /* RW; out radius of gdc correction region */
    TS_U32 u32X; /* RW; Range: [0, 4608] */
    TS_U32 u32Y; /* RW; Range: [0, 3456] */
    TS_U32 u32HorZoom; /* RW; Range: [1, 4095] */
    TS_U32 u32VerZoom; /* RW; Range: [1, 4095] */
    RECT_S stOutRect; /* RW; out Imge rectangle attribute */
} FISHEYE_REGION_ATTR_EX_S;

/* Fisheye all regions correction attribute */
typedef struct tsFISHEYE_ATTR_S {
    TS_BOOL bEnable; /* RW; Range: [0, 1];whether enable fisheye correction or not */
    TS_BOOL bLMF; /* RW; Range: [0, 1];whether gdc len's LMF coefficient is from user config or
                     from default linear config */
    TS_BOOL bBgColor; /* RW; Range: [0, 1];whether use background color or not */
    TS_U32 u32BgColor; /* RW; Range: [0,0xffffff];the background color RGB888 */

    TS_S32 s32HorOffset; /* RW; Range: [-511, 511];the horizontal offset between image center and
                            physical center of len */
    TS_S32 s32VerOffset; /* RW; Range: [-511, 511]; the vertical offset between image center and
                            physical center of len */

    TS_U32 u32TrapezoidCoef; /* RW; Range: [0, 32];strength coefficient of trapezoid correction */
    TS_S32 s32FanStrength; /* RW; Range: [-760, 760];strength coefficient of fan correction */

    FISHEYE_MOUNT_MODE_E enMountMode; /* RW; Range: [0, 2];gdc mount mode */

    TS_U32 u32RegionNum; /* RW; Range: [1, 4]; gdc correction region number */
    FISHEYE_REGION_ATTR_S astFishEyeRegionAttr[FISHEYE_MAX_REGION_NUM]; /* RW; attribution of gdc correction region */
} FISHEYE_ATTR_S;

typedef struct tsFISHEYE_ATTR_EX_S {
    TS_BOOL bEnable; /* RW; Range: [0, 1];whether enable fisheye correction or not */
    TS_BOOL bLMF; /* RW; Range: [0, 1];whether gdc len's LMF coefficient is from user config or
                     from default linear config */
    TS_BOOL bBgColor; /* RW; Range: [0, 1];whether use background color or not */
    TS_U32 u32BgColor; /* RW; Range: [0,0xffffff];the background color RGB888 */

    TS_S32 s32HorOffset; /* RW; Range: [-511, 511];the horizontal offset between image center and
                            physical center of len */
    TS_S32 s32VerOffset; /* RW; Range: [-511, 511]; the vertical offset between image center and
                            physical center of len */

    TS_U32 u32TrapezoidCoef; /* RW; Range: [0, 32];strength coefficient of trapezoid correction */
    TS_S32 s32FanStrength; /* RW; Range: [-760, 760];strength coefficient of fan correction */

    FISHEYE_MOUNT_MODE_E enMountMode; /* RW; Range: [0, 2];gdc mount mode */

    TS_U32 u32RegionNum; /* RW; Range: [1, 4]; gdc correction region number */
    /* RW; attribution of gdc correction region */
    FISHEYE_REGION_ATTR_EX_S astFishEyeRegionAttr[FISHEYE_MAX_REGION_NUM];
} FISHEYE_ATTR_EX_S;

/* Spread correction attribute */
typedef struct tsSPREAD_ATTR_S {
    TS_BOOL bEnable; /* RW; Range: [0, 1];whether enable spread or not,
                        When spread on,ldc DistortionRatio range should be [0, 500] */
    TS_U32 u32SpreadCoef; /* RW; Range: [0, 18];strength coefficient of spread correction */
    SIZE_S stDestSize; /* RW; dest size of spread */
} SPREAD_ATTR_S;

/* Fisheye Job Config */
typedef struct tsFISHEYE_JOB_CONFIG_S {
    TS_U64 u64LenMapPhyAddr; /* LMF coefficient Physic Addr */
} FISHEYE_JOB_CONFIG_S;

/* Fisheye Config */
typedef struct tsFISHEYE_CONFIG_S {
    TS_U16 au16LMFCoef[FISHEYE_LMFCOEF_NUM]; /* RW;  LMF coefficient of gdc len */
} FISHEYE_CONFIG_S;

/* Gdc PMF Attr */
typedef struct tsGDC_PMF_ATTR_S {
    TS_S64 as64PMFCoef[GDC_PMFCOEF_NUM]; /* W;  PMF coefficient of gdc */
} GDC_PMF_ATTR_S;

/* Gdc FISHEYE POINT QUERY Attr */
typedef struct tsGDC_FISHEYE_POINT_QUERY_ATTR_S {
    TS_U32 u32RegionIndex;
    FISHEYE_ATTR_S *pstFishEyeAttr;
    TS_U16 au16LMF[FISHEYE_LMFCOEF_NUM];
} GDC_FISHEYE_POINT_QUERY_ATTR_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __TS_COMM_GDC_H__ */
