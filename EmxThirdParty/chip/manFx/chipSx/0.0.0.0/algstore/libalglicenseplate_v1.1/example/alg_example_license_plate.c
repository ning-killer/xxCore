/*
 * @file: alg_example_license_plate.c
 * @brief: 
 * @version:0.1v
 * @date: 2023/10/12
 * @author: zzy
 * @copyright : Copyright (c) 2023 tsingmicro
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>

#include "ts_alg_type.h"
#include "ts_alg_log.h"
#include "ts_alg_license_plate.h"
#include "ts_alg_amr.h"
#include "layers_GP.h"
#include "ts_alg_vb.h"

#ifdef TX5215
#include "../model/license_plate_input.h"
#include "../model/tx5215/license_plate_cfg.h"
#include "../model/tx5215/license_plate_weight.h"
#endif

#ifdef TX5239
#include "../model/license_plate_input.h"
#include "../model/tx5239/license_plate_cfg.h"
#include "../model/tx5239/license_plate_weight.h"
#endif

#define ALG_IMAGE_C 3
#define ALG_IMAGE_H 24
#define ALG_IMAGE_W 94

TS_S32 main(TS_VOID)
{
	TS_S32 s32Ret;
	TS_VOID *phandle = NULL;
	ALG_MODEL_INIT_S param;

	param.pGraph = license_plate_cfg;
	param.pWeight = license_plate_weight;
	param.u32GraphSize = sizeof(license_plate_cfg);
	param.u32WeightSize = sizeof(license_plate_weight);
	param.eImageType = ALG_IMAGE_TYPE_INT8_HWC;
	param.vpSelfBuf = NULL;
	param.bRneOff = 0;

	ALG_LOGD("TS_ALG_LicensePlate_Init <start>");
	s32Ret = TS_ALG_LicensePlate_Init(&phandle, &param);
	if (0 != s32Ret) {
		ALG_LOGE("TS_ALG_LicensePlate_Init error\n");
		goto error;
	}

	ALG_LOGD("version:%s\n", TS_ALG_LicensePlate_GetVersion());

	ALG_IMAGE_S image;
	image.s32C = ALG_IMAGE_C;
	image.s32H = ALG_IMAGE_H;
	image.s32W = ALG_IMAGE_W;

#ifdef USE_CMA
	TS_U32 blkSize = image.s32C * image.s32H * image.s32W;
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

	s32Ret = TS_ALG_VB_GetBlock(poolId, blkSize, &image.pDataPhy, &image.pData);
	if (0 != s32Ret) {
		ALG_LOGE("TS_ALG_VB_GetBlock error\n");
		TS_ALG_VB_ReleasePool(poolId);
		TS_ALG_VB_Exit();
		return -1;
	}
	ALG_LOGD("image.pDataPhy: %x: image.pData: %p\n", image.pDataPhy, image.pData);
	memcpy(image.pData, license_plate_input, image.s32C * image.s32H * image.s32W);
#endif

	image.pData = &license_plate_input[0];
	//image.pData  = license_plate_input;

	TS_U32 time0 = TIME_CACL_GET();
	ALG_LICENSE_PLATE_RESULT_S result;
	s32Ret = TS_ALG_LicensePlate_Process(phandle, &image, &result);
	if (0 != s32Ret) {
		ALG_LOGE("TS_ALG_LicensePlate_Process error\n");
		goto error;
	}
	printf("------------- %s \n", result.s8CarPlate);
	TS_U32 time1 = TIME_CACL_GET();
	ALG_LOGD("algo  process:%d ms\n", time1 - time0);

#ifdef USE_CMA
	TS_ALG_VB_ReleaseBlock(image.pDataPhy);
	TS_ALG_VB_ReleasePool(poolId);
	TS_ALG_VB_Exit();
#endif

error:
	s32Ret = TS_ALG_LicensePlate_Exit(phandle);
	if (0 != s32Ret) {
		ALG_LOGE("TS_ALG_LicensePlate_Process error\n");
		return -1;
	}

    return s32Ret;
}