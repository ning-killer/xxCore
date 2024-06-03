/*
 * Copyright © 2019 TSING MICRO.com.cn All Rights Reserved
 * Description: ts_comm_dis.h
 * Author:TsingMicro multimedia software group
 * Create: 2022/06/02
 */
#include "ts_type.h"
#include "ts_common.h"
#include "ts_errno.h"
#include "ts_comm_video.h"

#ifndef __TS_COMM_DIS_H__
#define __TS_COMM_DIS_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* failure caused by malloc buffer */
#define TS_ERR_DIS_NOBUF         TS_DEF_ERR(TS_ID_DIS, EN_ERR_LEVEL_ERROR, EN_ERR_NOBUF)
#define TS_ERR_DIS_BUF_EMPTY     TS_DEF_ERR(TS_ID_DIS, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_EMPTY)
#define TS_ERR_DIS_NULL_PTR      TS_DEF_ERR(TS_ID_DIS, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
#define TS_ERR_DIS_ILLEGAL_PARAM TS_DEF_ERR(TS_ID_DIS, EN_ERR_LEVEL_ERROR, EN_ERR_ILLEGAL_PARAM)
#define TS_ERR_DIS_BUF_FULL      TS_DEF_ERR(TS_ID_DIS, EN_ERR_LEVEL_ERROR, EN_ERR_BUF_FULL)
#define TS_ERR_DIS_SYS_NOTREADY  TS_DEF_ERR(TS_ID_DIS, EN_ERR_LEVEL_ERROR, EN_ERR_SYS_NOTREADY)
#define TS_ERR_DIS_NOT_SUPPORT   TS_DEF_ERR(TS_ID_DIS, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_SUPPORT)
#define TS_ERR_DIS_NOT_PERMITTED TS_DEF_ERR(TS_ID_DIS, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
#define TS_ERR_DIS_BUSY          TS_DEF_ERR(TS_ID_DIS, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)
#define TS_ERR_DIS_INVALID_CHNID TS_DEF_ERR(TS_ID_DIS, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
#define TS_ERR_DIS_CHN_UNEXIST   TS_DEF_ERR(TS_ID_DIS, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)

/* Different mode of DIS */
typedef enum tsDIS_MODE_E {
    DIS_MODE_4_DOF_GME = 0, /* Only use with GME in 4 dof  */
    DIS_MODE_6_DOF_GME, /* Only use with GME in 6 dof  */
    DIS_MODE_GYRO, /* Only use with gryo in 6 dof  */
    DIS_MODE_DOF_BUTT,
} DIS_MODE_E;

/* The motion level of camera */
typedef enum tsDIS_MOTION_LEVEL_E {
    DIS_MOTION_LEVEL_LOW = 0, /* Low motion level */
    DIS_MOTION_LEVEL_NORMAL, /* Normal motion level */
    DIS_MOTION_LEVEL_HIGH, /* High motion level */
    DIS_MOTION_LEVEL_BUTT
} DIS_MOTION_LEVEL_E;

/* Different product type used DIS */
typedef enum tsDIS_PDT_TYPE_E {
    DIS_PDT_TYPE_IPC = 0, /* IPC product type */
    DIS_PDT_TYPE_DV, /* DV product type */
    DIS_PDT_TYPE_DRONE, /* DRONE product type */
    DIS_PDT_TYPE_BUTT
} DIS_PDT_TYPE_E;

/* The Attribute of DIS */
typedef struct tsDIS_ATTR_S {
    TS_BOOL bEnable; /* RW; DIS enable */
    TS_BOOL bGdcBypass; /* RW; gdc correction process , DIS = GME&GDC correction */
    TS_U32 u32MovingSubjectLevel; /* RW; Range:[0,6]; Moving Subject level */
    TS_S32 s32RollingShutterCoef; /* RW; Range:[0,1000]; Rolling shutter coefficients */
    TS_S32 s32Timelag; /* RW; Range:[-2000000,2000000]; Timestamp delay between Gyro and Frame PTS */
    TS_U32 u32ViewAngle; /* Reserved */
    TS_U32 u32HorizontalLimit; /* RW; Range:[0,1000]; Parameter to limit horizontal drift by large foreground */
    TS_U32 u32VerticalLimit; /* RW; Range:[0,1000]; Parameter to limit vertical drift by large foreground */
    TS_BOOL bStillCrop; /* RW; The stabilization will be not working ,but the output image still be cropped */
    TS_U32  u32Strength; /* RW. Range:[0,1024]; The DIS strength for different light, Only valid for MODE_GYRO */
} DIS_ATTR_S;

/* The Config of DIS */
typedef struct tsDIS_CONFIG_S {
    DIS_MODE_E enMode; /* RW; DIS Mode */
    DIS_MOTION_LEVEL_E enMotionLevel; /* RW; DIS Motion level of the camera */
    DIS_PDT_TYPE_E enPdtType; /* RW; DIS product type */
    TS_U32 u32BufNum; /* RW; Range:[5,10]; Buf num for DIS */
    TS_U32 u32CropRatio; /* RW; Range:[50,98]; Crop ratio of output image */
    TS_U32 u32FrameRate;
    TS_U32 u32GyroOutputRange; /* RW; Range:[0,200000]; The range of Gyro output in degree */
    TS_U32 u32GyroDataBitWidth; /* RW; Range:[0,32]; The bits used for gyro angular velocity output */
    TS_BOOL bCameraSteady; /* RW; The camera is steady or not */
    TS_BOOL bScale; /* RW; Scale output image or not */
} DIS_CONFIG_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __TS_COMM_DIS_H__ */
