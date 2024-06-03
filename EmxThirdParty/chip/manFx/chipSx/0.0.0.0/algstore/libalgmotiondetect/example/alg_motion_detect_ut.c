#include <stdio.h>
#include <stdlib.h>

#include "ts_alg_motion_detect.h"

#define IMG_W	640
#define IMG_H	360
#define IMG_C	3

TS_S32 main(void)
{
	TS_S32 s32Ret;
	TS_VOID *phandle = NULL;
	TS_U8 *resizeBuf = NULL;
	ALG_MOTION_DETECT_INIT_S param;
	TS_U8 motiondetect_iunput[IMG_W*IMG_H*3];
	FILE *pf_resize = fopen("test_rgb_rz_abs.bin", "rb");

	param.u32ImgW = IMG_W;
	param.u32ImgH = IMG_H;
	param.u32ImgC = IMG_C;

	log_set_level(LOG_DEBUG);

	s32Ret = TS_ALG_MotionDetect_Init(&phandle, &param);

	if (0 != s32Ret) {
		ALG_LOGE("TS_ALG_MotionDetect_Init error\n");
		goto End;
	}

	ALG_LOGD("version:%s\n", TS_ALG_MotionDetect_GetVersion());

	ALG_MOTION_DETECT_RESULT_S result;
	ALG_IMAGE_S image;

	image.s32C = param.u32ImgC;
	image.s32H = param.u32ImgH;
	image.s32W = param.u32ImgW;
	image.pData = motiondetect_iunput;

	//resize image
	if (IMG_H != image.s32H || IMG_W != image.s32W) {
		resizeBuf = (TS_U8 *)malloc(image.s32H * image.s32W * image.s32C);
		if (TS_NULL == resizeBuf) {
			ALG_LOGE("no enough memory for malloc resize buf\n");
			goto End;
		}

		memset(resizeBuf, 0, image.s32H * image.s32W * image.s32C);
		TS_ALG_Resize(image.pData, resizeBuf, IMG_W, IMG_H, image.s32W, image.s32H, IMG_C);

		image.pData = resizeBuf;
	}

	for (int i = 0; i < 10; i++) {

		TS_U32 time0 = TIME_CACL_GET();

		fread(image.pData, image.s32W*3, image.s32H, pf_resize);
		s32Ret = TS_ALG_MotionDetect_Process(phandle, &image, &result);
		if (0 != s32Ret) {
			ALG_LOGE("TS_ALG_BodyDetectProcess error\n");
			goto End;
		}
		TS_U32 time1 = TIME_CACL_GET();

		ALG_LOGD("algo motion detect process:%d ms\n", time1-time0);

		//输出矩形框的坐标和面积
		for (TS_U32 i = 0; i < result.u32ObjNum; i++)
			ALG_LOGD("point(%f, %f, %f, %f)\n", result.stBox[i].f32Xmin,
					result.stBox[i].f32Ymin, result.stBox[i].f32Xmax, result.stBox[i].f32Ymax);
	}

End:
	if (TS_NULL != resizeBuf)
		free(resizeBuf);

	s32Ret = TS_ALG_MotionDetect_Exit(phandle);
	if (0 != s32Ret) {
		ALG_LOGE("TS_ALG_BodyDetectRelease error\n");
		return -1;
	}
	fclose(pf_resize);

	printf("sucess !!!\n");
	return s32Ret;
}

