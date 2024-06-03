/**
 * @file alg_example_face_solution.c
 * @author niuyf
 * @brief face solution
 * @version 1.0
 * @date 2023-10-16
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
#include "ts_alg_amr.h"
#include "ts_alg_vb.h"
#include "layers_GP.h"
#include "ts_rne_device.h"

#include "ts_alg_facereco_solution.h"


#ifdef TX5215
	#include "../model/tx5215/face_detect_cfg.h"
	#include "../model/tx5215/face_detect_weight.h"

	#include "../model/tx5215/face_occlusion_cfg.h"
	#include "../model/tx5215/face_occlusion_weight.h"

	#include "../model/tx5215/face_bigangle_cfg.h"
	#include "../model/tx5215/face_bigangle_weight.h"

	#include "../model/tx5215/face_headpose_cfg.h"
	#include "../model/tx5215/face_headpose_weight.h"

	#include "../model/tx5215/face_maskclassify_cfg.h"
	#include "../model/tx5215/face_maskclassify_weight.h"
#endif

#ifdef TX5239
	#include "../model/tx5239/face_detect_cfg.h"
	#include "../model/tx5239/face_detect_weight.h"

	#include "../model/tx5239/face_occlusion_cfg.h"
	#include "../model/tx5239/face_occlusion_weight.h"

	#include "../model/tx5239/face_bigangle_cfg.h"
	#include "../model/tx5239/face_bigangle_weight.h"

	#include "../model/tx5239/face_headpose_cfg.h"
	#include "../model/tx5239/face_headpose_weight.h"

	#include "../model/tx5239/face_maskclassify_cfg.h"
	#include "../model/tx5239/face_maskclassify_weight.h"
#endif

#define TEST_0RGB

#ifdef TEST_0RGB
	#define ALG_FACE_SOLUTION_IMAGE_C 4
	// #include "../model/bodydetect_bgra_640x384.h"	//640*384*4
	#include "../model/face_640x384_bgra.h"	//640*384*4
	#include "../model/face_2560x1440_nv12.h"	//640*384*4
	ALG_IMAGE_TYPE_E eImageTyte = ALG_IMAGE_TYPE_FORMAT_0RGB;
#else
	#define ALG_FACE_SOLUTION_IMAGE_C 3
	#include "../model/face_detect_input.h"	//640*384*3
	ALG_IMAGE_TYPE_E eImageTyte = ALG_IMAGE_TYPE_INT8_HWC;
#endif

#define ALG_FACE_SOLUTION_IMAGE_H 1440
#define ALG_FACE_SOLUTION_IMAGE_W 2560

TS_S32 main(TS_VOID)
{
	TS_S32 s32Ret;
	TS_VOID *phandle = NULL;
	TS_U8 *resizeBuf = NULL;
	ALG_FACE_SOLUTION_INIT_S param;
	log_set_level(ALG_LOG_DEBUG);

	FILE *pf = fopen("test_rgb_2560_1440_5f.rgba", "rb");
	if(NULL == pf){
		ALG_LOGE("read file error !!!\n");
	}

	TS_U32 blkSize = 640*384*4;
	TS_U32 blkCnt = 1;
	s32Ret = TS_ALG_VB_Init(blkSize, blkCnt);
	if(0 != s32Ret) {
		ALG_LOGE("TS_ALG_VB_Init error\n");
		return -1;
	}

	ALG_LOGD("TS_ALG_VB_CreatePool\n");
	vb_pool poolId;
	ALG_IMAGE_S stImageTmp;
	stImageTmp.s32C = 1;
	stImageTmp.s32H = ALG_FACE_SOLUTION_IMAGE_H;
	stImageTmp.s32W = ALG_FACE_SOLUTION_IMAGE_W;
	stImageTmp.pData = face_2560x1440_nv12;
	ALG_IMAGE_S stImageDet;
#if 1
	s32Ret = TS_ALG_VB_CreatePool(&poolId, blkSize, blkCnt);
	if(0 != s32Ret) {
		ALG_LOGE("TS_ALG_VB_CreatePool error\n");
		return -1;
	}
	ALG_LOGD("TS_ALG_VB_GetBlock\n");
	s32Ret = TS_ALG_VB_GetBlock(poolId, blkSize, &(stImageDet.pDataPhy), &(stImageDet.pData));
	if(0 != s32Ret) {
		ALG_LOGE("TS_ALG_VB_GetBlock error\n");
		TS_ALG_VB_ReleasePool(poolId);
		return -1;
	}
	// ALG_LOGD("stImageTmp.pDataPhy = %p, stImageTmp.pData = %x\n", stImageTmp.pDataPhy, stImageTmp.pData);

	ALG_LOGD("TS_MPI_TRP_CVE_Init\n");
	s32Ret = TS_MPI_TRP_CVE_Init();
	if(0 != s32Ret) {
		ALG_LOGE("TS_MPI_TRP_CVE_Init error\n");
		return -1;
	}

	stImageDet.s32H = 384;
	stImageDet.s32W = 640;
	stImageDet.s32C = 4;
	
	// memset(stImageDet.pData, 114, stImageDet.s32H * stImageDet.s32W * stImageDet.s32C);
// #ifdef TEST_0RGB
	ALG_LOGI("version:%s\n" ,TS_ALG_FaceSolution_GetVersion());
	memcpy(stImageDet.pData, face_640x384, stImageDet.s32C*stImageDet.s32H*stImageDet.s32W);
// #else
// 	memcpy(stImageTmp.pData, face_detect_input, stImageTmp.s32C*stImageTmp.s32H*stImageTmp.s32W);
// #endif
	system("echo N > /sys/module/printk/parameters/ignore_loglevel");
	system("echo 4 > /proc/sys/kernel/printk");
#else
	#ifdef TEST_0RGB
		stImageTmp.pData = bodydetect_bgra_640x384;
	#else
		stImageTmp.pData = face_detect_input;
	#endif
#endif

// #ifdef TX5215
	TS_SIZE_T s32BlobMemLen = 3870720;
// #endif	
// #ifdef TX5239
// 	TS_SIZE_T s32BlobMemLen = 1351680;
// #endif	
	log_set_level(ALG_LOG_DEBUG);

	s32Ret = TS_MPI_TRP_RNE_OpenDevice(NULL, rne_register_gp_layers);
	if (0 != s32Ret) {
		ALG_LOGE("open device error!\n");
		return -1;
	}
	TS_VOID *pBlobMem = (TS_VOID *)TS_MPI_TRP_RNE_AllocLinearMem(s32BlobMemLen);

	param.u32ImgH = ALG_FACE_SOLUTION_IMAGE_H;
	param.u32ImgW = ALG_FACE_SOLUTION_IMAGE_W;

	param.stFaceDet.pGraph = face_detect_cfg;
	param.stFaceDet.pWeight = face_detect_weight;
	param.stFaceDet.u32GraphSize = sizeof(face_detect_cfg);
	param.stFaceDet.u32WeightSize = sizeof(face_detect_weight);
	param.stFaceDet.eImageType = eImageTyte;
	param.stFaceDet.vpSelfBuf = pBlobMem;
	param.stFaceDet.bRneOff = 1; //open/close rne device in solution init/release

	param.stFaceOcclusion.pGraph = face_occlusion_cfg;
	param.stFaceOcclusion.pWeight = face_occlusion_weight;
	param.stFaceOcclusion.u32GraphSize = sizeof(face_occlusion_cfg);
	param.stFaceOcclusion.u32WeightSize = sizeof(face_occlusion_weight);
	param.stFaceOcclusion.eImageType = eImageTyte;
	param.stFaceOcclusion.vpSelfBuf = pBlobMem;
	param.stFaceOcclusion.bRneOff = 1; //open/close rne device in solution init/release

	param.stBigAngle.pGraph = face_bigangle_cfg;
	param.stBigAngle.pWeight = face_bigangle_weight;
	param.stBigAngle.u32GraphSize = sizeof(face_bigangle_cfg);
	param.stBigAngle.u32WeightSize = sizeof(face_bigangle_weight);
	param.stBigAngle.eImageType = eImageTyte;
	param.stBigAngle.vpSelfBuf = pBlobMem;
	param.stBigAngle.bRneOff = 1; //open/close rne device in solution init/release

	param.stHeadPose.pGraph = face_headpose_cfg;
	param.stHeadPose.pWeight = face_headpose_weight;
	param.stHeadPose.u32GraphSize = sizeof(face_headpose_cfg);
	param.stHeadPose.u32WeightSize = sizeof(face_headpose_weight);
	param.stHeadPose.eImageType = eImageTyte;
	param.stHeadPose.vpSelfBuf = pBlobMem;
	param.stHeadPose.bRneOff = TS_TRUE; //open/close rne device in solution init/release

	param.stMaskclassify.pGraph = face_maskclassify_cfg;
	param.stMaskclassify.pWeight = face_maskclassify_weight;
	param.stMaskclassify.u32GraphSize = sizeof(face_maskclassify_cfg);
	param.stMaskclassify.u32WeightSize = sizeof(face_maskclassify_weight);
	param.stMaskclassify.eImageType = eImageTyte;
	param.stMaskclassify.vpSelfBuf = pBlobMem;
	param.stMaskclassify.bRneOff = TS_TRUE; //open/close rne device in solution init/release

	param.bOccusion = 1;
	param.bBigAngle = 1;
	param.bHeadpose = 1;
	param.bMaskclassify = 1;

	param.bRneOff = 1;
	s32Ret = TS_ALG_FaceSolution_Init(&phandle, &param);
	if(0 != s32Ret) {
		ALG_LOGE("TS_ALG_FaceSolution_Init error\n");
		return -1;
	}

	ALG_LOGI("version:%s\n" ,TS_ALG_FaceSolution_GetVersion());

	for(int k=0; k<1; k++){

		// fread(stImageTmp.pData, stImageTmp.s32W*stImageTmp.s32C, stImageTmp.s32H, pf);

		// TS_ALG_Resize(stImageTmp.pData, stImageDet.pData + 12*stImageDet.s32C*stImageDet.s32W, stImageTmp.s32W, stImageTmp.s32H, stImageDet.s32W, stImageDet.s32H-24, stImageTmp.s32C);

		ALG_FACE_SOLUTION_RESULT_S result;
		memset(&result, 0, sizeof(ALG_FACE_SOLUTION_RESULT_S));

		TS_U32 time0 = TIME_CACL_GET();
		s32Ret = TS_ALG_FaceSolution_Process(phandle, &stImageTmp, &stImageDet, &result);
		if(0 != s32Ret) {
			ALG_LOGE("TS_ALG_FaceSolutionProcess error\n");
			goto End;
		}

		TS_U32 time1 = TIME_CACL_GET();
		ALG_LOGD("algo face solution process:%d ms\n",time1-time0);

		for(TS_U32 i = 0; i < result.u32Num; i++) {

			ALG_LOGD("detect:%d --score:%f xmin:%f ymin:%f xmax:%f ymax:%f\n", i, result.stDetect.stBox[i].f32Score,
					result.stDetect.stBox[i].f32Xmin, result.stDetect.stBox[i].f32Ymin, result.stDetect.stBox[i].f32Xmax, result.stDetect.stBox[i].f32Ymax);

			ALG_LOGD("occlusion:%d --isOccusion:%d score: %f\n", i, result.stOcclusion[i].bOccusion, result.stOcclusion[i].f32Score);
			ALG_LOGD("bigangle:%d  --isBigAngle:%d score:%f\n", i, result.stBigAngle[i].bBigAngle, result.stBigAngle[i].f32Score);
			ALG_LOGD("headpose:%d --isgood:%d pose x:%f y:%f z:%f\n", i, result.stHeadPose[i].bGoodpose, result.stHeadPose[i].f32Headpose[0],
					result.stHeadPose[i].f32Headpose[1], result.stHeadPose[i].f32Headpose[2]);
			ALG_LOGD("maskclassify:%d --bClass:%d score: %f\n", i, result.stMaskClassify[i].bClass, result.stMaskClassify[i].f32ClassScore);
		}
	}
	fclose(pf);
End:
	if(TS_NULL != resizeBuf)
		free(resizeBuf);

	ALG_LOGD("TS_ALG_FaceSolution_Exit\n");
	s32Ret = TS_ALG_FaceSolution_Exit(phandle);
	if(0 != s32Ret) {
		ALG_LOGE("TS_ALG_FaceSolution_Exit error\n");
		return -1;
	}

	return s32Ret;
}
