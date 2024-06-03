/* SigmaStar trade secret */
/* Copyright (c) [2021~2022] SigmaStar Technology.
All rights reserved.

Unless otherwise stipulated in writing, any and all information contained
herein regardless in any format shall remain the sole proprietary of
SigmaStar and be kept in strict confidence
(SigmaStar Confidential Information) by the recipient.
Any unauthorized act including without limitation unauthorized disclosure,
copying, use, reproduction, sale, distribution, modification, disassembling,
reverse engineering and compiling of the contents of SigmaStar Confidential
Information is unlawful and strictly prohibited. SigmaStar hereby reserves the
rights to any and all damages, losses, costs and expenses resulting therefrom.
*/

#ifndef _MI_NIR_IQ_DATATYPE_H_
#define _MI_NIR_IQ_DATATYPE_H_

#define MI_NIR_AUTO_NUM 16
#define BLEND_SAT_X_NUM 8
#define BLEND_SAT_Y_NUM 9
#define CONTRAST_X_NUM  8
#define CONTRAST_Y_NUM  9
#define SAT_X_NUM       8
#define SAT_Y_NUM       9
#define WEIGHT_X_NUM    13
#define WEIGHT_Y_NUM    14

typedef enum __attribute__((aligned(4)))
{
    E_API_BLEND_SAT = 0, //[00]
    E_API_CONTRAST,      //[01]
    E_API_SATURATION,    //[02]
    E_API_WEIGHT,        //[03]
    E_API_MAX,           // for para reset
} MI_NIR_IQ_ApiId_e;

typedef enum
{
    E_NIR_IQ_FALSE = 0,
    E_NIR_IQ_TRUE  = !E_NIR_IQ_FALSE,
    E_NIR_IQ_BOOL_MAX,
} MI_NIR_IQ_Bool_e;

typedef enum
{
    E_NIR_IQ_OP_TYP_AUTO   = 0,
    E_NIR_IQ_OP_TYP_MANUAL = !E_NIR_IQ_OP_TYP_AUTO,
    E_NIRNIR_IQ_OP_TYP_MODE_MAX,
} MI_NIR_IQ_OpType_e;

typedef struct MI_NIR_IQ_BlendingSaturationGain_s
{
    MI_U8 u8YGain; // 0~255
    MI_U8 u8MaxUVRatio;
} MI_NIR_IQ_BlendingSaturationGain_t;

typedef struct MI_NIR_IQ_BlendingSaturationCurve_s
{
    MI_U16 u16CurveX[BLEND_SAT_X_NUM];
    MI_U16 u16CurveY[BLEND_SAT_Y_NUM];
} MI_NIR_IQ_BlendingSaturationCurve_t;

typedef struct MI_NIR_IQ_BlendingSaturationParam_s
{
    union
    {
        MI_NIR_IQ_BlendingSaturationGain_t  stBlendingSatGain;
        MI_NIR_IQ_BlendingSaturationCurve_t stBlendingSatLut;
    };
} MI_NIR_IQ_BlendingSaturationParam_t;

typedef struct MI_NIR_IQ_BlendingSaturationAutoAttr_s
{
    MI_NIR_IQ_BlendingSaturationParam_t stParaAPI[MI_NIR_AUTO_NUM];
} MI_NIR_IQ_BlendingSaturationAutoAttr_t;

typedef struct MI_NIR_IQ_BlendingSaturationManualAttr_s
{
    MI_NIR_IQ_BlendingSaturationParam_t stParaAPI;
} MI_NIR_IQ_BlendingSaturationManualAttr_t;

typedef struct MI_NIR_IQ_BlendingSaturationAttr_s
{
    MI_NIR_IQ_Bool_e                         bEnable;
    MI_NIR_IQ_OpType_e                       enOpType;
    MI_NIR_IQ_BlendingSaturationAutoAttr_t   stAuto;
    MI_NIR_IQ_BlendingSaturationManualAttr_t stManual;
} MI_NIR_IQ_BlendingSaturationAttr_t;

typedef struct MI_NIR_IQ_ContrastParam_s
{
    MI_U16 u16ContrastThe;
    MI_U16 u16ContrastMin;
    MI_U16 u16CurveX[CONTRAST_X_NUM];
    MI_U16 u16CurveY[CONTRAST_Y_NUM];
} MI_NIR_IQ_ContrastParam_t;

typedef struct MI_NIR_IQ_ContrastAutoAttr_s
{
    MI_NIR_IQ_ContrastParam_t stParaAPI[MI_NIR_AUTO_NUM];
} MI_NIR_IQ_ContrastAutoAttr_t;

typedef struct MI_NIR_IQ_ContrastManualAttr_s
{
    MI_NIR_IQ_ContrastParam_t stParaAPI;
} MI_NIR_IQ_ContrastManualAttr_t;

typedef struct MI_NIR_IQ_ContrastAttr_s
{
    MI_NIR_IQ_Bool_e               bEnable;
    MI_NIR_IQ_OpType_e             enOpType;
    MI_NIR_IQ_ContrastAutoAttr_t   stAuto;
    MI_NIR_IQ_ContrastManualAttr_t stManual;
} MI_NIR_IQ_ContrastAttr_t;

typedef struct MI_NIR_IQ_SaturationParam_s
{
    MI_U16 u16SatConf;
    MI_U16 u16CurveX[SAT_X_NUM];
    MI_U16 u16CurveY[SAT_Y_NUM];
} MI_NIR_IQ_SaturationParam_t;

typedef struct MI_NIR_IQ_SaturationAutoAttr_s
{
    MI_NIR_IQ_SaturationParam_t stParaAPI[MI_NIR_AUTO_NUM];
} MI_NIR_IQ_SaturationAutoAttr_t;

typedef struct MI_NIR_IQ_SaturationManualAttr_s
{
    MI_NIR_IQ_SaturationParam_t stParaAPI;
} MI_NIR_IQ_SaturationManualAttr_t;

typedef struct MI_NIR_IQ_SaturationAttr_s
{
    MI_NIR_IQ_Bool_e                 bEnable;
    MI_NIR_IQ_OpType_e               enOpType;
    MI_NIR_IQ_SaturationAutoAttr_t   stAuto;
    MI_NIR_IQ_SaturationManualAttr_t stManual;
} MI_NIR_IQ_SaturationAttr_t;

typedef struct MI_NIR_IQ_WeightParam_s
{
    MI_U16 u16VisCurveX[WEIGHT_X_NUM];
    MI_U16 u16VisCurveY[WEIGHT_Y_NUM];
    MI_U16 u16NirCurveX[WEIGHT_X_NUM];
    MI_U16 u16NirCurveY[WEIGHT_Y_NUM];
} MI_NIR_IQ_WeightParam_t;

typedef struct MI_NIR_IQ_WeightAutoAttr_s
{
    MI_NIR_IQ_WeightParam_t stParaAPI[MI_NIR_AUTO_NUM];
} MI_NIR_IQ_WeightAutoAttr_t;

typedef struct MI_NIR_IQ_WeightManualAttr_s
{
    MI_NIR_IQ_WeightParam_t stParaAPI;
} MI_NIR_IQ_WeightManualAttr_t;

typedef struct MI_NIR_IQ_WeightAttr_s
{
    MI_NIR_IQ_Bool_e             bEnable;
    MI_NIR_IQ_OpType_e           enOpType;
    MI_NIR_IQ_WeightAutoAttr_t   stAuto;
    MI_NIR_IQ_WeightManualAttr_t stManual;
} MI_NIR_IQ_WeightAttr_t;

#endif /* _MI_NIR_IQ_DATATYPE_H_ */
