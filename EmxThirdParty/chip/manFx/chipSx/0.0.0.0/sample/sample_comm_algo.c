
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <semaphore.h>
#include <dirent.h>
#include <sys/prctl.h>

#include "sample_comm.h"
#include "ts_alg_amr.h"
#include "ts_alg_type.h"
#include "ts_alg_log.h"
#include "ts_alg_vb.h"
#include "ts_rne_device.h"
#include "layers_GP.h"
#include "soft_line.h"

#if defined(DBG_JCY_FACEPLAN)
#include "ts_alg_facereco_solution.h"
#include "face_detect_cfg.h"
#include "face_detect_weight.h"

#include "face_occlusion_cfg.h"
#include "face_occlusion_weight.h"

#include "face_bigangle_cfg.h"
#include "face_bigangle_weight.h"

#include "face_headpose_cfg.h"
#include "face_headpose_weight.h"

#include "face_maskclassify_cfg.h"
#include "face_maskclassify_weight.h"
#endif

#if defined(DBG_JCY_BODYTRACK)
#include "ts_alg_face_detect.h"
#ifndef DBG_JCY_FACEPLAN
#include "face_detect_cfg.h"
#include "face_detect_weight.h"
#endif
#include "ts_alg_byte_track.h"
#endif

#if defined(DBG_JCY_PLATERECO)
#include "ts_alg_car_solution.h"
#include "license_plate_cfg.h"
#include "license_plate_weight.h"
//#include "ts_alg_licenseplate_det.h"
#include "licenseplate_det_cfg.h"
#include "licenseplate_det_weight.h"
#endif

#if defined(DBG_JCY_FACE)
#include "ts_alg_face_detect.h"
#if (!defined DBG_JCY_FACEPLAN) && (!defined DBG_JCY_BODYTRACK)
#include "face_detect_cfg.h"
#include "face_detect_weight.h"
#endif
#endif

#if defined(DBG_JCY_MUTITARGET)
#include "ts_alg_pbce_detect.h"
#include "pbce_detect_cfg.h"
#include "pbce_detect_weight.h"
#endif

#if defined(DBG_JCY_PLATE)
#include "ts_alg_licenseplate_det.h"
#ifndef DBG_JCY_PLATERECO
#include "licenseplate_det_cfg.h"
#include "licenseplate_det_weight.h"
#endif
#endif

#define ALG_DETECT_IMAGE_H 360//90//180//360
#define ALG_DETECT_IMAGE_W 640//160//320//640
#define ALG_SRC_IMAGE_H 1440//90//180//360
#define ALG_SRC_IMAGE_W 2560//160//320//640

#define ALG_RGB_CHN 	   3
#define ALG_RGBA_CHN 	   4

#define ALG_VPSS_GRP0 0
#define ALG_VPSS_CHN0 0
#define ALG_VPSS_CHN1 1
#define ALG_CPM_GRP0 0
#define ALG_CPM_PIPE0 0
#define ALG_CPM_PIPE1 1
#define ALG_CPM_CHN0 0
#define ALG_CPM_CHN1 1
#define ALG_VENC_CHN0 0
#define ALG_SKIP_INTERVAL 7
#define ALG_DEBUG 0

#define DRAW_BOX_CPM_NAME "draw-box-cpm"
#define ALGO_PIC_SAVE_CONFIG_PATH "/root/pic_save.config"
#define ALGO_FACE_RES_SAVE_PATH "/home/data/face_result.log"
#define ALGO_BODY_RES_SAVE_PATH "/home/data/body_result.log"

typedef struct stVB_INFO
{
    VB_BLK VbHandle;
    TS_U8 *pu8VirAddr;
    TS_U32 u32VbSize;
    TS_BOOL bVbUsed;
    TS_U32  u32VbPoolId;
} SAMPLE_VB_INFO;

typedef enum {
    BUFFER_STATUS_INIT = 0,
	BUFFER_STATUS_FILL,
	BUFFER_STATUS_ALGO_PROCESS,
    BUFFER_STATUS_ALGO_BYPASS
} ALGO_BUFFER_STATUS;

extern TS_S32 SavePicData2File(void * buf, int len);
extern TS_S32 SaveFaceResult2File(char *file_path, ALG_MOTION_DETECT_RESULT_S *face_result, char *pic_name);
extern TS_S32 SAMPLE_COMM_ALGO_FACE_StartProcess(void);

static pthread_t gs_AlgoProcPid;
TS_BOOL g_algo_process_run = TS_TRUE;
static VIDEO_FRAME_INFO_S g_algo_buffer;
static VIDEO_FRAME_INFO_S g_algo_srcbuffer;
static ALG_RESULT g_algo_result;
static pthread_mutex_t g_algoprocess_lock = PTHREAD_MUTEX_INITIALIZER;

//static vb_pool AlgoPoolId;

static ALGO_BUFFER_STATUS algo_buffer_status = BUFFER_STATUS_INIT;

TS_VOID *g_algo_motion_handle = NULL;
TS_VOID *g_algo_bike_handle = NULL;
TS_VOID *g_algo_faceplan_handle = NULL;
TS_VOID *g_algo_bodytrack_handle = NULL;
TS_VOID *g_algo_platereco_handle = NULL;
TS_VOID *g_algo_face_handle = NULL;
TS_VOID *g_algo_mutitarget_handle = NULL;
TS_VOID *g_algo_plate_handle = NULL;
TS_BOOL g_algo_save_enable = TS_FALSE;
//SAMPLE_ALGO_TYPE_E g_algo_type = SAMPLE_ALGO_MOTION;
SAMPLE_ALGO_TYPE_E g_algo_type_mask = 0;


static TS_VOID *pAlgoHandle = NULL;

drawprocess g_alg_draw_cb = NULL;

enum { LOG_TRACE, LOG_DEBUG, LOG_INFO, LOG_WARN, LOG_ERROR, LOG_FATAL };

long getSystemTime()
{
	struct timeval tv1;
	gettimeofday(&tv1, NULL);
	return (long)(tv1.tv_sec*1000) + (tv1.tv_usec/1000);
}

static void save2file_render(void * buf, int len, char *fileName){
    int fd = -1;

    if (-1 == fd){
        fd = open(fileName, O_WRONLY | O_CREAT | O_TRUNC,
                  S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if (fd < 0){
            SAMPLE_PRT("unable to create debug file.");
            fd = -2;
        }
    }

    if (fd > 0){
        write(fd, buf, len);
        // SAMPLE_PRT(" file %s write bytes %ld image file closed .",fileName, bytes);
        close(fd);
        fd = -2;
    }

}

TS_S32 TS_ALGO_FaceDetectInit(TS_VOID **handle, ALG_MODEL_INIT_S *param)
{
	TS_S32 s32Ret;

#if defined(DBG_JCY_MOTION)
	if (g_algo_type_mask & SAMPLE_ALGO_MASK_MOTION) {
		ALG_MOTION_DETECT_INIT_S param1;
		memset(&param1, 0, sizeof(ALG_MOTION_DETECT_INIT_S));
		param1.u32ImgW = ALG_DETECT_IMAGE_W;
		param1.u32ImgH = ALG_DETECT_IMAGE_H + 24;
		param1.u32ImgC = ALG_RGBA_CHN;
		log_set_level(LOG_ERROR);
		SAMPLE_PRT("motionDetect version:%s\n", TS_ALG_MotionDetect_GetVersion());
		s32Ret = TS_ALG_MotionDetect_Init(&g_algo_motion_handle, &param1);
		if(0 != s32Ret) {
			SAMPLE_PRT("TS_ALG_MotionDetect_Init error\n");
		}
	}
#endif

#if defined(DBG_JCY_FACEPLAN)
	if (g_algo_type_mask & SAMPLE_ALGO_MASK_FACEPLAN) {
		s32Ret = TS_MPI_TRP_CVE_Init();
		if(0 != s32Ret) {
			ALG_LOGE("TS_MPI_TRP_CVE_Init error\n");
			return -1;
		}

		TS_SIZE_T s32BlobMemLen = 3870720;
		s32Ret = TS_MPI_TRP_RNE_OpenDevice(NULL, rne_register_gp_layers);
		if (0 != s32Ret) {
			ALG_LOGE("open device error!\n");
			return -1;
		}
		TS_VOID *pBlobMem = (TS_VOID *)TS_MPI_TRP_RNE_AllocLinearMem(s32BlobMemLen);
		ALG_FACE_SOLUTION_INIT_S param2;
		memset(&param2, 0, sizeof(ALG_FACE_SOLUTION_INIT_S));
		param2.u32ImgH = ALG_SRC_IMAGE_H;
		param2.u32ImgW = ALG_SRC_IMAGE_W;
		
		param2.stFaceDet.pGraph = face_detect_cfg;
		param2.stFaceDet.pWeight = face_detect_weight;
		param2.stFaceDet.u32GraphSize = sizeof(face_detect_cfg);
		param2.stFaceDet.u32WeightSize = sizeof(face_detect_weight);
		param2.stFaceDet.eImageType = ALG_IMAGE_TYPE_FORMAT_0RGB;
		param2.stFaceDet.vpSelfBuf = pBlobMem;
		param2.stFaceDet.bRneOff = 1; //open/close rne device in solution init/release
		
		param2.stFaceOcclusion.pGraph = face_occlusion_cfg;
		param2.stFaceOcclusion.pWeight = face_occlusion_weight;
		param2.stFaceOcclusion.u32GraphSize = sizeof(face_occlusion_cfg);
		param2.stFaceOcclusion.u32WeightSize = sizeof(face_occlusion_weight);
		param2.stFaceOcclusion.eImageType = ALG_IMAGE_TYPE_FORMAT_0RGB;
		param2.stFaceOcclusion.vpSelfBuf = pBlobMem;
		param2.stFaceOcclusion.bRneOff = 1; //open/close rne device in solution init/release
		
		param2.stBigAngle.pGraph = face_bigangle_cfg;
		param2.stBigAngle.pWeight = face_bigangle_weight;
		param2.stBigAngle.u32GraphSize = sizeof(face_bigangle_cfg);
		param2.stBigAngle.u32WeightSize = sizeof(face_bigangle_weight);
		param2.stBigAngle.eImageType = ALG_IMAGE_TYPE_FORMAT_0RGB;
		param2.stBigAngle.vpSelfBuf = pBlobMem;
		param2.stBigAngle.bRneOff = 1; //open/close rne device in solution init/release
		
		param2.stHeadPose.pGraph = face_headpose_cfg;
		param2.stHeadPose.pWeight = face_headpose_weight;
		param2.stHeadPose.u32GraphSize = sizeof(face_headpose_cfg);
		param2.stHeadPose.u32WeightSize = sizeof(face_headpose_weight);
		param2.stHeadPose.eImageType = ALG_IMAGE_TYPE_FORMAT_0RGB;
		param2.stHeadPose.vpSelfBuf = pBlobMem;
		param2.stHeadPose.bRneOff = TS_TRUE; //open/close rne device in solution init/release
		
		param2.stMaskclassify.pGraph = face_maskclassify_cfg;
		param2.stMaskclassify.pWeight = face_maskclassify_weight;
		param2.stMaskclassify.u32GraphSize = sizeof(face_maskclassify_cfg);
		param2.stMaskclassify.u32WeightSize = sizeof(face_maskclassify_weight);
		param2.stMaskclassify.eImageType = ALG_IMAGE_TYPE_FORMAT_0RGB;
		param2.stMaskclassify.vpSelfBuf = pBlobMem;
		param2.stMaskclassify.bRneOff = TS_TRUE; //open/close rne device in solution init/release
		
		param2.bOccusion = 1;
		param2.bBigAngle = 1;
		param2.bHeadpose = 1;
		param2.bMaskclassify = 1;
		param2.bRneOff = 1;
		log_set_level(LOG_ERROR);
		s32Ret = TS_ALG_FaceSolution_Init(&g_algo_faceplan_handle, &param2);
		if(0 != s32Ret) {
			SAMPLE_PRT("TS_ALG_FaceSolution_Init error\n");
		}
		SAMPLE_PRT("objDetect version:%s\n", TS_ALG_FaceSolution_GetVersion());
	}
#endif

#if defined(DBG_JCY_BODYTRACK)
	if (g_algo_type_mask & SAMPLE_ALGO_MASK_BODYTRACK) {
		ALG_BYTE_TRACK_INPUT_S param3;
		memset(&param3, 0, sizeof(ALG_BYTE_TRACK_INPUT_S));
		param3.u32MaxTrackNum = 20;	
		param3.stFaceDet.pGraph = face_detect_cfg;
		param3.stFaceDet.pWeight = face_detect_weight;
		param3.stFaceDet.u32GraphSize = sizeof(face_detect_cfg);
		param3.stFaceDet.u32WeightSize = sizeof(face_detect_weight);
		param3.stFaceDet.eImageType = ALG_IMAGE_TYPE_FORMAT_0RGB;
		param3.stFaceDet.bRneOff = TS_FALSE;
		param3.bRneOff = TS_FALSE;

		log_set_level(LOG_ERROR);

		s32Ret = TS_ALG_ByteTrack_Init(&g_algo_bodytrack_handle, &param3);
		if (0 != s32Ret) {
			SAMPLE_PRT("TS_ALG_MultiBodyTrack_Init error\n");
			return -1;
		}
		if (!g_algo_bodytrack_handle){
			SAMPLE_PRT("g_algo_bodytrack_handle is null\n");
		}
		//TS_MPI_TRP_CVE_Init(); //init cve
	}
#endif

#if defined(DBG_JCY_PLATERECO)
	if (g_algo_type_mask & SAMPLE_ALGO_MASK_PLATERECO) {
		ALG_CAR_SOLUTION_INIT_S param4;
		memset(&param4, 0, sizeof(ALG_CAR_SOLUTION_INIT_S));
		param4.stCarPedDet.pGraph = licenseplate_det_cfg;
		param4.stCarPedDet.pWeight = licenseplate_det_weight;
		param4.stCarPedDet.u32GraphSize = sizeof(licenseplate_det_cfg);
		param4.stCarPedDet.u32WeightSize = sizeof(licenseplate_det_weight);
		param4.stCarPedDet.eImageType = ALG_IMAGE_TYPE_FORMAT_0RGB;
		param4.stCarPedDet.bRneOff = TS_FALSE;
		param4.stCarPedDet.vpSelfBuf = NULL;
		param4.stLinceplate.pGraph = license_plate_cfg;
		param4.stLinceplate.pWeight = license_plate_weight;
		param4.stLinceplate.u32GraphSize = sizeof(license_plate_cfg);
		param4.stLinceplate.u32WeightSize = sizeof(license_plate_weight);
		param4.stLinceplate.eImageType = ALG_IMAGE_TYPE_FORMAT_0RGB;
		log_set_level(LOG_ERROR);
		s32Ret = TS_ALG_CarSolution_Init(&g_algo_platereco_handle, &param4);
		if(0 != s32Ret) {
			SAMPLE_PRT("TS_ALG_CarSolution_Init error\n");
		}
	}
#endif

#if defined(DBG_JCY_PLATE)
	if (g_algo_type_mask & SAMPLE_ALGO_MASK_PLATE) {
		ALG_MODEL_INIT_S param6;
		memset(&param6, 0, sizeof(ALG_MODEL_INIT_S));
		param6.pGraph = licenseplate_det_cfg;
		param6.pWeight = licenseplate_det_weight;
		param6.u32GraphSize = sizeof(licenseplate_det_cfg);
		param6.u32WeightSize = sizeof(licenseplate_det_weight);
		param6.eImageType = ALG_IMAGE_TYPE_FORMAT_0RGB;
		param6.bRneOff = TS_FALSE;
		param6.vpSelfBuf = NULL;

		log_set_level(LOG_ERROR);

		s32Ret = TS_ALG_LicensePlateDet_Init(&g_algo_plate_handle, &param6);
		if(0 != s32Ret) {
			SAMPLE_PRT("TS_ALG_LicensePlateDet_Init error\n");
		}

		SAMPLE_PRT("plateDetect version:%s\n", TS_ALG_LicensePlateDet_GetVersion());
	}
#endif

#if defined(DBG_JCY_FACE)
	if (g_algo_type_mask & SAMPLE_ALGO_MASK_FACE) {
		ALG_MODEL_INIT_S param7;
		memset(&param7, 0, sizeof(ALG_MODEL_INIT_S));
		param7.pGraph = face_detect_cfg;
		param7.pWeight = face_detect_weight;
		param7.u32GraphSize = sizeof(face_detect_cfg);
		param7.u32WeightSize = sizeof(face_detect_weight);
		param7.eImageType = ALG_IMAGE_TYPE_FORMAT_0RGB;
		param7.bRneOff = TS_FALSE;
		param7.vpSelfBuf = NULL;

		log_set_level(LOG_ERROR);
		s32Ret = TS_ALG_FaceDetect_Init(&g_algo_face_handle, &param7);
		if(0 != s32Ret) {
			SAMPLE_PRT("TS_ALG_FaceDetect_Init error\n");
		}

		SAMPLE_PRT("faceDetect version:%s\n", TS_ALG_FaceDetect_GetVersion());
	}
#endif

#if defined(DBG_JCY_MUTITARGET)
	if (g_algo_type_mask & SAMPLE_ALGO_MASK_MUTITARGET) {
		ALG_MODEL_INIT_S param8;
		memset(&param8, 0, sizeof(ALG_MODEL_INIT_S));
		param8.pGraph = pbce_detect_cfg;
		param8.pWeight = pbce_detect_weight;
		param8.u32GraphSize = sizeof(pbce_detect_cfg);
		param8.u32WeightSize = sizeof(pbce_detect_weight);
		param8.eImageType = ALG_IMAGE_TYPE_FORMAT_0RGB;
		param8.bRneOff = TS_FALSE;
		param8.vpSelfBuf = NULL;

		log_set_level(LOG_ERROR);
		s32Ret = TS_ALG_PbceDetect_Init(&g_algo_mutitarget_handle, &param8);
		if(0 != s32Ret) {
			SAMPLE_PRT("TS_ALG_PbceDetect_Init error\n");
		}

		SAMPLE_PRT("multiDetect version:%s\n", TS_ALG_PbceDetect_GetVersion());
	}
#endif

    return TS_SUCCESS;
}

#if defined(DBG_JCY_MOTION)
static TS_S32 TS_ALGO_MotionDetectProcess(TS_VOID* handle, ALG_IMAGE_S *face_in, ALG_MOTION_DETECT_RESULT_S *result)
{
	TS_S32 s32Ret = 0;
	//struct timeval beforetime;
	//struct timeval aftertime;
	//long  runtime = 0;
	//gettimeofday(&beforetime, NULL);
	if (!g_algo_motion_handle || !face_in || !result){
		SAMPLE_PRT("TS_ALG_MotionDetect_Process handle is null\n");
		return TS_FAILURE;
	}
	s32Ret = TS_ALG_MotionDetect_Process(g_algo_motion_handle, face_in, result);
	if(0 != s32Ret) {
		SAMPLE_PRT("TS_ALG_MotionDetect_Process error\n");
	}
	//gettimeofday(&aftertime, NULL);
	//runtime = (aftertime.tv_sec - beforetime.tv_sec) *1000 + (aftertime.tv_usec - beforetime.tv_usec) / 1000;
	//SAMPLE_PRT("MotionDetect use time:%ld\n", runtime );
    return TS_SUCCESS;
}
#endif

#if defined(DBG_JCY_FACEPLAN)
TS_S32 TS_ALGO_FaceplanDetectProcess(TS_VOID* handle, ALG_IMAGE_S *pImage, ALG_IMAGE_S *pImageDet, ALG_FACE_SOLUTION_RESULT_S *result)
{
	TS_S32 s32Ret;
	if (!g_algo_faceplan_handle || !pImage || !pImageDet || !result){
		SAMPLE_PRT("TS_ALG_FaceSolution_Process handle is null\n");
		return TS_FAILURE;
	}
	s32Ret = TS_ALG_FaceSolution_Process(g_algo_faceplan_handle, pImage, pImageDet, result);
	if(0 != s32Ret) {
		SAMPLE_PRT("TS_ALG_FaceSolution_Process error\n");
	}
    return TS_SUCCESS;
}
#endif

#if defined(DBG_JCY_BODYTRACK)
TS_S32 TS_ALGO_MultiBodyTrackProcess(TS_VOID* handle, ALG_IMAGE_S *in, ALG_BYTE_TRACK_RESULTS_S *result)
{
	TS_S32 s32Ret;
	if (!g_algo_bodytrack_handle || !in || !result){
		SAMPLE_PRT("TS_ALG_ByteTrack_Process handle is null\n");
		return TS_FAILURE;
	}
	s32Ret = TS_ALG_ByteTrack_Process(g_algo_bodytrack_handle, in, result);
	if(0 != s32Ret) {
		SAMPLE_PRT("TS_ALG_ByteTrack_Process error\n");
	}
    return TS_SUCCESS;
}
#endif

#if defined(DBG_JCY_PLATERECO)
TS_S32 TS_ALGO_PlateRecoProcess(TS_VOID* handle, ALG_IMAGE_S *pImage, ALG_IMAGE_S *pImageDet, ALG_CAR_SOLUTION_RESULT_S *result)
{
	TS_S32 s32Ret;
	if (!g_algo_platereco_handle || !pImage || !pImageDet || !result){
		SAMPLE_PRT("TS_ALGO_PlateRecoProcess handle is null\n");
		return TS_FAILURE;
	}
	s32Ret = TS_ALG_CarSolution_Process(g_algo_platereco_handle, pImage, pImageDet, result);
	if(0 != s32Ret) {
		SAMPLE_PRT("TS_ALGO_PlateRecoProcess error\n");
	}
    return TS_SUCCESS;
}
#endif

#if defined(DBG_JCY_FACE)
TS_S32 TS_ALGO_FaceDetectProcess(TS_VOID* handle, ALG_IMAGE_S *in, ALG_FACE_DETECT_RESULT_S *result)
{
	TS_S32 s32Ret;
	if (!g_algo_face_handle || !in || !result){
		SAMPLE_PRT("TS_ALG_FaceDetect_Process handle is null\n");
		return TS_FAILURE;
	}
	s32Ret = TS_ALG_FaceDetect_Process(g_algo_face_handle, in, result);
	if(0 != s32Ret) {
		SAMPLE_PRT("TS_ALG_FaceDetect_Process error\n");
	}
    return TS_SUCCESS;
}
#endif

#if defined(DBG_JCY_MUTITARGET)
TS_S32 TS_ALGO_MutiDetectProcess(TS_VOID* handle, ALG_IMAGE_S *in, ALG_PBCE_DETECT_RESULT_S *result)
{
	TS_S32 s32Ret;
	if (!g_algo_mutitarget_handle || !in || !result){
		SAMPLE_PRT("TS_ALGO_MutiDetectProcess handle is null\n");
		return TS_FAILURE;
	}
	s32Ret = TS_ALG_PbceDetect_Process(g_algo_mutitarget_handle, in, result);
	if(0 != s32Ret) {
		SAMPLE_PRT("TS_ALG_PbceDetect_Process error\n");
	}
    return TS_SUCCESS;
}
#endif

#if defined(DBG_JCY_PLATE)
TS_S32 TS_ALGO_PlateDetectProcess(TS_VOID* handle, ALG_IMAGE_S *in, ALG_LICENSE_PLATE_DET_RESULT_S *result)
{
	TS_S32 s32Ret = 0;
	if (!g_algo_plate_handle || !in || !result){
		SAMPLE_PRT("TS_ALG_PlateDetect_Process handle is null\n");
		return TS_FAILURE;
	}
	s32Ret = TS_ALG_LicensePlateDet_Process(g_algo_plate_handle, in, result);
	if(0 != s32Ret) {
		SAMPLE_PRT("TS_ALG_PlateDetect_Process error\n");
	}
    return TS_SUCCESS;
}
#endif

TS_S32 TS_ALGO_FaceDetectRelease(TS_VOID* handle)
{
    SAMPLE_PRT("enter algo exit!\n");
    return TS_SUCCESS;
}

TS_S32 TS_ALGO_FaceDetectGetResult(TS_VOID* handle, TS_U32 *Count)
{
    SAMPLE_PRT("enter algo get result!\n");
    return TS_SUCCESS;
}

const TS_CHAR* TS_ALGO_FaceDetectGetVersion(TS_VOID)
{
    SAMPLE_PRT("enter algo get version!\n");
    const TS_CHAR *AlgoVersion = "TX536_ALGO_FACE_V1.0";
    return AlgoVersion;
}

void debug_save2file_render_algo(void * buf, int len, char *fileName){
    int fd = -1;
    unsigned long bytes;
    if (-1 == fd){
        fd = open(fileName, O_WRONLY | O_CREAT | O_TRUNC,
                  S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if (fd < 0){
            SAMPLE_PRT("unable to create debug file.");
            fd = -2;
        }
    }

    if (fd > 0){
        bytes = write(fd, buf, len);
        SAMPLE_PRT(" file %s write bytes %ld yuv debug file closed .",fileName, bytes);
        close(fd);
        fd = -2;
    }
}

TS_S32  SAMPLE_COMM_ALGO_DRAW_RectBox_BySoft(VIDEO_FRAME_INFO_S *in, VIDEO_FRAME_INFO_S *out)
{
    TS_U32 ImageRatio = 0;
    //to do in

	TS_U32 width = 0;
	TS_U32 height = 0;

	if (SENSOR0_TYPE == EVB_SENSOR_5MP) {
		width = 2688;
		height = 1860;
	} else if (SENSOR0_TYPE == EVB_SENSOR) {
		width = 1920;
		height = 1080;
	} else if (SENSOR0_TYPE == EVB_SENSOR_4K) {
		width = 3840;
		height = 2160;
	} else if (SENSOR0_TYPE == EVB_SENSOR_4MP) {
		width = 2560;
		height = 1440;
	}

    unsigned char * YuvBuf = (unsigned char *)(TS_UL)(in->stVFrame.u64VirAddr[0]);
    RECT rect = {0};

	ImageRatio = 1;
    TS_BOOL openDebug = TS_FALSE;
    TS_BOOL saveYuvFile = TS_FALSE;
    TS_S32 len = width*height*3/2;
    TS_CHAR filename[50] = {0};
    static TS_U32 frameCount = 0;
    //static TS_S32 indexFile = 0;

#if defined(DBG_JCY_MOTION)
	pthread_mutex_lock(&g_algoprocess_lock);
	ALG_MOTION_DETECT_RESULT_S *pmotion_result = &g_algo_result.g_algo_motion_result;
	for (int i = 0; i < pmotion_result->u32ObjNum; i++)
	{
        rect.left = ImageRatio * pmotion_result->stBox[i].f32Xmin  * width;
        rect.top = ImageRatio * pmotion_result->stBox[i].f32Ymin *  height;
        rect.right = ImageRatio * pmotion_result->stBox[i].f32Xmax * width;
        rect.bottom = ImageRatio *pmotion_result->stBox[i].f32Ymax * height;
        YUV_Draw_Rect(YuvBuf, width, height, 12, &rect, YUV_BLUE, 5);
	}
	pthread_mutex_unlock(&g_algoprocess_lock);
#endif

#if defined(DBG_JCY_FACEPLAN)
	ALG_FACE_SOLUTION_RESULT_S *pobj_result = &g_algo_result.g_algo_faceplan_result;
	for (int i = 0; i < pobj_result->stDetect.u32FaceNum; i++)
	{
        rect.left = ImageRatio * pobj_result->stDetect.stBox[i].f32Xmin  * width;
        rect.top = ImageRatio * pobj_result->stDetect.stBox[i].f32Ymin *  height;
        rect.right = ImageRatio * pobj_result->stDetect.stBox[i].f32Xmax * width;
        rect.bottom = ImageRatio * pobj_result->stDetect.stBox[i].f32Ymax * height;
        YUV_Draw_Rect(YuvBuf, width, height, 12, &rect, YUV_RED, 5);
		//printf("algo_faceplan goodpose:%d,head:%f,bclass:%d,classscore:%f\n",pobj_result->stHeadPose[i].bGoodpose, pobj_result->stHeadPose[i].f32Headpose[0],
			//pobj_result->stMaskClassify[i].bClass, pobj_result->stMaskClassify[i].f32ClassScore);
	}
	//printf("algo_faceplan num :%d\n",pobj_result->u32Num);
#endif

#if defined(DBG_JCY_BODYTRACK)
	pthread_mutex_lock(&g_algoprocess_lock);
	ALG_BYTE_TRACK_RESULTS_S *pbodytrack_result = &g_algo_result.g_algo_bodytrack_result;
	for (int i = 0; i < pbodytrack_result->stObjRes.u32FaceNum; i++)
	{
        rect.left = ImageRatio * pbodytrack_result->stObjRes.stBox[i].f32Xmin  * width;
        rect.top = ImageRatio * pbodytrack_result->stObjRes.stBox[i].f32Ymin *  height;
        rect.right = ImageRatio * pbodytrack_result->stObjRes.stBox[i].f32Xmax * width;
        rect.bottom = ImageRatio *pbodytrack_result->stObjRes.stBox[i].f32Ymax * height;
		YUV_Draw_Rect(YuvBuf, width, height, 12, &rect, YUV_GREEN, 5);
	}

	/*for (int i = 0; i < pbodytrack_result->trackRes.u32TrackNum; i++)
	{
		rect.left = ImageRatio * pbodytrack_result->trackRes.stBox[i].f32X * width;
		rect.top = ImageRatio * pbodytrack_result->trackRes.stBox[i].f32Y * height;
		rect.right = ImageRatio * pbodytrack_result->trackRes.stBox[i].f32Width * width;
		rect.bottom = ImageRatio * pbodytrack_result->trackRes.stBox[i].f32Height * height;
		YUV_Draw_Rect(YuvBuf, width, height, 12, &rect, YUV_GREEN, 5);
		printf("bodytrack track_id:%d\n",pbodytrack_result->trackRes.stBox[i].u32TrackId);
		//YUV_Draw_BodyTrack_Rect(YuvBuf, width, height, 12, &rect, pbodytrack_result->trackRes.stBox[i].u32TrackId, YUV_GREEN, 5);
	}*/
	pthread_mutex_unlock(&g_algoprocess_lock);
#endif

#if defined(DBG_JCY_PLATERECO)
	pthread_mutex_lock(&g_algoprocess_lock);
	ALG_CAR_SOLUTION_RESULT_S *platereco_result = &g_algo_result.g_algo_platereco_result;
	for (int i = 0; i < platereco_result->u32Num; i++)
	{
		rect.left = ImageRatio * platereco_result->licenseDet.stBox[i].f32Xmin * width;
		rect.top = ImageRatio * platereco_result->licenseDet.stBox[i].f32Ymin *  height;
		rect.right = ImageRatio * platereco_result->licenseDet.stBox[i].f32Xmax * width;
		rect.bottom = ImageRatio *platereco_result->licenseDet.stBox[i].f32Ymax * height;
		YUV_Draw_Rect(YuvBuf, width, height, 12, &rect, YUV_BLUE, 5);

		if (frameCount % 5 == 0){
			SAMPLE_PRT("platereco_result plate plate num:%12s\n", platereco_result->lincensePlate[i].s8CarPlate);
		}

	}
	pthread_mutex_unlock(&g_algoprocess_lock);
#endif

#if defined(DBG_JCY_FACE)
	pthread_mutex_lock(&g_algoprocess_lock);
	ALG_FACE_DETECT_RESULT_S *face_result = &g_algo_result.g_algo_face_result;
	for (int i = 0; i < face_result->u32FaceNum; i++)
	{
		rect.left = ImageRatio * face_result->stBox[i].f32Xmin	* width;
		rect.top = ImageRatio * face_result->stBox[i].f32Ymin *  height;
		rect.right = ImageRatio * face_result->stBox[i].f32Xmax * width;
		rect.bottom = ImageRatio *face_result->stBox[i].f32Ymax * height;
		
		YUV_Draw_Rect(YuvBuf, width, height, 12, &rect, YUV_ORANGE, 5);
	}
	pthread_mutex_unlock(&g_algoprocess_lock);
#endif

#if defined(DBG_JCY_MUTITARGET)
	pthread_mutex_lock(&g_algoprocess_lock);
	ALG_PBCE_DETECT_RESULT_S *mutitarget_result = &g_algo_result.g_algo_mutitarget_result;
	for (int i = 0; i < mutitarget_result->u32ObjNum; i++)
	{
		rect.left = ImageRatio * mutitarget_result->stBox[i].f32Xmin	* width;
		rect.top = ImageRatio * mutitarget_result->stBox[i].f32Ymin *  height;
		rect.right = ImageRatio * mutitarget_result->stBox[i].f32Xmax * width;
		rect.bottom = ImageRatio *mutitarget_result->stBox[i].f32Ymax * height;

		if (0 == mutitarget_result->u8ObjNo[i])
			YUV_Draw_Rect(YuvBuf, width, height, 12, &rect, YUV_ORANGE, 5);
		else if (1 == mutitarget_result->u8ObjNo[i])
			YUV_Draw_Rect(YuvBuf, width, height, 12, &rect, YUV_GRAY, 5);
		else if (2 == mutitarget_result->u8ObjNo[i])
			YUV_Draw_Rect(YuvBuf, width, height, 12, &rect, YUV_GREEN, 5);
		else if (3 == mutitarget_result->u8ObjNo[i])
			YUV_Draw_Rect(YuvBuf, width, height, 12, &rect, YUV_PURPLE, 5);
		else if (4 == mutitarget_result->u8ObjNo[i])
			YUV_Draw_Rect(YuvBuf, width, height, 12, &rect, YUV_RED, 5);
	}
	pthread_mutex_unlock(&g_algoprocess_lock);
#endif

#if defined(DBG_JCY_PLATE)
	pthread_mutex_lock(&g_algoprocess_lock);
	ALG_LICENSE_PLATE_DET_RESULT_S *plate_result = &g_algo_result.g_algo_plate_result;
	for (int i = 0; i < plate_result->u32LicensePlateDetNum; i++)
	{
		rect.left = ImageRatio * plate_result->stBox[i].f32Xmin	* width;
		rect.top = ImageRatio * plate_result->stBox[i].f32Ymin *  height;
		rect.right = ImageRatio * plate_result->stBox[i].f32Xmax * width;
		rect.bottom = ImageRatio *plate_result->stBox[i].f32Ymax * height;

		YUV_Draw_Rect(YuvBuf, width, height, 12, &rect, YUV_LIGHT_BLUE, 5);
	}
	pthread_mutex_unlock(&g_algoprocess_lock);
#endif

    if(saveYuvFile == TS_TRUE && openDebug == TS_TRUE){
        debug_save2file_render_algo(YuvBuf, len, filename);
    }

    frameCount++;

    memcpy(out, in, sizeof(VIDEO_FRAME_INFO_S));
    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_ALGO_Yuv2Rgb(TS_U8 *y_image, TS_U8 *uv_image, TS_U8 *rgb_image,TS_U32 src_width, TS_U32 src_height, TS_U32 des_width, TS_U32 des_height, ALG_RGB_TYPE_E rgb_type)
{
#if 0
    FILE *fp0 = NULL;
	FILE *fp1 = NULL;
    fp0 = fopen("./420sp.yuv", "r");
	if (fp0 == NULL)
	{
		printf("open fp0 error\n");
		return;
	}
	fread(yuv_image, 1, image_width * image_height * 3 /2, fp0);
	fclose(fp0);
#endif
	TS_ALG_YUV2RGB(y_image, uv_image, rgb_image, src_width, src_height, des_width, des_height, rgb_type);
#if 0
	fp1 = fopen("./out.rgb", "w+");
	fwrite(rgb_image, 1, image_width * image_height * 3, fp1);
	fclose(fp1);
#endif
    return TS_SUCCESS;
}

TS_S32  SAMPLE_COMM_ALGO_FACE_CPM_GetResult_Cb(VIDEO_FRAME_INFO_S *in, VIDEO_FRAME_INFO_S *out)
{
    CHECK_NULL_PTR(in);
    CHECK_NULL_PTR(out);
    SAMPLE_COMM_ALGO_DRAW_RectBox_BySoft(in, out);
    return TS_SUCCESS;
}

TS_S32 SAMPLE_COMM_ALGO_DrawCallBack(drawprocess cb)
{
	if(g_alg_draw_cb){
		SAMPLE_PRT("g_alg_draw_cb is not NULL!\n");
		return TS_FAILURE;
	}
	else{
		g_alg_draw_cb = cb;
		return TS_SUCCESS;
	}
	
	return TS_SUCCESS;
}


TS_S32 SAMPLE_COMM_ALGO_FACE_CPM_Exit(TS_VOID *pHandle)
{

    TS_S32   s32Ret = TS_SUCCESS;

    CHECK_NULL_PTR(pHandle);
	#if 0
    s32Ret =TS_ALG_FaceDetect_Exit(pHandle);
	if(TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("TS_ALG_FaceDetect_Exit error\n");
		return TS_FAILURE;
	}
	#endif

#if defined(DBG_JCY_MOTION)
	s32Ret =TS_ALG_MotionDetect_Exit(pHandle);
	if(TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("TS_ALG_MotionDetect_Exit error\n");
		return TS_FAILURE;
	}
#endif

    return s32Ret;
}

TS_S32 SAMPLE_COMM_ALGO_FACE_CPM_GetResult(TS_VOID *pHandle, TS_VOID *pResult)
{
    TS_S32   s32Ret = TS_SUCCESS;
    CHECK_NULL_PTR(pHandle);
    TS_ALGO_FaceDetectGetResult(pHandle, pResult);
    printf("count=%d\n", *(TS_U32*)pResult);
	//memcpy(pResult, &test_func->count, sizeof(int));

    return s32Ret;
}

TS_S32 SAMPLE_COMM_ALGO_CPM_GetResult(TS_VOID *pHandle, TS_VOID *pResult)
{
    return TS_MPI_CPM_GetResult((CPM_GRP)pHandle,pResult);
}

TS_S32 SAMPLE_COMM_ALGO_CPM_SetParam(TS_VOID *pHandle, TS_VOID *pParam)
{
    printf("cpm set param \n");
    return TS_SUCCESS;
}

TS_S32  SAMPLE_COMM_ALGO_CPM_GetParam(TS_VOID *pHandle, TS_VOID *pParam)
{
    printf("cpm get param \n");
    return TS_SUCCESS;
}

TS_VOID SAMPLE_COMM_ALGO_FACE_BODY_CPM_Exit(TS_VOID *pHandle)
{
    return;
}

TS_S32  SAMPLE_COMM_ALGO_FACE_BODY_CPM_Init(TS_VOID **pHandle)
{

    SAMPLE_PRT("Start face box cpm init \n");
    TS_S32   s32Ret = TS_SUCCESS;
	ALG_MODEL_INIT_S param;

    CHECK_NULL_PTR(pHandle);
	SAMPLE_PRT("Version:%s\n" ,TS_ALGO_FaceDetectGetVersion());
	s32Ret = TS_ALGO_FaceDetectInit(&pAlgoHandle, &param);
	if(TS_SUCCESS != s32Ret) {
		SAMPLE_PRT("TS_ALGO_FaceDetectInit error\n");
        return TS_FAILURE;
	}

    SAMPLE_COMM_ALGO_FACE_StartProcess();
    *pHandle = pAlgoHandle;

    return TS_SUCCESS;
}

TS_S32  SAMPLE_COMM_ALGO_FACE_BODY_CPM_Process(TS_VOID *pHandle, TS_VOID **in, TS_VOID **out)
{
	CHECK_NULL_PTR(in);
    CHECK_NULL_PTR(*in);
    CHECK_NULL_PTR(out);
    CHECK_NULL_PTR(*out);
	VIDEO_FRAME_INFO_S *inPipesrcFrameAlgo = in[0];
    VIDEO_FRAME_INFO_S *inPipeFrameAlgo = in[1];
    VIDEO_FRAME_INFO_S *outChnFrameVenc = out[0];
	static int count = 0;
	
	if (!((count++)%1000)) {
	    SAMPLE_PRT("algo status=%d\n", algo_buffer_status);
	}
    switch (algo_buffer_status)
    {
    case BUFFER_STATUS_INIT:
        {
        	memcpy(&g_algo_srcbuffer, inPipesrcFrameAlgo, sizeof(VIDEO_FRAME_INFO_S));
            memcpy(&g_algo_buffer, inPipeFrameAlgo, sizeof(VIDEO_FRAME_INFO_S));
            memset(&g_algo_result.g_algo_motion_result, 0, sizeof(ALG_MOTION_DETECT_RESULT_S));
			memset(&g_algo_result.g_algo_platereco_result, 0, sizeof(ALG_LICENSE_PLATE_RESULT_S));
            memset(&g_algo_result.g_algo_face_result, 0, sizeof(ALG_FACE_DETECT_RESULT_S));
			memset(&g_algo_result.g_algo_faceplan_result, 0, sizeof(ALG_FACE_SOLUTION_RESULT_S));
			memset(&g_algo_result.g_algo_bodytrack_result, 0, sizeof(ALG_BYTE_TRACK_RESULTS_S));
			memset(&g_algo_result.g_algo_plate_result, 0, sizeof(ALG_LICENSE_PLATE_DET_RESULT_S));
			memset(&g_algo_result.g_algo_mutitarget_result, 0, sizeof(ALG_PBCE_DETECT_RESULT_S));
            algo_buffer_status = BUFFER_STATUS_ALGO_PROCESS;
        }
        break;
    case BUFFER_STATUS_FILL:
        {	
        	memcpy(&g_algo_srcbuffer, inPipesrcFrameAlgo, sizeof(VIDEO_FRAME_INFO_S));
            memcpy(&g_algo_buffer, inPipeFrameAlgo, sizeof(VIDEO_FRAME_INFO_S));
            algo_buffer_status = BUFFER_STATUS_ALGO_PROCESS;
        }
        break;
    case BUFFER_STATUS_ALGO_BYPASS:
        {
            memcpy(outChnFrameVenc,inPipeFrameAlgo,sizeof(VIDEO_FRAME_INFO_S));
            return TS_SUCCESS;
        }
    default:

        break;
    }
    #if 0
    /* Save venc chn src file*/
    if (TS_TRUE == g_algo_save_enable){
        SavePicData2File(ALGO_PIC_SAVE_PATH, inPipeFrame, picName);
    }
    #endif

	if(g_alg_draw_cb){
		g_alg_draw_cb(in,out);
	}
	else{
		memcpy(out[0], in[0], sizeof(VIDEO_FRAME_INFO_S));
	}
	
    return TS_SUCCESS;
}

TS_S32  SAMPLE_COMM_ALGO_FACE_BODY_CPM_GetResult(TS_VOID *pHandle, TS_VOID *pResult)
{
	CHECK_NULL_PTR(pResult);
	pthread_mutex_lock(&g_algoprocess_lock);
	memcpy(pResult,&g_algo_result,sizeof(ALG_RESULT));
	pthread_mutex_unlock(&g_algoprocess_lock);
    return TS_SUCCESS;
}

TS_S32 SAMPLE_ALGO_CPM_Config(AVS_GRP CPMGrp, int pipeNum, SAMPLE_ALGO_TYPE_E algo_type)
{
    TS_S32         s32Ret = TS_SUCCESS;
    CPM_GRP_ATTR_S  stCPMGrpAttr;
    TS_U32         u32Width;
    TS_U32         u32Height;
    TS_S32  i = 0;
    CPM_Handle_S    stCPMHandle;

    memset(&stCPMGrpAttr, 0, sizeof(CPM_GRP_ATTR_S));

	if (SENSOR0_TYPE == EVB_SENSOR_5MP) {
        u32Width = 2688;
        u32Height = 1860;
	} else if (SENSOR0_TYPE == EVB_SENSOR) {
        u32Width = 1920;
        u32Height = 1080;
	} else if (SENSOR0_TYPE == EVB_SENSOR_4K) {
        u32Width = 3840;
        u32Height = 2160;
	} else if (SENSOR0_TYPE == EVB_SENSOR_4MP) {
		u32Width = 2560;
		u32Height = 1440;
	}
    g_algo_type_mask = algo_type;

    stCPMGrpAttr.u32GrpId        =  0;
    stCPMGrpAttr.u32PipeNum      =  pipeNum;

    stCPMGrpAttr.u32ChnNum   =  1;


    stCPMGrpAttr.u32Interval     =  1;
    stCPMGrpAttr.stChnAttr[0].u32Width = u32Width;
    stCPMGrpAttr.stChnAttr[0].u32Height = u32Height;
    stCPMGrpAttr.stChnAttr[0].enPixelFormat = PIXEL_FORMAT_NV_12;

    s32Ret = TS_MPI_CPM_CreateGrp(CPMGrp, &stCPMGrpAttr);

    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Creat grp failed with %#x!\n", s32Ret);
        goto exit;
    }

    /*register algorithm*/
    memset(&stCPMHandle, 0, sizeof(CPM_Handle_S));

    stCPMHandle.s32PipeNum = pipeNum;
    for(i = 0;i<pipeNum ;i++)
    {
        stCPMHandle.stPipeAttr[i].u32Width = u32Width;
        stCPMHandle.stPipeAttr[i].u32Height = u32Height;
        stCPMHandle.stPipeAttr[i].enPixelFormat = PIXEL_FORMAT_NV_12;
        /*algo channel*/
        if(i == 1)
        {
            stCPMHandle.stPipeAttr[1].u32Width = 640;
            stCPMHandle.stPipeAttr[1].u32Height = 360;
			//stCPMHandle.stPipeAttr[i].enPixelFormat = PIXEL_FORMAT_ARGB_8888;
        }
    }

    stCPMHandle.init = SAMPLE_COMM_ALGO_FACE_BODY_CPM_Init;
    stCPMHandle.exit = SAMPLE_COMM_ALGO_FACE_BODY_CPM_Exit;
    stCPMHandle.process = SAMPLE_COMM_ALGO_FACE_BODY_CPM_Process;
    stCPMHandle.set_param = SAMPLE_COMM_ALGO_CPM_SetParam;
    stCPMHandle.get_param = SAMPLE_COMM_ALGO_CPM_GetParam;
    stCPMHandle.get_result = SAMPLE_COMM_ALGO_FACE_BODY_CPM_GetResult;
    stCPMHandle.release_result = SAMPLE_COMM_ALGO_CPM_GetResult;

    s32Ret = TS_MPI_CPM_Register(CPMGrp, &stCPMHandle);
    if (TS_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("TS_MPI_CPM_Register failed with %#x!\n", s32Ret);
        goto exit;
    }

    return TS_SUCCESS;

exit:
    TS_MPI_CPM_DestroyGrp(CPMGrp);

    return TS_FAILURE;

}

TS_VOID* SAMPLE_COMM_ALGO_FACE_Process(void* p)
{
    TS_S32  s32Ret = TS_SUCCESS;
    static TS_S32  Frame_count = 0;
    TS_U8 *RgbImage = NULL;
    TS_S32  algo_rgb_size = (ALG_DETECT_IMAGE_H + 24) * ALG_DETECT_IMAGE_W * ALG_RGBA_CHN;
    ALG_IMAGE_S AlgoFaceIn;
	static int count = 0;
	struct timeval TimeoutVal;
	TS_UL	runtime = 0;
	uint32_t vbPool = 0;
	TS_U64 u64PhyAddr = 0;
	VB_POOL_CONFIG_S stVbPoolCfg;
	VB_BLK vbHandle = 0;
	//TS_U8 *RgbsrcImage = NULL;
	//TS_S32  algo_srcrgb_size = ALG_SRC_IMAGE_H * ALG_SRC_IMAGE_W * 3 / 2;
	prctl(PR_SET_NAME, "algo_process");
	stVbPoolCfg.u32BlkCnt = 1;
    stVbPoolCfg.u64BlkSize = algo_rgb_size;
	vbPool = TS_MPI_VB_CreatePool(&stVbPoolCfg);
	vbHandle = TS_MPI_VB_GetBlock(vbPool, algo_rgb_size, TS_NULL);
    if (VB_INVALID_HANDLE == vbHandle)
    {
        SAMPLE_PRT("TS_MPI_VB_GetBlock failed!\n");
		goto exit2;
    }
	u64PhyAddr = TS_MPI_VB_Handle2PhysAddr(vbHandle);
    if (0 == u64PhyAddr)
    {
        SAMPLE_PRT("TS_MPI_VB_Handle2PhysAddr failed!.\n");
		goto exit1;
    }
	 s32Ret = TS_MPI_VB_MmapPool(vbPool);
	 if (s32Ret != 0)
	 {
		 SAMPLE_PRT("TS_MPI_VB_MmapPool failed!.\n");
		 goto exit;
	 }

	s32Ret = TS_MPI_VB_GetBlockVirAddr(vbPool, u64PhyAddr, (TS_VOID **)(&RgbImage));
	if (s32Ret != 0)
    {
        SAMPLE_PRT("TS_MPI_VB_GetBlockVirAddr failed!\n");
		goto exit;
    }
	memset(RgbImage, 114, algo_rgb_size);
	/*FILE* pfInput = fopen("/tmp/nfs/raw.rgb", "rb");
	if(!pfInput){
		SAMPLE_PRT("xxn_xxn file open failed!\n");
	}
	else{
		SAMPLE_PRT("xxn_xxn file open successed!\n");
	}
	if(pfInput){
		fread((TS_U8 *)RgbImage, 1, 640 * 4 * 384, pfInput);
		SAMPLE_PRT("xxn_xxn file read read read!\n");
	}
	RgbsrcImage = (TS_U8 *)malloc(algo_srcrgb_size);
    if (TS_NULL == RgbsrcImage){
        SAMPLE_PRT("RgbsrcImage malloc is null error\n");
        goto exit;
    }
	FILE* Input = fopen("/tmp/nfs/yuan.yuv", "rb");
	if(!Input){
		SAMPLE_PRT("xxn_xxn file open failed!\n");
	}
	else{
		SAMPLE_PRT("xxn_xxn file open successed!\n");
	}
	if(Input){
		fread((TS_U8 *)RgbsrcImage, 1, algo_srcrgb_size, Input);
		SAMPLE_PRT("xxn_xxn file read read read!\n");
	}*/
    SAMPLE_PRT("SAMPLE_COMM_ALGO_FACE_Process run[%d]\n", s32Ret);
    while (g_algo_process_run) {
        if (algo_buffer_status != BUFFER_STATUS_ALGO_PROCESS){
			if (!(count%1000)) {
				SAMPLE_PRT("algo status=%d\n", algo_buffer_status);
			}
            goto algo_next_loop1;
        }

        /*3.yuv to rgb  */
        Frame_count++;

		SAMPLE_COMM_ALGO_Yuv2Rgb((TS_U8*)(uintptr_t)g_algo_buffer.stVFrame.u64VirAddr[0], (TS_U8*)(uintptr_t)(g_algo_buffer.stVFrame.u64VirAddr[0] + ALG_DETECT_IMAGE_W * ALG_DETECT_IMAGE_H),
			(RgbImage + ALG_RGBA_CHN * 12 * ALG_DETECT_IMAGE_W), ALG_DETECT_IMAGE_W, ALG_DETECT_IMAGE_H, ALG_DETECT_IMAGE_W, ALG_DETECT_IMAGE_H, ALG_RGB_TYPE_RGBA32);
		
		AlgoFaceIn.s32H = (ALG_DETECT_IMAGE_H + 24);
	    AlgoFaceIn.s32W = ALG_DETECT_IMAGE_W;
		AlgoFaceIn.s32C = ALG_RGBA_CHN;
        AlgoFaceIn.pData = RgbImage;
		AlgoFaceIn.pDataPhy = u64PhyAddr;

        /* 4.face detect */
#if defined(DBG_JCY_FACEPLAN) || defined(DBG_JCY_PLATERECO)
		ALG_IMAGE_S AlgoSrcFaceIn;
		AlgoSrcFaceIn.s32H = ALG_SRC_IMAGE_H;
	    AlgoSrcFaceIn.s32W = ALG_SRC_IMAGE_W;
        AlgoSrcFaceIn.s32C = 1;
		//memcpy((void*)(uintptr_t)g_algo_srcbuffer.stVFrame.u64VirAddr[0], RgbsrcImage, algo_srcrgb_size);
        AlgoSrcFaceIn.pData = (void*)(uintptr_t)g_algo_srcbuffer.stVFrame.u64VirAddr[0];
		AlgoSrcFaceIn.pDataPhy = g_algo_srcbuffer.stVFrame.u64PhyAddr[0];
#endif

		if (TS_TRUE == g_algo_save_enable){
			SavePicData2File(AlgoFaceIn.pData, (ALG_DETECT_IMAGE_H + 24) * ALG_DETECT_IMAGE_W * ALG_RGB_CHN);
		}

		if (!(count%1000)) {
			gettimeofday(&TimeoutVal,NULL);
			runtime = TimeoutVal.tv_sec * 1000;
			SAMPLE_PRT("algoType=%d,time:%ld\n", g_algo_type_mask, runtime);
		}
		
#if defined(DBG_JCY_MOTION)
		if (g_algo_type_mask & SAMPLE_ALGO_MASK_MOTION) {
			pthread_mutex_lock(&g_algoprocess_lock);
	        s32Ret = TS_ALGO_MotionDetectProcess(pAlgoHandle, &AlgoFaceIn, &g_algo_result.g_algo_motion_result);
	        if(TS_SUCCESS != s32Ret) {
	            SAMPLE_PRT("TS_ALGO_FaceDetectProcess error\n");
				pthread_mutex_unlock(&g_algoprocess_lock);
	            goto exit;
	        }
			pthread_mutex_unlock(&g_algoprocess_lock);
		}
#endif

#if defined(DBG_JCY_FACEPLAN)
		if (g_algo_type_mask & SAMPLE_ALGO_MASK_FACEPLAN) {
			pthread_mutex_lock(&g_algoprocess_lock);
			s32Ret = TS_ALGO_FaceplanDetectProcess(pAlgoHandle, &AlgoSrcFaceIn, &AlgoFaceIn, &g_algo_result.g_algo_faceplan_result);
	        if(TS_SUCCESS != s32Ret) {
	            SAMPLE_PRT("TS_ALGO_FaceplanDetectProcess error\n");
				pthread_mutex_unlock(&g_algoprocess_lock);
	            goto exit;
	        }
			pthread_mutex_unlock(&g_algoprocess_lock);
			//printf("detect faceplan number:%d,facenum:%d\n", g_algo_result.g_algo_faceplan_result.u32Num, g_algo_result.g_algo_faceplan_result.stDetect.u32FaceNum);
//			for(TS_U32 i = 0; i < g_algo_obj_result.u32ObjNum; i++) {
//				printf("[%d] index:%d, score:%f xmin:%f ymin:%f xmax:%f ymax:%f\n", i, g_algo_obj_result.u8ObjNo[i], g_algo_obj_result.stBox[i].f32Score,
//				g_algo_obj_result.stBox[i].f32Xmin, g_algo_obj_result.stBox[i].f32Ymin, g_algo_obj_result.stBox[i].f32Xmax, g_algo_obj_result.stBox[i].f32Ymax);
//			}
		}
#endif

#if defined(DBG_JCY_BODYTRACK)
		if (g_algo_type_mask & SAMPLE_ALGO_MASK_BODYTRACK) {
			pthread_mutex_lock(&g_algoprocess_lock);
			s32Ret = TS_ALGO_MultiBodyTrackProcess(pAlgoHandle, &AlgoFaceIn, &g_algo_result.g_algo_bodytrack_result);
	        if(TS_SUCCESS != s32Ret) {
	            SAMPLE_PRT("TS_ALGO_MultiBodyTrackProcess error\n");
				pthread_mutex_unlock(&g_algoprocess_lock);
	            goto exit;
	        }
			//printf("detect bodyTrack number:%d\n", g_algo_result.g_algo_bodytrack_result.u32Num);
			pthread_mutex_unlock(&g_algoprocess_lock);
//			for(TS_U32 i = 0; i < g_algo_bodyTrack_result.u32Num; i++) {
//				printf("%d, %f, %f, %f, %f\n", g_algo_bodyTrack_result.pTrack[i].u32TagId, g_algo_bodyTrack_result.pTrack[i].f32Box[0],
//				g_algo_bodyTrack_result.pTrack[i].f32Box[1], g_algo_bodyTrack_result.pTrack[i].f32Box[2], g_algo_bodyTrack_result.pTrack[i].f32Box[3]);
//			}
		}
#endif


#if defined(DBG_JCY_PLATERECO)
		if (g_algo_type_mask & SAMPLE_ALGO_MASK_PLATERECO) {
			pthread_mutex_lock(&g_algoprocess_lock);
			s32Ret = TS_ALGO_PlateRecoProcess(pAlgoHandle, &AlgoSrcFaceIn, &AlgoFaceIn, &g_algo_result.g_algo_platereco_result);
			if(TS_SUCCESS != s32Ret) {
				SAMPLE_PRT("TS_ALGO_PlateRecoProcess error\n");
				pthread_mutex_unlock(&g_algoprocess_lock);
				goto exit;
			}
			pthread_mutex_unlock(&g_algoprocess_lock);
		}
#endif

#if defined(DBG_JCY_FACE)
		if (g_algo_type_mask & SAMPLE_ALGO_MASK_FACE) {
			pthread_mutex_lock(&g_algoprocess_lock);
			s32Ret = TS_ALGO_FaceDetectProcess(pAlgoHandle, &AlgoFaceIn, &g_algo_result.g_algo_face_result);
			if(TS_SUCCESS != s32Ret) {
				SAMPLE_PRT("TS_ALGO_FaceDetectProcess error\n");
				pthread_mutex_unlock(&g_algoprocess_lock);
				goto exit;
			}
			pthread_mutex_unlock(&g_algoprocess_lock);
			/*printf("detect face number:%d\n", g_algo_result.g_algo_face_result.u32FaceNum);
			for(TS_U32 i = 0; i < g_algo_result.g_algo_face_result.u32FaceNum; i++) {
				printf("[%d] score:%f xmin:%f ymin:%f xmax:%f ymax:%f\n", i, g_algo_result.g_algo_face_result.stBox[i].f32Score, g_algo_result.g_algo_face_result.stBox[i].f32Xmin, 
					g_algo_result.g_algo_face_result.stBox[i].f32Ymin, g_algo_result.g_algo_face_result.stBox[i].f32Xmax, g_algo_result.g_algo_face_result.stBox[i].f32Ymax);
			}*/
		}
#endif

#if defined(DBG_JCY_MUTITARGET)
		if (g_algo_type_mask & SAMPLE_ALGO_MASK_MUTITARGET) {
			pthread_mutex_lock(&g_algoprocess_lock);
			s32Ret = TS_ALGO_MutiDetectProcess(pAlgoHandle, &AlgoFaceIn, &g_algo_result.g_algo_mutitarget_result);
			if(TS_SUCCESS != s32Ret) {
				SAMPLE_PRT("TS_ALGO_MutiDetectProcess error\n");
				pthread_mutex_unlock(&g_algoprocess_lock);
				goto exit;
			}
			//printf("detect mask result:%d\n", g_algo_mask_result.u32mask);
			pthread_mutex_unlock(&g_algoprocess_lock);
		}
#endif

#if defined(DBG_JCY_PLATE)
		if (g_algo_type_mask & SAMPLE_ALGO_MASK_PLATE) {
			pthread_mutex_lock(&g_algoprocess_lock);
			s32Ret = TS_ALGO_PlateDetectProcess(pAlgoHandle, &AlgoFaceIn, &g_algo_result.g_algo_plate_result);
			if(TS_SUCCESS != s32Ret) {
				SAMPLE_PRT("TS_ALGO_PlateDetectProcess error\n");
				pthread_mutex_unlock(&g_algoprocess_lock);
				goto exit;
			}
			pthread_mutex_unlock(&g_algoprocess_lock);
			/*printf("detect plate number:%d\n", g_algo_result.g_algo_plate_result.u32LicensePlateDetNum);
			for(TS_U32 i = 0; i < g_algo_result.g_algo_plate_result.u32LicensePlateDetNum; i++) {
				printf("[%d] score:%f xmin:%f ymin:%f xmax:%f ymax:%f\n", i, g_algo_result.g_algo_plate_result.stBox[i].f32Score, g_algo_result.g_algo_plate_result.stBox[i].f32Xmin, 
					g_algo_result.g_algo_plate_result.stBox[i].f32Ymin, g_algo_result.g_algo_plate_result.stBox[i].f32Xmax, g_algo_result.g_algo_plate_result.stBox[i].f32Ymax);
			}*/
		}
#endif

algo_next_loop1:
    	usleep(1000 * 100);
		count++;
	}

exit:
    TS_MPI_VB_MunmapPool(vbPool);
exit1:
	TS_MPI_VB_ReleaseBlock(vbHandle);
exit2:
	TS_MPI_VB_DestroyPool(vbPool);
    return TS_NULL;
}

TS_S32 SAMPLE_COMM_ALGO_FACE_StartProcess(void)
{
    return pthread_create(&gs_AlgoProcPid, 0, SAMPLE_COMM_ALGO_FACE_Process, TS_NULL);
}

TS_S32 SAMPLE_COMM_ALGO_FACE_StopProcess(void)
{
    if (TS_TRUE == g_algo_process_run)
    {
        g_algo_process_run = TS_FALSE;
        pthread_join(gs_AlgoProcPid, 0);
    }
    return TS_SUCCESS;
}

TS_S32 SaveFaceResult2File(char *file_path, ALG_MOTION_DETECT_RESULT_S *face_result, char *pic_name)
{
	FILE *fp = NULL;
	CHECK_NULL_PTR(file_path);
    CHECK_NULL_PTR(face_result);
	CHECK_NULL_PTR(pic_name);

	fp = fopen(file_path, "a+");
	if (TS_NULL == fp)
	{
        SAMPLE_PRT("Save result input path erro \n");
		return TS_FAILURE;
	}

	fprintf(fp, "---\npic:[%s]\nfaceNum:[%d]\n", pic_name, face_result->u32ObjNum);

	for(int i = 0; i < face_result->u32ObjNum; i++)
	{
		fprintf(fp, "[id:%d][f32Score=%f][f32Xmin=%f,f32Ymin=%f,f32Xmax=%f,f32Ymax=%f]\n\n",
				i, face_result->stBox[i].f32Score, face_result->stBox[i].f32Xmin, face_result->stBox[i].f32Ymin,
				face_result->stBox[i].f32Xmax, face_result->stBox[i].f32Ymax);
	}

	fclose(fp);

    return TS_SUCCESS;
}

#if 1
TS_S32 SavePicData2File(void * buf, int len)
{
	static char dirPath[64] = {0};
	static int indexFile = 0;
	char filePath[128] = {0};
	DIR *dirp = TS_NULL;
	FILE *fp = NULL;
	int fileSize = 0;

	CHECK_NULL_PTR(buf);

	if (0 == strlen(dirPath)) {
		fp = fopen(ALGO_PIC_SAVE_CONFIG_PATH, "r");
		if (TS_NULL == fp) {
	        SAMPLE_PRT("open %s failed\n", ALGO_PIC_SAVE_CONFIG_PATH);
			return TS_FAILURE;
		}

		fseek(fp, 0L, SEEK_END);
		fileSize = ftell(fp);
		fseek(fp, 0L, SEEK_SET);

		if (fileSize >= sizeof(dirPath) - 1) {
			SAMPLE_PRT("%s is too long\n", ALGO_PIC_SAVE_CONFIG_PATH);
			return TS_FAILURE;
		}

		fread(dirPath, 1, fileSize, fp);
		dirPath[fileSize-1] = '\0';
		fclose(fp);
	}

	dirp = opendir(dirPath);
    if (TS_NULL == dirp ) {
        SAMPLE_PRT("Input save path erro. \n");
        return TS_FAILURE;
    }
    closedir(dirp);

    snprintf(filePath, sizeof(filePath)-1, "%s/%d.img", dirPath, indexFile);
    save2file_render(buf, len, filePath);
	indexFile++;

    return TS_SUCCESS;
}
#else
TS_S32 SavePicData2File(char *file_path, VIDEO_FRAME_INFO_S * pic_info, char *picName)
{

    static int indexFile = 0;
    char fileName[64] = {0};
    DIR *dirp = TS_NULL;

    CHECK_NULL_PTR(file_path);
    CHECK_NULL_PTR(pic_info);
    indexFile++;
    dirp = opendir(file_path);
    if (TS_NULL == dirp ) {
        SAMPLE_PRT("Input save path erro. \n");
        return TS_FAILURE;
    }
    closedir(dirp);

    SAMPLE_PRT("Save pic ID = %d, u32Width = %d,u32Height = %d, addr = %llx\n",indexFile, pic_info->stVFrame.u32Width, pic_info->stVFrame.u32Height, pic_info->stVFrame.u64VirAddr[0]);

    sprintf(picName, "Img_%d_%d_%d.raw",pic_info->stVFrame.u32Width, pic_info->stVFrame.u32Height, indexFile);
    sprintf(fileName, "%s/%s",file_path, picName);
    save2file_render(pic_info->stVFrame.u64VirAddr[0], pic_info->stVFrame.u32Width * pic_info->stVFrame.u32Height * 3 / 2, fileName);

    return TS_SUCCESS;
}
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
