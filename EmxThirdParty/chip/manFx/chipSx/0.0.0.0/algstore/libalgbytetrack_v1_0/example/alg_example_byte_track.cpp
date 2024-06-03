/**
 * @file alg_example_byte_track.cpp
 * @author yhc
 * @brief byte track
 * @version 1.0
 * @date 2023-10-13
 *
 * @copyright Copyright (c) 2023 tsingmicro
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <sys/sysinfo.h>

#include "ts_alg_type.h"
#include "ts_alg_log.h"
#include "ts_alg_face_detect.h"
#include "ts_alg_amr.h"
#include "ts_alg_vb.h"
//#include "ts_rne_device.h"

#include "ts_alg_byte_track.h"

#define TEST_0RGB

#ifdef TEST_0RGB
	#define ALG_FACE_DETECT_IMAGE_C 4
	#define TEST_FRAME_NUM			1		// 测试帧数量
#else
	#define ALG_FACE_DETECT_IMAGE_C 3
	#define TEST_FRAME_NUM			50		// 测试帧数量
#endif

#define MAX_OBJ_NUM				20



#define ALG_MULTI_OBJ_IMAGE_H 	384
#define ALG_MULTI_OBJ_IMAGE_W 	640
// #define USE_CMA


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

//read rgb data from file
TS_S32 main(TS_VOID)
{
	ALG_LOGI("version:%s\n", TS_ALG_ByteTrack_GetVersion());
	TS_S32 s32Ret;
	TS_VOID *phandle = NULL;
	ALG_BYTE_TRACK_INPUT_S param;
	log_set_level(ALG_LOG_DEBUG);
#ifdef TEST_0RGB
	char inputs[TEST_FRAME_NUM][30] = {
		"image_rgba.bin"
	};
#else
	char inputs[TEST_FRAME_NUM][30] = {
		"00000001.nn.int8.bin", "00000002.nn.int8.bin", "00000003.nn.int8.bin", "00000004.nn.int8.bin", "00000005.nn.int8.bin",
		"00000006.nn.int8.bin", "00000007.nn.int8.bin", "00000008.nn.int8.bin", "00000009.nn.int8.bin", "00000010.nn.int8.bin",

		"00000011.nn.int8.bin", "00000012.nn.int8.bin", "00000013.nn.int8.bin", "00000014.nn.int8.bin", "00000015.nn.int8.bin",
		"00000016.nn.int8.bin", "00000017.nn.int8.bin", "00000018.nn.int8.bin", "00000019.nn.int8.bin", "00000020.nn.int8.bin",

		"00000021.nn.int8.bin", "00000022.nn.int8.bin", "00000023.nn.int8.bin", "00000024.nn.int8.bin", "00000025.nn.int8.bin",
		"00000026.nn.int8.bin", "00000027.nn.int8.bin", "00000028.nn.int8.bin", "00000029.nn.int8.bin", "00000030.nn.int8.bin",

		"00000031.nn.int8.bin", "00000032.nn.int8.bin", "00000033.nn.int8.bin", "00000034.nn.int8.bin", "00000035.nn.int8.bin",
		"00000036.nn.int8.bin", "00000037.nn.int8.bin", "00000038.nn.int8.bin", "00000039.nn.int8.bin", "00000040.nn.int8.bin",

		"00000041.nn.int8.bin", "00000042.nn.int8.bin", "00000043.nn.int8.bin", "00000044.nn.int8.bin", "00000045.nn.int8.bin",
		"00000046.nn.int8.bin", "00000047.nn.int8.bin", "00000048.nn.int8.bin", "00000049.nn.int8.bin", "00000050.nn.int8.bin"
	};
#endif

#ifdef TX5368X
	FILE *pf_cfg = fopen("face_detect_cfg_v1_7_tx5368x.cfg", "rb");
	FILE *pf_weight = fopen("face_detect_weight_v1_7_tx5368x.weight", "rb");
#endif

#ifdef TX521X
	FILE *pf_cfg = fopen("face_detect_cfg_v1_7_tx521x.cfg", "rb");
	FILE *pf_weight = fopen("face_detect_weight_v1_7_tx521x.weight", "rb");
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
	fclose(pf_cfg);
	fclose(pf_weight);

	FILE* pfSaveRes = fopen("out_rest.txt", "w");

	ALG_IMAGE_S image;
	image.s32C = ALG_FACE_DETECT_IMAGE_C;
	image.s32H = ALG_MULTI_OBJ_IMAGE_H;
	image.s32W = ALG_MULTI_OBJ_IMAGE_W;

#ifdef USE_CMA
	TS_U32 blkSize = ALG_FACE_DETECT_IMAGE_C * ALG_MULTI_OBJ_IMAGE_H * ALG_MULTI_OBJ_IMAGE_W;
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
#endif

	TS_U32 time_sum = 0;
	memset(&param, 0, sizeof(ALG_BYTE_TRACK_INPUT_S));	//这个地方必须要初始化
	param.u32MaxTrackNum = MAX_OBJ_NUM;
	param.stFaceDet.pGraph = pCfgAddr;
	param.stFaceDet.pWeight = pWeightAddr;
	param.stFaceDet.u32GraphSize = u32CfgSize;
	param.stFaceDet.u32WeightSize = u32WeightSize;

#if (ALG_FACE_DETECT_IMAGE_C == 4)
	param.stFaceDet.eImageType = ALG_IMAGE_TYPE_FORMAT_0RGB;
#else
	param.stFaceDet.eImageType = ALG_IMAGE_TYPE_INT8_HWC;
#endif
	param.stFaceDet.bRneOff = 0; //open/close rne device in solution init/release

	param.bRneOff = 0;
	s32Ret = TS_ALG_ByteTrack_Init(&phandle, &param);
	if (0 != s32Ret) {
		ALG_LOGE("TS_ALG_ByteTrack_Init error\n");
		return -1;
	}
	free(pCfgAddr);
	free(pWeightAddr);

	ALG_BYTE_TRACK_RESULTS_S result;
	memset(&result, 0, sizeof(ALG_BYTE_TRACK_RESULTS_S));
	// 填充上下各12行的边距
	ALG_LOGD("%12s, %12s, %12s, %12s, %12s, %12s, %12s, %12s\n", "scn_radio", "index", "trackid", "x", "y", "w", "h", "score");
	for (size_t i = 0; i < 100; i++) {
		FILE* pfInput = fopen(inputs[0], "rb");
		if (NULL == pfInput){
			ALG_LOGE("fopen error\n");
		}
		//fread((TS_U8 *)image.pData + 12*image.s32C*image.s32W, image.s32W * image.s32C, 360, pfInput);
		fread((TS_U8 *)image.pData, image.s32W * image.s32C, image.s32H, pfInput);
		TS_U32 time0 = TIME_CACL_GET();
		s32Ret = TS_ALG_ByteTrack_Process(phandle, &image, &result);
		if(0 != s32Ret) {
			ALG_LOGE("TS_ALG_ByteTrack_Process error\n");
			goto End;
		}

		TS_U32 time1 = TIME_CACL_GET();

		time_sum += (time1 - time0);
		//ALG_LOGD("TS_ALG_ByteTrack_Process: %d ms\n",time1-time0);
		
		for (TS_U32 j = 0; j < result.u32Num; j++) {
			ALG_LOGD("%12s, %12d, %12d, %12f, %12f, %12f, %12f, %12f\n", "640x384", i+1, 
												result.trackRes.stBox[j].u32TrackId, result.trackRes.stBox[j].f32X, 
												result.trackRes.stBox[j].f32Y, result.trackRes.stBox[j].f32Width, 
												result.trackRes.stBox[j].f32Height, result.trackRes.stBox[j].f32Score);
			ALG_LOGD("%12s, %12d, %12d, %12f, %12f, %12f, %12f, %12f\n", "1920x1080", i+1, 
												result.trackRes.stBox[j].u32TrackId, result.trackRes.stBox[j].f32X*1920/640, 
												result.trackRes.stBox[j].f32Y*1080/360, result.trackRes.stBox[j].f32Width*1920/640, 
												result.trackRes.stBox[j].f32Height*1080/360, result.trackRes.stBox[j].f32Score);
			fprintf(pfSaveRes, "No: %d, %d, %f, %f, %f, %f, %f\n", i+1, 
												result.trackRes.stBox[j].u32TrackId, result.trackRes.stBox[j].f32X*1920/640, 
												result.trackRes.stBox[j].f32Y*1080/360, result.trackRes.stBox[j].f32Width*1920/640, 
												result.trackRes.stBox[j].f32Height*1080/360, result.trackRes.stBox[j].f32Score);
		}
		fclose(pfInput);
		pfInput = NULL;
	}
	ALG_LOGD("TS_ALG_ByteTrack_Processs: avg timecost %d ms\n", time_sum/TEST_FRAME_NUM);
End:
	s32Ret = TS_ALG_ByteTrack_Exit(phandle);
	if (0 != s32Ret) {
		ALG_LOGE("TS_ALG_ByteTrack_Exit error\n");
		return -1;
	}

#ifdef USE_CMA	
	TS_ALG_VB_ReleaseBlock(image.pDataPhy);
	TS_ALG_VB_ReleasePool(poolId);
	TS_ALG_VB_Exit();
#endif
	
	if(pfSaveRes)
		fclose(pfSaveRes);
	return s32Ret;
}

