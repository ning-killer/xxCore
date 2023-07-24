/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: include/cvi_common_isp.h
 * Description:
 */

#ifndef __CVI_COMM_ISP_H__
#define __CVI_COMM_ISP_H__

#include "cvi_common.h"
#include "cvi_comm_video.h"
#include "cvi_defines.h"
#include "cvi_debug.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define ISP_CHECK_POINTER(addr)                                                                                        \
	do {                                                                                                           \
		if (addr == NULL)                                                                                      \
			return CVI_FAILURE;                                                                            \
	} while (0)

#define CVI_TRACE_ISP(level, fmt, ...)  \
	CVI_TRACE(level, CVI_ID_ISP, "%s:%d:%s(): " fmt, __FILENAME__, __LINE__, __func__, ##__VA_ARGS__)

#define CCM_MATRIX_SIZE (9)
#define AE_MAX_ZONE_ROW (15)
#define AE_MAX_ZONE_COLUMN (17)
#define AE_ZONE_ROW (15)
#define AE_ZONE_COLUMN (17)
#define AE_MAX_NUM (1)
#define ISP_SENSOR_MAX_DELAY_NUM (5)
#define AWB_ZONE_ORIG_ROW (32)
#define AWB_ZONE_ORIG_COLUMN (32)
#define AWB_ZONE_MAX_COLUMN (33)
#define AWB_ZONE_NUM (AWB_ZONE_ORIG_ROW * AWB_ZONE_ORIG_COLUMN)
#define AWB_MAX_NUM (1)
#define HIST_BIN_DEPTH (8)
#define MAX_HIST_BINS (1 << HIST_BIN_DEPTH)
#define MAX_EXPOSURE_RATIO 256
#define MAX_DRC_HIST_BINS (MAX_HIST_BINS * MAX_EXPOSURE_RATIO)
#define DCF_DRSCRIPTION_LENGTH 32
#define DCF_CAPTURE_TIME_LENGTH 20
#define ISP_AUTO_ISO_STRENGTH_NUM 16
#define ISP_AUTO_EXP_RATIO_NUM 16
#define ISP_AUTO_COLORTEMP_NUM 7
#define CVI_ISP_LSC_GRID_COL (37)
#define CVI_ISP_LSC_GRID_ROW (37)
#define CVI_ISP_LSC_GRID_POINTS (CVI_ISP_LSC_GRID_COL * CVI_ISP_LSC_GRID_ROW)
#define DCI_BINS_NUM (256)
#define ISP_MAX_SNS_REGS 32
#define ISP_MAX_WDR_FRAME_NUM 2
#define BAYER_PATTERN_NUM 4
#define MG_ZONE_ROW (15)
#define MG_ZONE_COLUMN (17)
#define AF_ZONE_ROW (15)
#define AF_ZONE_COLUMN (17)
#define AF_XOFFSET_MIN (8)
#define AF_YOFFSET_MIN (2)
#define MAX_AWB_LIB_NUM (VI_MAX_PIPE_NUM)
#define MAX_AE_LIB_NUM (VI_MAX_PIPE_NUM)
#define LTM_DARK_CURVE_NODE_NUM (257)
#define LTM_BRIGHT_CURVE_NODE_NUM (513)
#define ISP_3ALIB_FIND_FAIL (-1)
#define SHARPEN_LUT_NUM 33
#define MOTION_NR_TABLE_NUM 16
#define HIST_NUM 256
#define ISP_BASE_ADDR 0x0A000000
#define ISP_REG_RANGE 0x80000
#define DEHAZE_LUT_NUM 32
#define DIS_MAX_WINDOW_X_NUM 3
#define DIS_MAX_WINDOW_Y_NUM 3
#define XHIST_LENGTH			256		// 512 bin
#define YHIST_LENGTH			256		// 256 bin

#define MIN_LV	(-5)
#define MAX_LV	(15)
#define LV_TOTAL_NUM	(MAX_LV - MIN_LV + 1)
#define AE_LV_FACTOR	(100)
#define ISP_AUTO_LV_NUM (LV_TOTAL_NUM)

/*Defines the format of the input Bayer image*/
typedef enum _ISP_BAYER_FORMAT_E {
	BAYER_BGGR,
	BAYER_GBRG,
	BAYER_GRBG,
	BAYER_RGGB,
	//for RGBIR sensor
	BAYER_GRGBI,
	BAYER_RGBGI,
	BAYER_GBGRI,
	BAYER_BGRGI,
	BAYER_IGRGB,
	BAYER_IRGBG,
	BAYER_IBGRG,
	BAYER_IGBGR,
	BAYER_BUTT
} ISP_BAYER_FORMAT_E;

typedef enum bmISP_OP_TYPE_E {
	OP_TYPE_AUTO,
	OP_TYPE_MANUAL,
	OP_TYPE_BUTT
} ISP_OP_TYPE_E;

typedef enum _ISP_FMW_STATE_E {
	ISP_FMW_STATE_RUN,
	ISP_FMW_STATE_FREEZE,
	ISP_FMW_STATE_BUTT
} ISP_FMW_STATE_E;

typedef enum _ISP_CHANNEL_LIST_E {
	ISP_CHANNEL_LE,
	ISP_CHANNEL_SE,
	ISP_CHANNEL_MAX_NUM,
} ISP_CHANNEL_LIST_E;

typedef enum _ISP_BAYER_CHANNEL_E {
	ISP_BAYER_CHN_R,
	ISP_BAYER_CHN_GR,
	ISP_BAYER_CHN_GB,
	ISP_BAYER_CHN_B,
	ISP_BAYER_CHN_NUM,
} ISP_BAYER_CHANNEL_E;

typedef enum _ISP_SNS_TYPE_E {
	SNS_I2C_TYPE,
	SNS_SSP_TYPE,
	SNS_TYPE_BUTT,
} ISP_SNS_TYPE_E;

typedef enum _ISP_WDR_FRAME_IDX_E {
	ISP_WDR_FRAME_IDX_1,
	ISP_WDR_FRAME_IDX_2,
	ISP_WDR_FRAME_IDX_3,
	ISP_WDR_FRAME_IDX_4,
	ISP_WDR_FRAME_IDX_SIZE
} ISP_WDR_FRAME_IDX_E;

typedef union _ISP_SNS_COMMBUS_U {
	CVI_S8 s8I2cDev;
	struct {
		CVI_S8 bit4SspDev : 4;
		CVI_S8 bit4SspCs : 4;
	} s8SspDev;
} ISP_SNS_COMMBUS_U;

typedef struct _ISP_I2C_DATA_S {
	CVI_BOOL bUpdate;
	CVI_BOOL bDropFrm;
	CVI_U8 u8DelayFrmNum; /*RW; Number of delayed frames for the sensor register*/
	CVI_U8 u8DropFrmNum; /*RW; Number of frame to drop*/
	CVI_U8 u8IntPos; /*RW;Position where the configuration of the sensor register takes effect */
	CVI_U8 u8DevAddr; /*RW;Sensor device address*/
	CVI_U32 u32RegAddr; /*RW;Sensor register address*/
	CVI_U32 u32AddrByteNum; /*RW;Bit width of the sensor register address*/
	CVI_U32 u32Data; /*RW;Sensor register data*/
	CVI_U32 u32DataByteNum; /*RW;Bit width of sensor register data*/
} ISP_I2C_DATA_S;

typedef struct _ISP_SSP_DATA_S {
	CVI_BOOL bUpdate;
	CVI_U8 u8DelayFrmNum; /*RW; Number of delayed frames for the sensor register*/
	CVI_U8 u8IntPos; /*RW;Position where the configuration of the sensor register takes effect */
	CVI_U32 u32DevAddr; /*RW;Sensor device address*/
	CVI_U32 u32DevAddrByteNum; /*RW;Bit width of the sensor device address*/
	CVI_U32 u32RegAddr; /*RW;Sensor register address*/
	CVI_U32 u32RegAddrByteNum; /*RW;Bit width of the sensor register address*/
	CVI_U32 u32Data; /*RW;Sensor register data*/
	CVI_U32 u32DataByteNum; /*RW;Bit width of sensor register data*/
} ISP_SSP_DATA_S;

typedef struct _ISP_SNS_REGS_INFO_S {
	ISP_SNS_TYPE_E enSnsType;
	CVI_U32 u32RegNum;
	CVI_U8 u8Cfg2ValidDelayMax;
	ISP_SNS_COMMBUS_U unComBus;
	union {
		ISP_I2C_DATA_S astI2cData[ISP_MAX_SNS_REGS];
		ISP_SSP_DATA_S astSspData[ISP_MAX_SNS_REGS];
	};

	struct {
		CVI_BOOL bUpdate;
		CVI_U8 u8DelayFrmNum;
		CVI_U32 u32SlaveVsTime; /* RW;time of vsync. Unit: inck clock cycle */
		CVI_U32 u32SlaveBindDev;
	} stSlvSync;

	CVI_BOOL bConfig;

	CVI_U8 use_snsr_sram;
	CVI_U8 need_update;
} ISP_SNS_REGS_INFO_S;

typedef enum _ISP_VD_TYPE_E {
	ISP_VD_FE_START = 0,
	ISP_VD_FE_END,
	ISP_VD_BE_END,
	ISP_VD_MAX
} ISP_VD_TYPE_E;

typedef struct _ISP_MANUAL_WDR_ATTR_S {
	CVI_S32 devno;
	CVI_S32 manual_en;
	CVI_S16 l2s_distance;
	CVI_S16 lsef_length;
	CVI_S32 discard_padding_lines;
	CVI_S32 update;
} ISP_MANUAL_WDR_ATTR_S;

typedef struct _ISP_SNS_CIF_INFO_S {
	ISP_MANUAL_WDR_ATTR_S wdr_manual;
	CVI_U8 u8DelayFrmNum; /*RW; Number of delayed frames for the cif setting */
	CVI_U8 need_update;
} ISP_SNS_CIF_INFO_S;

typedef struct _ISP_WDR_SIZE_S {
	RECT_S stWndRect;
	SIZE_S stSnsSize;
	SIZE_S stMaxSize;
} ISP_WDR_SIZE_S;

typedef struct _ISP_SNS_ISP_INFO_S {
	CVI_U32 frm_num;
	ISP_WDR_SIZE_S img_size[ISP_MAX_WDR_FRAME_NUM];
	CVI_U8 u8DelayFrmNum; /*RW; Number of delayed frames for the isp setting */
	CVI_U8 need_update;
	ISP_BAYER_FORMAT_E bayer_fmt;
	CVI_U8 bayer_need_update;
} ISP_SNS_ISP_INFO_S;

typedef struct _ISP_SNS_BAYER_INFO_S {
	ISP_BAYER_FORMAT_E bayer_fmt;
	CVI_U8 u8DelayFrmNum;
	CVI_U8 need_update;
} ISP_SNS_BAYER_INFO_S;

typedef struct _ISP_SNS_SYNC_INFO_S {
	ISP_SNS_REGS_INFO_S snsCfg;
	ISP_SNS_ISP_INFO_S ispCfg;
	ISP_SNS_CIF_INFO_S cifCfg;
	ISP_SNS_BAYER_INFO_S bayerCfg;
} ISP_SNS_SYNC_INFO_S;

typedef struct _ISP_PUB_ATTR_S {
	RECT_S stWndRect;
	SIZE_S stSnsSize;
	CVI_FLOAT f32FrameRate;
	ISP_BAYER_FORMAT_E enBayer;
	WDR_MODE_E enWDRMode;
	CVI_U8 u8SnsMode;
} ISP_PUB_ATTR_S;

typedef struct _ISP_CTRL_PARAM_S {
	CVI_U32 u32AEStatIntvl;
	CVI_U32 u32AWBStatIntvl;
	CVI_U32 u32AFStatIntvl;
	CVI_U32 u32ProcParam;
	CVI_U32 u32ProcLevel;
	CVI_U32 u32UpdatePos;
	CVI_U32 u32IntTimeOut;
	CVI_U32 u32PwmNumber;
	CVI_U32 u32PortIntDelay;
} ISP_CTRL_PARAM_S;

typedef union _ISP_MODULE_CTRL_U {
	CVI_U64 u64Key;
	struct {
		CVI_U64 bitBypassISPDGain : 1; /* RW;[0] */
		CVI_U64 bitBypassCrosstalkR : 1; /* RW;[1] */
		CVI_U64 bitBypassDPC : 1; /* RW;[2] */
		CVI_U64 bitBypassNR : 1; /* RW;[3] */
		CVI_U64 bitBypassDehaze : 1; /* RW;[4] */
		CVI_U64 bitBypassWBGain : 1; /* RW;[5] */
		CVI_U64 bitBypassMeshShading : 1; /* RW;[6] */
		CVI_U64 bitBypassDRC : 1; /* RW;[7] */
		CVI_U64 bitBypassDemosaic : 1; /* RW;[8] */
		CVI_U64 bitBypassColorMatrix : 1; /* RW;[9] */
		CVI_U64 bitBypassGamma : 1; /* RW;[10] */
		CVI_U64 bitBypassFSWDR : 1; /* RW;[11] */
		CVI_U64 bitBypassSharpen : 1; /* RW;[12] */
		CVI_U64 bitBypassCAC : 1; /* RW;[13] */
		CVI_U64 bitBypassLdci : 1; /* RW;[14] */
		CVI_U64 bitBypassRadialShading : 1; /* RW;[15] */
		CVI_U64 bitBypassAEStatFE : 1; /* RW;[16] */
		CVI_U64 bitBypassMGStat : 1; /* RW;[17] */
		CVI_U64 bitBypassAFStatFE : 1; /* RW;[18] */
		CVI_U64 bitBypassAWBStat : 1; /* RW;[19] */
		CVI_U64 bitBypassCLUT : 1; /* RW;[20] */
		CVI_U64 bitBypassYnr : 1; /* RW;[22] */
		CVI_U64 bitBypassCnr : 1; /* RW;[23] */
		CVI_U64 bitBypass3DNR : 1; /* RW;[24] */
		CVI_U64 bitRsv : 39; /* H  ; [25:63] */
	};
} ISP_MODULE_CTRL_U;

typedef union _ISP_STATISTICS_CTRL_U {
	CVI_U64 u64Key;
	struct {
		CVI_U64 bit1FEAeGloStat : 1; /* [0] */
		CVI_U64 bit1FEAeLocStat : 1; /* [1] */
		CVI_U64 bit1AwbStat1 : 1; /* [2] Awb Stat1 means global awb data. */
		CVI_U64 bit1AwbStat2 : 1; /* [3] Awb Stat2 means local awb data. */
		CVI_U64 bit1FEAfStat : 1; /* [4] */
		CVI_U64 bit14Rsv : 59; /* [5:63] */
	};
} ISP_STATISTICS_CTRL_U;

typedef enum _ISP_AE_STAT_MODE_E {
	ISP_AE_MODE_BUTT,
} ISP_AE_STAT_MODE_E;

typedef enum _ISP_AE_SWITCH_E {
	ISP_AE_SWITCH_BUTT,
} ISP_AE_SWITCH_E;

typedef enum _ISP_AE_FOUR_PLANE_MODE_E {
	ISP_AE_FOUR_PLANE_MODE_BUTT,
} ISP_AE_FOUR_PLANE_MODE_E;

typedef enum _ISP_AE_HIST_SKIP_E {
	ISP_AE_HIST_SKIP_BUTT,
} ISP_AE_HIST_SKIP_E;

typedef enum _ISP_AE_HIST_OFFSET_X_E {
	ISP_AE_HIST_OFFSET_X_BUTT,
} ISP_AE_HIST_OFFSET_X_E;

typedef enum _ISP_AE_HIST_OFFSET_Y_E {
	ISP_AE_HIST_OFFSET_Y_BUTT,
} ISP_AE_HIST_OFFSET_Y_E;

typedef enum _ISP_AE_ANTIFLICKER_FREQUENCE_E {
	AE_FREQUENCE_60HZ = 0,
	AE_FREQUENCE_50HZ,
} ISP_AE_ANTIFLICKER_FREQUENCE_E;

typedef enum _ISP_AE_METER_MODE_E {
	AE_METER_MULTI = 0,
	AE_METER_AVERAGE,
	AE_METER_HIGHLIGHT_PRIORITY,
	AE_METER_FISHEYE,
} ISP_AE_METER_MODE_E;

typedef enum _ISP_AE_IR_CUT_FORCE_STATUS {
	AE_IR_CUT_FORCE_AUTO = 0,
	AE_IR_CUT_FORCE_ON,
	AE_IR_CUT_FORCE_OFF,
} ISP_AE_IR_CUT_FORCE_STATUS;

typedef struct _ISP_AE_HIST_CONFIG_S {
	ISP_AE_HIST_SKIP_E enHistSkipX;
	ISP_AE_HIST_SKIP_E enHistSkipY;
	ISP_AE_HIST_OFFSET_X_E enHistOffsetX;
	ISP_AE_HIST_OFFSET_Y_E enHistOffsetY;
} ISP_AE_HIST_CONFIG_S;

typedef struct _ISP_3AWIN_CONFIG_S {
	CVI_U16 winWidth;
	CVI_U16 winHeight;
	CVI_U16 winXOffset;
	CVI_U16 winYOffset;
	CVI_U8 winXNum;
	CVI_U8 winYNum;
} ISP_3AWIN_CONFIG_S;

typedef struct _ISP_AE_CROP_S {
	CVI_BOOL bEnable;
	CVI_U16 u16X;
	CVI_U16 u16Y;
	CVI_U16 u16W;
	CVI_U16 u16H;
} ISP_AE_CROP_S;

typedef struct _ISP_AE_STATISTICS_CFG_S {
	ISP_AE_SWITCH_E enAESwitch; /*RW; Range:[0,2]; */
	ISP_AE_HIST_CONFIG_S stHistConfig;
	ISP_AE_FOUR_PLANE_MODE_E enFourPlaneMode; /*RW; Range:[0,1]; */
	ISP_AE_STAT_MODE_E enHistMode; /*RW; Range:[0,1]; */
	ISP_AE_STAT_MODE_E enAverMode; /*RW; Range:[0,1]; */
	ISP_AE_STAT_MODE_E enMaxGainMode; /*RW; Range:[0,1]; */
	ISP_AE_CROP_S stCrop[AE_MAX_NUM];
	CVI_U8 au8Weight[AE_ZONE_ROW][AE_ZONE_COLUMN]; /*RW; Range:[0x0, 0xF]; */
} ISP_AE_STATISTICS_CFG_S;

typedef struct _ISP_AE_WIN_STATISTICS_CFG_S {
	CVI_BOOL	bHisStatisticsEnable; /*RW; Range:[0, 1]*/
	CVI_BOOL	bCropEnable; /*RW; Range:[0, 1]*/
	CVI_U16		ZoneOffsetX; /*RW; Range:[0x0, 0x1FF]*/
	CVI_U16		ZoneOffsetY; /*RW; Range:[0x0, 0x1FF]*/
	CVI_U16		ZoneWidth; /*RW; Range:[0x0, 0x1FF]*/
	CVI_U16		ZoneHeight; /*RW; Range:[0x0, 0x1FF]*/
	CVI_U8		StatisticsShiftBits; /*RW; Range:[0x0, 0x7]*/
	CVI_U16		RGain; /*RW; Range:[0x0, 0x3FF]*/
	CVI_U16		GGain; /*RW; Range:[0x0, 0x3FF]*/
	CVI_U16		BGain; /*RW; Range:[0x0, 0x3FF]*/
	CVI_U8		au8WinWeight[AE_ZONE_ROW][AE_ZONE_COLUMN]; /*RW; Range:[0x0, 0xFF]*/
} ISP_AE_WIN_STATISTICS_CFG_S;

typedef enum _ISP_IRIS_TYPE_E {
	ISP_IRIS_DC_TYPE = 0,
	ISP_IRIS_P_TYPE,
	ISP_IRIS_TYPE_BUTT,
} ISP_IRIS_TYPE_E;

//Defines the F value of the ISP iris
typedef enum _ISP_IRIS_F_NO_E {
	ISP_IRIS_F_NO_32_0 = 0,
	ISP_IRIS_F_NO_22_0,
	ISP_IRIS_F_NO_16_0,
	ISP_IRIS_F_NO_11_0,
	ISP_IRIS_F_NO_8_0,
	ISP_IRIS_F_NO_5_6,
	ISP_IRIS_F_NO_4_0,
	ISP_IRIS_F_NO_2_8,
	ISP_IRIS_F_NO_2_0,
	ISP_IRIS_F_NO_1_4,
	ISP_IRIS_F_NO_1_0,

	ISP_IRIS_F_NO_BUTT,
} ISP_IRIS_F_NO_E;

#define ISP_AE_ROUTE_MIN_NODES (1)
#define ISP_AE_ROUTE_MAX_NODES (16)
typedef struct _ISP_AE_ROUTE_NODE_S {
	CVI_U32 u32IntTime;
	CVI_U32 u32SysGain;
	ISP_IRIS_F_NO_E enIrisFNO;
	CVI_U32 u32IrisFNOLin;
} ISP_AE_ROUTE_NODE_S;

typedef struct _ISP_AE_ROUTE_S {
	CVI_U32 u32TotalNum;
	ISP_AE_ROUTE_NODE_S astRouteNode[ISP_AE_ROUTE_MAX_NODES];
} ISP_AE_ROUTE_S;

#define ISP_AE_ROUTE_EX_MIN_NODES (1)
#define ISP_AE_ROUTE_EX_MAX_NODES (16)
typedef struct _ISP_AE_ROUTE_EX_NODE_S {
	CVI_U32 u32IntTime;
	CVI_U32 u32Again;
	CVI_U32 u32Dgain;
	CVI_U32 u32IspDgain;
	ISP_IRIS_F_NO_E enIrisFNO;
	CVI_U32 u32IrisFNOLin;
} ISP_AE_ROUTE_EX_NODE_S;

typedef struct _ISP_AE_ROUTE_EX_S {
	CVI_U32 u32TotalNum;
	ISP_AE_ROUTE_EX_NODE_S astRouteExNode[ISP_AE_ROUTE_EX_MAX_NODES];
} ISP_AE_ROUTE_EX_S;

typedef struct _ISP_EXP_INFO_S {
	CVI_U32 u32ExpTime;
	CVI_U32 u32ShortExpTime;
	CVI_U32 u32MedianExpTime;
	CVI_U32 u32LongExpTime;
	CVI_U32 u32AGain;
	CVI_U32 u32DGain;
	CVI_U32 u32ISPDGain;
	CVI_U32 u32Exposure;
	CVI_BOOL bExposureIsMAX;
	CVI_S16 s16HistError;
	CVI_U32 au32AE_Hist1024Value[HIST_NUM];

	CVI_U8 u8AveLum;
	CVI_U32 u32LinesPer500ms;
	CVI_U32 u32PirisFNO;
	CVI_U32 u32Fps;
	CVI_U32 u32ISO;
	CVI_U32 u32ISOCalibrate;
	CVI_U32 u32RefExpRatio;
	CVI_U32 u32FirstStableTime;
	ISP_AE_ROUTE_S stAERoute;
	ISP_AE_ROUTE_EX_S stAERouteEx;
	CVI_U8 u8WDRShortAveLuma;
	CVI_U32 u32WDRExpRatio;
	CVI_U8 u8LEFrameAvgLuma;
	CVI_U8 u8SEFrameAvgLuma;
	CVI_FLOAT fLightValue;
} ISP_EXP_INFO_S;

#define WDR_EXP_RATIO_NUM       (3)
typedef struct _ISP_WDR_EXPOSURE_ATTR_S {
	ISP_OP_TYPE_E enExpRatioType;
	CVI_U32 au32ExpRatio[WDR_EXP_RATIO_NUM];
	CVI_U32 u32ExpRatioMax;
	CVI_U32 u32ExpRatioMin;
	CVI_U16 u16Tolerance;
	CVI_U16 u16Speed;
	CVI_U16 u16RatioBias;
	CVI_U8 u8SECompensation;
	CVI_U16 u16SEHisThr;
	CVI_U16 u16SEHisCntRatio1;
	CVI_U16 u16SEHisCntRatio2;
	CVI_U32 u16SEHis255CntThr1;
	CVI_U32 u16SEHis255CntThr2;
	CVI_U8 au8LEAdjustTargetMin[LV_TOTAL_NUM];
	CVI_U8 au8LEAdjustTargetMax[LV_TOTAL_NUM];
	CVI_U8 au8SEAdjustTargetMin[LV_TOTAL_NUM];
	CVI_U8 au8SEAdjustTargetMax[LV_TOTAL_NUM];
	CVI_U8 u8AdjustTargetDetectFrmNum;
	CVI_U32 u32DiffPixelNum;
	CVI_U16 u16LELowBinThr;
	CVI_U16 u16LEHighBinThr;
	CVI_U16 u16SELowBinThr;
	CVI_U16 u16SEHighBinThr;
	CVI_U8 au8FrameAvgLumaMin[LV_TOTAL_NUM];
	CVI_U8 au8FrameAvgLumaMax[LV_TOTAL_NUM];
} ISP_WDR_EXPOSURE_ATTR_S;

typedef enum _ISP_IR_STATUS_E {
	ISP_IR_STATUS_NORMAL = 0,
	ISP_IR_STATUS_IR = 1,
	ISP_IR_BUTT
} ISP_IR_STATUS_E;

typedef enum _ISP_IR_SWITCH_STATUS_E {
	ISP_IR_SWITCH_NONE = 0,
	ISP_IR_SWITCH_TO_NORMAL = 1,
	ISP_IR_SWITCH_TO_IR = 2,
	ISP_IR_SWITCH_BUTT
} ISP_IR_SWITCH_STATUS_E;

typedef struct _ISP_IR_AUTO_ATTR_S {
	CVI_BOOL bEnable;
	CVI_U32 u32Normal2IrIsoThr;
	CVI_U32 u32Ir2NormalIsoThr;
	CVI_U32 u32RGMax;
	CVI_U32 u32RGMin;
	CVI_U32 u32BGMax;
	CVI_U32 u32BGMin;
	ISP_IR_STATUS_E enIrStatus;
	ISP_IR_SWITCH_STATUS_E enIrSwitch;
} ISP_IR_AUTO_ATTR_S;


typedef struct _ISP_AWB_CROP_S {
	CVI_BOOL bEnable;
	CVI_U16 u16X;
	CVI_U16 u16Y;
	CVI_U16 u16W;
	CVI_U16 u16H;
} ISP_AWB_CROP_S;

typedef enum _ISP_AWB_SWITCH_E {
	ISP_AWB_AFTER_DG,
	ISP_AWB_AFTER_DRC,
	ISP_AWB_SWITCH_BUTT,
} ISP_AWB_SWITCH_E;

typedef enum _ISP_AWB_ALG_TYPE_E {
	AWB_ALG_LOWCOST,
	AWB_ALG_ADVANCE,
	AWB_ALG_BUTT
} ISP_AWB_ALG_TYPE_E;

typedef enum _ISP_AWB_MULTI_LS_TYPE_E {
	AWB_MULTI_LS_SAT,
	AWB_MULTI_LS_CCM,
	AWB_MULTI_LS_BUTT
} ISP_AWB_MULTI_LS_TYPE_E;

typedef enum _ISP_AWB_INDOOR_OUTDOOR_STATUS_E {
	AWB_INDOOR_MODE,
	AWB_OUTDOOR_MODE,
	AWB_INDOOR_OUTDOOR_BUTT
} ISP_AWB_INDOOR_OUTDOOR_STATUS_E;

typedef struct _ISP_WB_STATISTICS_CFG_S {
	ISP_AWB_SWITCH_E enAWBSwitch;
	CVI_U16 u16ZoneRow; /*RW; Range:[0x0, AWB_ZONE_ORIG_ROW]*/
	CVI_U16 u16ZoneCol; /*RW; Range:[0x0, AWB_ZONE_ORIG_COLUMN]*/
	CVI_U16 u16ZoneBin;
	CVI_U16 au16HistBinThresh[4];
	CVI_U16 u16WhiteLevel; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 u16BlackLevel; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 u16CbMax;
	CVI_U16 u16CbMin;
	CVI_U16 u16CrMax;
	CVI_U16 u16CrMin;
	ISP_AWB_CROP_S stCrop;
} ISP_WB_STATISTICS_CFG_S;

#define FIR_H_GAIN_NUM 5
#define FIR_V_GAIN_NUM 3
#define AF_GAMMA_NUM 256
// AF window X region must between [0x8 ~ image xsize - 8].
// Y region must between [0x2 ~ image ysize - 2].
typedef struct _ISP_AF_CROP_S {
	CVI_BOOL bEnable;
	CVI_U16 u16X;	// Range: must more than 8.
	CVI_U16 u16Y;	// Range: must more than 2.
	CVI_U16 u16W;
	CVI_U16 u16H;
} ISP_AF_CROP_S;

typedef struct _ISP_AF_RAW_CFG_S {
	CVI_U8 PreGammaEn;
	CVI_U8 PreGammaTable[AF_GAMMA_NUM];
} ISP_AF_RAW_CFG_S;

typedef struct _ISP_AF_PRE_FILTER_CFG_S {
	CVI_BOOL PreFltEn;
} ISP_AF_PRE_FILTER_CFG_S;

typedef struct _ISP_AF_CFG_S {
	CVI_BOOL bEnable;
	CVI_U16 u16Hwnd;
	CVI_U16 u16Vwnd;
	CVI_U8 u8HFltShift;
	CVI_S8 s8HVFltLpCoeff[FIR_H_GAIN_NUM];
	ISP_AF_RAW_CFG_S stRawCfg;
	ISP_AF_PRE_FILTER_CFG_S stPreFltCfg;
	ISP_AF_CROP_S stCrop;
	CVI_U16 u16HighLumaTh;
} ISP_AF_CFG_S;

typedef struct _ISP_AF_H_PARAM_S {
	CVI_S8 s8HFltHpCoeff[FIR_H_GAIN_NUM];
} ISP_AF_H_PARAM_S;

typedef struct _ISP_AF_V_PARAM_S {
	CVI_S8 s8VFltHpCoeff[FIR_V_GAIN_NUM];
} ISP_AF_V_PARAM_S;

typedef struct _ISP_FOCUS_STATISTICS_CFG_S {
	ISP_AF_CFG_S stConfig;
	ISP_AF_H_PARAM_S stHParam_FIR0;
	ISP_AF_H_PARAM_S stHParam_FIR1;
	ISP_AF_V_PARAM_S stVParam_FIR;
} ISP_FOCUS_STATISTICS_CFG_S;

typedef struct _ISP_STATISTICS_CFG_S {
	ISP_STATISTICS_CTRL_U unKey;
	ISP_AE_STATISTICS_CFG_S stAECfg;
	ISP_WB_STATISTICS_CFG_S stWBCfg;
	ISP_FOCUS_STATISTICS_CFG_S stFocusCfg;
} ISP_STATISTICS_CFG_S;

typedef struct _ISP_AE_GRID_INFO_S {
	CVI_U16 au16GridYPos[AE_ZONE_ROW + 1];
	CVI_U16 au16GridXPos[AE_ZONE_COLUMN + 1];
	CVI_U8 u8Status;
} ISP_AE_GRID_INFO_S;

typedef struct _ISP_AE_STATISTICS_S {
	CVI_U32 au32FEHist1024Value[ISP_CHANNEL_MAX_NUM][AE_MAX_NUM][MAX_HIST_BINS];
	CVI_U16 au16FEGlobalAvg[ISP_CHANNEL_MAX_NUM][AE_MAX_NUM][BAYER_PATTERN_NUM];
	CVI_U16 au16FEZoneAvg[ISP_CHANNEL_MAX_NUM][AE_MAX_NUM][AE_ZONE_ROW][AE_ZONE_COLUMN][BAYER_PATTERN_NUM];
	CVI_U32 au32BEHist1024Value[MAX_HIST_BINS];
	CVI_U16 au16BEGlobalAvg[BAYER_PATTERN_NUM];
	CVI_U16 au16BEZoneAvg[AE_ZONE_ROW][AE_ZONE_COLUMN][BAYER_PATTERN_NUM];
	ISP_AE_GRID_INFO_S stFEGridInfo;
} ISP_AE_STATISTICS_S;

typedef struct _ISP_AWB_GRID_INFO_S {
	CVI_U16 au16GridYPos[AWB_ZONE_ORIG_ROW + 1];
	CVI_U16 au16GridXPos[AWB_ZONE_ORIG_COLUMN + 1];
	CVI_U8 u8Status;
} ISP_AWB_GRID_INFO_S;

typedef struct _ISP_WB_STATISTICS_S {
	CVI_U16 u16GlobalR; /*RW; Range:[0x0, 0x3FF]*/
	CVI_U16 u16GlobalG; /*RW; Range:[0x0, 0x3FF]*/
	CVI_U16 u16GlobalB; /*RW; Range:[0x0, 0x3FF]*/
	CVI_U16 u16CountAll; /*RW; Range:[0x0, 0xFFFF]*/
	CVI_U16 au16ZoneAvgR[AWB_ZONE_NUM]; /*RW; Range:[0x0, 0x3FF]*/
	CVI_U16 au16ZoneAvgG[AWB_ZONE_NUM]; /*RW; Range:[0x0, 0x3FF]*/
	CVI_U16 au16ZoneAvgB[AWB_ZONE_NUM]; /*RW; Range:[0x0, 0x3FF]*/
	CVI_U16 au16ZoneCountAll[AWB_ZONE_NUM]; /*RW; Range:[0x0, 0xFFFF]*/
	ISP_AWB_GRID_INFO_S stGridInfo;
} ISP_WB_STATISTICS_S;

typedef struct _ISP_FOCUS_ZONE_S {
	CVI_U16 u16HlCnt;
	CVI_U64 u64h0;
	CVI_U64 u64h1;
	CVI_U32 u32v0;
} ISP_FOCUS_ZONE_S;

typedef struct _ISP_FE_FOCUS_STATISTICS_S {
	ISP_FOCUS_ZONE_S stZoneMetrics[AF_ZONE_ROW][AF_ZONE_COLUMN]; /*R; The zoned measure of contrast*/
} ISP_FE_FOCUS_STATISTICS_S;

typedef struct _ISP_AF_STATISTICS_S {
	ISP_FE_FOCUS_STATISTICS_S stFEAFStat;
} ISP_AF_STATISTICS_S;

typedef struct _ISP_MG_STATISTICS_S {
	CVI_U16 au16ZoneAvg[MG_ZONE_ROW][MG_ZONE_COLUMN][BAYER_PATTERN_NUM];
} ISP_MG_STATISTICS_S;

#define AWB_CALIB_PTS_NUM (3)
#define AWB_CT_BIN_NUM	(8)

typedef struct _ISP_AWB_LightBox_Gain_S {
	CVI_U16 u16AvgRgain;
	CVI_U16 u16AvgBgain;
} ISP_AWB_LightBox_Gain_S;

typedef struct _ISP_AWB_Calibration_Gain_S {
	CVI_U16 u16AvgRgain[AWB_CALIB_PTS_NUM];
	CVI_U16 u16AvgBgain[AWB_CALIB_PTS_NUM];
	CVI_U16 u16ColorTemperature[AWB_CALIB_PTS_NUM];
} ISP_AWB_Calibration_Gain_S;

#define AWB_CALIB_PTS_NUM_EX (10)
typedef struct _ISP_AWB_Calibration_Gain_S_EX {
	CVI_U16 u16AvgRgain[AWB_CALIB_PTS_NUM_EX];
	CVI_U16 u16AvgBgain[AWB_CALIB_PTS_NUM_EX];
	CVI_U16 u16ColorTemperature[AWB_CALIB_PTS_NUM_EX];
	CVI_U8	u8Weight[AWB_CALIB_PTS_NUM_EX];
} ISP_AWB_Calibration_Gain_S_EX;


#define AWB_LUM_HIST_NUM (6)
typedef struct _ISP_AWB_LUM_HISTGRAM_ATTR_S {
	CVI_BOOL bEnable;
	ISP_OP_TYPE_E enOpType;
	CVI_U8 au8HistThresh[AWB_LUM_HIST_NUM];
	CVI_U16 au16HistWt[AWB_LUM_HIST_NUM];
} ISP_AWB_LUM_HISTGRAM_ATTR_S;

typedef struct _ISP_AWB_CT_LIMIT_ATTR_S {
	CVI_BOOL bEnable;
	ISP_OP_TYPE_E enOpType;
	CVI_U16 u16HighRgLimit;//H temp,Rgain max
	CVI_U16 u16HighBgLimit;//H temp,Bgain min
	CVI_U16 u16LowRgLimit;//Low temp,Rgain min
	CVI_U16 u16LowBgLimit;//Low temp,Bgain max
} ISP_AWB_CT_LIMIT_ATTR_S;

typedef struct _ISP_AWB_CBCR_TRACK_ATTR_S {
	CVI_BOOL bEnable;
	CVI_U16 au16CrMax[ISP_AUTO_ISO_STRENGTH_NUM];//R/G at L tmp
	CVI_U16 au16CrMin[ISP_AUTO_ISO_STRENGTH_NUM];//R/G at H tmp
	CVI_U16 au16CbMax[ISP_AUTO_ISO_STRENGTH_NUM];//B/G at H tmp
	CVI_U16 au16CbMin[ISP_AUTO_ISO_STRENGTH_NUM];//B/G at L tmp
} ISP_AWB_CBCR_TRACK_ATTR_S;

typedef struct _ISP_AWB_IN_OUT_ATTR_S {
	CVI_BOOL bEnable;
	ISP_OP_TYPE_E enOpType;
	ISP_AWB_INDOOR_OUTDOOR_STATUS_E enOutdoorStatus;
	CVI_U32 u32OutThresh;
	CVI_U16 u16LowStart;//5000K
	CVI_U16 u16LowStop;//4500K
	CVI_U16 u16HighStart;//6000K
	CVI_U16 u16HighStop;//7200K
	CVI_BOOL bGreenEnhanceEn;//1,0~1
	CVI_U16 u8OutShiftLimit;//32,0~255
} ISP_AWB_IN_OUT_ATTR_S;

typedef struct _ISP_AWB_EXTRA_LIGHTSOURCE_INFO_S {
	CVI_U16 u16WhiteRgain;
	CVI_U16 u16WhiteBgain;
	CVI_U16 u16ExpQuant;
	CVI_U8	u8LightStatus;
	CVI_U8	u8Radius;
} ISP_AWB_EXTRA_LIGHTSOURCE_INFO_S;

#define AWB_CURVE_PARA_NUM  (6)
#define AWB_CURVE_BOUND_NUM	(8)
#define AWB_LS_NUM	(4)
#define AWB_ZONE_WT_W	(32)
#define AWB_ZONE_WT_H	(32)
#define AWB_ZONE_WT_NUM	(AWB_ZONE_WT_W*AWB_ZONE_WT_H)

typedef struct _ISP_AWB_ATTR_S {
	CVI_BOOL bEnable;

	CVI_U16 u16RefColorTemp;
	CVI_U16 au16StaticWB[ISP_BAYER_CHN_NUM];// G/R
	CVI_S32 as32CurvePara[AWB_CURVE_PARA_NUM];

	ISP_AWB_ALG_TYPE_E enAlgType;

	CVI_U8 u8RGStrength;
	CVI_U8 u8BGStrength;
	CVI_U16 u16Speed;
	CVI_U16 u16ZoneSel;
	CVI_U16 u16HighColorTemp;
	CVI_U16 u16LowColorTemp;
	ISP_AWB_CT_LIMIT_ATTR_S stCTLimit;
	CVI_BOOL bShiftLimitEn;
	CVI_U16 u16ShiftLimit[AWB_CURVE_BOUND_NUM];
	CVI_BOOL bGainNormEn;
	CVI_BOOL bNaturalCastEn;

	ISP_AWB_CBCR_TRACK_ATTR_S stCbCrTrack;// R/G
	ISP_AWB_LUM_HISTGRAM_ATTR_S stLumaHist;
	CVI_BOOL bAWBZoneWtEn;
	CVI_U8 au8ZoneWt[AWB_ZONE_WT_NUM];
} ISP_AWB_ATTR_S;

typedef struct _ISP_AWB_ATTR_EX_S {
	CVI_U8 u8Tolerance;//2,0~255
	CVI_U8 u8ZoneRadius;//16,0~255
	CVI_U16 u16CurveLLimit;//80,0~256,R/G,B/G
	CVI_U16 u16CurveRLimit;//192,192~4095,R/G,B/G
	CVI_BOOL bExtraLightEn;
	ISP_AWB_EXTRA_LIGHTSOURCE_INFO_S stLightInfo[AWB_LS_NUM];//G/R
	ISP_AWB_IN_OUT_ATTR_S stInOrOut;
	CVI_BOOL bMultiLightSourceEn;
	ISP_AWB_MULTI_LS_TYPE_E enMultiLSType;
	CVI_U16 u16MultiLSScaler;//192,0~256
	CVI_U16 au16MultiCTBin[AWB_CT_BIN_NUM];
	CVI_U16 au16MultiCTWt[AWB_CT_BIN_NUM];//0~1024
	CVI_BOOL bFineTunEn;
	CVI_U8 u8FineTunStrength;//128,0~255
} ISP_AWB_ATTR_EX_S;

typedef struct _ISP_MWB_ATTR_S {
	CVI_U16 u16Rgain;
	CVI_U16 u16Grgain;
	CVI_U16 u16Gbgain;
	CVI_U16 u16Bgain;
} ISP_MWB_ATTR_S;

typedef enum _ISP_AWB_ALG_E {
	ALG_AWB,
	ALG_AWB_SPEC,
	ALG_BUTT
} ISP_AWB_ALG_E;

typedef struct _ISP_WB_ATTR_S {
	CVI_BOOL bByPass;
	CVI_U8 u8AWBRunInterval;
	ISP_OP_TYPE_E enOpType;
	ISP_MWB_ATTR_S stManual;
	ISP_AWB_ATTR_S stAuto;
	ISP_AWB_ALG_E enAlgType;
	CVI_U8	u8DebugMode;
} ISP_WB_ATTR_S;

typedef struct _ISP_WB_INFO_S {
	CVI_U16 u16Rgain;
	CVI_U16 u16Grgain;
	CVI_U16 u16Gbgain;
	CVI_U16 u16Bgain;
	CVI_U16 u16Saturation;
	CVI_U16 u16ColorTemp;
	CVI_U16 au16CCM[CCM_MATRIX_SIZE];
	CVI_U16 u16LS0CT;
	CVI_U16 u16LS1CT;
	CVI_U16 u16LS0Area;
	CVI_U16 u16LS1Area;
	CVI_U8  u8MultiDegree;
	CVI_U16 u16ActiveShift;
	CVI_U32 u32FirstStableTime;
	ISP_AWB_INDOOR_OUTDOOR_STATUS_E enInOutStatus;
	CVI_S16	s16Bv;
} ISP_WB_INFO_S;

typedef struct _ISP_DCF_CONST_INFO_S {
	CVI_U8 au8ImageDescription[DCF_DRSCRIPTION_LENGTH]; /*Describes image*/
	CVI_U8 au8Make[DCF_DRSCRIPTION_LENGTH]; /*Shows manufacturer of digital cameras*/
	CVI_U8 au8Model[DCF_DRSCRIPTION_LENGTH]; /*Shows model number of digital cameras*/
	CVI_U8 au8Software[DCF_DRSCRIPTION_LENGTH];
	/*Shows firmware (internal software of digital cameras) version number*/

	CVI_U8 u8LightSource;
	CVI_U32 u32FocalLength; /*Focal length of lens used to take image. Unit is millimeter*/
	CVI_U8 u8SceneType;
	/*Indicates the type of scene. Value '0x01' means that the image was directly photographed.*/
	CVI_U8 u8CustomRendered;
	CVI_U8 u8FocalLengthIn35mmFilm; /*Indicates the equivalent focal length assuming a 35mm film camera, in mm*/
	CVI_U8 u8SceneCaptureType;
	CVI_U8 u8GainControl;
	CVI_U8 u8Contrast;
	CVI_U8 u8Saturation;
	CVI_U8 u8Sharpness;
	CVI_U8 u8MeteringMode;
} ISP_DCF_CONST_INFO_S;

typedef struct _ISP_DCF_UPDATE_INFO_S {
	CVI_U32 u32ISOSpeedRatings; /*CCD sensitivity equivalent to Ag-Hr film speedrate*/
	CVI_U32 u32ExposureTime; /*Exposure time (reciprocal of shutter speed).*/
	CVI_U32 u32ExposureBiasValue; /*Exposure bias (compensation) value of taking picture*/
	CVI_U8 u8ExposureProgram;
	CVI_U32 u32FNumber; /*The actual F-number (F-stop) of lens when the image was taken*/
	CVI_U32 u32MaxApertureValue; /*Maximum aperture value of lens.*/
	CVI_U8 u8ExposureMode;
	CVI_U8 u8WhiteBalance;
} ISP_DCF_UPDATE_INFO_S;

typedef struct _ISP_DCF_INFO_S {
	ISP_DCF_CONST_INFO_S stIspDCFConstInfo;
	ISP_DCF_UPDATE_INFO_S stIspDCFUpdateInfo;
} ISP_DCF_INFO_S;

typedef struct _ISP_MOD_PARAM_S {
	CVI_U32 u32IntBotHalf;
} ISP_MOD_PARAM_S;
#if 0
typedef struct _ISP_MESHSHADING_MANUAL_ATTR_S {
	CVI_U16 lsc_strength;
	CVI_U16 lscr_strnth;
	CVI_U16 lscr_gain_lut[32];
} ISP_MESHSHADING_MANUAL_ATTR_S;

typedef struct _ISP_MESHSHADING_AUTO_ATTR_S {
	CVI_U16 lsc_strength[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U16 lscr_strnth[ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U16 lscr_gain_lut[ISP_AUTO_ISO_STRENGTH_NUM][32];
} ISP_MESHSHADING_AUTO_ATTR_S;

typedef struct _ISP_MESHSHADING_ATTR_S {
	CVI_U8 lsc_enable;
	CVI_U8 lsc_debug;
	CVI_U8 lsc_xblknumm1;
	CVI_U8 reg_lsc_yblknumm1;
	CVI_U16 reg_lsc_xstep;
	CVI_U16 reg_lsc_ystep;
	CVI_U32 reg_lsc_imgx0;
	CVI_U32 reg_lsc_imgy0;
	CVI_U8 reg_lsc_gain_base;
	CVI_U16 reg_lsc_scaler[256];
	CVI_U8 lscr_enable;
	CVI_U16 lscr_centerx;
	CVI_U16 lscr_centery;
	CVI_U16 lscr_norm;
	ISP_OP_TYPE_E enOpType;
	ISP_MESHSHADING_MANUAL_ATTR_S stManual;
	ISP_MESHSHADING_AUTO_ATTR_S stAuto;
} ISP_MESHSHADING_ATTR_S;
#endif

//-----------------------------------------------------------------------------
//  Black Level Correction(BLC)
//-----------------------------------------------------------------------------
typedef struct _ISP_BLACK_LEVEL_MANUAL_ATTR_S {
	CVI_U16 OffsetR; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 OffsetGr; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 OffsetGb; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 OffsetB; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 OffsetR2; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 OffsetGr2; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 OffsetGb2; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 OffsetB2; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 GainR; /*RW; Range:[0x0, 0xffff]*/
	CVI_U16 GainGr; /*RW; Range:[0x0, 0xffff]*/
	CVI_U16 GainGb; /*RW; Range:[0x0, 0xffff]*/
	CVI_U16 GainB; /*RW; Range:[0x0, 0xffff]*/
} ISP_BLACK_LEVEL_MANUAL_ATTR_S;

typedef struct _ISP_BLACK_LEVEL_AUTO_ATTR_S {
	CVI_U16 OffsetR[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 OffsetGr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 OffsetGb[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 OffsetB[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 OffsetR2[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 OffsetGr2[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 OffsetGb2[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 OffsetB2[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 GainR[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xffff]*/
	CVI_U16 GainGr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xffff]*/
	CVI_U16 GainGb[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xffff]*/
	CVI_U16 GainB[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xffff]*/
} ISP_BLACK_LEVEL_AUTO_ATTR_S;

typedef struct _ISP_BLACK_LEVEL_ATTR_S {
	CVI_U8 Enable; /*RW; Range:[0x0, 0x1]*/
	ISP_OP_TYPE_E enOpType;
	CVI_U8 UpdateInterval;
	ISP_BLACK_LEVEL_MANUAL_ATTR_S stManual;
	ISP_BLACK_LEVEL_AUTO_ATTR_S stAuto;
} ISP_BLACK_LEVEL_ATTR_S;

//-----------------------------------------------------------------------------
//  Radial Lens-Shading Correction(RLSC)
//-----------------------------------------------------------------------------
#define ISP_RLSC_WINDOW_SIZE 32
typedef enum _ISP_RLSC_COLOR_TEMPERATURE {
	ISP_RLSC_COLOR_TEMPERATURE_0,
	ISP_RLSC_COLOR_TEMPERATURE_1,
	ISP_RLSC_COLOR_TEMPERATURE_2,
	ISP_RLSC_COLOR_TEMPERATURE_3,
	ISP_RLSC_COLOR_TEMPERATURE_4,
	ISP_RLSC_COLOR_TEMPERATURE_5,
	ISP_RLSC_COLOR_TEMPERATURE_6,
	ISP_RLSC_COLOR_TEMPERATURE_SIZE,
} ISP_RLSC_COLOR_TEMPERATURE;

typedef struct _ISP_RADIAL_SHADING_MANUAL_ATTR_S {
	CVI_U16 RadiusStr; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 RadiusIRStr; /*RW; Range:[0x0, 0xfff]*/
} ISP_RADIAL_SHADING_MANUAL_ATTR_S;

typedef struct _ISP_RADIAL_SHADING_AUTO_ATTR_S {
	CVI_U16 RadiusStr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 RadiusIRStr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xfff]*/
} ISP_RADIAL_SHADING_AUTO_ATTR_S;

typedef struct _ISP_RADIAL_SHADING_ATTR_S {
	CVI_BOOL Enable; /*RW; Range:[0, 1]*/
	ISP_OP_TYPE_E enOpType;
	CVI_U8 UpdateInterval;
	CVI_U16 CenterX; /*RW; Range:[0x0, 0x1fff]*/
	CVI_U16 CenterY; /*RW; Range:[0x0, 0x1fff]*/
	CVI_U16 RadiusScaleRGB;  /*RW; Range:[0x0, 0x7fff]*/
	CVI_U16 RadiusScaleIR;  /*RW; Range:[0x0, 0x7fff]*/
	ISP_RADIAL_SHADING_MANUAL_ATTR_S stManual;
	ISP_RADIAL_SHADING_AUTO_ATTR_S stAuto;
} ISP_RADIAL_SHADING_ATTR_S;

typedef struct _ISP_RADIAL_SHADING_GAIN_LUT_S {
	CVI_U16 RGain[ISP_RLSC_WINDOW_SIZE]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 GGain[ISP_RLSC_WINDOW_SIZE]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 BGain[ISP_RLSC_WINDOW_SIZE]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 IrGain[ISP_RLSC_WINDOW_SIZE]; /*RW; Range:[0x0, 0xfff]*/
} ISP_RADIAL_SHADING_GAIN_LUT_S;

typedef struct _ISP_RADIAL_SHADING_GAIN_LUT_ATTR_S {
	CVI_U8 Size; /*RW; Range:[0x1, 0x7]*/
	CVI_U16 ColorTemperature[ISP_RLSC_COLOR_TEMPERATURE_SIZE]; /*RW; Range:[0x0, 0xffff]*/
	ISP_RADIAL_SHADING_GAIN_LUT_S RLscGainLut[ISP_RLSC_COLOR_TEMPERATURE_SIZE];
} ISP_RADIAL_SHADING_GAIN_LUT_ATTR_S;

//-----------------------------------------------------------------------------
//  Dead pixel correction(DPC)
//-----------------------------------------------------------------------------
typedef struct _ISP_DP_DYNAMIC_MANUAL_ATTR_S {
	CVI_U8 ClusterSize; /*RW; Range:[0x0, 0x3]*/
	CVI_U8 BrightDefectToNormalPixRatio; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 DarkDefectToNormalPixRatio; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 FlatThreR; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 FlatThreG; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 FlatThreB; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 FlatThreMinG; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 FlatThreMinRB; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 IrBrightDefectToNormalPixRatio; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 IrDarkDefectToNormalPixRatio; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 FlatThreIR; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 FlatThreMinIR; /*RW; Range:[0x0, 0xff]*/
} ISP_DP_DYNAMIC_MANUAL_ATTR_S;

typedef struct _ISP_DP_DYNAMIC_AUTO_ATTR_S {
	CVI_U8 ClusterSize[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x3]*/
	CVI_U8 BrightDefectToNormalPixRatio[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 DarkDefectToNormalPixRatio[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 FlatThreR[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 FlatThreG[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 FlatThreB[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 FlatThreMinG[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 FlatThreMinRB[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 IrBrightDefectToNormalPixRatio[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 IrDarkDefectToNormalPixRatio[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 FlatThreIR[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 FlatThreMinIR[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
} ISP_DP_DYNAMIC_AUTO_ATTR_S;

typedef struct _ISP_DP_DYNAMIC_ATTR_S {
	CVI_BOOL Enable; /*RW; Range:[0, 1]*/
	CVI_U32 DynamicDPCEnable;
	ISP_OP_TYPE_E enOpType;
	CVI_U8 UpdateInterval;
	ISP_DP_DYNAMIC_MANUAL_ATTR_S stManual;
	ISP_DP_DYNAMIC_AUTO_ATTR_S stAuto;
} ISP_DP_DYNAMIC_ATTR_S;

#define STATIC_DP_COUNT_MAX 4096
typedef struct _ISP_DP_STATIC_ATTR_S {
	CVI_BOOL Enable; /*RW; Range:[0, 1]*/
	CVI_U16 BrightCount; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 DarkCount; /*RW; Range:[0x0, 0xfff]*/
	CVI_U32 BrightTable[STATIC_DP_COUNT_MAX]; /*RW; Range:[0x0, 0x1fff1fff]*/
	CVI_U32 DarkTable[STATIC_DP_COUNT_MAX]; /*RW; Range:[0x0, 0x1fff1fff]*/
	CVI_BOOL Show;	// not support yet
} ISP_DP_STATIC_ATTR_S;

typedef enum _CVI_STATIC_DP_TYPE_E {
	ISP_STATIC_DP_BRIGHT,
	ISP_STATIC_DP_DARK,
} CVI_STATIC_DP_TYPE_E;

typedef enum _ISP_STATUS_E {
	ISP_STATUS_INIT,
	ISP_STATUS_SUCCESS,
	ISP_STATUS_TIMEOUT,
	ISP_STATUS_SIZE,
} ISP_STATUS_E;

typedef struct _ISP_DP_CALIB_ATTR_S {
	CVI_BOOL EnableDetect; /*RW; Range:[0, 1]*/
	CVI_STATIC_DP_TYPE_E StaticDPType; /*RW; Range:[0, 1]*/
	CVI_U8 StartThresh; /*RW; Range:[0x0, 0xff]*/
	CVI_U16 CountMax; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 CountMin; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 TimeLimit; /*RW; Range:[0x0, 0x640]*/
	CVI_BOOL saveFileEn;

	// read only
	CVI_U32 Table[STATIC_DP_COUNT_MAX]; /*R; Range:[0x0, 0x1fff1fff]*/
	CVI_U8 FinishThresh; /*R; Range:[0x0, 0xff]*/
	CVI_U16 Count; /*R; Range:[0x0, 0xfff]*/
	ISP_STATUS_E Status; /*R; Range:[0x0, 0x2]*/
} ISP_DP_CALIB_ATTR_S;

//-----------------------------------------------------------------------------
//  Crosstalk
//-----------------------------------------------------------------------------
typedef struct _ISP_CROSSTALK_MANUAL_ATTR_S {
	CVI_U16 Strength; /*RW; Range:[0x0, 0x100]*/
} ISP_CROSSTALK_MANUAL_ATTR_S;

typedef struct _ISP_CROSSTALK_AUTO_ATTR_S {
	CVI_U16 Strength[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x100]*/
} ISP_CROSSTALK_AUTO_ATTR_S;

typedef struct _ISP_CROSSTALK_ATTR_S {
	CVI_BOOL Enable; /*RW; Range:[0, 1]*/
	ISP_OP_TYPE_E enOpType;
	CVI_U8 UpdateInterval;
	CVI_U16 GrGbDiffThreSec[4]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 FlatThre[4]; /*RW; Range:[0x0, 0xfff]*/
	ISP_CROSSTALK_MANUAL_ATTR_S stManual;
	ISP_CROSSTALK_AUTO_ATTR_S stAuto;
} ISP_CROSSTALK_ATTR_S;

//-----------------------------------------------------------------------------
//  Bayer domain noise reduction (BNR)
//-----------------------------------------------------------------------------
typedef struct _ISP_NR_MANUAL_ATTR_S {
	CVI_U8 WindowType; /*RW; Range:[0x0, 0xb]*/
	CVI_U8 DetailSmoothMode; /*RW; Range:[0x0, 0x1]*/
	CVI_U8 NoiseSuppressStr; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 FilterType; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 NoiseSuppressStrMode; /*RW; Range:[0x0, 0xff]*/
} ISP_NR_MANUAL_ATTR_S;

typedef struct _ISP_NR_AUTO_ATTR_S {
	CVI_U8 WindowType[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xb]*/
	CVI_U8 DetailSmoothMode[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x1]*/
	CVI_U8 NoiseSuppressStr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 FilterType[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 NoiseSuppressStrMode[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
} ISP_NR_AUTO_ATTR_S;

typedef struct _ISP_NR_ATTR_S {
	CVI_BOOL Enable; /*RW; Range:[0, 1]*/
	ISP_OP_TYPE_E enOpType;
	CVI_U8 UpdateInterval;
	ISP_NR_MANUAL_ATTR_S stManual;
	ISP_NR_AUTO_ATTR_S stAuto;
} ISP_NR_ATTR_S;

typedef struct _ISP_NR_FILTER_MANUAL_ATTR_S {
	CVI_U8 LumaStr[8]; /*RW; Range:[0x0, 0x1f]*/
	CVI_U8 VarThr; /*RW; Range:[0x0, 0xff]*/
	CVI_U16 CoringWgtLF; /*RW; Range:[0x0, 0x100]*/
	CVI_U16 CoringWgtHF; /*RW; Range:[0x0, 0x100]*/
	CVI_U8 NonDirFiltStr; /*RW; Range:[0x0, 0x1f]*/
	CVI_U8 VhDirFiltStr; /*RW; Range:[0x0, 0x1f]*/
	CVI_U8 AaDirFiltStr; /*RW; Range:[0x0, 0x1f]*/
} ISP_NR_FILTER_MANUAL_ATTR_S;

typedef struct _ISP_NR_FILTER_AUTO_ATTR_S {
	CVI_U8 LumaStr[8][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x1f]*/
	CVI_U8 VarThr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U16 CoringWgtLF[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x100]*/
	CVI_U16 CoringWgtHF[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x100]*/
	CVI_U8 NonDirFiltStr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x1f]*/
	CVI_U8 VhDirFiltStr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x1f]*/
	CVI_U8 AaDirFiltStr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x1f]*/
} ISP_NR_FILTER_AUTO_ATTR_S;

typedef enum _ISP_NR_TUNING_MODE_E {
	NR_RESULT = 8,
	NR_EDGE_SMOOTH_REGION = 11,
} ISP_NR_TUNING_MODE_E;

typedef struct _ISP_NR_FILTER_ATTR_S {
	ISP_OP_TYPE_E enOpType;
	CVI_U8 TuningMode; /*RW; Range:[0x0, 0xf]*/
	ISP_NR_FILTER_MANUAL_ATTR_S stManual;
	ISP_NR_FILTER_AUTO_ATTR_S stAuto;
} ISP_NR_FILTER_ATTR_S;

typedef struct _ISP_RLSC_MANUAL_ATTR_S {
	CVI_U16 RadialStr; /*RW; Range:[0x0, 0xfff]*/
} ISP_RLSC_MANUAL_ATTR_S;

typedef struct _ISP_RLSC_AUTO_ATTR_S {
	CVI_U16 RadialStr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xfff]*/
} ISP_RLSC_AUTO_ATTR_S;

typedef struct _ISP_RLSC_ATTR_S {
	CVI_BOOL RlscEnable; /*RW; Range:[0, 1]*/
	CVI_U16 RlscCenterX; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 RlscCenterY; /*RW; Range:[0x0, 0xfff]*/
	ISP_OP_TYPE_E enOpType;
	ISP_RLSC_MANUAL_ATTR_S stManual;
	ISP_RLSC_AUTO_ATTR_S stAuto;
} ISP_RLSC_ATTR_S;

//-----------------------------------------------------------------------------
//  Demosaic
//-----------------------------------------------------------------------------
typedef struct _ISP_DEMOSAIC_MANUAL_ATTR_S {
	CVI_U16 CoarseEdgeThr; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 CoarseStr; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 FineEdgeThr; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 FineStr; /*RW; Range:[0x0, 0xfff]*/
	CVI_U8 FilterMode; /*RW; Range:[0, 1]*/
} ISP_DEMOSAIC_MANUAL_ATTR_S;

typedef struct _ISP_DEMOSAIC_AUTO_ATTR_S {
	CVI_U16 CoarseEdgeThr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 CoarseStr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 FineEdgeThr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 FineStr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U8 FilterMode[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0, 1]*/
} ISP_DEMOSAIC_AUTO_ATTR_S;

typedef struct _ISP_DEMOSAIC_ATTR_S {
	CVI_BOOL Enable; /*RW; Range:[0, 1]*/
	CVI_BOOL TuningMode; /*RW; Range:[0, 1]*/
	ISP_OP_TYPE_E enOpType;
	CVI_U8 UpdateInterval;
	ISP_DEMOSAIC_MANUAL_ATTR_S stManual;
	ISP_DEMOSAIC_AUTO_ATTR_S stAuto;
} ISP_DEMOSAIC_ATTR_S;

typedef struct _ISP_DEMOSAIC_DEMOIRE_MANUAL_ATTR_S {
	CVI_BOOL DetailSmoothEnable; /*RW; Range:[0, 1]*/
	CVI_U8 DetailSmoothStr; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 EdgeWgtStr; /*RW; Range:[0x0, 0xff]*/
} ISP_DEMOSAIC_DEMOIRE_MANUAL_ATTR_S;

typedef struct _ISP_DEMOSAIC_DEMOIRE_AUTO_ATTR_S {
	CVI_BOOL DetailSmoothEnable[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0, 1]*/
	CVI_U8 DetailSmoothStr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 EdgeWgtStr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
} ISP_DEMOSAIC_DEMOIRE_AUTO_ATTR_S;

typedef struct _ISP_DEMOSAIC_DEMOIRE_ATTR_S {
	ISP_OP_TYPE_E enOpType;
	ISP_DEMOSAIC_DEMOIRE_MANUAL_ATTR_S stManual;
	ISP_DEMOSAIC_DEMOIRE_AUTO_ATTR_S stAuto;
} ISP_DEMOSAIC_DEMOIRE_ATTR_S;

typedef struct _ISP_DEMOSAIC_FILTER_MANUAL_ATTR_S {
	CVI_U16 DetailWgtThr; /*RW; Range:[0x0, 0xfff]*/
	CVI_U8 DetailWgtSlope; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 DetailWgtMin; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 DetailWgtMax; /*RW; Range:[0x0, 0xff]*/
	CVI_U16 EdgeWgtThr; /*RW; Range:[0x0, 0xfff]*/
	CVI_U8 EdgeWgtSlope; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 EdgeWgtMin; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 EdgeWgtMax; /*RW; Range:[0x0, 0xff]*/
	CVI_U16 LumaWgtThr; /*RW; Range:[0x0, 0xfff]*/
	CVI_U8 LumaWgtSlope; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 LumaWgtMin; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 LumaWgtMax; /*RW; Range:[0x0, 0xff]*/
} ISP_DEMOSAIC_FILTER_MANUAL_ATTR_S;

typedef struct _ISP_DEMOSAIC_FILTER_AUTO_ATTR_S {
	CVI_U16 DetailWgtThr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U8 DetailWgtSlope[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 DetailWgtMin[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 DetailWgtMax[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U16 EdgeWgtThr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U8 EdgeWgtSlope[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 EdgeWgtMin[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 EdgeWgtMax[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U16 LumaWgtThr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U8 LumaWgtSlope[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 LumaWgtMin[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 LumaWgtMax[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
} ISP_DEMOSAIC_FILTER_AUTO_ATTR_S;

typedef struct _ISP_DEMOSAIC_FILTER_ATTR_S {
	ISP_OP_TYPE_E enOpType;
	ISP_DEMOSAIC_FILTER_MANUAL_ATTR_S stManual;
	ISP_DEMOSAIC_FILTER_AUTO_ATTR_S stAuto;
} ISP_DEMOSAIC_FILTER_ATTR_S;

//-----------------------------------------------------------------------------
//  RGBCAC
//-----------------------------------------------------------------------------
typedef struct _ISP_RGBCAC_MANUAL_ATTR_S {
	CVI_U8 DePurpleStr; /*RW; Range:[0x0, 0xff]*/
} ISP_RGBCAC_MANUAL_ATTR_S;

typedef struct _ISP_RGBCAC_AUTO_ATTR_S {
	CVI_U8 DePurpleStr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
} ISP_RGBCAC_AUTO_ATTR_S;

typedef struct _ISP_RGBCAC_ATTR_S {
	CVI_BOOL Enable; /*RW; Range:[0, 1]*/
	ISP_OP_TYPE_E enOpType;
	CVI_U8 UpdateInterval;
	CVI_U16 VarThr; /*RW; Range:[0x0, 0x1ff]*/
	CVI_U8 PurpleDetRange; /*RW; Range:[0x0, 0x80]*/
	CVI_U16 PurpleCb; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 PurpleCr; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 PurpleCb2; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 PurpleCr2; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 PurpleCb3; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 PurpleCr3; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 GreenCb; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 GreenCr; /*RW; Range:[0x0, 0xfff]*/
	CVI_U8 TuningMode; /*RW; Range:[0x0, 0x2]*/
	ISP_RGBCAC_MANUAL_ATTR_S stManual;
	ISP_RGBCAC_AUTO_ATTR_S stAuto;
} ISP_RGBCAC_ATTR_S;

//-----------------------------------------------------------------------------
//  Mesh lens shading correction (MLSC)
//-----------------------------------------------------------------------------
typedef enum _ISP_MLSC_COLOR_TEMPERATURE {
	ISP_MLSC_COLOR_TEMPERATURE_0,
	ISP_MLSC_COLOR_TEMPERATURE_1,
	ISP_MLSC_COLOR_TEMPERATURE_2,
	ISP_MLSC_COLOR_TEMPERATURE_3,
	ISP_MLSC_COLOR_TEMPERATURE_4,
	ISP_MLSC_COLOR_TEMPERATURE_5,
	ISP_MLSC_COLOR_TEMPERATURE_6,
	ISP_MLSC_COLOR_TEMPERATURE_SIZE,
} ISP_MLSC_COLOR_TEMPERATURE;

typedef struct _ISP_MESH_SHADING_MANUAL_ATTR_S {
	CVI_U16 MeshStr; /*RW; Range:[0x0, 0xfff]*/
} ISP_MESH_SHADING_MANUAL_ATTR_S;

typedef struct _ISP_MESH_SHADING_AUTO_ATTR_S {
	CVI_U16 MeshStr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xfff]*/
} ISP_MESH_SHADING_AUTO_ATTR_S;

typedef struct _ISP_MESH_SHADING_ATTR_S {
	CVI_BOOL Enable; /*RW; Range:[0x0, 0x1]*/
	ISP_OP_TYPE_E enOpType;
	CVI_U8 UpdateInterval;
	CVI_BOOL OverflowProtection;
	ISP_MESH_SHADING_MANUAL_ATTR_S stManual;
	ISP_MESH_SHADING_AUTO_ATTR_S stAuto;
	CVI_U8 CompLSCMethod;/*RW; Range:[0x0,0x1]*/
} ISP_MESH_SHADING_ATTR_S;

typedef struct _ISP_MESH_SHADING_GAIN_LUT_S {
	CVI_U16 RGain[CVI_ISP_LSC_GRID_POINTS]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 GGain[CVI_ISP_LSC_GRID_POINTS]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 BGain[CVI_ISP_LSC_GRID_POINTS]; /*RW; Range:[0x0, 0xfff]*/
} ISP_MESH_SHADING_GAIN_LUT_S;

typedef struct _ISP_MESH_SHADING_GAIN_LUT_ATTR_S {
	CVI_U8 Size; /*RW; Range:[0x1, 0x7]*/
	CVI_U16 ColorTemperature[ISP_MLSC_COLOR_TEMPERATURE_SIZE]; /*RW; Range:[0x0, 0xffff]*/
	ISP_MESH_SHADING_GAIN_LUT_S LscGainLut[ISP_MLSC_COLOR_TEMPERATURE_SIZE];
} ISP_MESH_SHADING_GAIN_LUT_ATTR_S;

//-----------------------------------------------------------------------------
//  CCM
//-----------------------------------------------------------------------------
// Saturation
typedef struct _ISP_SATURATION_MANUAL_ATTR_S {
	CVI_U8 Saturation; /*RW; Range:[0x0, 0xff]*/
} ISP_SATURATION_MANUAL_ATTR_S;

typedef struct _ISP_SATURATION_AUTO_ATTR_S {
	CVI_U8 Saturation[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
} ISP_SATURATION_AUTO_ATTR_S;

typedef struct _ISP_SATURATION_ATTR_SS {
	ISP_OP_TYPE_E enOpType;
	ISP_SATURATION_MANUAL_ATTR_S stManual;
	ISP_SATURATION_AUTO_ATTR_S stAuto;
} ISP_SATURATION_ATTR_S;

// CCM Saturation
typedef struct _ISP_CCM_SATURATION_MANUAL_ATTR_S {
	CVI_U8 SaturationLE; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 SaturationSE; /*RW; Range:[0x0, 0xff]*/
} ISP_CCM_SATURATION_MANUAL_ATTR_S;

typedef struct _ISP_CCM_SATURATION_AUTO_ATTR_S {
	CVI_U8 SaturationLE[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 SaturationSE[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
} ISP_CCM_SATURATION_AUTO_ATTR_S;

typedef struct _ISP_CCM_SATURATION_ATTR_SS {
	ISP_CCM_SATURATION_MANUAL_ATTR_S stManual;
	ISP_CCM_SATURATION_AUTO_ATTR_S stAuto;
} ISP_CCM_SATURATION_ATTR_S;

// CCM
typedef struct _ISP_COLORMATRIX_ATTR_S {
	CVI_U16 ColorTemp; /*RW; Range:[0x1f4, 0x7530]*/
	CVI_S16 CCM[9]; /*RW; Range:[-8192, 8191]*/
} ISP_COLORMATRIX_ATTR_S;

typedef struct _ISP_CCM_MANUAL_ATTR_S {
	CVI_U8 SatEnable; /*RW; Range:[0, 1]*/
	CVI_S16 CCM[9]; /*RW; Range:[-8192, 8191]*/
} ISP_CCM_MANUAL_ATTR_S;

typedef struct _ISP_CCM_AUTO_ATTR_S {
	CVI_U8 ISOActEnable; /*RW; Range:[0, 1]*/
	CVI_U8 TempActEnable; /*RW; Range:[0, 1]*/
	CVI_U8 CCMTabNum; /*RW; Range:[0x3, 0x7]*/
	ISP_COLORMATRIX_ATTR_S CCMTab[7];
} ISP_CCM_AUTO_ATTR_S;

typedef struct _ISP_CCM_ATTR_S {
	CVI_BOOL Enable; /*RW; Range:[0, 1]*/
	ISP_OP_TYPE_E enOpType;
	CVI_U8 UpdateInterval;
	ISP_CCM_MANUAL_ATTR_S stManual;
	ISP_CCM_AUTO_ATTR_S stAuto;
} ISP_CCM_ATTR_S;

//-----------------------------------------------------------------------------
//  Color tone
//-----------------------------------------------------------------------------
typedef struct _ISP_COLOR_TONE_ATTR_S {
	CVI_BOOL wbg_enable; /*RW; Range:[0, 1] TODO@Kidd remove this*/
	CVI_U16 u16RedCastGain; /*RW; Range:[0x0, 0x1000]*/
	CVI_U16 u16GreenCastGain; /*RW; Range:[0x0, 0x1000]*/
	CVI_U16 u16BlueCastGain; /*RW; Range:[0x0, 0x1000]*/
} ISP_COLOR_TONE_ATTR_S;

//-----------------------------------------------------------------------------
//  FSWDR
//-----------------------------------------------------------------------------
typedef struct _ISP_FSWDR_MANUAL_ATTR_S {
	CVI_U16 WDRCombineLongThr; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 WDRCombineShortThr; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 WDRCombineMinWeight; /*RW; Range:[0x0, 0x100]*/
	CVI_U16 WDRCombineMaxWeight; /*RW; Range:[0x0, 0x100]*/
	CVI_U16 WDRMotionCombineLongThr; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 WDRMotionCombineShortThr; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 WDRMotionCombineMinWeight; /*RW; Range:[0x0, 0x100]*/
	CVI_U16 WDRMotionCombineMaxWeight; /*RW; Range:[0x0, 0x100]*/
	CVI_U8 WDRMtIn[4]; /*RW; Range:[0x0, 0xff]*/
	CVI_U16 WDRMtOut[4]; /*RW; Range:[0x0, 0x100]*/
	CVI_U16 WDRLongWgt; /*RW; Range:[0x0, 0x100]*/
	CVI_BOOL WDRLumaMode; /*RW; Range:[0x0, 0x1]*/
	CVI_U8 WDRMotioinFusionMode; /*RW; Range:[0x0, 0x3]*/
	CVI_U8 MergeModeAlpha; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 WDRCombineSNRAwareToleranceLevel; /*RW; Range:[0x0, 0xff]*/
} ISP_FSWDR_MANUAL_ATTR_S;

typedef struct _ISP_FSWDR_AUTO_ATTR_S {
	CVI_U16 WDRCombineLongThr[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 WDRCombineShortThr[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 WDRCombineMinWeight[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0x100]*/
	CVI_U16 WDRCombineMaxWeight[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0x100]*/
	CVI_U16 WDRMotionCombineLongThr[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 WDRMotionCombineShortThr[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 WDRMotionCombineMinWeight[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0x100]*/
	CVI_U16 WDRMotionCombineMaxWeight[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0x100]*/
	CVI_U8 WDRMtIn[4][ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U16 WDRMtOut[4][ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0x100]*/
	CVI_U16 WDRLongWgt[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0x100]*/
	CVI_BOOL WDRLumaMode[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0x1]*/
	CVI_U8 WDRMotioinFusionMode[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0x3]*/
	CVI_U8 MergeModeAlpha[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 WDRCombineSNRAwareToleranceLevel[ISP_AUTO_LV_NUM];
} ISP_FSWDR_AUTO_ATTR_S;

typedef struct _ISP_FSWDR_ATTR_S {
	CVI_BOOL Enable; /*RW; Range:[0, 1]*/
	ISP_OP_TYPE_E enOpType; /*RW; Range:[0, 1]*/
	CVI_U8 UpdateInterval;
	CVI_BOOL MotionCompEnable; /*RW; Range:[0, 1]*/
	CVI_U8 TuningMode; /*RW; Range:[0x0, 0xA]*/
	CVI_U8 WDRType; /*RW; Range:[0x0, 0x2]*/
	CVI_U8 WDRDitherEnable; /*RW; Range:[0, 1]*/
	CVI_U8 WDRDitherBit; /*RW; Range:[0, 0xf]*/
	CVI_U16 WDRDitherRange; /*RW; Range:[0, 0xfff]*/
	CVI_BOOL WDRCombineSNRAwareEn; /*RW; Range:[0, 1]*/
	CVI_U16 WDRCombineSNRAwareLowThr; /*RW; Range:[0x0, 0xffff]*/
	CVI_U16 WDRCombineSNRAwareHighThr; /*RW; Range:[0x0, 0xffff]*/
	CVI_U16 WDRCombineSNRAwareSmoothLevel; /*RW; Range:[0x0, 0xbb8]*/
	CVI_BOOL MtMode; /*RW; Range:[0, 1]*/
	ISP_FSWDR_MANUAL_ATTR_S stManual;
	ISP_FSWDR_AUTO_ATTR_S stAuto;
} ISP_FSWDR_ATTR_S;

//-----------------------------------------------------------------------------
//  DRC
//-----------------------------------------------------------------------------
#define DRC_DARK_USER_DEFINE_NUM		(257)
#define DRC_BRIGHT_USER_DEFINE_NUM		(513)
#define DRC_GLOBAL_USER_DEFINE_NUM		(1025)

typedef struct _ISP_DRC_MANUAL_ATTR_S {
	CVI_U32 LETargetYScale; /*RW; Range:[0x0, 0x800]*/
	CVI_U32 TargetYScale; /*RW; Range:[0x0, 0x800]*/
	CVI_U16 HdrStrength; /*RW; Range:[0x0, 0x100]*/
	CVI_U16 DarkOffset; /*RW; Range:[0x0, 0x100]*/
	CVI_U16 ContrastDarkMinThrd; /*RW; Range:[0x0, 0x3ff]*/
	CVI_U16 ContrastDarkMaxThrd; /*RW; Range:[0x0, 0x3ff]*/
	CVI_U8 ContrastDarkMinWeight; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 ContrastDarkMaxWeight; /*RW; Range:[0x0, 0xff]*/
	CVI_U16 ContrastBrightMinThrd; /*RW; Range:[0x0, 0x3ff]*/
	CVI_U16 ContrastBrightMaxThrd; /*RW; Range:[0x0, 0x3ff]*/
	CVI_U8 ContrastBrightMinWeight; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 ContrastBrightMaxWeight; /*RW; Range:[0x0, 0xff]*/
	CVI_BOOL LmapLeMode; /*RW; Range:[0x0, 0x1]*/
	CVI_BOOL LmapSeMode; /*RW; Range:[0x0, 0x1]*/
	CVI_BOOL LmapLumaMode; /*RW; Range:[0x0, 0x1]*/
	CVI_BOOL BrightToneLeMode; /*RW; Range:[0x0, 0x1]*/
	CVI_BOOL DarkToneLeMode; /*RW; Range:[0x0, 0x1]*/
	CVI_BOOL DetailEnhanceMode; /*RW; Range:[0x0, 0x1]*/
	CVI_S16 UnderShootThrMin; /*RW; Range:[-0x100, 0x100]*/
	CVI_S16 OverShootThrMax; /*RW; Range:[-0x100, 0x100]*/
	CVI_U8 DeltaAdpGain[33]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 LumaAdpGain[33]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 SatStrength; /*RW; Range:[0x0, 0xff]*/
	CVI_U16 ColorCorrectionIn[4]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 ColorCorrectionOut[4]; /*RW; Range:[0x0, 0x7ff]*/
	CVI_U16 ContrastGain; /*RW; Range:[0x0, 0x100]*/
	CVI_U16 DarkToneRange; /*RW; Range:[0x0, 0x100]*/
	CVI_U8 DEAdaptPercentile; /*RW; Range:[0x0, 0x19]*/
	CVI_U8 DEAdaptTargetGain; /*RW; Range:[0x1, 0x40]*/
	CVI_U8 DEAdaptGainLB; /*RW; Range:[0x1, 0xff]*/
	CVI_U8 DEAdaptGainUB; /*RW; Range:[0x1, 0xff]*/
	CVI_U8 SdrDEAdaptPercentile; /*RW; Range:[0x0, 0x19]*/
	CVI_U8 SdrDEAdaptTargetGain; /*RW; Range:[0x1, 0x40]*/
	CVI_U8 SdrDEAdaptGainLB; /*RW; Range:[0x1, 0xff]*/
	CVI_U8 SdrDEAdaptGainUB; /*RW; Range:[0x1, 0xff]*/
	CVI_U8 BritInflectPtLuma; /*RW; Range:[0x0, 0x64]*/
	CVI_U8 BritContrastLow; /*RW; Range:[0x0, 0x64]*/
	CVI_U8 BritContrastHigh; /*RW; Range:[0x0, 0x64]*/
	CVI_U8 SdrTargetY; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 SdrTargetYGain; /*RW; Range:[0x20, 0x80]*/
	CVI_U16 SdrGlobalToneStr; /*RW; Range:[0x0, 0x100]*/
	CVI_U8 SdrBritInflectPtLuma; /*RW; Range:[0x0, 0x64]*/
	CVI_U8 SdrBritContrastLow; /*RW; Range:[0x0, 0x64]*/
	CVI_U8 SdrBritContrastHigh; /*RW; Range:[0x0, 0x64]*/
} ISP_DRC_MANUAL_ATTR_S;

typedef struct _ISP_DRC_AUTO_ATTR_S {
	CVI_U32 LETargetYScale[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0x800]*/
	CVI_U32 TargetYScale[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0x800]*/
	CVI_U16 HdrStrength[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0x100]*/
	CVI_U16 DarkOffset[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0x100]*/
	CVI_U16 ContrastDarkMinThrd[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0x3ff]*/
	CVI_U16 ContrastDarkMaxThrd[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0x3ff]*/
	CVI_U8 ContrastDarkMinWeight[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 ContrastDarkMaxWeight[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U16 ContrastBrightMinThrd[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0x3ff]*/
	CVI_U16 ContrastBrightMaxThrd[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0x3ff]*/
	CVI_U8 ContrastBrightMinWeight[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 ContrastBrightMaxWeight[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_BOOL LmapLeMode[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0x1]*/
	CVI_BOOL LmapSeMode[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0x1]*/
	CVI_BOOL LmapLumaMode[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0x1]*/
	CVI_BOOL BrightToneLeMode[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0x1]*/
	CVI_BOOL DarkToneLeMode[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0x1]*/
	CVI_BOOL DetailEnhanceMode[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0x1]*/
	CVI_S16 UnderShootThrMin[ISP_AUTO_LV_NUM]; /*RW; Range:[-0x100, 0x100]*/
	CVI_S16 OverShootThrMax[ISP_AUTO_LV_NUM]; /*RW; Range:[-0x100, 0x100]*/
	CVI_U8 DeltaAdpGain[33][ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 LumaAdpGain[33][ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 SatStrength[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U16 ColorCorrectionIn[4][ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 ColorCorrectionOut[4][ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0x7ff]*/
	CVI_U16 ContrastGain[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0x100]*/
	CVI_U16 DarkToneRange[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0x100]*/
	CVI_U8 DEAdaptPercentile[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0x19]*/
	CVI_U8 DEAdaptTargetGain[ISP_AUTO_LV_NUM]; /*RW; Range:[0x1, 0x40]*/
	CVI_U8 DEAdaptGainLB[ISP_AUTO_LV_NUM]; /*RW; Range:[0x1, 0xff]*/
	CVI_U8 DEAdaptGainUB[ISP_AUTO_LV_NUM]; /*RW; Range:[0x1, 0xff]*/
	CVI_U8 SdrDEAdaptPercentile[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0x19]*/
	CVI_U8 SdrDEAdaptTargetGain[ISP_AUTO_LV_NUM]; /*RW; Range:[0x1, 0x40]*/
	CVI_U8 SdrDEAdaptGainLB[ISP_AUTO_LV_NUM]; /*RW; Range:[0x1, 0xff]*/
	CVI_U8 SdrDEAdaptGainUB[ISP_AUTO_LV_NUM]; /*RW; Range:[0x1, 0xff]*/
	CVI_U8 BritInflectPtLuma[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0x64]*/
	CVI_U8 BritContrastLow[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0x64]*/
	CVI_U8 BritContrastHigh[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0x64]*/
	CVI_U8 SdrTargetY[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 SdrTargetYGain[ISP_AUTO_LV_NUM]; /*RW; Range:[0x20, 0x80]*/
	CVI_U16 SdrGlobalToneStr[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0x100]*/
	CVI_U8 SdrBritInflectPtLuma[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0x64]*/
	CVI_U8 SdrBritContrastLow[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0x64]*/
	CVI_U8 SdrBritContrastHigh[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0x64]*/
} ISP_DRC_AUTO_ATTR_S;

typedef struct _ISP_DRC_ATTR_S {
	CVI_BOOL Enable; /*RW; Range:[0, 1]*/
	ISP_OP_TYPE_E enOpType;
	CVI_U8 UpdateInterval;
	CVI_U8 TuningMode; /*RW; Range:[0x0, 0x8]*/
	CVI_U32 LocalToneEn; /*RW; Range:[0x0, 0x1]*/
	CVI_U8 CoarseFltScale; /*RW; Range:[0x3, 0x6]*/
	CVI_U16 DarkUserDefine[DRC_DARK_USER_DEFINE_NUM]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 BrightUserDefine[DRC_BRIGHT_USER_DEFINE_NUM]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 CurveUserDefine[DRC_GLOBAL_USER_DEFINE_NUM]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U8 ToneCurveSelect; /*RW; Range:[0x0, 0x1]*/
	CVI_U16 LinearStart; /*RW; Range:[0x0, 0xfff]*/
	CVI_U16 LinearEnd; /*RW; Range:[0x0, 0xfff]*/
	CVI_U32 ToneCurveSmooth; /*RW; Range:[0x0, 0x1f4]*/
	CVI_BOOL DetailEnhanceEnable; /*RW; Range:[0x0, 0x1]*/
	CVI_U8 CompLTMeeMethod;/*RW;Range:[0x0,0x1]*/
	CVI_BOOL DrcMode; /*RW; Range:[0x0, 0x1]*/
	CVI_BOOL SatEnable; /*RW; Range:[0x0, 0x1]*/
	CVI_U8 SdrToneCurveSelect; /*RW; Range:[0x0, 0x3]*/
	CVI_BOOL DEAdaptEn; /*RW; Range:[0x0, 0x1]*/
	CVI_BOOL DEAdaptMode; /*RW; Range:[0x0, 0x1]*/
	CVI_BOOL SdrDEAdaptMode; /*RW; Range:[0x0, 0x1]*/
	CVI_U8 DarkMapStr; /*RW; Range:[0x1, 0x80]*/
	CVI_U8 SdrDarkMapStr; /*RW; Range:[0x1, 0x80]*/
	CVI_U8 BritMapStr; /*RW; Range:[0x1, 0x80]*/
	CVI_U8 SdrBritMapStr; /*RW; Range:[0x1, 0x80]*/
	CVI_U8 SdrTargetYGainMode; /*RW; Range:[0x0, 0x1]*/
	CVI_U32 DRCMu[32]; /*RW; Range:[0x0, 0xffff]*/
	ISP_DRC_MANUAL_ATTR_S stManual;
	ISP_DRC_AUTO_ATTR_S stAuto;
} ISP_DRC_ATTR_S;

//-----------------------------------------------------------------------------
//  Gamma
//-----------------------------------------------------------------------------
#define GAMMA_NODE_NUM 256
#define GAMMA_MAX_INTERPOLATION_NUM 5
typedef enum _ISP_GAMMA_CURVE_TYPE_E {
	ISP_GAMMA_CURVE_DEFAULT,
	ISP_GAMMA_CURVE_SRGB,
	ISP_GAMMA_CURVE_USER_DEFINE,
	ISP_GAMMA_CURVE_AUTO,
	ISP_GAMMA_CURVE_MAX
} ISP_GAMMA_CURVE_TYPE_E;

typedef struct _ISP_GAMMA_ATTR_S {
	CVI_BOOL Enable; /*RW; Range:[0, 1]*/
	CVI_U8 UpdateInterval;
	CVI_U16 Table[GAMMA_NODE_NUM]; /*RW; Range:[0x0, 0xfff]*/
	ISP_GAMMA_CURVE_TYPE_E enCurveType;
} ISP_GAMMA_ATTR_S;

typedef struct _ISP_GAMMA_CURVE_ATTR_S {
	CVI_S16 Lv; /*RW; Range:[-500, 1500]*/
	CVI_U16 Tbl[GAMMA_NODE_NUM]; /*RW; Range:[0x0, 0xfff]*/
} ISP_GAMMA_CURVE_ATTR_S;

typedef struct _ISP_AUTO_GAMMA_ATTR_S {
	CVI_U8 GammaTabNum; /*RW; Range:[1, GAMMA_MAX_INTERPOLATION_NUM]*/
	ISP_GAMMA_CURVE_ATTR_S GammaTab[GAMMA_MAX_INTERPOLATION_NUM];
} ISP_AUTO_GAMMA_ATTR_S;

//-----------------------------------------------------------------------------
//  Dehaze
//-----------------------------------------------------------------------------
typedef struct _ISP_DEHAZE_MANUAL_ATTR_S {
	CVI_U8 Strength; /*RW; Range:[0x0, 0x64]*/
	CVI_U8 DehazeLumaLut[32]; /*RW; Range:[0x0, 0x7f]*/
	CVI_U8 DehazeSkinLut[32]; /*RW; Range:[0x0, 0x7f]*/
} ISP_DEHAZE_MANUAL_ATTR_S;

typedef struct _ISP_DEHAZE_AUTO_ATTR_S {
	CVI_U8 Strength[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x64]*/
	CVI_U8 DehazeLumaLut[32][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x7f]*/
	CVI_U8 DehazeSkinLut[32][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x7f]*/
} ISP_DEHAZE_AUTO_ATTR_S;

typedef struct _ISP_DEHAZE_ATTR_S {
	CVI_BOOL Enable; /*RW; Range:[0, 1]*/
	ISP_OP_TYPE_E enOpType;
	CVI_U8 UpdateInterval;
	CVI_U16 CumulativeThr; /*RW; Range:[0x0, 0x3fff]*/
	CVI_U16 MinTransMapValue; /*RW; Range:[0x0, 0x1fff]*/
	CVI_BOOL DehazeLumaEnable; /*RW; Range:[0, 1]*/
	CVI_BOOL DehazeSkinEnable; /*RW; Range:[0, 1]*/
	CVI_U8 SkinCb; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 SkinCr; /*RW; Range:[0x0, 0xff]*/
	ISP_DEHAZE_MANUAL_ATTR_S stManual;
	ISP_DEHAZE_AUTO_ATTR_S stAuto;
} ISP_DEHAZE_ATTR_S;

//-----------------------------------------------------------------------------
//  CLUT
//-----------------------------------------------------------------------------
#define ISP_CLUT_LUT_LENGTH 4913
typedef struct _ISP_CLUT_ATTR_S {
	CVI_BOOL Enable;
	CVI_U8 UpdateInterval;
	CVI_U16 ClutR[ISP_CLUT_LUT_LENGTH];
	CVI_U16 ClutG[ISP_CLUT_LUT_LENGTH];
	CVI_U16 ClutB[ISP_CLUT_LUT_LENGTH];
} ISP_CLUT_ATTR_S;

typedef struct _ISP_CLUT_SATURATION_MANUAL_ATTR_S {
	CVI_U16 SatIn[4];
	CVI_U16 SatOut[4];
} ISP_CLUT_SATURATION_MANUAL_ATTR_S;

typedef struct _ISP_CLUT_SATURATION_AUTO_ATTR_S {
	CVI_U16 SatIn[4][ISP_AUTO_ISO_STRENGTH_NUM];
	CVI_U16 SatOut[4][ISP_AUTO_ISO_STRENGTH_NUM];
} ISP_CLUT_SATURATION_AUTO_ATTR_S;

typedef struct _ISP_CLUT_SATURATION_ATTR_S {
	CVI_BOOL Enable;
	ISP_OP_TYPE_E enOpType;
	ISP_CLUT_SATURATION_MANUAL_ATTR_S stManual;
	ISP_CLUT_SATURATION_AUTO_ATTR_S stAuto;
} ISP_CLUT_SATURATION_ATTR_S;

//-----------------------------------------------------------------------------
//  DCI
//-----------------------------------------------------------------------------
typedef struct _ISP_DCI_MANUAL_ATTR_S {
	CVI_U16 ContrastGain; /*RW; Range:[0x0, 0x100]*/
	CVI_U8 BlcThr; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 WhtThr; /*RW; Range:[0x0, 0xff]*/
	CVI_U16 BlcCtrl; /*RW; Range:[0x0, 0x200]*/
	CVI_U16 WhtCtrl; /*RW; Range:[0x0, 0x200]*/
	CVI_U16 DciGainMax; /*RW; Range:[0x0, 0x100]*/
} ISP_DCI_MANUAL_ATTR_S;

typedef struct _ISP_DCI_AUTO_ATTR_S {
	CVI_U16 ContrastGain[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x100]*/
	CVI_U8 BlcThr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 WhtThr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U16 BlcCtrl[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x200]*/
	CVI_U16 WhtCtrl[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x200]*/
	CVI_U16 DciGainMax[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x100]*/
} ISP_DCI_AUTO_ATTR_S;

typedef struct _ISP_DCI_ATTR_S {
	CVI_BOOL Enable; /*RW; Range:[0, 1]*/
	CVI_BOOL TuningMode; /*RW; Range:[0, 1]*/
	ISP_OP_TYPE_E enOpType;
	CVI_U8 UpdateInterval;
	CVI_U8 Method; /*RW; Range:[0x0, 0x1]*/
	CVI_U32 Speed; /*RW; Range:[0x0, 0x1f4]*/
	CVI_U16 DciStrength; /*RW; Range:[0x0, 0x100]*/
	CVI_U16 DciGamma; /*RW; Range:[0x64, 0x320]*/
	CVI_U8 DciOffset; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 ToleranceY; /*RW; Range:[0x0, 0xff]*/
	ISP_DCI_MANUAL_ATTR_S stManual;
	ISP_DCI_AUTO_ATTR_S stAuto;
} ISP_DCI_ATTR_S;

//-----------------------------------------------------------------------------
//  CA (CA/CP)
//-----------------------------------------------------------------------------
#define CA_LUT_NUM 256

typedef struct _ISP_CA_MANUAL_ATTR_S {
	CVI_U16 ISORatio; /*RW; Range:[0x0, 0x17e]*/
	CVI_U16 YRatioLut[CA_LUT_NUM]; /*RW; Range:[0x0, 0x7ff]*/
} ISP_CA_MANUAL_ATTR_S;

typedef struct _ISP_CA_AUTO_ATTR_S {
	CVI_U16 ISORatio[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x17e]*/
	CVI_U16 YRatioLut[CA_LUT_NUM][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x7ff]*/
} ISP_CA_AUTO_ATTR_S;

typedef struct _ISP_CA_ATTR_S {
	CVI_BOOL Enable; /*RW; Range:[0, 1]*/
	ISP_OP_TYPE_E enOpType;
	CVI_U8 UpdateInterval;
	CVI_BOOL CaCpMode;
	CVI_U8 CPLutY[CA_LUT_NUM];
	CVI_U8 CPLutU[CA_LUT_NUM];
	CVI_U8 CPLutV[CA_LUT_NUM];
	ISP_CA_MANUAL_ATTR_S stManual;
	ISP_CA_AUTO_ATTR_S stAuto;
} ISP_CA_ATTR_S;

//-----------------------------------------------------------------------------
//  PreSharpen
//-----------------------------------------------------------------------------
typedef struct _ISP_PRESHARPEN_MANUAL_ATTR_S {
	CVI_U8 EdgeGain; /*RW; Range:[0x0, 0x3f]*/
	CVI_U16 TextureGain; /*RW; Range:[0x0, 0x4]*/
	CVI_U8 EdgeFreq; /*RW; Range:[0x0, 0x4]*/
	CVI_U8 TextureFreq; /*RW; Range:[0x0, 0x4]*/
	CVI_U16 LumaAdpCoring; /*RW; Range:[0x0, 0x3ff]*/
	CVI_U8 LumaAdpGain[SHARPEN_LUT_NUM]; /*RW; Range:[0x0, 0x3f]*/
	CVI_U8 GlobalGain; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 EdgeGlobalGain; /*RW; Range:[0x0, 0x3f]*/
	CVI_U8 DeltaShtCtrl[SHARPEN_LUT_NUM]; /*RW; Range:[0x0, 0x3f]*/
	CVI_U8 DeltaShtCtrlUnit; /*RW; Range:[0x0, 0x3]*/
	CVI_U8 OverShootThr; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 UnderShootThr; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 YNoiseLevel; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 OverShootThrMax; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 UnderShootThrMin; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 MotionShtGainIn[4]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 MotionShtGainOut[4]; /*RW; Range:[0x0, 0xff]*/
	CVI_BOOL NoiseSuppressMode; /*RW; Range:[0, 1]*/
} ISP_PRESHARPEN_MANUAL_ATTR_S;

typedef struct _ISP_PRESHARPEN_AUTO_ATTR_S {
	CVI_U8 EdgeGain[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x3f]*/
	CVI_U16 TextureGain[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x4]*/
	CVI_U8 EdgeFreq[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x4]*/
	CVI_U8 TextureFreq[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x4]*/
	CVI_U16 LumaAdpCoring[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x3ff]*/
	CVI_U8 LumaAdpGain[SHARPEN_LUT_NUM][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x3f]*/
	CVI_U8 GlobalGain[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 EdgeGlobalGain[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x3f]*/
	CVI_U8 DeltaShtCtrl[SHARPEN_LUT_NUM][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x3f]*/
	CVI_U8 DeltaShtCtrlUnit[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x3]*/
	CVI_U8 OverShootThr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 UnderShootThr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 YNoiseLevel[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 OverShootThrMax[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 UnderShootThrMin[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 MotionShtGainIn[4][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 MotionShtGainOut[4][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_BOOL NoiseSuppressMode[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0, 1]*/
} ISP_PRESHARPEN_AUTO_ATTR_S;

typedef struct _ISP_PRESHARPEN_ATTR_S {
	CVI_BOOL Enable; /*RW; Range:[0, 1]*/
	ISP_OP_TYPE_E enOpType;
	CVI_U8 UpdateInterval;

	CVI_U8 TuningMode; /*RW; Range:[0x0, 0xb]*/
	CVI_U8 TuningModeLevelShift; /*RW; Range:[0x0, 0x8]*/
	CVI_BOOL FilterCoefByISOFuncEn; /*RW; Range:[0x0, 0x1]*/
	CVI_U32 FilterCoefByISOLowThd; /*RW; Range:[0x0, 0xffffffff]*/
	CVI_U32 FilterCoefByISOHighThd; /*RW; Range:[0x0, 0xffffffff]*/
	CVI_U8 FilterCoefNonDirType; /*RW; Range:[0x0, 0xff]*/
	CVI_BOOL LumaAdpCoringEn; /*RW; Range:[0, 1]*/
	CVI_BOOL LumaAdpGainEn; /*RW; Range:[0, 1]*/
	CVI_BOOL DeltaAdpGainEn; /*RW; Range:[0, 1]*/
	CVI_U8 DeltaAdpGain[SHARPEN_LUT_NUM]; /*RW; Range:[0x0, 0x3f]*/
	CVI_BOOL DeltaShtCtrlEn; /*RW; Range:[0, 1]*/
	CVI_BOOL WdrCoringCompensationEn; /*RW; Range:[0, 1]*/
	CVI_U8 WdrCoringCompensationMode; /*RW; Range:[0x0, 0x1]*/
	CVI_U16 WdrCoringToleranceLevel; /*RW; Range:[0x0, 0x1ff]*/
	CVI_U8 WdrCoringHighThr; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 WdrCoringLowThr; /*RW; Range:[0x0, 0xff]*/
	CVI_BOOL NoiseSuppressEnable; /*RW; Range:[0, 1]*/

	ISP_PRESHARPEN_MANUAL_ATTR_S stManual;
	ISP_PRESHARPEN_AUTO_ATTR_S stAuto;
} ISP_PRESHARPEN_ATTR_S;

//-----------------------------------------------------------------------------
//  Time-domain noise reduction (TNR)
//-----------------------------------------------------------------------------
typedef struct _ISP_TNR_MANUAL_ATTR_S {
	CVI_U8 TnrStrength0; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 MapThdLow0; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 MapThdHigh0; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 MtDetectUnit; /*RW; Range:[0x3, 0x6]*/
	CVI_S16 BrightnessNoiseLevelLE; /*RW; Range:[0x1, 0x3ff]*/
	CVI_S16 BrightnessNoiseLevelSE; /*RW; Range:[0x1, 0x3ff]*/
	CVI_BOOL MtFiltMode; /*RW; Range:[0x0, 0x1]*/
	CVI_U16 MtFiltWgt; /*RW; Range:[0x0, 0x100]*/
} ISP_TNR_MANUAL_ATTR_S;

typedef struct _ISP_TNR_AUTO_ATTR_S {
	CVI_U8 TnrStrength0[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 MapThdLow0[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 MapThdHigh0[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 MtDetectUnit[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x3, 0x6]*/
	CVI_S16 BrightnessNoiseLevelLE[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x1, 0x3ff]*/
	CVI_S16 BrightnessNoiseLevelSE[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x1, 0x3ff]*/
	CVI_BOOL MtFiltMode[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x1]*/
	CVI_U16 MtFiltWgt[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x100]*/
} ISP_TNR_AUTO_ATTR_S;

typedef struct _ISP_TNR_ATTR_S {
	CVI_BOOL Enable; /*RW; Range:[0, 1]*/
	ISP_OP_TYPE_E enOpType;
	CVI_U8 UpdateInterval;
	CVI_BOOL TuningMode; /*RW; Range:[0, 1]*/
	CVI_U8 DeflickerMode; /*RW; Range:[0x0, 0x4]*/
	CVI_U16 DeflickerToleranceLevel; /*RW; Range:[0x40, 0x3fff]*/
	CVI_BOOL TnrMtMode; /*RW; Range:[0, 1]*/
	CVI_BOOL YnrCnrSharpenMtMode; /*RW; Range:[0, 1]*/
	CVI_BOOL PreSharpenMtMode; /*RW; Range:[0, 1]*/
	CVI_U8 ChromaScalingDownMode; /*RW; Range:[0, 3]*/
	ISP_TNR_MANUAL_ATTR_S stManual;
	ISP_TNR_AUTO_ATTR_S stAuto;
} ISP_TNR_ATTR_S;

typedef struct _ISP_TNR_NOISE_MODEL_MANUAL_ATTR_S {
	CVI_U8 RNoiseLevel0; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 GNoiseLevel0; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 BNoiseLevel0; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 RNoiseLevel1; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 GNoiseLevel1; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 BNoiseLevel1; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 RNoiseHiLevel0; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 GNoiseHiLevel0; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 BNoiseHiLevel0; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 RNoiseHiLevel1; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 GNoiseHiLevel1; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 BNoiseHiLevel1; /*RW; Range:[0x0, 0xff]*/
} ISP_TNR_NOISE_MODEL_MANUAL_ATTR_S;

typedef struct _ISP_TNR_NOISE_MODEL_AUTO_ATTR_S {
	CVI_U8 RNoiseLevel0[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 GNoiseLevel0[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 BNoiseLevel0[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 RNoiseLevel1[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 GNoiseLevel1[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 BNoiseLevel1[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 RNoiseHiLevel0[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 GNoiseHiLevel0[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 BNoiseHiLevel0[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 RNoiseHiLevel1[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 GNoiseHiLevel1[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 BNoiseHiLevel1[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
} ISP_TNR_NOISE_MODEL_AUTO_ATTR_S;

typedef struct _ISP_TNR_NOISE_MODEL_ATTR_S {
	ISP_OP_TYPE_E enOpType;
	ISP_TNR_NOISE_MODEL_MANUAL_ATTR_S stManual;
	ISP_TNR_NOISE_MODEL_AUTO_ATTR_S stAuto;
} ISP_TNR_NOISE_MODEL_ATTR_S;

typedef struct _ISP_TNR_LUMA_MOTION_MANUAL_ATTR_S {
	CVI_U16 L2mIn0[4]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U8 L2mOut0[4]; /*RW; Range:[0x0, 0x3f]*/
	CVI_U16 L2mIn1[4]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U8 L2mOut1[4]; /*RW; Range:[0x0, 0x3f]*/
	CVI_BOOL MtLumaMode; /*RW; Range:[0x0, 0x1]*/
} ISP_TNR_LUMA_MOTION_MANUAL_ATTR_S;

typedef struct _ISP_TNR_LUMA_MOTION_AUTO_ATTR_S {
	CVI_U16 L2mIn0[4][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U8 L2mOut0[4][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x3f]*/
	CVI_U16 L2mIn1[4][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xfff]*/
	CVI_U8 L2mOut1[4][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x3f]*/
	CVI_BOOL MtLumaMode[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x1]*/
} ISP_TNR_LUMA_MOTION_AUTO_ATTR_S;

typedef struct _ISP_TNR_LUMA_MOTION_ATTR_S {
	ISP_OP_TYPE_E enOpType;
	ISP_TNR_LUMA_MOTION_MANUAL_ATTR_S stManual;
	ISP_TNR_LUMA_MOTION_AUTO_ATTR_S stAuto;
} ISP_TNR_LUMA_MOTION_ATTR_S;

typedef struct _ISP_TNR_GHOST_MANUAL_ATTR_S {
	CVI_U8 PrvMotion0[4]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 PrtctWgt0[4]; /*RW; Range:[0x0, 0xf]*/
	CVI_U8 MotionHistoryStr; /*RW; Range:[0x0, 0xf]*/
} ISP_TNR_GHOST_MANUAL_ATTR_S;

typedef struct _ISP_TNR_GHOST_AUTO_ATTR_S {
	CVI_U8 PrvMotion0[4][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 PrtctWgt0[4][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xf]*/
	CVI_U8 MotionHistoryStr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xf]*/
} ISP_TNR_GHOST_AUTO_ATTR_S;

typedef struct _ISP_TNR_GHOST_ATTR_S {
	ISP_OP_TYPE_E enOpType;
	ISP_TNR_GHOST_MANUAL_ATTR_S stManual;
	ISP_TNR_GHOST_AUTO_ATTR_S stAuto;
} ISP_TNR_GHOST_ATTR_S;

typedef struct _ISP_TNR_MT_PRT_MANUAL_ATTR_S {
	CVI_U8 LowMtPrtLevelY; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 LowMtPrtLevelU; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 LowMtPrtLevelV; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 LowMtPrtInY[4]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 LowMtPrtInU[4]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 LowMtPrtInV[4]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 LowMtPrtOutY[4]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 LowMtPrtOutU[4]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 LowMtPrtOutV[4]; /*RW; Range:[0x0, 0xff]*/
} ISP_TNR_MT_PRT_MANUAL_ATTR_S;

typedef struct _ISP_TNR_MT_PRT_AUTO_ATTR_S {
	CVI_U8 LowMtPrtLevelY[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 LowMtPrtLevelU[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 LowMtPrtLevelV[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 LowMtPrtInY[4][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 LowMtPrtInU[4][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 LowMtPrtInV[4][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 LowMtPrtOutY[4][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 LowMtPrtOutU[4][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 LowMtPrtOutV[4][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
} ISP_TNR_MT_PRT_AUTO_ATTR_S;

typedef struct _ISP_TNR_MT_PRT_ATTR_S {
	CVI_U8 LowMtPrtEn; /*RW; Range:[0, 1]*/
	ISP_TNR_MT_PRT_MANUAL_ATTR_S stManual;
	ISP_TNR_MT_PRT_AUTO_ATTR_S stAuto;
} ISP_TNR_MT_PRT_ATTR_S;

typedef struct _ISP_TNR_MOTION_ADAPT_MANUAL_ATTR_S {
	CVI_U8 AdaptNrLumaStrIn[4]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 AdaptNrLumaStrOut[4]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 AdaptNrChromaStrIn[4]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 AdaptNrChromaStrOut[4]; /*RW; Range:[0x0, 0xff]*/
} ISP_TNR_MOTION_ADAPT_MANUAL_ATTR_S;

typedef struct _ISP_TNR_MOTION_ADAPT_AUTO_ATTR_S {
	CVI_U8 AdaptNrLumaStrIn[4][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 AdaptNrLumaStrOut[4][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 AdaptNrChromaStrIn[4][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 AdaptNrChromaStrOut[4][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
} ISP_TNR_MOTION_ADAPT_AUTO_ATTR_S;

typedef struct _ISP_TNR_MOTION_ADAP_ATTR_S {
	ISP_TNR_MOTION_ADAPT_MANUAL_ATTR_S stManual;
	ISP_TNR_MOTION_ADAPT_AUTO_ATTR_S stAuto;
} ISP_TNR_MOTION_ADAPT_ATTR_S;

//-----------------------------------------------------------------------------
//	y domain noise reduction (YNR)
//-----------------------------------------------------------------------------
typedef struct _ISP_YNR_MANUAL_ATTR_S {
	CVI_U8 WindowType; /*RW; Range:[0x0, 0xb]*/
	CVI_U8 DetailSmoothMode; /*RW; Range:[0x0, 0x1]*/
	CVI_U8 NoiseSuppressStr; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 FilterType; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 NoiseCoringMax; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 NoiseCoringBase; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 NoiseCoringAdv; /*RW; Range:[0x0, 0xff]*/
} ISP_YNR_MANUAL_ATTR_S;

typedef struct _ISP_YNR_AUTO_ATTR_S {
	CVI_U8 WindowType[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xb]*/
	CVI_U8 DetailSmoothMode[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x1]*/
	CVI_U8 NoiseSuppressStr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 FilterType[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 NoiseCoringMax[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 NoiseCoringBase[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 NoiseCoringAdv[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
} ISP_YNR_AUTO_ATTR_S;

typedef enum _ISP_YNR_TUNING_MODE_E {
	YNR_RESULT = 8,
	YNR_EDGE_SMOOTH_REGION = 11,
} ISP_YNR_TUNING_MODE_E;

typedef struct _ISP_YNR_ATTR_S {
	CVI_BOOL Enable; /*RW; Range:[0, 1]*/
	ISP_OP_TYPE_E enOpType;
	CVI_U8 UpdateInterval;
	CVI_BOOL CoringParamEnable; /*RW; Range:[0, 1]*/
	CVI_BOOL FiltModeEnable; /*RW; Range:[0, 1]*/
	CVI_U16 FiltMode; /*RW; Range:[0x0, 0x100]*/
	CVI_U8 TuningMode; /*RW; Range:[0x0, 0xf]*/
	ISP_YNR_MANUAL_ATTR_S stManual;
	ISP_YNR_AUTO_ATTR_S stAuto;
} ISP_YNR_ATTR_S;

typedef struct _ISP_YNR_MOTION_NR_MANUAL_ATTR_S {
	CVI_U8 MotionThr; /*RW; Range:[0x0, 0xf]*/
	CVI_U8 MotionNrPosGain; /*RW; Range:[0x0, 0x40]*/
	CVI_U8 MotionNrNegGain; /*RW; Range:[0x0, 0x40]*/
	CVI_U8 MotionCoringWgtMax; /*RW; Range:[0x0, 0xff]*/
	CVI_U16 MotionYnrLut[16]; /*RW; Range:[0x0, 0xff]*/
	CVI_U16 MotionCoringWgt[16]; /*RW; Range:[0x0, 0x100]*/
} ISP_YNR_MOTION_NR_MANUAL_ATTR_S;

typedef struct _ISP_YNR_MOTION_NR_AUTO_ATTR_S {
	CVI_U8 MotionThr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xf]*/
	CVI_U8 MotionNrPosGain[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x40]*/
	CVI_U8 MotionNrNegGain[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x40]*/
	CVI_U8 MotionCoringWgtMax[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U16 MotionYnrLut[16][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U16 MotionCoringWgt[16][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x100]*/
} ISP_YNR_MOTION_NR_AUTO_ATTR_S;

typedef struct _ISP_YNR_MOTION_NR_ATTR_S {
	ISP_OP_TYPE_E enOpType;
	ISP_YNR_MOTION_NR_MANUAL_ATTR_S stManual;
	ISP_YNR_MOTION_NR_AUTO_ATTR_S stAuto;
} ISP_YNR_MOTION_NR_ATTR_S;

typedef struct _ISP_YNR_FILTER_MANUAL_ATTR_S {
	CVI_U8 LumaStr[8]; /*RW; Range:[0x0, 0x1f]*/
	CVI_U8 VarThr; /*RW; Range:[0x0, 0xff]*/
	CVI_U16 CoringWgtLF; /*RW; Range:[0x0, 0x100]*/
	CVI_U16 CoringWgtHF; /*RW; Range:[0x0, 0x100]*/
	CVI_U8 NonDirFiltStr; /*RW; Range:[0x0, 0x1f]*/
	CVI_U8 VhDirFiltStr; /*RW; Range:[0x0, 0x1f]*/
	CVI_U8 AaDirFiltStr; /*RW; Range:[0x0, 0x1f]*/
	CVI_U8 CoringWgtMax; /*RW; Range:[0x0, 0xff]*/
	CVI_U16 FilterMode; /*RW; Range:[0x0, 0x3ff]*/
} ISP_YNR_FILTER_MANUAL_ATTR_S;

typedef struct _ISP_YNR_FILTER_AUTO_ATTR_S {
	CVI_U8 LumaStr[8][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x1f]*/
	CVI_U8 VarThr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U16 CoringWgtLF[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x100]*/
	CVI_U16 CoringWgtHF[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x100]*/
	CVI_U8 NonDirFiltStr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x1f]*/
	CVI_U8 VhDirFiltStr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x1f]*/
	CVI_U8 AaDirFiltStr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x1f]*/
	CVI_U8 CoringWgtMax[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U16 FilterMode[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x3ff]*/
} ISP_YNR_FILTER_AUTO_ATTR_S;

typedef struct _ISP_YNR_FILTER_ATTR_S {
	ISP_OP_TYPE_E enOpType;
	ISP_YNR_FILTER_MANUAL_ATTR_S stManual;
	ISP_YNR_FILTER_AUTO_ATTR_S stAuto;
} ISP_YNR_FILTER_ATTR_S;

//-----------------------------------------------------------------------------
//	uv domain noise reduction (CNR)
//-----------------------------------------------------------------------------
typedef struct _ISP_CNR_MANUAL_ATTR_S {
	CVI_U8 CnrStr; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 NoiseSuppressStr; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 NoiseSuppressGain; /*RW; Range:[0x1, 0x8]*/
	CVI_U8 FilterType; /*RW; Range:[0x0, 0x1f]*/
	CVI_U8 MotionNrStr; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 LumaWgt; /*RW; Range:[0x0, 0x8]*/
	CVI_U8 DetailSmoothMode; /*RW; Range:[0x0, 0x1]*/
} ISP_CNR_MANUAL_ATTR_S;

typedef struct _ISP_CNR_AUTO_ATTR_S {
	CVI_U8 CnrStr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 NoiseSuppressStr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 NoiseSuppressGain[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x1, 0x8]*/
	CVI_U8 FilterType[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x1f]*/
	CVI_U8 MotionNrStr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 LumaWgt[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x8]*/
	CVI_U8 DetailSmoothMode[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x1]*/
} ISP_CNR_AUTO_ATTR_S;

typedef struct _ISP_CNR_ATTR_S {
	CVI_BOOL Enable; /*RW; Range:[0, 1]*/
	ISP_OP_TYPE_E enOpType;
	CVI_U8 UpdateInterval;
	ISP_CNR_MANUAL_ATTR_S stManual;
	ISP_CNR_AUTO_ATTR_S stAuto;
} ISP_CNR_ATTR_S;

typedef struct _ISP_CNR_MOTION_NR_MANUAL_ATTR_S {
	CVI_U8 MotionCnrStrLut[16]; /*RW; Range:[0x0, 0xff]*/
} ISP_CNR_MOTION_NR_MANUAL_ATTR_S;

typedef struct _ISP_CNR_MOTION_NR_AUTO_ATTR_S {
	CVI_U8 MotionCnrStrLut[16][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
} ISP_CNR_MOTION_NR_AUTO_ATTR_S;

typedef struct _ISP_CNR_MOTION_NR_ATTR_S {
	CVI_BOOL MotionCnrEnable; /*RW; Range:[0, 1]*/
	ISP_CNR_MOTION_NR_MANUAL_ATTR_S stManual;
	ISP_CNR_MOTION_NR_AUTO_ATTR_S stAuto;
} ISP_CNR_MOTION_NR_ATTR_S;

//-----------------------------------------------------------------------------
//  CAC
//-----------------------------------------------------------------------------
typedef struct _ISP_CAC_MANUAL_ATTR_S {
	CVI_U8 DePurpleStr; /*RW; Range:[0x0, 0xff]*/
} ISP_CAC_MANUAL_ATTR_S;

typedef struct _ISP_CAC_AUTO_ATTR_S {
	CVI_U8 DePurpleStr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
} ISP_CAC_AUTO_ATTR_S;

typedef struct _ISP_CAC_ATTR_S {
	CVI_BOOL Enable; /*RW; Range:[0, 1]*/
	ISP_OP_TYPE_E enOpType;
	CVI_U8 UpdateInterval;
	CVI_U16 VarThr; /*RW; Range:[0x0, 0x1ff]*/
	CVI_U8 PurpleDetRange; /*RW; Range:[0x0, 0x80]*/
	CVI_U8 PurpleCb; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 PurpleCr; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 PurpleCb2; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 PurpleCr2; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 PurpleCb3; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 PurpleCr3; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 GreenCb; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 GreenCr; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 TuningMode; /*RW; Range:[0x0, 0x2]*/
	ISP_CAC_MANUAL_ATTR_S stManual;
	ISP_CAC_AUTO_ATTR_S stAuto;
} ISP_CAC_ATTR_S;

//-----------------------------------------------------------------------------
//  Sharpen
//-----------------------------------------------------------------------------
typedef struct _ISP_SHARPEN_MANUAL_ATTR_S {
	CVI_U8 EdgeGain; /*RW; Range:[0x0, 0x3f]*/
	CVI_U16 TextureGain; /*RW; Range:[0x0, 0x4]*/
	CVI_U8 EdgeFreq; /*RW; Range:[0x0, 0x4]*/
	CVI_U8 TextureFreq; /*RW; Range:[0x0, 0x4]*/
	CVI_U16 LumaAdpCoring; /*RW; Range:[0x0, 0x3ff]*/
	CVI_U8 LumaAdpGain[SHARPEN_LUT_NUM]; /*RW; Range:[0x0, 0x3f]*/
	CVI_U8 GlobalGain; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 EdgeGlobalGain; /*RW; Range:[0x0, 0x3f]*/
	CVI_U8 DeltaShtCtrl[SHARPEN_LUT_NUM]; /*RW; Range:[0x0, 0x3f]*/
	CVI_U8 DeltaShtCtrlUnit; /*RW; Range:[0x0, 0x3]*/
	CVI_U8 OverShootThr; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 UnderShootThr; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 YNoiseLevel; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 OverShootThrMax; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 UnderShootThrMin; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 MotionShtGainIn[4]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 MotionShtGainOut[4]; /*RW; Range:[0x0, 0xff]*/
	CVI_BOOL NoiseSuppressMode; /*RW; Range:[0, 1]*/
} ISP_SHARPEN_MANUAL_ATTR_S;

typedef struct _ISP_SHARPEN_AUTO_ATTR_S {
	CVI_U8 EdgeGain[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x3f]*/
	CVI_U16 TextureGain[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x4]*/
	CVI_U8 EdgeFreq[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x4]*/
	CVI_U8 TextureFreq[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x4]*/
	CVI_U16 LumaAdpCoring[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x3ff]*/
	CVI_U8 LumaAdpGain[SHARPEN_LUT_NUM][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x3f]*/
	CVI_U8 GlobalGain[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 EdgeGlobalGain[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x3f]*/
	CVI_U8 DeltaShtCtrl[SHARPEN_LUT_NUM][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x3f]*/
	CVI_U8 DeltaShtCtrlUnit[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0x3]*/
	CVI_U8 OverShootThr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 UnderShootThr[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 YNoiseLevel[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 OverShootThrMax[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 UnderShootThrMin[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 MotionShtGainIn[4][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 MotionShtGainOut[4][ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0x0, 0xff]*/
	CVI_BOOL NoiseSuppressMode[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW; Range:[0, 1]*/
} ISP_SHARPEN_AUTO_ATTR_S;

typedef struct _ISP_SHARPEN_ATTR_S {
	CVI_BOOL Enable; /*RW; Range:[0x0, 0x1]*/
	ISP_OP_TYPE_E enOpType;
	CVI_U8 UpdateInterval;
	CVI_U8 TuningMode; /*RW; Range:[0x0, 0xb]*/
	CVI_U8 TuningModeLevelShift; /*RW; Range:[0x0, 0x7]*/
	CVI_BOOL FilterCoefByISOFuncEn; /*RW; Range:[0x0, 0x1]*/
	CVI_U32 FilterCoefByISOLowThd; /*RW; Range:[0x0, 0xffffffff]*/
	CVI_U32 FilterCoefByISOHighThd; /*RW; Range:[0x0, 0xffffffff]*/
	CVI_U8 FilterCoefNonDirType; /*RW; Range:[0x0, 0xff]*/
	CVI_BOOL LumaAdpCoringEn; /*RW; Range:[0x0, 0x1]*/
	CVI_BOOL LumaAdpGainEn; /*RW; Range:[0x0, 0x1]*/
	CVI_BOOL DeltaAdpGainEn; /*RW; Range:[0x0, 0x1]*/
	CVI_U8 DeltaAdpGain[SHARPEN_LUT_NUM]; /*RW; Range:[0x0, 0x3f]*/
	CVI_BOOL DeltaShtCtrlEn; /*RW; Range:[0x0, 0x1]*/
	CVI_BOOL WdrCoringCompensationEn; /*RW; Range:[0x0, 0x1]*/
	CVI_U8 WdrCoringCompensationMode; /*RW; Range:[0x0, 0x1]*/
	CVI_U16 WdrCoringToleranceLevel; /*RW; Range:[0x0, 0x1ff]*/
	CVI_U8 WdrCoringHighThr; /*RW; Range:[0x0, 0xff]*/
	CVI_U8 WdrCoringLowThr; /*RW; Range:[0x0, 0xff]*/
	CVI_BOOL NoiseSuppressEnable; /*RW; Range:[0, 1]*/

	ISP_SHARPEN_MANUAL_ATTR_S stManual;
	ISP_SHARPEN_AUTO_ATTR_S stAuto;
} ISP_SHARPEN_ATTR_S;

//-----------------------------------------------------------------------------
//  Y Contrast
//-----------------------------------------------------------------------------
typedef struct _ISP_YCONTRAST_MANUAL_ATTR_S {
	CVI_U8 ContrastLow; /*RW; Range:[0x0, 0x64]*/
	CVI_U8 ContrastHigh; /*RW; Range:[0x0, 0x64]*/
	CVI_U8 CenterLuma; /*RW; Range:[0x0, 0x40]*/
} ISP_YCONTRAST_MANUAL_ATTR_S;

typedef struct _ISP_YCONTRAST_AUTO_ATTR_S {
	CVI_U8 ContrastLow[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0x64]*/
	CVI_U8 ContrastHigh[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0x64]*/
	CVI_U8 CenterLuma[ISP_AUTO_LV_NUM]; /*RW; Range:[0x0, 0x40]*/
} ISP_YCONTRAST_AUTO_ATTR_S;

typedef struct _ISP_YCONTRAST_ATTR_S {
	CVI_BOOL Enable; /*RW; Range:[0, 1]*/
	ISP_OP_TYPE_E enOpType;
	CVI_U8 UpdateInterval;
	ISP_YCONTRAST_MANUAL_ATTR_S stManual;
	ISP_YCONTRAST_AUTO_ATTR_S stAuto;
} ISP_YCONTRAST_ATTR_S;

//-----------------------------------------------------------------------------
//  Mono
//-----------------------------------------------------------------------------
typedef struct _ISP_MONO_ATTR_S {
	CVI_BOOL Enable;
	CVI_U8 UpdateInterval;
} ISP_MONO_ATTR_S;

#define MAX_VPSS_ADJUSTMENT_GRP 16
typedef struct _VPSS_ATTR_S {
	CVI_S32 brightness[MAX_VPSS_ADJUSTMENT_GRP];
	CVI_S32 contrast[MAX_VPSS_ADJUSTMENT_GRP];
	CVI_S32 saturation[MAX_VPSS_ADJUSTMENT_GRP];
	CVI_S32 hue[MAX_VPSS_ADJUSTMENT_GRP];
} VPSS_ADJUSTMENT_ATTR_S;

/* DIS */
typedef enum _ISP_DIS_MODE_E {
	DIS_MODE_2_DOF_GME = 0, /* Only use with GME in 2 dof */
	DIS_MODE_DOF_BUTT,
} DIS_MODE_E;

typedef enum _ISP_DIS_MOTION_LEVEL_E {
	DIS_MOTION_LEVEL_NORMAL = 1,
	DIS_MOTION_LEVEL_BUTT
} DIS_MOTION_LEVEL_E;

typedef struct _ISP_DIS_CONFIG_S {
	DIS_MODE_E mode;
	DIS_MOTION_LEVEL_E motionLevel;
	CVI_U32 cropRatio;
} ISP_DIS_CONFIG_S;

typedef struct _ISP_DIS_ATTR_S {
	CVI_BOOL enable;
	CVI_U32 movingSubjectLevel;
	CVI_U32 horizontalLimit;
	CVI_U32 verticalLimit;
	CVI_BOOL stillCrop;
} ISP_DIS_ATTR_S;

typedef struct _ISP_INNER_STATE_INFO_S {
	CVI_U32 wdrHistBinNum;
	CVI_U32 blcOffsetR;
	CVI_U32 blcOffsetGr;
	CVI_U32 blcOffsetGb;
	CVI_U32 blcOffsetB;
	CVI_U32 blcGainR;
	CVI_U32 blcGainGr;
	CVI_U32 blcGainGb;
	CVI_U32 blcGainB;
	CVI_S32 ccm[9];
	CVI_U32 wdrHistogramBefore[MAX_HIST_BINS * MAX_EXPOSURE_RATIO];
	CVI_U32 wdrHistogramAfter[MAX_HIST_BINS * MAX_EXPOSURE_RATIO];
	CVI_U32 drcDarkTone[LTM_DARK_CURVE_NODE_NUM];
	CVI_U32 drcBrightTone[LTM_BRIGHT_CURVE_NODE_NUM];
	CVI_BOOL bWDRSwitchFinish;
	// For 2TO1/3TO1/4TO1 use.
	CVI_U32 u32WDRExpRatioActual[ISP_WDR_FRAME_IDX_SIZE];
} ISP_INNER_STATE_INFO_S;

typedef struct _ISP_VC_ATTR_S {
	CVI_U8 MotionThreshold; /*RW; Range:[0x0, 0xff] */
} ISP_VC_ATTR_S;

// Backward Compatible
// #include "cvi_isp_backwardcompatible.h"

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __CVI_COMM_ISP_H__ */

