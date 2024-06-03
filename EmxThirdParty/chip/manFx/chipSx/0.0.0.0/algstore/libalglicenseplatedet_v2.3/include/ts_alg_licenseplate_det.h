/*
 * @file: ts_alg_licenseplate_det.h
 * @brief: 
 * @version:0.1v
 * @date: 2023/10/12
 * @author: zzy
 * @copyright : Copyright (c) 2023 tsingmicro
 */

#ifndef _TS_ts_alg_licenseplate_det_H
#define _TS_ts_alg_licenseplate_det_H

#include "ts_alg_type.h"

#define MAX_LICENSE_PLATE_DET_DETECT_NUM 25

typedef struct tsALG_LICENSE_PLATE_DET_BOX_S {
	// the range of x and y is 0.0f~1.0f
	TS_FLOAT f32Xmin;
	TS_FLOAT f32Ymin;
	TS_FLOAT f32Xmax;
	TS_FLOAT f32Ymax;
	TS_FLOAT f32Score;
	TS_FLOAT f32Reserve;
} ALG_LICENSE_PLATE_DET_BOX_S;

typedef struct tsALG_LICENSE_PLATE_DET_RESULT_S {
	TS_U32 u32LicensePlateDetNum;
	// TS_U32 u32licenseplatedet[MAX_EBIKE_DETECT_NUM];
	ALG_LICENSE_PLATE_DET_BOX_S stBox[MAX_LICENSE_PLATE_DET_DETECT_NUM];
} ALG_LICENSE_PLATE_DET_RESULT_S;

typedef struct tsALG_LICENSE_PLATE_DET_PARAM_S {
	TS_FLOAT f32Thresh;
	TS_FLOAT f32IouThresh; //default 0.45
} ALG_LICENSE_PLATE_DET_PARAM_S;

TS_S32 TS_ALG_LicensePlateDet_Init(TS_VOID **ppHandle, ALG_MODEL_INIT_S *pParam);
TS_S32 TS_ALG_LicensePlateDet_Exit(TS_VOID *pHandle);
TS_S32 TS_ALG_LicensePlateDet_Process(TS_VOID *pHandle, ALG_IMAGE_S *pImage, ALG_LICENSE_PLATE_DET_RESULT_S *pKeypoint);
TS_S32 TS_ALG_LicensePlateDet_SetParam(TS_VOID *pHandle, ALG_LICENSE_PLATE_DET_PARAM_S *pParam);
TS_S32 TS_ALG_LicensePlateDet_GetParam(TS_VOID *pHandle, ALG_LICENSE_PLATE_DET_PARAM_S *pParam);
const TS_CHAR *TS_ALG_LicensePlateDet_GetVersion(TS_VOID);

#endif
