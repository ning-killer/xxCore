/*
 * Copyright © 2019 TSING MICRO.com.cn All Rights Reserved
 * Description: ts_comm_3a.h
 * Author:TsingMicro multimedia software group
 * Create: 2022/06/02
 */

#ifndef __TS_COMM_3A_H__
#define __TS_COMM_3A_H__

#include "ts_common.h"
#include "ts_comm_isp.h"
#include "ts_comm_sns.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define IS_LINEAR_MODE(mode)      (WDR_MODE_NONE == (mode))
#define IS_WDR_MODE(mode)         ((!IS_LINEAR_MODE(mode)) && ((mode) < WDR_MODE_BUTT))
#define IS_HDR_MODE(mode)         (((mode) > DYNAMIC_RANGE_SDR10) && ((mode) < DYNAMIC_RANGE_BUTT))
#define IS_BUILT_IN_WDR_MODE(mode) (WDR_MODE_BUILT_IN == (mode))
#define IS_FS_WDR_MODE(mode)      ((!IS_LINEAR_MODE(mode))&& (!IS_BUILT_IN_WDR_MODE(mode)) && ((mode) < WDR_MODE_BUTT))
#define IS_2to1_WDR_MODE(mode)    ((WDR_MODE_2To1_FRAME == (mode)) || (WDR_MODE_2To1_FRAME_FULL_RATE == (mode)) || (WDR_MODE_2To1_LINE == (mode))  || (WDR_MODE_QUDRA == (mode)))
#define IS_3to1_WDR_MODE(mode)    ((WDR_MODE_3To1_FRAME == (mode)) || (WDR_MODE_3To1_FRAME_FULL_RATE == (mode)) || (WDR_MODE_3To1_LINE == (mode)))
#define IS_4to1_WDR_MODE(mode)    ((WDR_MODE_4To1_FRAME == (mode)) || (WDR_MODE_4To1_FRAME_FULL_RATE == (mode)) || (WDR_MODE_4To1_LINE == (mode)))
#define IS_FULL_WDR_MODE(mode)    ((WDR_MODE_2To1_FRAME_FULL_RATE == (mode)) || (WDR_MODE_3To1_FRAME_FULL_RATE == (mode)) || (WDR_MODE_4To1_FRAME_FULL_RATE == (mode)))
#define IS_HALF_WDR_MODE(mode)    ((WDR_MODE_2To1_FRAME == (mode)) || (WDR_MODE_3To1_FRAME == (mode)) || (WDR_MODE_4To1_FRAME == (mode)))
#define IS_LINE_WDR_MODE(mode)    ((WDR_MODE_2To1_LINE == (mode)) || (WDR_MODE_3To1_LINE == (mode)) || (WDR_MODE_4To1_LINE == (mode)) || (WDR_MODE_QUDRA == (mode)))

#define IS_STITCH_MAIN_PIPE(ViPipe,MainPipe) ((ViPipe) == (MainPipe))
#define IS_OFFLINE_MODE(mode)    (ISP_MODE_RUNNING_OFFLINE    == (mode))
#define IS_ONLINE_MODE(mode)     (ISP_MODE_RUNNING_ONLINE     == (mode))
#define IS_SIDEBYSIDE_MODE(mode) (ISP_MODE_RUNNING_SIDEBYSIDE == (mode))
#define IS_STRIPING_MODE(mode)   (ISP_MODE_RUNNING_STRIPING   == (mode))

#define MAX_REGISTER_ALG_LIB_NUM 2

typedef enum tsISP_ALG_MOD_E
{
    ISP_ALG_AE = 0,
    ISP_ALG_AF,
    ISP_ALG_AWB,
    ISP_ALG_BLC,
    ISP_ALG_FELOGLUT,
    ISP_ALG_LOGLUT,
    ISP_ALG_DP,
    ISP_ALG_PREGAMMA,
    ISP_ALG_DRC,
    ISP_ALG_DEMOSAIC,
    ISP_ALG_ANTIFALSECOLOR,
    ISP_ALG_GAMMA,
    ISP_ALG_GE,
    ISP_ALG_SHARPEN,
    ISP_ALG_EDGEAMRK,
    ISP_ALG_FrameWDR,
    ISP_ALG_FPN,
    ISP_ALG_DEHAZE,
    ISP_ALG_ACM,
    ISP_ALG_LCAC,/**Not support in dt56**/
    ISP_ALG_GCAC,/**Not support in dt56**/
    ISP_ALG_CSC,
    ISP_ALG_EXPANDER,
    ISP_ALG_MCDS,
    ISP_ALG_LSC,
    ISP_ALG_RLSC,
    ISP_ALG_FeLSC,
    ISP_ALG_RC,
    ISP_ALG_RGBIR,
    ISP_ALG_HRS,
    ISP_ALG_DG,
    ISP_ALG_BAYERNR,
    ISP_ALG_FLICKER,
    ISP_ALG_SPLIT,
    ISP_ALG_LDCI,
    ISP_ALG_CA,
    ISP_ALG_CLUT,
    ISP_ALG_DETAIL,
    ISP_ALG_BUTT,
} ISP_ALG_MOD_E;

typedef enum tsISP_CTRL_CMD_E
{
    ISP_WDR_MODE_SET = 8000,
    ISP_PROC_WRITE,

    ISP_AE_FPS_BASE_SET,
    ISP_AE_BLC_SET,
    ISP_AE_RC_SET,
    ISP_AE_BAYER_FORMAT_SET,

    ISP_AWB_ISO_SET,  /* set iso, change saturation when iso change */
    ISP_CHANGE_IMAGE_MODE_SET,
    ISP_UPDATE_INFO_GET,
    ISP_FRAMEINFO_GET,
    ISP_ATTACHINFO_GET,
    ISP_COLORGAMUTINFO_GET,
    ISP_AWB_INTTIME_SET,
    ISP_BAS_MODE_SET,
    ISP_PROTRIGGER_SET,
    ISP_AWB_PIRIS_SET,
    ISP_AWB_SNAP_MODE_SET,
    ISP_AWB_ZONE_ROW_SET,
    ISP_AWB_ZONE_COL_SET,
    ISP_AWB_ZONE_BIN_SET,
    ISP_CTRL_CMD_BUTT,
} ISP_CTRL_CMD_E;

typedef struct tsISP_CTRL_PROC_WRITE_S
{
    TS_CHAR *pcProcBuff;
    TS_U32   u32BuffLen;
    TS_U32   u32WriteLen;   /* The len count should contain '\0'. */
} ISP_CTRL_PROC_WRITE_S;


typedef struct tsISP_STITCH_ATTR_S
{
    TS_BOOL bStitchEnable;
    TS_BOOL bMainPipe;
    TS_U8   u8StitchPipeNum;
    TS_S8   as8StitchBindId[VI_MAX_PIPE_NUM];
} ISP_STITCH_ATTR_S;
/********************************  AE  *************************************/
/* the init param of ae alg */
//typedef struct tsISP_AE_PARAM_S
//{
//    SENSOR_ID SensorId;
//    TS_U8  u8WDRMode;
//    TS_U8  u8HDRMode;
//    TS_U16 u16BlackLevel;
//    TS_FLOAT f32Fps;
//    ISP_BAYER_FORMAT_E enBayer;
//    ISP_STITCH_ATTR_S stStitchAttr;

//    TS_S32 s32Rsv;
//} ISP_AE_PARAM_S;

/* the statistics of ae alg */
typedef struct tsISP_FE_AE_STAT_1_S
{
    TS_U32  u32PixelCount[ISP_CHN_MAX_NUM];
    TS_U32  u32PixelWeight[ISP_CHN_MAX_NUM];
    TS_U32  au32HistogramMemArray[ISP_CHN_MAX_NUM][1024];
} ISP_FE_AE_STAT_1_S;

typedef struct tsISP_FE_AE_STAT_2_S
{
    TS_U16  u16GlobalAvgR[ISP_CHN_MAX_NUM];
    TS_U16  u16GlobalAvgGr[ISP_CHN_MAX_NUM];
    TS_U16  u16GlobalAvgGb[ISP_CHN_MAX_NUM];
    TS_U16  u16GlobalAvgB[ISP_CHN_MAX_NUM];
} ISP_FE_AE_STAT_2_S;

typedef struct tsISP_FE_AE_STAT_3_S
{
    TS_U16  au16ZoneAvg[ISP_CHN_MAX_NUM][AE_ZONE_ROW][AE_ZONE_COLUMN][4];
} ISP_FE_AE_STAT_3_S;

typedef struct tsISP_FE_AE_STITCH_STAT_3_S
{
    TS_U16  au16ZoneAvg[VI_MAX_PIPE_NUM][ISP_CHN_MAX_NUM][AE_ZONE_ROW][AE_ZONE_COLUMN][4];
} ISP_FE_AE_STITCH_STAT_3_S;

typedef struct tsISP_BE_AE_STAT_1_S
{
    TS_U32  u32PixelCount;
    TS_U32  u32PixelWeight;
    TS_U32  au32HistogramMemArray[1024];
} ISP_BE_AE_STAT_1_S;

typedef struct tsISP_BE_AE_STAT_2_S
{
    TS_U16  u16GlobalAvgR;
    TS_U16  u16GlobalAvgGr;
    TS_U16  u16GlobalAvgGb;
    TS_U16  u16GlobalAvgB;
} ISP_BE_AE_STAT_2_S;

typedef struct tsISP_BE_AE_STAT_3_S
{
    TS_U16  au16ZoneAvg[AE_ZONE_ROW][AE_ZONE_COLUMN][4];
} ISP_BE_AE_STAT_3_S;

typedef struct tsISP_BE_AE_STITCH_STAT_3_S
{
    TS_U16  au16ZoneAvg[VI_MAX_PIPE_NUM][AE_ZONE_ROW][AE_ZONE_COLUMN][4];
} ISP_BE_AE_STITCH_STAT_3_S;

typedef struct tsISP_AE_INFO_S
{
    TS_U32  u32FrameCnt;    /* the counting of frame */

    ISP_FE_AE_STAT_1_S *pstFEAeStat1;
    ISP_FE_AE_STAT_2_S *pstFEAeStat2;
    ISP_FE_AE_STAT_3_S *pstFEAeStat3;
    ISP_FE_AE_STITCH_STAT_3_S *pstFEAeStiStat;
    ISP_BE_AE_STAT_1_S *pstBEAeStat1;
    ISP_BE_AE_STAT_2_S *pstBEAeStat2;
    ISP_BE_AE_STAT_3_S *pstBEAeStat3;
    ISP_BE_AE_STITCH_STAT_3_S *pstBEAeStiStat;
} ISP_AE_INFO_S;


typedef struct tsISP_AE_STAT_ATTR_S
{
    TS_BOOL bChange;

    TS_BOOL bHistAdjust;
    TS_U8 u8AEBESel;
    TS_U8 u8FourPlaneMode;
    TS_U8 u8HistOffsetX;
    TS_U8 u8HistOffsetY;
    TS_U8 u8HistSkipX;
    TS_U8 u8HistSkipY;

    TS_BOOL bModeUpdate;
    TS_U8 u8HistMode;
    TS_U8 u8AverMode;
    TS_U8 u8MaxGainMode;

    TS_BOOL bWightTableUpdate;
    TS_U8 au8WeightTable[VI_MAX_PIPE_NUM][AE_ZONE_ROW][AE_ZONE_COLUMN];
} ISP_AE_STAT_ATTR_S;

/* the final calculate of ae alg */
#define AE_INT_TIME_NUM     (4)
typedef struct tsISP_AE_RESULT_S
{
    TS_U32  u32IntTime[AE_INT_TIME_NUM];
    TS_U32  u32IspDgain;
    TS_U32  u32Again;
    TS_U32  u32Dgain;
    TS_U32  u32Iso;
    TS_U8   u8AERunInterval;

    TS_BOOL bPirisValid;
    TS_S32  s32PirisPos;
    TS_U32  u32PirisGain;

    ISP_FSWDR_MODE_E enFSWDRMode;
    TS_U32  au32WDRGain[4];
    TS_U32  u32HmaxTimes; /* unit is ns */

    ISP_AE_STAT_ATTR_S stStatAttr;
    ISP_DCF_UPDATE_INFO_S stUpdateInfo;
} ISP_AE_RESULT_S;

typedef struct tsISP_AE_EXP_FUNC_S
{
    TS_S32 (*pfn_ae_init)(TS_S32 s32Handle, const ISP_AE_PARAM_S *pstAeParam);
    TS_S32 (*pfn_ae_run)(TS_S32 s32Handle,
                         const ISP_AE_INFO_S *pstAeInfo,
                         ISP_AE_RESULT_S *pstAeResult,
                         TS_S32 s32Rsv
                        );
    TS_S32 (*pfn_ae_ctrl)(TS_S32 s32Handle, TS_U32 u32Cmd, TS_VOID *pValue);
    TS_S32 (*pfn_ae_exit)(TS_S32 s32Handle);
} ISP_AE_EXP_FUNC_S;

typedef struct tsISP_AE_REGISTER_S
{
    ISP_AE_EXP_FUNC_S stAeExpFunc;
} ISP_AE_REGISTER_S;

/********************************  AWB  *************************************/
typedef enum tsAWB_CTRL_CMD_E
{
    AWB_CCM_CONFIG_SET,
    AWB_CCM_CONFIG_GET,

    AWB_DEBUG_ATTR_SET,
    AWB_DEBUG_ATTR_GET,

    AWB_CTRL_BUTT,
} AWB_CTRL_CMD_E;

typedef struct tsAWB_CCM_CONFIG_S
{
    TS_BOOL bAWBBypassEn;
    TS_BOOL bManualSatEn;
    TS_BOOL bManualTempEn;

    TS_U32  u32ManualSatValue;
    TS_U32  u32ManualTempValue;
    TS_U16  u16CCMSpeed;

    TS_U16  au16HighCCM[9];
    TS_U16  au16LowCCM[9];
    TS_U16  u16HighColorTemp;
    TS_U16  u16LowColorTemp;
} AWB_CCM_CONFIG_S;

/* the init param of awb alg */
//typedef struct tsISP_AWB_PARAM_S
//{
//    SENSOR_ID SensorId;
//    TS_U8 u8WDRMode;
//    TS_U8 u8AWBZoneRow;
//    TS_U8 u8AWBZoneCol;
//    TS_U8 u8AWBZoneBin;
//    ISP_STITCH_ATTR_S stStitchAttr;
//    TS_U16 u16AWBWidth;
//    TS_U16 u16AWBHeight;
//    TS_S8 s8Rsv;
//} ISP_AWB_PARAM_S;

/* the statistics of awb alg */


typedef struct tsISP_AWB_STAT_1_S
{
    TS_U16  u16MeteringAwbAvgR;
    TS_U16  u16MeteringAwbAvgG;
    TS_U16  u16MeteringAwbAvgB;
    TS_U16  u16MeteringAwbCountAll;

} ISP_AWB_STAT_1_S;

typedef struct tsISP_AWB_STAT_2_S
{
    TS_U16  au16MeteringMemArrayAvgR[AWB_ZONE_NUM];
    TS_U16  au16MeteringMemArrayAvgG[AWB_ZONE_NUM];
    TS_U16  au16MeteringMemArrayAvgB[AWB_ZONE_NUM];
    TS_U16  au16MeteringMemArrayCountAll[AWB_ZONE_NUM];

} ISP_AWB_STAT_2_S;

typedef struct tsISP_AWB_STAT_STITCH_S
{
    TS_U16  u16ZoneCol;
    TS_U16  u16ZoneRow;
    TS_U16  u16ZoneBin;
    TS_U16  au16MeteringMemArrayAvgR[AWB_ZONE_STITCH_MAX];
    TS_U16  au16MeteringMemArrayAvgG[AWB_ZONE_STITCH_MAX];
    TS_U16  au16MeteringMemArrayAvgB[AWB_ZONE_STITCH_MAX];
    TS_U16  au16MeteringMemArrayCountAll[AWB_ZONE_STITCH_MAX];

} ISP_AWB_STAT_STITCH_S;

typedef struct tsISP_AWB_STAT_RESULT_S
{
    TS_U16 *pau16ZoneAvgR;
    TS_U16 *pau16ZoneAvgG;
    TS_U16 *pau16ZoneAvgB;
    TS_U16 *pau16ZoneCount;
} ISP_AWB_STAT_RESULT_S;

typedef struct tsISP_AWB_INFO_S
{
    TS_U32  u32FrameCnt;

    ISP_AWB_STAT_1_S *pstAwbStat1;
    ISP_AWB_STAT_RESULT_S stAwbStat2;
    TS_U8 u8AwbGainSwitch;
    TS_U32 au32WDRWBGain[ISP_BAYER_CHN_NUM];
} ISP_AWB_INFO_S;

/* the statistics's attr of awb alg */


typedef struct tsISP_AWB_RAW_STAT_ATTR_S
{
    TS_BOOL bStatCfgUpdate;

    TS_U16  u16MeteringWhiteLevelAwb;
    TS_U16  u16MeteringBlackLevelAwb;
    TS_U16  u16MeteringCrRefMaxAwb;
    TS_U16  u16MeteringCbRefMaxAwb;
    TS_U16  u16MeteringCrRefMinAwb;
    TS_U16  u16MeteringCbRefMinAwb;
} ISP_AWB_RAW_STAT_ATTR_S;

/* the final calculate of awb alg */
typedef struct tsISP_AWB_RESULT_S
{
    TS_U32  au32WhiteBalanceGain[ISP_BAYER_CHN_NUM];
    TS_U16  au16ColorMatrix[CCM_MATRIX_SIZE];
    TS_U32  u32ColorTemp;
    TS_U8   u8Saturation;
    ISP_AWB_RAW_STAT_ATTR_S stRawStatAttr;
} ISP_AWB_RESULT_S;

typedef struct tsISP_AWB_EXP_FUNC_S
{
    TS_S32 (*pfn_awb_init)(TS_S32 s32Handle, const ISP_AWB_PARAM_S *pstAwbParam);
    TS_S32 (*pfn_awb_run)(TS_S32 s32Handle,
                          const ISP_AWB_INFO_S *pstAwbInfo,
                          ISP_AWB_RESULT_S *pstAwbResult,
                          TS_S32 s32Rsv
                         );
    TS_S32 (*pfn_awb_ctrl)(TS_S32 s32Handle, TS_U32 u32Cmd, TS_VOID *pValue);
    TS_S32 (*pfn_awb_exit)(TS_S32 s32Handle);
} ISP_AWB_EXP_FUNC_S;

typedef struct tsISP_AWB_REGISTER_S
{
    ISP_AWB_EXP_FUNC_S stAwbExpFunc;
} ISP_AWB_REGISTER_S;

/********************************  AF  *************************************/

/* the statistics of af alg */
typedef struct tsISP_AF_ZONE_S
{
    TS_U16  u16v1;
    TS_U16  u16h1;
    TS_U16  u16v2;
    TS_U16  u16h2;
    TS_U16  u16y;
    TS_U16  u16HlCnt;
} ISP_AF_ZONE_S;

typedef struct tsISP_FE_AF_STAT_S
{
    ISP_AF_ZONE_S stZoneMetrics[WDR_CHN_MAX][AF_ZONE_ROW][AF_ZONE_COLUMN]; /*R; The zoned measure of contrast*/
} ISP_FE_AF_STAT_S;

typedef struct tsISP_BE_AF_STAT_S
{
    ISP_AF_ZONE_S stZoneMetrics[AF_ZONE_ROW][AF_ZONE_COLUMN]; /*R; The zoned measure of contrast*/
} ISP_BE_AF_STAT_S;

#define ALG_LIB_NAME_SIZE_MAX   (20)
typedef struct tsALG_LIB_S
{
    TS_S32  s32Id;
    TS_CHAR acLibName[ALG_LIB_NAME_SIZE_MAX];
} ALG_LIB_S;

typedef struct tsISP_BIND_ATTR_S
{
    SENSOR_ID   SensorId;
    ALG_LIB_S   stAeLib;
    ALG_LIB_S   stAfLib;
    ALG_LIB_S   stAwbLib;
} ISP_BIND_ATTR_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /*__TS_COMM_SNS_H__ */
