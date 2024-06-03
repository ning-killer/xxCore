/*
 * @file: ts_alg_license_plate.h
 * @brief: 
 * @version:0.1v
 * @date: 2023/10/12
 * @author: zzy
 * @copyright : Copyright (c) 2023 tsingmicro
 */

#ifndef _TS_ts_alg_license_plate_H
#define _TS_ts_alg_license_plate_H

#include "ts_alg_type.h"

#define ALG_LICENSE_PLATE_IMAGE_C 3
#define ALG_LICENSE_PLATE_IMAGE_H 24
#define ALG_LICENSE_PLATE_IMAGE_W 94

typedef struct tsALG_LICENSE_PLATE_RESULT_S {
	TS_U32 u32PlateNum;
	TS_S8 s8CarPlate[16];
} ALG_LICENSE_PLATE_RESULT_S;

TS_S32 TS_ALG_LicensePlate_Init(TS_VOID **ppHandle, ALG_MODEL_INIT_S *pParam);
TS_S32 TS_ALG_LicensePlate_Exit(TS_VOID *pHandle);
TS_S32 TS_ALG_LicensePlate_Process(TS_VOID *pHandle, ALG_IMAGE_S *pImage, ALG_LICENSE_PLATE_RESULT_S *pResult);
TS_S32 TS_ALG_LicensePlate_SetParam(TS_VOID *pHandle, TS_FLOAT *pParam);
TS_S32 TS_ALG_LicensePlate_GetParam(TS_VOID *pHandle, TS_FLOAT *pParam);
const TS_CHAR *TS_ALG_LicensePlate_GetVersion(TS_VOID);

#endif
