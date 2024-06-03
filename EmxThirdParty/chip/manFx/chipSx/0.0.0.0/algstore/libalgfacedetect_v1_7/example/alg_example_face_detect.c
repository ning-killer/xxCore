/**
 * @file alg_example_face_detect.c
 * @author hxd
 * @brief face detect
 * @version 0.1
 * @date 2022-09-6
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
#include "ts_alg_vb.h"
#include "ts_alg_face_detect.h"
#include "ts_alg_amr.h"
#include "ts_rne_device.h"
#include "layers_GP.h"

#ifdef TX5215
#include "../model/tx5215/face_detect_cfg.h"
#include "../model/tx5215/face_detect_weight.h"
#endif

#ifdef TX5239
#include "../model/tx5239/face_detect_cfg.h"
#include "../model/tx5239/face_detect_weight.h"
#endif

#define TEST_0RGB

#ifdef TEST_0RGB
	#define ALG_FACE_DETECT_IMAGE_C 4
	//#include "../model/bodydetect_bgra_640x384.h" //640*384*4
#else
	#define ALG_FACE_DETECT_IMAGE_C 3
	#include "../model/face_detect_input.h"	//640*384*3
#endif
#define ALG_FACE_DETECT_IMAGE_H 384
#define ALG_FACE_DETECT_IMAGE_W 640

#if 1
//head file model
TS_S32 main(TS_VOID)
{
	TS_S32 s32Ret;
	TS_VOID *phandle = NULL;
	TS_U8 *resizeBuf = NULL;
	ALG_MODEL_INIT_S param;
	TS_SIZE_T s32BlobMemLen = 3870720;
	log_set_level(ALG_LOG_DEBUG);

	s32Ret = TS_MPI_TRP_RNE_OpenDevice(NULL, rne_register_gp_layers);
	if (0 != s32Ret) {
		ALG_LOGE("open device error!\n");
		return -1;
	}
	TS_VOID *pBlobMem = (TS_VOID *)TS_MPI_TRP_RNE_AllocLinearMem(s32BlobMemLen);
	memset(&param, 0, sizeof(ALG_MODEL_INIT_S));

	param.pGraph = face_detect_cfg;
	param.pWeight = face_detect_weight;
	param.u32GraphSize = sizeof(face_detect_cfg);
	param.u32WeightSize = sizeof(face_detect_weight);
	param.vpSelfBuf = pBlobMem;
	// param.vpSelfBuf = NULL;

#ifdef TEST_0RGB
	param.eImageType = ALG_IMAGE_TYPE_FORMAT_0RGB;
#else
	param.eImageType = ALG_IMAGE_TYPE_INT8_HWC;
#endif
	param.bRneOff = 1;

	ALG_LOGD("cfgsize: %d, weightsize: %d \n", param.u32GraphSize, param.u32WeightSize);
	
	s32Ret = TS_ALG_FaceDetect_Init(&phandle, &param);
	if(0 != s32Ret) {
		ALG_LOGE("TS_ALG_FaceDetectInit error\n");
		goto End;
	}

	ALG_LOGD("version:%s\n" ,TS_ALG_FaceDetect_GetVersion());

	ALG_FACE_DETECT_RESULT_S result;
	ALG_IMAGE_S image;
	image.s32C = ALG_FACE_DETECT_IMAGE_C;
	image.s32H = ALG_FACE_DETECT_IMAGE_H;
	image.s32W = ALG_FACE_DETECT_IMAGE_W;
	FILE* pfInput = fopen("image_rgba.bin", "rb");
	if (NULL == pfInput){
		ALG_LOGE("fopen error\n");
	}

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
		//memcpy(image.pData, bodydetect_bgra_640x384, image.s32C*image.s32H*image.s32W);
		// memcpy(image.pData, test_argb, image.s32C*image.s32H*image.s32W);
		fread((TS_U8 *)image.pData, image.s32W * image.s32C, image.s32H, pfInput);
		fclose(pfInput);
	#else
		memcpy(image.pData, face_detect_input, image.s32C*image.s32H*image.s32W);
	#endif
#else	
	#ifdef TEST_0RGB	
		//image.pData = bodydetect_bgra_640x384;
		fread((TS_U8 *)image.pData, image.s32W * image.s32C, image.s32H, pfInput);
		fclose(pfInput);
	#else
		image.pData = face_detect_input;
	#endif
#endif

	//resize image
	if(ALG_FACE_DETECT_IMAGE_H != image.s32H || ALG_FACE_DETECT_IMAGE_W != image.s32W)
	{
		resizeBuf = (TS_U8 *)malloc(ALG_FACE_DETECT_IMAGE_C * ALG_FACE_DETECT_IMAGE_H * ALG_FACE_DETECT_IMAGE_W);
		if(TS_NULL == resizeBuf) {
			ALG_LOGE("no enough memory for malloc resize buf\n");
			goto End;
		}

		memset(resizeBuf, 0, ALG_FACE_DETECT_IMAGE_C * ALG_FACE_DETECT_IMAGE_H * ALG_FACE_DETECT_IMAGE_W);
		TS_ALG_Resize(image.pData, resizeBuf, image.s32W, image.s32H, ALG_FACE_DETECT_IMAGE_W,
						ALG_FACE_DETECT_IMAGE_H, ALG_FACE_DETECT_IMAGE_C);

		image.s32C = ALG_FACE_DETECT_IMAGE_C;
		image.s32H = ALG_FACE_DETECT_IMAGE_H;
		image.s32W = ALG_FACE_DETECT_IMAGE_W;
		image.pData = resizeBuf;
	}

	TS_U32 time0 = TIME_CACL_GET();

	s32Ret = TS_ALG_FaceDetect_Process(phandle, &image, &result);
	if(0 != s32Ret) {
		ALG_LOGE("TS_ALG_FaceDetectProcess error\n");
		goto End;
	}

	TS_U32 time1 = TIME_CACL_GET();
	ALG_LOGD("algo face process:%d ms\n",time1-time0);

	for(TS_U32 i = 0; i < result.u32FaceNum; i++) {

		ALG_LOGD("face:%d --score:%f xmin:%f ymin:%f xmax:%f ymax:%f\n", i, result.stBox[i].f32Score,
				result.stBox[i].f32Xmin*image.s32W, result.stBox[i].f32Ymin*(image.s32H-24), result.stBox[i].f32Xmax*image.s32W, result.stBox[i].f32Ymax*(image.s32H-24));
	}

End:
	if(TS_NULL != resizeBuf)
		free(resizeBuf);

	s32Ret = TS_ALG_FaceDetect_Exit(phandle);
	if(0 != s32Ret) {
		ALG_LOGE("TS_ALG_FaceDetectRelease error\n");
		return -1;
	}

	return s32Ret;
}
#endif

#if 0
//bin file model

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

#ifdef TX521X
	FILE *pf_cfg = fopen("tx5215/face_detect_cfg.cfg", "rb");
	FILE *pf_weight = fopen("tx5215/face_detect_weight.weight", "rb");
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

	param.pGraph = pCfgAddr;
	param.pWeight = pWeightAddr;
	param.u32GraphSize = u32CfgSize;
	param.u32WeightSize = u32WeightSize;

#ifdef TEST_0RGB
	param.eImageType = ALG_IMAGE_TYPE_FORMAT_0RGB;
#else
	param.eImageType = ALG_IMAGE_TYPE_INT8_HWC;
#endif
	param.bRneOff = 0;

	log_set_level(ALG_LOG_DEBUG);
	ALG_IMAGE_S image;

	image.s32C = ALG_FACE_DETECT_IMAGE_C;
	image.s32H = ALG_FACE_DETECT_IMAGE_H;
	image.s32W = ALG_FACE_DETECT_IMAGE_W;
#ifdef TEST_0RGB	
	image.pData = face_test;
#else
	image.pData = facedet_v1_7_input;
#endif

	s32Ret = TS_ALG_FaceDetect_Init(&phandle, &param);
	if(0 != s32Ret) {
		ALG_LOGE("TS_ALG_FaceDetect_Init error\n");
		goto End;
	}

	free(pWeightAddr);
	free(pCfgAddr);

	fclose(pf_cfg);
	fclose(pf_weight);

	ALG_LOGD("version:%s\n" ,TS_ALG_FaceDetect_GetVersion());

	ALG_LOGD("cfgsize: %d, weightsize: %d \n", param.u32GraphSize, param.u32WeightSize);
	ALG_FACE_DETECT_RESULT_S result;

	TS_U32 time0 = TIME_CACL_GET();
	s32Ret = TS_ALG_FaceDetect_Process(phandle, &image, &result);
	if(0 != s32Ret) {
		ALG_LOGE("TS_ALG_FaceDetect_Process error\n");
		goto End;
	}
	TS_U32 time1 = TIME_CACL_GET();
	ALG_LOGD("TS_ALG_FaceDetect_Process:%d ms\n",time1-time0);

	for(TS_U32 i = 0; i < result.u32FaceNum; i++) {

		ALG_LOGD("face:%d --score:%f xmin:%f ymin:%f xmax:%f ymax:%f\n", i, result.stBox[i].f32Score,
				result.stBox[i].f32Xmin, result.stBox[i].f32Ymin, result.stBox[i].f32Xmax, result.stBox[i].f32Ymax);
	}
End:
	if(TS_NULL != resizeBuf)
		free(resizeBuf);

	s32Ret = TS_ALG_FaceDetect_Exit(phandle);
	if(0 != s32Ret) {
		ALG_LOGE("TS_ALG_FaceDetectRelease error\n");
		return -1;
	}

	return s32Ret;
}
#endif
