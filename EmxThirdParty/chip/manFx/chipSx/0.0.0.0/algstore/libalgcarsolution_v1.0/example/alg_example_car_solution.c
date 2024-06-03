/*
 * @file: alg_example_car_solution.c
 * @brief: 
 * @version:0.1v
 * @date: 2023/10/16
 * @author: zzy
 * @copyright : Copyright (c) 2023 tsingmicro
 *  车辆检测+车牌检测+车牌识别
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>

#include "ts_alg_type.h"
#include "ts_alg_log.h"
#include "ts_alg_car_solution.h"
#include "ts_alg_amr.h"
#include "layers_GP.h"
#include "ts_alg_vb.h"


#ifdef TX5215
#include "../model/tx5215/license_plate_cfg.h"
#include "../model/tx5215/license_plate_weight.h"
#endif

#ifdef TX5239
#include "../model/PROJECT_input.h"
#include "../model/tx5239/PROJECT_cfg.h"
#include "../model/tx5239/PROJECT_weight.h"
#endif

#define ALG_IMAGE_C 3
#define ALG_IMAGE_H 384
#define ALG_IMAGE_W 640

#define ORI_IMAGE_H 1440
#define ORI_IMAGE_W 2560

#define DST_IMAGE_H 24
#define DST_IMAGE_W 94

static TS_U32 getFileSize(FILE *pf)
{
	TS_U32 fileSize = 0;
	if(NULL != pf){
		if(0 == fseek(pf, 0, SEEK_END)){
			fileSize = ftell(pf);
		}
		rewind(pf);
	}
	return fileSize;
}

TS_S32 main(TS_VOID)
{
	TS_S32 s32Ret;
	TS_VOID *phandle = NULL;
	ALG_CAR_SOLUTION_INIT_S param;
	log_set_level(ALG_LOG_DEBUG);
	//struct timeval start_tm, end_tm;

	ALG_IMAGE_S imageOri;
	imageOri.s32C = 1;
	imageOri.s32H = ORI_IMAGE_H;
	imageOri.s32W = ORI_IMAGE_W;

	ALG_IMAGE_S imageDet;
	imageDet.s32C = ALG_IMAGE_C;
	imageDet.s32H = ALG_IMAGE_H;
	imageDet.s32W = ALG_IMAGE_W;

	TS_U32 time0 = TIME_CACL_GET();
	TS_U32 time1 = TIME_CACL_GET();
	ALG_LOGE("11111 time:%d\n",time1-time0);

#ifdef USE_CMA
	TS_U32 detSize = imageDet.s32C * imageDet.s32H * imageDet.s32W;
	TS_U32 oriSize = ORI_IMAGE_W * ORI_IMAGE_H * 1.5;
	TS_U32 blkSize = detSize + oriSize;
	TS_U32 blkCnt = 1;
	s32Ret = TS_ALG_VB_Init(blkSize, blkCnt);
	if (0 != s32Ret) {
		ALG_LOGE("TS_ALG_SYS_Init error\n");
		return -1;
	}

	vb_pool poolId;
	s32Ret = TS_ALG_VB_CreatePool(&poolId, blkSize, blkCnt);
	if (0 != s32Ret) {
		ALG_LOGE("TS_ALG_VB_CreatePool error\n");
		TS_ALG_VB_Exit();
		return -1;
	}

	s32Ret = TS_ALG_VB_GetBlock(poolId, blkSize, &imageDet.pDataPhy, &imageDet.pData);
	if (0 != s32Ret) {
		ALG_LOGE("TS_ALG_VB_GetBlock error\n");
		TS_ALG_VB_ReleasePool(poolId);
		TS_ALG_VB_Exit();
		return -1;
	}
	ALG_LOGD("imageDet.pDataPhy: %x: imageDet.pData: %x\n", imageDet.pDataPhy, imageDet.pData);

	imageOri.pDataPhy = imageDet.pDataPhy + detSize;
	imageOri.pData = imageDet.pData + detSize;
	ALG_LOGD("imageOri.pDataPhy: %x: imageOri.pData: %x\n", imageOri.pDataPhy, imageOri.pData);
#endif


	// 加载车辆检测模型
	FILE *pf_det_cfg = fopen("licenseplate_det_v2.2_r.cfg", "rb");
	FILE *pf_det_weight = fopen("licenseplate_det_v2.2_r.weight", "rb");
	if((NULL == pf_det_cfg) || (NULL == pf_det_weight)){
		ALG_LOGE("car plate det model file fopen failed !!!\n");
	}

	memset(&param, 0, sizeof(ALG_CAR_SOLUTION_INIT_S));
	param.stCarPedDet.u32GraphSize = getFileSize(pf_det_cfg);
	param.stCarPedDet.u32WeightSize = getFileSize(pf_det_weight);

	TS_U8 *pDetCfgAddr = (TS_U8 *)malloc(param.stCarPedDet.u32GraphSize*sizeof(TS_U8));
	TS_U8 *pDetWeightAddr = (TS_U8 *)malloc(param.stCarPedDet.u32WeightSize*sizeof(TS_U8));

	fread(pDetCfgAddr, param.stCarPedDet.u32GraphSize, 1, pf_det_cfg);
	fread(pDetWeightAddr, param.stCarPedDet.u32WeightSize, 1, pf_det_weight);

	param.stCarPedDet.pGraph = pDetCfgAddr;
	param.stCarPedDet.pWeight = pDetWeightAddr;
	param.stCarPedDet.eImageType = ALG_IMAGE_TYPE_INT8_HWC;
	param.stCarPedDet.bRneOff = 0;

	// 加载车牌检测模型
	param.stLinceplate.pGraph = license_plate_cfg;
	param.stLinceplate.pWeight = license_plate_weight;
	param.stLinceplate.u32GraphSize = sizeof(license_plate_cfg);
	param.stLinceplate.u32WeightSize = sizeof(license_plate_weight);
	param.stLinceplate.eImageType = ALG_IMAGE_TYPE_INT8_HWC;

	ALG_LOGD("TS_ALG_CarSolution_Init <start>");
	s32Ret = TS_ALG_CarSolution_Init(&phandle, &param);
	if (0 != s32Ret) {
		ALG_LOGE("TS_ALG_CarSolution_Init error\n");
		goto error;
	}

	free(pDetCfgAddr);
	free(pDetWeightAddr);
	fclose(pf_det_cfg);
	fclose(pf_det_weight);

	ALG_LOGD("version:%s\n", TS_ALG_CarSolution_GetVersion());

	FILE* pfInput = fopen("123.bin", "rb");
	if (NULL == pfInput){
		ALG_LOGE("fopen error\n");
	}
	fread((TS_U8 *)imageDet.pData, imageDet.s32W * imageDet.s32C, imageDet.s32H, pfInput);
	fclose(pfInput);

	// 从原图中裁剪车牌框
	pfInput = fopen("ori_nv12.bin", "rb");
	if (NULL == pfInput){
		ALG_LOGE("fopen error\n");
	}
	fread((TS_U8 *)imageOri.pData, imageOri.s32W * 1.5, imageOri.s32H, pfInput);
	fclose(pfInput);

	ALG_CAR_SOLUTION_RESULT_S result;

	// 车辆检测	
	s32Ret = TS_ALG_CarSolution_Process(phandle, &imageOri, &imageDet, &result);
	if (0 != s32Ret) {
		ALG_LOGE("TS_ALG_CarSolution_Process error\n");
		goto error;
	}

	ALG_LOGD("%12s%12s%12s%12s%12s%12s%12s\n", "serial", "X", "Y", "WIDTH", "HEIGHT", "SCORE", "PLATE");
	for (int j = 0; j < result.u32Num; ++j) {
		ALG_LOGD("%12d%12f%12f%12f%12f%12f%12s\n", j+1, result.licenseDet.stBox[j].f32Xmin, result.licenseDet.stBox[j].f32Ymin,
														result.licenseDet.stBox[j].f32Xmax, result.licenseDet.stBox[j].f32Ymax,
														result.licenseDet.stBox[j].f32Score, result.lincensePlate[j].s8CarPlate);
	}

error:
	s32Ret = TS_ALG_CarSolution_Exit(phandle);
	if (0 != s32Ret) {
		ALG_LOGE("TS_ALG_CarSolution_Process error\n");
		return -1;
	}

	return s32Ret;
}
