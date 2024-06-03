/*
 * @Description: 
 * @Autor: yhc
 * @Date: 2023-09-28 10:23:26
 * @LastEditors: yhc
 * @LastEditTime: 2023-10-18 13:30
 */
/**
 * @file    alg_example_ebike_detect.c
 * @author  yhc
 * @brief   ebike detect
 * @version 1.0
 * @date    2022-09-28
 *
 * @copyright Copyright (c) 2022 tsingmicro
 *
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "ts_alg_type.h"
#include "ts_alg_log.h"
#include "ts_alg_ebike_detect.h"
#include "ts_alg_amr.h"
#include "ts_alg_vb.h"
//#include "layers_GP.h"

//#define TEST_0RGB

#ifdef TEST_0RGB
	#define ALG_FACE_DETECT_IMAGE_C 4
	#include "../face_test.h" //640*384*4
#else
	#define ALG_EBIKE_DETECT_IMAGE_C 3
	#include "../ebike_detect_input.h"	//640*384*3
	//#include "../D01_20230809104104.h"
	//#include "../D01_20230811120604.h"
	//#include "../img_5011.h"
#endif
#define ALG_EBIKE_DETECT_IMAGE_H 384
#define ALG_EBIKE_DETECT_IMAGE_W 640

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
	TS_U8 *resizeBuf = NULL;
	ALG_MODEL_INIT_S param;
	log_set_level(ALG_LOG_DEBUG);

	FILE *pf_cfg = fopen("ebike_detect.cfg", "rb");
	FILE *pf_weight = fopen("ebike_detect.weight", "rb");

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

#ifdef TEST_0RGB
	param.eImageType = ALG_IMAGE_TYPE_FORMAT_0RGB;
#else
	param.eImageType = ALG_IMAGE_TYPE_INT8_HWC;
#endif
	param.bRneOff = 0;

	
	ALG_IMAGE_S image;

	image.s32C = ALG_EBIKE_DETECT_IMAGE_C;
	image.s32H = ALG_EBIKE_DETECT_IMAGE_H;
	image.s32W = ALG_EBIKE_DETECT_IMAGE_W;

#ifdef USE_CMA
	TS_U32 blkSize = image.s32C*image.s32H*image.s32W;
	TS_U32 blkCnt = 1;
	s32Ret = TS_ALG_VB_Init(blkSize, blkCnt);
	if(0 != s32Ret) {
		ALG_LOGE("TS_ALG_SYS_Init error\n");
		return -1;
	}

	vb_pool poolId;
	s32Ret = TS_ALG_VB_CreatePool(&poolId, blkSize, blkCnt);
	if(0 != s32Ret) {
		ALG_LOGE("TS_ALG_VB_CreatePool error\n");
		TS_ALG_VB_Exit();
		return -1;
	}

	s32Ret = TS_ALG_VB_GetBlock(poolId, blkSize, &image.pDataPhy, &image.pData);
	if(0 != s32Ret) {
		ALG_LOGE("TS_ALG_VB_GetBlock error\n");
		TS_ALG_VB_ReleasePool(poolId);
		TS_ALG_VB_Exit();
		return -1;
	}
	ALG_LOGD("image.pDataPhy: %x: image.pData: %p\n", image.pDataPhy, image.pData);
	#ifdef TEST_0RGB	
		memcpy(image.pData, hand_detect_bgra, image.s32C*image.s32H*image.s32W);
	#else
		memcpy(image.pData, ebike_detect_input, image.s32C*image.s32H*image.s32W);
		//memcpy(image.pData, D01_20230809104104, image.s32C*image.s32H*image.s32W);		// D01_20230809104104
		//memcpy(image.pData, D01_20230811120604, image.s32C*image.s32H*image.s32W);		// D01_20230811120604
		//memcpy(image.pData, img_5011, image.s32C*image.s32H*image.s32W);		// img_5011
	#endif
#else	
	#ifdef TEST_0RGB	
		image.pData = hand_detect_bgra;
	#else
		image.pData = ebike_detect_input;
		//image.pData = D01_20230809104104;
		//image.pData = D01_20230811120604;
		//image.pData = img_5011;
	#endif
#endif

	s32Ret = TS_ALG_EbikeDetect_Init(&phandle, &param);
	if(0 != s32Ret) {
		ALG_LOGE("TS_ALG_EbikeDetect_Init error\n");
		goto End;
	}

	free(pWeightAddr);
	free(pCfgAddr);

	fclose(pf_cfg);
	fclose(pf_weight);

	ALG_LOGD("version:%s\n" ,TS_ALG_EbikeDetect_GetVersion());

	ALG_LOGD("cfgsize: %d, weightsize: %d \n", param.u32GraphSize, param.u32WeightSize);
	ALG_EBIKE_DETECT_RESULT_S result;

	TS_U32 time0 = TIME_CACL_GET();
	s32Ret = TS_ALG_EbikeDetect_Process(phandle, &image, &result);
	if(0 != s32Ret) {
		ALG_LOGE("TS_ALG_EbikeDetect_Process error\n");
		goto End;
	}
	TS_U32 time1 = TIME_CACL_GET();
	ALG_LOGD("TS_ALG_EbikeDetect_Process:%d ms\n",time1-time0);
	for(TS_U32 i = 0; i < result.u32ObjNum; i++) {
		ALG_LOGD("face:%d --score:%f \n\txmin:%f ymin:%f xmax:%f ymax:%f \n\tx1:%d y1:%d x2:%d y2:%d\n", i, result.stBox[i].f32Score,
				result.stBox[i].f32Xmin, result.stBox[i].f32Ymin, result.stBox[i].f32Xmax, result.stBox[i].f32Ymax,
				(TS_U32)(result.stBox[i].f32Xmin*ALG_EBIKE_DETECT_IMAGE_W), (TS_U32)(result.stBox[i].f32Ymin*ALG_EBIKE_DETECT_IMAGE_H), 
				(TS_U32)(result.stBox[i].f32Xmax*ALG_EBIKE_DETECT_IMAGE_W), (TS_U32)(result.stBox[i].f32Ymax*ALG_EBIKE_DETECT_IMAGE_H)
				);
	}
End:
	if(TS_NULL != resizeBuf)
		free(resizeBuf);

	s32Ret = TS_ALG_EbikeDetect_Exit(phandle);
	if(0 != s32Ret) {
		ALG_LOGE("TS_ALG_EbikeDetectRelease error\n");
		return -1;
	}

	return s32Ret;
}
