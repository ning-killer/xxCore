/*
 * Copyright © 2019 TSING MICRO.com.cn All Rights Reserved
 * Description: ts_awb_comm.h
 * Author:TsingMicro multimedia software group
 * Create: 2022/06/02
 */
#ifndef __TS_AWB_COMM_H__
#define __TS_AWB_COMM_H__
#include "ts_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define TS_AWB_LIB_NAME "ts_awb_lib"

typedef struct tsAWB_DBG_ATTR_S {
	TS_U16 u16WhiteLevel;
	TS_U16 u16BlackLevel;
	TS_U16 u16CrMax;
	TS_U16 u16CrMin;
	TS_U16 u16CbMax;
	TS_U16 u16CbMin;
	TS_U16 u16CrHigh;
	TS_U16 u16CrLow;
	TS_U16 u16CbHigh;
	TS_U16 u16CbLow;
	TS_U16 u16RawWhiteLevel;
	TS_U16 u16RawBlackLevel;
	TS_U16 u16RawCrMax;
	TS_U16 u16RawCrMin;
	TS_U16 u16RawCbMax;
	TS_U16 u16RawCbMin;
	TS_U16 u16RawCrHigh;
	TS_U16 u16RawCrLow;
	TS_U16 u16RawCbHigh;
	TS_U16 u16RawCbLow;

	TS_U16 u16WDRMode;
	TS_U16 u16Enable;
	TS_U16 u16ManualEnable;
	TS_U16 u16Zone;
	TS_U16 u16HighTemp;
	TS_U16 u16LowTemp;
	TS_U16 u16RefTemp;
	TS_U16 u16RgainBase;
	TS_U16 u16GgainBase;
	TS_U16 u16BgainBase;
	TS_S32 s32p1;
	TS_S32 s32p2;
	TS_S32 s32q;
	TS_S32 s32a;
	TS_S32 s32c;

	TS_U16 u16ManSatEnable;
	TS_U16 u16SatTarget;
} AWB_DBG_ATTR_S;

typedef struct tsAWB_ZONE_DBG_S {
    TS_U16 u16Sum;
    TS_U16 u16Rg;
    TS_U16 u16Bg;
    TS_U16 u16CountAll;
    TS_U16 u16CountMin;
    TS_U16 u16CountMax;
    TS_U16 u16RawRAvg;
    TS_U16 u16RawGAvg;
    TS_U16 u16RawBAvg;
    TS_U16 u16TK;
    TS_U16 u16Weight;
    TS_S16 s16Shift;
} AWB_ZONE_DBG_S;

typedef struct tsAWB_DBG_STATUS_S
{
    TS_U32 u32FrmNumBgn;
    TS_U32 u32GlobalSum;
    TS_U16 u16GlobalRgSta;
    TS_U16 u16GlobalBgSta;
    TS_U16 u16GlobalCountAll;
    TS_U16 u16GlobalCountMin;
    TS_U16 u16GlobalCountMax;
    TS_U16 u16GlobalRAvg;
    TS_U16 u16GlobalGAvg;
    TS_U16 u16GlobalBAvg;
    TS_U16 u16TK;
    TS_U16 u16Rgain;
    TS_U16 u16Ggain;
    TS_U16 u16Bgain;
    TS_U16 au16CCM[CCM_MATRIX_SIZE];

    TS_U32 au32HistInfo[256];
    AWB_ZONE_DBG_S astZoneDebug[AWB_ZONE_NUM];

    TS_U32 u32FrmNumEnd;
} AWB_DBG_STATUS_S;

/************************** sensor's interface to awb *********************/

typedef struct tsAWB_CCM_TAB_S
{
    TS_U16 u16ColorTemp;                   /*RW;  Range:[2000,10000]; Format:16.0; the current color temperature */
    TS_U16 au16CCM[CCM_MATRIX_SIZE];       /*RW;  Range: [0x0, 0xFFFF]; Format:8.8; CCM matrixes for different color temperature*/
} AWB_CCM_TAB_S;

typedef struct tsAWB_CCM_S
{
    TS_U16  u16CCMTabNum;                   /*RW;  Range: [0x3, 0x7]; Format:16.0; The number of CCM matrixes*/
    AWB_CCM_TAB_S astCCMTab[CCM_MATRIX_NUM];
} AWB_CCM_S;

typedef struct tsAWB_AGC_TABLE_S
{
    TS_BOOL bValid;

    TS_U8   au8Saturation[ISP_AUTO_ISO_STRENGTH_NUM];   /* RW;adjust saturation, different iso with different saturation */
} AWB_AGC_TABLE_S;

typedef struct tsAWB_SENSOR_DEFAULT_S
{
    TS_U16  u16WbRefTemp;       /* RW;reference color temperature for WB  */
    TS_U16  au16GainOffset[ISP_BAYER_CHN_NUM];  /*RW; gain offset for white balance */
    TS_S32  as32WbPara[AWB_CURVE_PARA_NUM];      /*RW; parameter for wb curve,p1,p2,q1,a1,b1,c1 */

    TS_U16  u16GoldenRgain;      /* Rgain for the golden sample */
    TS_U16  u16GoldenBgain;      /* Bgain for the golden sample */
    TS_U16  u16SampleRgain;      /* Rgain for the current sample */
    TS_U16  u16SampleBgain;      /* Bgain for the current sample */
    AWB_AGC_TABLE_S stAgcTbl;
    AWB_CCM_S stCcm;
    TS_U16    u16InitRgain;           /*Init WB gain*/
    TS_U16    u16InitGgain;
    TS_U16    u16InitBgain;
    TS_U8     u8AWBRunInterval;       /*RW;AWB Run Interval*/
} AWB_SENSOR_DEFAULT_S;

typedef struct tsAWB_SPEC_SENSOR_DEFAULT_S
{
    ISP_SPECAWB_ATTR_S  stSpecAwbAttrs;
    ISP_SPECAWB_CAA_CONTROl_S stCaaControl;
} AWB_SPEC_SENSOR_DEFAULT_S;

typedef struct tsAWB_SENSOR_EXP_FUNC_S
{
    TS_S32(*pfn_cmos_get_awb_default)(VI_PIPE ViPipe, AWB_SENSOR_DEFAULT_S *pstAwbSnsDft);
    TS_S32(*pfn_cmos_get_awb_spec_default)(VI_PIPE ViPipe, AWB_SPEC_SENSOR_DEFAULT_S *pstAwbSpecSnsDft);
} AWB_SENSOR_EXP_FUNC_S;

typedef struct tsAWB_SENSOR_REGISTER_S
{
    AWB_SENSOR_EXP_FUNC_S stSnsExp;
} AWB_SENSOR_REGISTER_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
