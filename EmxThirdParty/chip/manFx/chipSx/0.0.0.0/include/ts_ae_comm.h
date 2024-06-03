/*
 * Copyright © 2019 TSING MICRO.com.cn All Rights Reserved
 * Description: ts_ae_comm.h
 * Author:TsingMicro multimedia software group
 * Create: 2022/06/02
 */
#ifndef __TS_AE_COMM_H__
#define __TS_AE_COMM_H__

#include "ts_type.h"
#include "ts_comm_isp.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define TS_AE_LIB_NAME "ts_ae_lib"

/************************** ae ctrl cmd **************************************/
typedef enum tsAE_CTRL_CMD_E
{
    AE_DEBUG_ATTR_SET,
    AE_DEBUG_ATTR_GET,

    AE_CTRL_BUTT,
} AE_CTRL_CMD_E;

typedef struct tsAE_DBG_ATTR_S
{
    TS_U32  u32MaxIntTime;
    TS_U32  u32MinIntTime;
    TS_U32  u32MaxAgain;
    TS_U32  u32MinAgain;
    TS_U32  u32MaxDgain;
    TS_U32  u32MinDgain;
    TS_U32  u32MaxIspDgain;
    TS_U32  u32MinIspDgain;
    TS_U32  u32MaxSysGain;
    TS_U32  u32MinSysGain;
    TS_U32  u32Compensation;
    TS_U32  u32EVBias;
    TS_BOOL bManualExposureEn;
    TS_BOOL bManualTimeEn;
    TS_BOOL bManualAgainEn;
    TS_BOOL bManualDgainEn;
    TS_BOOL bManualIspDgainEn;
    TS_U32  u32ManualExposureLines;
    TS_U32  u32ManualAgain;
    TS_U32  u32ManualDgain;
    TS_U32  u32ManualIspDgain;
    TS_U32  au32AeWeights[AE_ZONE_ROW *AE_ZONE_COLUMN];
} AE_DBG_ATTR_S;

typedef struct tsAE_DBG_STATUS_S
{
    TS_U32  u32FrmNumBgn;
    TS_U32  u32FullLines;
    TS_U32  u32IntTime;
    TS_U32  u32ShortIntTime;
    TS_U32  u32MedIntTime;
    TS_U32  u32LongIntTime;
    TS_U32  u32Again;
    TS_U32  u32Dgain;
    TS_U32  u32IspDgain;
    TS_U32  u32IrisFNOLin;
    TS_U64  u64Exposure;
    TS_U32  u32Increment;
    TS_S32  s32HistError;
    TS_S32  s32HistOriAverage;
    TS_S32  s32LumaOffset;
    TS_U32  u32Iso;
    TS_U32  u32ExpRatio;
    TS_U32  u32OverExpRatio;
    TS_U32  u32OverExpRatioFilter;
} AE_DBG_STATUS_S;

/************************** sensor's interface to ae *********************/

/* eg: 0.35db, enAccuType=AE_ACCURACY_DB, f32Accuracy=0.35
*  and the multiply of 0.35db is power(10, (0.35/20))
*  eg: 1/16, 2/16, 3/16 multiplies, enAccuType=AE_ACCURACY_LINEAR, f32Accuracy=0.0625
*  eg: 1,2,4,8,16 multiplies, enAccuType=AE_ACCURACY_DB, f32Accuracy=6
*/
typedef enum tsAE_ACCURACY_E
{
    AE_ACCURACY_DB = 0,
    AE_ACCURACY_LINEAR,
    AE_ACCURACY_TABLE,

    AE_ACCURACY_BUTT,
} AE_ACCURACY_E;

typedef struct tsAE_ACCURACY_S
{
    AE_ACCURACY_E enAccuType;
    float   f32Accuracy;
    float   f32Offset;
} AE_ACCURACY_S;

typedef struct tsAE_SENSOR_DEFAULT_S
{
    TS_U8   au8HistThresh[HIST_THRESH_NUM];
    TS_U8   u8AeCompensation;

    TS_U32  u32LinesPer500ms;
    TS_U32  u32FlickerFreq;
    TS_FLOAT f32Fps;
    TS_U32  u32HmaxTimes; /* unit is ns */
    TS_U32  u32InitExposure;
    TS_U32  u32InitAESpeed;
    TS_U32  u32InitAETolerance;

    TS_U32  u32FullLinesStd;
    TS_U32  u32FullLinesMax;
    TS_U32  u32FullLines;
    TS_U32  u32MaxIntTime;     /* RW;unit is line */
    TS_U32  u32MinIntTime;
    TS_U32  u32MaxIntTimeTarget;
    TS_U32  u32MinIntTimeTarget;
    AE_ACCURACY_S stIntTimeAccu;

    TS_U32  u32MaxAgain;
    TS_U32  u32MinAgain;
    TS_U32  u32MaxAgainTarget;
    TS_U32  u32MinAgainTarget;
    AE_ACCURACY_S stAgainAccu;

    TS_U32  u32MaxDgain;
    TS_U32  u32MinDgain;
    TS_U32  u32MaxDgainTarget;
    TS_U32  u32MinDgainTarget;
    AE_ACCURACY_S stDgainAccu;

    TS_U32  u32MaxISPDgainTarget;
    TS_U32  u32MinISPDgainTarget;
    TS_U32  u32ISPDgainShift;

    TS_U32  u32MaxIntTimeStep;
    TS_U32  u32LFMaxShortTime;
    TS_U32  u32LFMinExposure;

    ISP_AE_ROUTE_S stAERouteAttr;
    TS_BOOL bAERouteExValid;
    ISP_AE_ROUTE_EX_S stAERouteAttrEx;

    TS_U16 u16ManRatioEnable;
    TS_U32 au32Ratio[EXP_RATIO_NUM];

    ISP_IRIS_TYPE_E  enIrisType;
    ISP_PIRIS_ATTR_S stPirisAttr;
    ISP_IRIS_F_NO_E  enMaxIrisFNO;  /*RW; Range:[0, 10]; Format:4.0; Max F number of Piris's aperture, it's related to the specific iris */
    ISP_IRIS_F_NO_E  enMinIrisFNO;  /*RW; Range:[0, 10]; Format:4.0; Min F number of Piris's aperture, it's related to the specific iris */

    ISP_AE_STRATEGY_E enAeExpMode;

    TS_U16 u16ISOCalCoef;
    TS_U8  u8AERunInterval;
} AE_SENSOR_DEFAULT_S;

typedef struct tsAE_FSWDR_ATTR_S
{
    ISP_FSWDR_MODE_E enFSWDRMode;
} AE_FSWDR_ATTR_S;

typedef struct tsAE_SENSOR_EXP_FUNC_S
{
    TS_S32(*pfn_cmos_get_ae_default)(VI_PIPE ViPipe, AE_SENSOR_DEFAULT_S *pstAeSnsDft);

    /* the function of sensor set fps */
    TS_VOID(*pfn_cmos_fps_set)(VI_PIPE ViPipe, TS_FLOAT f32Fps, AE_SENSOR_DEFAULT_S *pstAeSnsDft);
    TS_VOID(*pfn_cmos_slow_framerate_set)(VI_PIPE ViPipe, TS_U32 u32FullLines, AE_SENSOR_DEFAULT_S *pstAeSnsDft);

    /* while isp notify ae to update sensor regs, ae call these funcs. */
    TS_VOID(*pfn_cmos_inttime_update)(VI_PIPE ViPipe, TS_U32 u32IntTime);
    TS_VOID(*pfn_cmos_gains_update)(VI_PIPE ViPipe, TS_U32 u32Again, TS_U32 u32Dgain);

    TS_VOID (*pfn_cmos_again_calc_table)(VI_PIPE ViPipe, TS_U32 *pu32AgainLin, TS_U32 *pu32AgainDb);
    TS_VOID (*pfn_cmos_dgain_calc_table)(VI_PIPE ViPipe, TS_U32 *pu32DgainLin, TS_U32 *pu32DgainDb);

    TS_VOID (*pfn_cmos_get_inttime_max)(VI_PIPE ViPipe, TS_U16 u16ManRatioEnable, TS_U32 *au32Ratio, TS_U32 *au32IntTimeMax, TS_U32 *au32IntTimeMin, TS_U32 *pu32LFMaxIntTime);

    /* long frame mode set */
    TS_VOID(*pfn_cmos_ae_fswdr_attr_set)(VI_PIPE ViPipe, AE_FSWDR_ATTR_S *pstAeFSWDRAttr);

} AE_SENSOR_EXP_FUNC_S;

typedef struct tsAE_SENSOR_REGISTER_S
{
    AE_SENSOR_EXP_FUNC_S stSnsExp;
} AE_SENSOR_REGISTER_S;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
