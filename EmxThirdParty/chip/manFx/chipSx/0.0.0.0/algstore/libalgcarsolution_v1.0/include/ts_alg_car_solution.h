
/*
 * @file: ts_alg_car_solution.h
 * @brief: 
 * @version:0.1v
 * @date: 2023/10/16
 * @author: zzy
 * @copyright : Copyright (c) 2023 tsingmicro
 */

#ifndef _TS_ts_alg_car_solution_H
#define _TS_ts_alg_car_solution_H

#include "ts_alg_type.h"
// #include "ts_alg_muti_targetdet.h"
#include "ts_alg_license_plate.h"
#include "ts_alg_licenseplate_det.h"

#include "ts_alg_type.h"

#define MAX_LICENSE_PLATE_NUM 25

typedef struct tsALG_CAR_SOLUTION_RESULT_S {
	TS_U32 u32Num;
	ALG_LICENSE_PLATE_DET_RESULT_S licenseDet; // 车牌检测
	ALG_LICENSE_PLATE_RESULT_S lincensePlate[MAX_LICENSE_PLATE_NUM]; // 车牌识别
} ALG_CAR_SOLUTION_RESULT_S;

typedef struct tsALG_CAR_SOLUTION_INIT_S {
	ALG_MODEL_INIT_S stCarPedDet; //车牌检测
	ALG_MODEL_INIT_S stLinceplate; //车牌识别
	TS_U8 bRneOff; // not open RNE device, revent multiple operations
} ALG_CAR_SOLUTION_INIT_S;

TS_S32 TS_ALG_CarSolution_Init(TS_VOID **ppHandle, ALG_CAR_SOLUTION_INIT_S *pParam);
TS_S32 TS_ALG_CarSolution_Exit(TS_VOID *pHandle);
TS_S32 TS_ALG_CarSolution_Process(TS_VOID *pHandle, ALG_IMAGE_S *pImage, ALG_IMAGE_S *pDetImage, ALG_CAR_SOLUTION_RESULT_S *result);
//TS_S32 TS_ALG_CarSolution_Process_Detect(TS_VOID *pHandle, ALG_IMAGE_S *pDetImage, ALG_CAR_SOLUTION_RESULT_S *result);
//TS_S32 TS_ALG_CarSolution_Process_Identidy(TS_VOID *pHandle, ALG_IMAGE_S *pIdenImage, ALG_CAR_SOLUTION_RESULT_S *result);
TS_S32 TS_ALG_CarSolution_SetParam(TS_VOID *pHandle, TS_FLOAT *pParam);
TS_S32 TS_ALG_CarSolution_GetParam(TS_VOID *pHandle, TS_FLOAT *pParam);
const TS_CHAR *TS_ALG_CarSolution_GetVersion(TS_VOID);

#endif
