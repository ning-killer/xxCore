/**
 * @file alg_example_pcpp_detect.c
 * @author niuyf
 * @brief pbce detect
 * @version 0.1
 * @date 2023-11-10
 *
 * @copyright Copyright (c) 2023 tsingmicro
 *
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/sysinfo.h>

#include "ts_alg_type.h"
#include "ts_alg_log.h"
#include "ts_alg_vb.h"
#include "layers_GP.h"
#include "ts_alg_amr.h"
#include "ts_rne_device.h"
#include "ts_alg_pbce_detect.h"


#define TEST_0RGB

#ifdef TEST_0RGB
	#define ALG_PBCE_DETECT_IMAGE_C 4
#else
	#define ALG_PBCE_DETECT_IMAGE_C 3
#endif
#define ALG_PBCE_DETECT_IMAGE_H 384
#define ALG_PBCE_DETECT_IMAGE_W 640

#ifdef TEST_0RGB
	#include "../model/bodydetect_bgra_640x384.h"  //640*384*4
#else
	#include "../model/pbce_detect_input.h"  		//640*384*3
#endif


#if 1
#ifdef TX5215
#include "../model/tx5215/pbce_detect_cfg.h"
#include "../model/tx5215/pbce_detect_weight.h"
#endif

TS_S32 main(TS_VOID)
{
	TS_S32 s32Ret;
	TS_VOID *phandle = NULL;
	ALG_MODEL_INIT_S param;
	TS_SIZE_T s32BlobMemLen = 3010560;
	log_set_level(ALG_LOG_DEBUG);

	s32Ret = TS_MPI_TRP_RNE_OpenDevice(NULL, rne_register_gp_layers);
	if (0 != s32Ret) {
		ALG_LOGE("open device error!\n");
		return -1;
	}
	TS_VOID *pBlobMem = (TS_VOID *)TS_MPI_TRP_RNE_AllocLinearMem(s32BlobMemLen);
	ALG_IMAGE_S image;
	memset(&image, 0, sizeof(ALG_IMAGE_S));
	
	//get cma address
	param.pGraph = pbce_detect_cfg;
	param.pWeight = pbce_detect_weight;
	param.u32GraphSize = sizeof(pbce_detect_cfg);
	param.u32WeightSize = sizeof(pbce_detect_weight);
	param.vpSelfBuf = pBlobMem;

#ifdef TEST_0RGB
	param.eImageType = ALG_IMAGE_TYPE_FORMAT_0RGB;
#else
	param.eImageType = ALG_IMAGE_TYPE_INT8_HWC;
#endif
	param.bRneOff = 1;

	log_set_level(ALG_LOG_DEBUG);

	#ifdef RNE_0COPY
	s32Ret = TS_ALG_PbceDetect_Init(&phandle, &param, &image);
	#else
	s32Ret = TS_ALG_PbceDetect_Init(&phandle, &param);
	#endif
	if(0 != s32Ret) {
		ALG_LOGE("TS_ALG_PcppDetect_Init error\n");
		goto End;
	}

	ALG_LOGD("version:%s\n" ,TS_ALG_PbceDetect_GetVersion());

	ALG_LOGD("cfgsize: %d, weightsize: %d \n", param.u32GraphSize, param.u32WeightSize);
	ALG_PBCE_DETECT_RESULT_S result;
	image.s32C = ALG_PBCE_DETECT_IMAGE_C;
	image.s32H = ALG_PBCE_DETECT_IMAGE_H;
	image.s32W = ALG_PBCE_DETECT_IMAGE_W;
	
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
		memcpy(image.pData, bodydetect_bgra_640x384, image.s32C*image.s32H*image.s32W);
	#else
		memcpy(image.pData, pbce_detect_input, image.s32C*image.s32H*image.s32W);
	#endif
#else	
#ifdef TEST_0RGB	
	image.pData = bodydetect_bgra_640x384;
#else
	image.pData = pbce_detect_input;
#endif
#endif

	s32Ret = TS_ALG_PbceDetect_Process(phandle, &image, &result);
	if(0 != s32Ret) {
		ALG_LOGE("TS_ALG_PcppDetect_Process error\n");
		goto End;
	}
	ALG_LOGD("pcppdetect target num: %d\n", result.u32ObjNum);
	for(TS_U32 j = 0; j < result.u32ObjNum; j++) {

		ALG_LOGD("objidx:%d --score:%f xmin:%f ymin:%f xmax:%f ymax:%f\n", result.u8ObjNo[j], result.stBox[j].f32Score,
				result.stBox[j].f32Xmin * image.s32W, result.stBox[j].f32Ymin * image.s32H, result.stBox[j].f32Xmax * image.s32W, result.stBox[j].f32Ymax * image.s32H);
	}

	/* check */
// 	TS_U8 *pSrc1 = (TS_U8 *)image.pData;
// #ifdef TEST_0RGB	
// 	TS_U8 *pSrc2 = bodydetect_bgra_640x384;
// #else
// 	TS_U8 *pSrc2 = pbce_detect_input;
// #endif
	// for(int i=0; i<image.s32C*image.s32H*image.s32W; i++){
	// 	if(pSrc1[i] != pSrc2[i]){
	// 		ALG_LOGE("pcppdetect check error, idx = %d, pSrc1[i] = %d, pSrc2[i] = %d\n", i, pSrc1[i], pSrc2[i]);
	// 		break;
	// 	}
	// }
End:
	s32Ret =TS_ALG_PbceDetect_Exit(phandle);
	if(0 != s32Ret) {
		ALG_LOGE("TS_ALG_BodyDetectRelease error\n");
		return -1;
	}

#ifdef USE_CMA
	TS_ALG_VB_ReleaseBlock(image.pDataPhy);
	TS_ALG_VB_ReleasePool(poolId);
	TS_ALG_VB_Exit();
#endif

	return s32Ret;
}
#endif
