/*
 * @file: alg_example_licenseplate_det.c
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
#include "ts_alg_licenseplate_det.h"
#include "ts_alg_amr.h"
#include "layers_GP.h"
#include "ts_alg_vb.h"

#ifdef TX5215
//#include "../model/licenseplate_det_input.h"
//#include "../model/tx5215/licenseplate_det_cfg.h"
//#include "../model/tx5215/licenseplate_det_weight.h"
#endif

#ifdef TX5239
#include "../model/licenseplate_det_input.h"
#include "../model/tx5239/licenseplate_det_cfg.h"
#include "../model/tx5239/licenseplate_det_weight.h"
#endif

#define ALG_IMAGE_C 3
#define ALG_IMAGE_H 384
#define ALG_IMAGE_W 640

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
	ALG_MODEL_INIT_S param;

#ifdef TX521X
	FILE *pf_cfg = fopen("licenseplate_det_v2.2_r.cfg", "rb");
	FILE *pf_weight = fopen("licenseplate_det_v2.2_r.weight", "rb");
#endif
	if((NULL == pf_cfg) || (NULL == pf_weight)){
		ALG_LOGE("model file fopen failed !!!\n");
	}

	TS_U32 u32CfgSize = getFileSize(pf_cfg);
	TS_U32 u32WeightSize = getFileSize(pf_weight);
	TS_U8 *pCfgAddr = (TS_U8 *)malloc(u32CfgSize*sizeof(TS_U8));
	TS_U8 *pWeightAddr = (TS_U8 *)malloc(u32WeightSize*sizeof(TS_U8));
	fread(pCfgAddr, u32CfgSize, 1, pf_cfg);
	fread(pWeightAddr, u32WeightSize, 1, pf_weight);
	
	
	memset(&param, 0, sizeof(ALG_MODEL_INIT_S));
	param.pGraph = pCfgAddr;
	param.pWeight = pWeightAddr;
	param.u32GraphSize = u32CfgSize;
	param.u32WeightSize = u32WeightSize;
	param.vpSelfBuf = NULL;

	param.eImageType = ALG_IMAGE_TYPE_INT8_HWC;
	param.bRneOff = 0;

	ALG_LOGD("TS_ALG_LicensePlateDet_Init <start>");
	s32Ret = TS_ALG_LicensePlateDet_Init(&phandle, &param);
	if (0 != s32Ret) {
		ALG_LOGE("TS_ALG_LicensePlateDet_Init error\n");
		goto error;
	}

	free(pCfgAddr);
	free(pWeightAddr);

	fclose(pf_cfg);
	fclose(pf_weight);

	ALG_LOGD("version:%s\n", TS_ALG_LicensePlateDet_GetVersion());

	FILE* pfInput = fopen("123.bin", "rb");
	if (NULL == pfInput){
		ALG_LOGE("fopen error\n");
	}

	ALG_IMAGE_S image;
	image.s32C = ALG_IMAGE_C;
	image.s32H = ALG_IMAGE_H;
	image.s32W = ALG_IMAGE_W;
	//    image.pData = licenseplate_det_input;

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
	//memcpy(image.pData, licenseplate_det_input, image.s32C * image.s32H * image.s32W);
#endif
	fread((TS_U8 *)image.pData, image.s32W * image.s32C, image.s32H, pfInput);
	fclose(pfInput);
	ALG_LICENSE_PLATE_DET_RESULT_S result;
	TS_U32 time0 = TIME_CACL_GET();
	s32Ret = TS_ALG_LicensePlateDet_Process(phandle, &image, &result);
	if (0 != s32Ret) {
		ALG_LOGE("TS_ALG_LicensePlateDet_Process error\n");
		goto error;
	}
	TS_U32 time1 = TIME_CACL_GET();
	ALG_LOGD("####algo  process:%d ms\n", time1 - time0);

	for(int i=0; i<result.u32LicensePlateDetNum; i++) {
		ALG_LOGD("num=%d f32Xmin=%f f32Ymin=%f f32Xmax=%f f32Ymax=%f f32Score=%f \n",
				i+1, result.stBox[i].f32Xmin, result.stBox[i].f32Ymin,
				 result.stBox[i].f32Xmax, result.stBox[i].f32Ymax, result.stBox[i].f32Score);
	}

error:
	s32Ret = TS_ALG_LicensePlateDet_Exit(phandle);
	if (0 != s32Ret) {
		ALG_LOGE("TS_ALG_LicensePlateDet_Process error\n");
		return -1;
	}

	return s32Ret;
}