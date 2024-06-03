/*
 * Copyright © 2019 TSING MICRO.com.cn All Rights Reserved
 * Description: ts_comm_sns.h
 * Author:TsingMicro multimedia software group
 * Create: 2022/06/02
 */

#ifndef __TS_COMM_SNS_H__
#define __TS_COMM_SNS_H__

#include "ts_type.h"
#include "ts_common.h"
#include "ts_comm_isp.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

typedef struct tsISP_CMOS_BLACK_LEVEL_S
{
    TS_BOOL bUpdate;
    TS_U16  au16BlackLevel[ISP_BAYER_CHN_NUM];
} ISP_CMOS_BLACK_LEVEL_S;

typedef struct tsISP_SNS_ATTR_INFO_S
{
    SENSOR_ID            eSensorId;
} ISP_SNS_ATTR_INFO_S;

#define ISP_SPLIT_POINT_NUM      (5)
#define ISP_EXPANDER_POINT_NUM   (5)

typedef struct tsISP_CMOS_DEMOSAIC_S
{
    TS_BOOL bEnable;
    TS_U8   au8NonDirStr[ISP_AUTO_ISO_STRENGTH_NUM];
    TS_U8   au8NonDirMFDetailEhcStr[ISP_AUTO_ISO_STRENGTH_NUM];
    TS_U8   au8NonDirHFDetailEhcStr[ISP_AUTO_ISO_STRENGTH_NUM];
    TS_U8   au8DetailSmoothRange[ISP_AUTO_ISO_STRENGTH_NUM];
} ISP_CMOS_DEMOSAIC_S;

#define WDR_MAX_FRAME    (2)

typedef struct tsISP_CMOS_BAYERNR_S
{
    TS_BOOL  bEnable;
    TS_BOOL  bBnrMonoSensorEn;
    TS_BOOL  bNrLscEnable;
    TS_U8    u8BnrLscMaxGain;
    TS_U16   u16BnrLscCmpStrength;

    TS_U8    au8LutFineStr[ISP_AUTO_ISO_STRENGTH_NUM];
    TS_U8    au8ChromaStr[ISP_BAYER_CHN_NUM][ISP_AUTO_ISO_STRENGTH_NUM];
    TS_U8    au8WDRFrameStr[WDR_MAX_FRAME_NUM];
    TS_U8    au8FusionFrameStr[WDR_MAX_FRAME_NUM];
    TS_U16   au16CoarseStr[ISP_BAYER_CHN_NUM][ISP_AUTO_ISO_STRENGTH_NUM];
    TS_U16   au16LutCoringWgt[ISP_AUTO_ISO_STRENGTH_NUM];
    TS_U16   au16LutCoringRatio[TS_ISP_BAYERNR_LUT_LENGTH];
} ISP_CMOS_BAYERNR_S;

typedef struct tsISP_CMOS_NOISE_CALIBRATION_S
{
    TS_U16   u16CalibrationLutNum;
    TS_FLOAT afCalibrationCoef[BAYER_CALIBTAION_MAX_NUM][3];
} ISP_CMOS_NOISE_CALIBRATION_S;

typedef struct tsISP_CMOS_DETAIL_S
{
    ISP_DE_ATTR_S stAttr;
} ISP_CMOS_DETAIL_S;


typedef struct tsISP_CMOS_SHARPEN_MANUAL_S
{
    TS_U8  au8LumaWgt[ISP_SHARPEN_LUMA_NUM];        /* RW; Range: [0, 127]; Adjust the sharpen strength according to luma. Sharpen strength will be weaker when it decrease. */
    TS_U16 au16TextureStr[ISP_SHARPEN_GAIN_NUM];    /* RW; Range: [0, 4095]; Format:7.5;Undirectional sharpen strength for texture and detail enhancement*/
    TS_U16 au16EdgeStr[ISP_SHARPEN_GAIN_NUM];       /* RW; Range: [0, 4095]; Format:7.5;Directional sharpen strength for edge enhancement*/
    TS_U16 u16TextureFreq;         /* RW; Range: [0, 4095];Format:6.6; Texture frequency adjustment. Texture and detail will be finer when it increase*/
    TS_U16 u16EdgeFreq;            /* RW; Range: [0, 4095];Format:6.6; Edge frequency adjustment. Edge will be narrower and thiner when it increase*/
    TS_U8  u8OverShoot;            /* RW; Range: [0, 127]; Format:7.0;u8OvershootAmt*/
    TS_U8  u8UnderShoot;           /* RW; Range: [0, 127]; Format:7.0;u8UndershootAmt*/
    TS_U8  u8ShootSupStr;          /* RW; Range: [0, 255]; Format:8.0;overshoot and undershoot suppression strength, the amplitude and width of shoot will be decrease when shootSupSt increase*/
    TS_U8  u8ShootSupAdj;          /* RW; Range: [0, 15]; Format:4.0;overshoot and undershoot suppression adjusting, adjust the edge shoot suppression strength*/
    TS_U8  u8DetailCtrl;           /* RW; Range: [0, 255]; Format:8.0;Different sharpen strength for detail and edge. When it is bigger than 128, detail sharpen strength will be stronger than edge.*/
    TS_U8  u8DetailCtrlThr;        /* RW; Range: [0, 255]; Format:8.0; The threshold of DetailCtrl, it is used to distinguish detail and edge. */
    TS_U8  u8EdgeFiltStr;          /* RW; Range: [0, 63]; Format:6.0;The strength of edge filtering.*/
    TS_U8  u8RGain;                /* RW; Range: [0, 32]; Format:5.0;Sharpen Gain for Red Area*/
    TS_U8  u8BGain;                /* RW; Range: [0, 32]; Format:5.0;Sharpen Gain for Blue Area*/
    TS_U8  u8SkinGain;             /* RW; Range: [0, 31]; Format:5.0;Sharpen Gain for Skin Area*/
    TS_U16 u16MaxSharpGain;        /* RW; Range: [0, 0x7FF]; Format:8.3; Maximum sharpen gain*/
} ISP_CMOS_SHARPEN_MANUAL_S;


typedef struct tsISP_CMOS_SHARPEN_AUTO_S
{
    TS_U8  au8LumaWgt[ISP_SHARPEN_LUMA_NUM][ISP_AUTO_ISO_STRENGTH_NUM];      /* RW; Range: [0, 127];  Adjust the sharpen strength according to luma. Sharpen strength will be weaker when it decrease. */
    TS_U16 au16TextureStr[ISP_SHARPEN_GAIN_NUM][ISP_AUTO_ISO_STRENGTH_NUM];  /* RW; Range: [0, 4095]; Format:7.5;Undirectional sharpen strength for texture and detail enhancement*/
    TS_U16 au16EdgeStr[ISP_SHARPEN_GAIN_NUM][ISP_AUTO_ISO_STRENGTH_NUM];     /* RW; Range: [0, 4095]; Format:7.5;Directional sharpen strength for edge enhancement*/
    TS_U16 au16TextureFreq[ISP_AUTO_ISO_STRENGTH_NUM];         /* RW; Range: [0, 4095]; Format:6.6;Texture frequency adjustment. Texture and detail will be finer when it increase*/
    TS_U16 au16EdgeFreq[ISP_AUTO_ISO_STRENGTH_NUM];            /* RW; Range: [0, 4095]; Format:6.6;Edge frequency adjustment. Edge will be narrower and thiner when it increase*/
    TS_U8  au8OverShoot[ISP_AUTO_ISO_STRENGTH_NUM];            /* RW; Range: [0, 127];  Format:7.0;u8OvershootAmt*/
    TS_U8  au8UnderShoot[ISP_AUTO_ISO_STRENGTH_NUM];           /* RW; Range: [0, 127];  Format:7.0;u8UndershootAmt*/
    TS_U8  au8ShootSupStr[ISP_AUTO_ISO_STRENGTH_NUM];          /* RW; Range: [0, 255]; Format:8.0;overshoot and undershoot suppression strength, the amplitude and width of shoot will be decrease when shootSupSt increase*/
    TS_U8  au8ShootSupAdj[ISP_AUTO_ISO_STRENGTH_NUM];          /* RW; Range: [0, 15]; Format:4.0;overshoot and undershoot suppression adjusting, adjust the edge shoot suppression strength*/
    TS_U8  au8DetailCtrl[ISP_AUTO_ISO_STRENGTH_NUM];           /* RW; Range: [0, 255]; Format:8.0;Different sharpen strength for detail and edge. When it is bigger than 128, detail sharpen strength will be stronger than edge.*/
    TS_U8  au8DetailCtrlThr[ISP_AUTO_ISO_STRENGTH_NUM];        /* RW; Range: [0, 255]; Format:8.0; The threshold of DetailCtrl, it is used to distinguish detail and edge. */
    TS_U8  au8EdgeFiltStr[ISP_AUTO_ISO_STRENGTH_NUM];          /* RW; Range: [0, 63]; Format:6.0;The strength of edge filtering.*/
    TS_U8  au8RGain[ISP_AUTO_ISO_STRENGTH_NUM];                /* RW; Range: [0, 32]; Format:5.0;Sharpen Gain for Red Area*/
    TS_U8  au8BGain[ISP_AUTO_ISO_STRENGTH_NUM];                /* RW; Range: [0, 32]; Format:5.0;Sharpen Gain for Blue Area*/
    TS_U8  au8SkinGain[ISP_AUTO_ISO_STRENGTH_NUM];             /* RW; Range: [0, 31]; Format:5.0;*/
    TS_U16 au16MaxSharpGain[ISP_AUTO_ISO_STRENGTH_NUM];        /* RW; Range: [0, 0x7FF]; Format:8.3;*/
} ISP_CMOS_SHARPEN_AUTO_S;

typedef struct tsISP_CMOS_SHARPEN_S
{
    TS_U8 u8SkinUmin;
    TS_U8 u8SkinVmin;
    TS_U8 u8SkinUmax;
    TS_U8 u8SkinVmax;
    ISP_CMOS_SHARPEN_MANUAL_S stManual;
    ISP_CMOS_SHARPEN_AUTO_S   stAuto;
} ISP_CMOS_SHARPEN_S;

typedef struct tsISP_CMOS_EDGEMARK_S
{
    TS_BOOL bEnable;               /*RW; Range:[0, 1]; Format:1.0;Enable/Disable Edge Mark*/
    TS_U8   u8Threshold;           /* RW; range: [0, 255];  Format:8.0;*/
    TS_U32  u32Color;              /* RW; range: [0, 0xFFFFFF];  Format:32.0;*/
} ISP_CMOS_EDGEMARK_S;

typedef struct tsISP_CMOS_HLC_S
{
    TS_BOOL bEnable;         /*RW; Range:[0, 1]; Format:1.0;Enable/Disable HLC module*/
    TS_U8   u8LumaThr;       /* RW; range: [0, 255];  Format:8.0;*/
    TS_U8   u8LumaTarget;    /* RW; range: [0, 255];  Format:8.0;*/
} ISP_CMOS_HLC_S;

typedef struct tsISP_CMOS_DRC_S
{
    TS_BOOL bEnable;

    TS_U16  u16ManualStrength;
    TS_U16  u16AutoStrength;

    TS_U8   u8SpatialFltCoef;
    TS_U8   u8RangeFltCoef;
    TS_U8   u8ContrastControl;
    TS_S8   s8DetailAdjustFactor;

    TS_U8   u8FltScaleFine;
    TS_U8   u8FltScaleCoarse;
    TS_U8   u8GradRevMax;
    TS_U8   u8GradRevThr;

    TS_U8   u8PDStrength;
    TS_U8   u8LocalMixingBrightMax;
    TS_U8   u8LocalMixingBrightMin;
    TS_U8   u8LocalMixingDarkMax;
    TS_U8   u8LocalMixingDarkMin;
    TS_U16  u16ColorCorrectionLut[33];

    TS_U8   u8Asymmetry;
    TS_U8   u8SecondPole;
    TS_U8   u8Stretch;
    TS_U8   u8Compress;

    TS_U8   u8CurveSel;

    TS_U16 au16Xpoint[5];
    TS_U16 au16Ypoint[5];
    TS_U16 au16Slope[5];
} ISP_CMOS_DRC_S;

typedef struct tsISP_CMOS_WDR_S
{
    TS_BOOL  bFusionMode;
    TS_BOOL  bMotionComp;

    TS_U16   u16ShortThr;
    TS_U16   u16LongThr;

    TS_BOOL  bForceLong;
    TS_U16   u16ForceLongLowThr;
    TS_U16   u16ForceLongHigThr;

    TS_BOOL  bShortExpoChk;
    TS_U16   u16ShortCheckThd;
    TS_BOOL  bMDRefFlicker;

    TS_U8    au8MdThrLowGain[ISP_AUTO_ISO_STRENGTH_NUM];
    TS_U8    au8MdThrHigGain[ISP_AUTO_ISO_STRENGTH_NUM];

    ISP_BNR_MODE_E  enBnrMode;
    TS_U16   au16FusionThr[WDR_MAX_FRAME];
    TS_U8    u8MdtStillThd;
    TS_U8    u8MdtLongBlend;
} ISP_CMOS_WDR_S;

typedef struct tsISP_CMOS_DEHAZE_S
{
    TS_BOOL       bEnable;           /* RW;Range:[0,1];Format:1.0;*/
    TS_BOOL       bUserLutEnable;    /*RW;Range:[0,1];0:Auto Lut 1:User Lut*/
    ISP_OP_TYPE_E enOpType;
    TS_U8         u8AutoStrength;
    TS_U8         u8ManualStrength;
} ISP_CMOS_DEHAZE_S;

typedef struct tsISP_CMOS_PREGAMMA_S
{
    TS_BOOL bEnable;
    TS_U32  au32PreGamma[PREGAMMA_NODE_NUM];
} ISP_CMOS_PREGAMMA_S;

#define GAMMA_NODE_NUMBER      257   //Update NODE NUMBER
#define GAMMA_EXT_NODE_NUMBER  1025  //Update NODE NUMBER
#define GAMMA_OUTSEG_NUMBER    8     //Update NODE NUMBER
typedef struct tsISP_CMOS_GAMMA_S
{
    TS_U16  au16Gamma[GAMMA_EXT_NODE_NUMBER];
} ISP_CMOS_GAMMA_S;

typedef struct tsISP_CMOS_SENSOR_MAX_RESOLUTION_S
{
    TS_U32  u32MaxWidth;
    TS_U32  u32MaxHeight;
} ISP_CMOS_SENSOR_MAX_RESOLUTION_S;

typedef struct tsISP_CMOS_DPC_S
{
    TS_U16  au16Strength[ISP_AUTO_ISO_STRENGTH_NUM];
    TS_U16  au16BlendRatio[ISP_AUTO_ISO_STRENGTH_NUM];
} ISP_CMOS_DPC_S;

typedef struct tsISP_LSC_CABLI_TABLE_S
{
    TS_U16 au16R_Gain[TS_ISP_LSC_GRID_POINTS];
    TS_U16 au16Gr_Gain[TS_ISP_LSC_GRID_POINTS];
    TS_U16 au16Gb_Gain[TS_ISP_LSC_GRID_POINTS];
    TS_U16 au16B_Gain[TS_ISP_LSC_GRID_POINTS];
} ISP_LSC_CABLI_TABLE_S;

typedef struct tsISP_BNR_LSC_CALIB_TABLE_S
{
    TS_U16 au16R_Gain[TS_ISP_RLSC_POINTS];
    TS_U16 au16Gr_Gain[TS_ISP_RLSC_POINTS];
    TS_U16 au16Gb_Gain[TS_ISP_RLSC_POINTS];
    TS_U16 au16B_Gain[TS_ISP_RLSC_POINTS];
} ISP_BNR_LSC_CALIB_TABLE_S;

typedef struct tsISP_CMOS_LSC_S
{
    TS_U16  u16MeshStrength;
    TS_U8   u8MeshScale;
    ISP_LSC_CABLI_TABLE_S astLscCalibTable[2];
    ISP_BNR_LSC_CALIB_TABLE_S stBNRLscCalibTable;
} ISP_CMOS_LSC_S;

typedef struct tsISP_CMOS_CA_S
{
    TS_BOOL   bEnable;
    TS_U16    au16YRatioLut[TS_ISP_CA_YRATIO_LUT_LENGTH];  //1.10bit  Y Ratio For UV ; Max = 2047 FW Limit
    TS_S16    as16ISORatio[ISP_AUTO_ISO_STRENGTH_NUM];     //1.10bit  ISO Ratio  For UV ,Max = 2047 FW Limi
} ISP_CMOS_CA_S;

typedef struct tsISP_CMOS_CLUT_S
{
    TS_BOOL bEnable;
    TS_U32  u32GainR;
    TS_U32  u32GainG;
    TS_U32  u32GainB;
    ISP_CLUT_LUT_S stClutLut;
} ISP_CMOS_CLUT_S;

typedef struct tsISP_CMOS_EXPANDER_POINT_S
{
    TS_U16 u16X;                    /*RW;Range:[0x0,0x101];Format:9.0;The X point of the knee */
    TS_U32 u32Y;                    /*RW;Range:[0x0,0x100000];Format:21.0;The Y point of the  knee */
} ISP_CMOS_EXPANDER_POINT_S;

typedef struct tsISP_CMOS_EXPANDER_S
{
    TS_BOOL bEnable;
    TS_U8   u8BitDepthIn;            /*RW;Range:[0xC,0x14];Format:5.0;The Bit depth of input */
    TS_U8   u8BitDepthOut;           /*RW;Range:[0xC,0x14];Format:5.0;The Bit depth of output */
    ISP_CMOS_EXPANDER_POINT_S astExpanderPoint[ISP_EXPANDER_POINT_NUM];
} ISP_CMOS_EXPANDER_S;
#if 0
typedef struct tsISP_CMOS_SPLIT_POINT_S
{
    TS_U8  u8X;                     /*RW;Range:[0x0,0x81];Format:8.0;The X point of the knee */
    TS_U16 u16Y;                    /*RW;Range:[0x0,0x8000];Format:16.0;The Y point of the  knee */
} ISP_CMOS_SPLIT_POINT_S;

typedef struct tsISP_CMOS_SPLIT_S
{
    TS_BOOL bValid;
    TS_BOOL bEnable;             /*RW;Range:[0x0,0x1];Format:1.0;*/
    TS_U8   u8InputWidthSel;     /*RW;Range:[0x0,0x3];Format:2.0;Inputwidthselect: 0=12bit; 1=14bit; 2=16bit; */
    TS_U8   u8ModeIn;            /*RW;Range:[0x0,0x3];Format:2.0;ModeIn: 0=linear; 2=16LOG; 3=sensor-built-in*/
    TS_U8   u8ModeOut;           /*RW;Range:[0x0,0x3];Format:2.0;ModeOut: 0= 16bit when decompress; 1=2chn ; 2=3chn; */
    TS_U32  u32BitDepthOut;      /*RW;Range:[0xC,0x14];Format:5.0;The Bit depth of output */
    ISP_CMOS_SPLIT_POINT_S astSplitPoint[ISP_SPLIT_POINT_NUM];
} ISP_CMOS_SPLIT_S;
#endif
typedef struct tsISP_CMOS_GE_S
{
    TS_BOOL bEnable;                                 /*RW,Range: [   0, 1]      */
    TS_U8  u8Slope;                                  /*RW,Range: [   0, 0xE]    */
    TS_U8  u8SensiSlope;                             /*RW,Range: [   0, 0xE]    */
    TS_U16 u16SensiThr;                              /*RW,Range: [   0, 0x3FFF] */
    TS_U16 au16Threshold[ISP_AUTO_ISO_STRENGTH_NUM]; /*RW,Range: [   0, 0x3FFF] */
    TS_U16 au16Strength[ISP_AUTO_ISO_STRENGTH_NUM];  /*RW,Range: [   0, 0x100]  */
    TS_U16 au16NpOffset[ISP_AUTO_ISO_STRENGTH_NUM];  /*RW,Range: [0x200, 0x3FFF] */
} ISP_CMOS_GE_S;

typedef struct tsISP_CMOS_ANTIFALSECOLOR_S
{
    TS_BOOL bEnable;                                                     /*RW;Range:[0x0,0x1];Format:1.0; AntiFalseColor Enable*/
    TS_U8   au8AntiFalseColorThreshold[ISP_AUTO_ISO_STRENGTH_NUM];       /*RW;Range:[0x0,0x20];Format:6.0;Threshold for antifalsecolor */
    TS_U8   au8AntiFalseColorStrength[ISP_AUTO_ISO_STRENGTH_NUM];        /*RW;Range:[0x0,0x1F];Format:5.0;Strength of antifalsecolor */
} ISP_CMOS_ANTIFALSECOLOR_S;

typedef struct tsISP_CMOS_LDCI_S
{
    TS_BOOL  bEnable;
    TS_U8    u8GaussLPFSigma;
    TS_U8    au8HePosWgt[ISP_AUTO_ISO_STRENGTH_NUM];
    TS_U8    au8HePosSigma[ISP_AUTO_ISO_STRENGTH_NUM];
    TS_U8    au8HePosMean[ISP_AUTO_ISO_STRENGTH_NUM];
    TS_U8    au8HeNegWgt[ISP_AUTO_ISO_STRENGTH_NUM];
    TS_U8    au8HeNegSigma[ISP_AUTO_ISO_STRENGTH_NUM];
    TS_U8    au8HeNegMean[ISP_AUTO_ISO_STRENGTH_NUM];
    TS_U16   au16BlcCtrl[ISP_AUTO_ISO_STRENGTH_NUM];
} ISP_CMOS_LDCI_S;

typedef struct tsISP_CMOS_SENSOR_MODE_S
{
    TS_U32  u32SensorID;
    TS_U8   u8SensorMode;
    TS_BOOL bValidDngRawFormat;
    DNG_RAW_FORMAT_S stDngRawFormat;
} ISP_CMOS_SENSOR_MODE_S;

typedef struct tsISP_CMOS_DNG_COLORPARAM_S
{
    ISP_DNG_WBGAIN_S stWbGain1;/*the calibration White balance gain of colorcheker in A Light*/
    ISP_DNG_WBGAIN_S stWbGain2;/*the calibration White balance gain of colorcheker in D50 Light*/
} ISP_CMOS_DNG_COLORPARAM_S;

typedef struct tsISP_CMOS_WDR_SWITCH_ATTR_S
{
    TS_U32   au32ExpRatio[EXP_RATIO_NUM];
}ISP_CMOS_WDR_SWITCH_ATTR_S;

typedef union tsISP_CMOS_ALG_KEY_U
{
    TS_U64  u64Key;
    struct
    {
        TS_U64  bit1Drc             : 1 ;   /* [0] */
        TS_U64  bit1Demosaic        : 1 ;   /* [1] */
        TS_U64  bit1PreGamma        : 1 ;   /* [2] */
        TS_U64  bit1Gamma           : 1 ;   /* [3] */
        TS_U64  bit1Sharpen         : 1 ;   /* [4] */
        TS_U64  bit1EdgeMark        : 1 ;   /* [5] */
        TS_U64  bit1Hlc             : 1 ;   /* [6] */
        TS_U64  bit1Ldci            : 1 ;   /* [7] */
        TS_U64  bit1Dpc             : 1 ;   /* [8] */
        TS_U64  bit1Lsc             : 1 ;   /* [9] */
        TS_U64  bit1Ge              : 1 ;   /* [10] */
        TS_U64  bit1AntiFalseColor  : 1 ;   /* [11] */
        TS_U64  bit1BayerNr         : 1 ;   /* [12] */
        TS_U64  bit1Detail          : 1 ;   /* [13] */
        TS_U64  bit1Ca              : 1 ;   /* [14] */
        TS_U64  bit1Expander        : 1 ;   /* [15] */
        TS_U64  bit1Clut            : 1 ;   /* [16] */
        TS_U64  bit1Wdr             : 1 ;   /* [17] */
        TS_U64  bit1Dehaze          : 1 ;   /* [18] */
        TS_U64  bit45Rsv            : 45;   /* [19:63] */
    };
} ISP_CMOS_ALG_KEY_U;

typedef struct tsISP_CMOS_DEFAULT_S
{
    ISP_CMOS_ALG_KEY_U               unKey;
    const ISP_CMOS_DRC_S             *pstDrc;
    const ISP_CMOS_DEMOSAIC_S        *pstDemosaic;
    const ISP_CMOS_PREGAMMA_S        *pstPreGamma;
    const ISP_CMOS_GAMMA_S           *pstGamma;
    const ISP_CMOS_SHARPEN_S         *pstSharpen;
    const ISP_CMOS_EDGEMARK_S        *pstEdgeMark;
    const ISP_CMOS_HLC_S             *pstHlc;
    const ISP_CMOS_LDCI_S            *pstLdci;
    const ISP_CMOS_DPC_S             *pstDpc;
    const ISP_CMOS_LSC_S             *pstLsc;
    const ISP_CMOS_GE_S              *pstGe;
    const ISP_CMOS_ANTIFALSECOLOR_S  *pstAntiFalseColor;
    const ISP_CMOS_BAYERNR_S         *pstBayerNr;
    const ISP_CMOS_DETAIL_S          *pstDetail;
    const ISP_CMOS_CA_S              *pstCa;
    const ISP_CMOS_EXPANDER_S        *pstExpander;
    const ISP_CMOS_CLUT_S            *pstClut;
    const ISP_CMOS_WDR_S             *pstWdr;
    const ISP_CMOS_DEHAZE_S          *pstDehaze;
    ISP_CMOS_NOISE_CALIBRATION_S     stNoiseCalibration;
    ISP_CMOS_SENSOR_MAX_RESOLUTION_S stSensorMaxResolution;
    ISP_CMOS_SENSOR_MODE_S           stSensorMode;
    ISP_CMOS_DNG_COLORPARAM_S        stDngColorParam;
    ISP_CMOS_WDR_SWITCH_ATTR_S       stWdrSwitchAttr;
} ISP_CMOS_DEFAULT_S;

typedef struct tsISP_CMOS_SENSOR_IMAGE_MODE_S
{
    TS_U16   u16Width;
    TS_U16   u16Height;
    TS_FLOAT f32Fps;
    TS_U8    u8SnsMode;
} ISP_CMOS_SENSOR_IMAGE_MODE_S;

typedef struct tsISP_SENSOR_EXP_FUNC_S
{
    TS_VOID(*pfn_cmos_sensor_init)(VI_PIPE ViPipe);
    TS_VOID(*pfn_cmos_sensor_exit)(VI_PIPE ViPipe);
    TS_VOID(*pfn_cmos_sensor_global_init)(VI_PIPE ViPipe);
    TS_S32 (*pfn_cmos_set_image_mode)(VI_PIPE ViPipe, ISP_CMOS_SENSOR_IMAGE_MODE_S *pstSensorImageMode);
    TS_S32 (*pfn_cmos_set_wdr_mode)(VI_PIPE ViPipe, TS_U8 u8Mode);

    /* the algs get data which is associated with sensor, except 3a */
    TS_S32(*pfn_cmos_get_isp_default)(VI_PIPE ViPipe, ISP_CMOS_DEFAULT_S *pstDef);
    TS_S32(*pfn_cmos_get_isp_black_level)(VI_PIPE ViPipe, ISP_CMOS_BLACK_LEVEL_S *pstBlackLevel);
    TS_S32(*pfn_cmos_get_sns_reg_info)(VI_PIPE ViPipe, ISP_SNS_REGS_INFO_S *pstSnsRegsInfo);

    /* the function of sensor set pixel detect */
    TS_VOID(*pfn_cmos_set_pixel_detect)(VI_PIPE ViPipe, TS_BOOL bEnable);
} ISP_SENSOR_EXP_FUNC_S;

typedef struct tsISP_SENSOR_REGISTER_S
{
    ISP_SENSOR_EXP_FUNC_S stSnsExp;
} ISP_SENSOR_REGISTER_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /*__TS_COMM_SNS_H__ */
