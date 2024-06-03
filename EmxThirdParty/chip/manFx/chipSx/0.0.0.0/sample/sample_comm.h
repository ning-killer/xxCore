

#ifndef __SAMPLE_COMM_H__
#define __SAMPLE_COMM_H__

#include <pthread.h>

#include "ts_common.h"
#include "ts_buffer.h"
#include "ts_comm_sys.h"
#include "ts_comm_vb.h"
#include "ts_comm_isp.h"
#include "ts_comm_vi.h"
#include "ts_comm_vo.h"
#include "ts_comm_venc.h"
#include "ts_comm_vdec.h"
#include "ts_comm_vpss.h"
#include "ts_comm_region.h"
#include "ts_comm_vgs.h"
#include "ts_comm_cpm.h"
#include "ts_comm_aio.h"
#include "ts_comm_adec.h"
#include "ts_comm_aenc.h"
#include "ts_comm_ai.h"
#include "ts_comm_ao.h"
#include "ts_defines.h"
#include "ts_comm_hdmi.h"
#include "ts_mipi.h"
//#include "ts_comm_vgs.h"
#include "../algstore/libalgmotiondetect/include/ts_alg_motion_detect.h"
#include "../algstore/libalglicenseplatedet_v2.3/include/ts_alg_licenseplate_det.h"
#include "../algstore/libalgfacedetect_v1_7/include/ts_alg_face_detect.h"
#include "../algstore/libalgfacerecosolution/include/ts_alg_facereco_solution.h"
#include "../algstore/libalgbytetrack_v1_0/include/ts_alg_byte_track.h"
#include "../algstore/libalgcarsolution_v1.0/include/ts_alg_car_solution.h"
#include "../algstore/libalgpbcedetect_v1_1/include/ts_alg_pbce_detect.h"

#include "mpi_sys.h"
#include "mpi_vb.h"
#include "mpi_vi.h"
#include "mpi_vo.h"
#include "mpi_venc.h"
#include "mpi_vdec.h"
#include "mpi_vpss.h"
#include "mpi_region.h"
#include "mpi_audio.h"
#include "mpi_isp.h"
#include "mpi_ae.h"
#include "mpi_awb.h"
#include "ts_math.h"
//#include "ts_sns_ctrl.h"
#include "mpi_hdmi.h"
//#include "mpi_vgs.h"
#include "mpi_cpm.h"

/*******************************************************
 *    macro define
 *******************************************************/
#define FILE_NAME_LEN               128

#define CHECK_CHN_RET(express, Chn, name)\
	do {\
		TS_S32 Ret;\
		Ret = express;\
		if (TS_SUCCESS != Ret) {\
			printf("\033[0;31m%s chn %d failed at %s: LINE: %d with %#x!\033[0;39m\n", \
				name, Chn, __func__, __LINE__, Ret);\
			fflush(stdout);\
			return Ret;\
		} \
	} while (0)

#define CHECK_RET(express, name)\
	do {\
		TS_S32 Ret;\
		Ret = express;\
		if (TS_SUCCESS != Ret) {\
			printf("\033[0;31m%s failed at %s: LINE: %d with %#x!\033[0;39m\n",\
				name, __func__, __LINE__, Ret);\
			return Ret;\
		} \
	} while (0)

#define SAMPLE_PIXEL_FORMAT         PIXEL_FORMAT_YVU_SEMIPLANAR_420

#define TLV320_FILE "/dev/tlv320aic31"
#define COLOR_RGB_RED      0xFF0000
#define COLOR_RGB_GREEN    0x00FF00
#define COLOR_RGB_BLUE     0x0000FF
#define COLOR_RGB_BLACK    0x000000
#define COLOR_RGB_YELLOW   0xFFFF00
#define COLOR_RGB_CYN      0x00ffff
#define COLOR_RGB_WHITE    0xffffff

#define SAMPLE_VO_DEV_DHD0 0                  /* VO's device HD0 */
#define SAMPLE_VO_DEV_DHD1 1                  /* VO's device HD1 */
#define SAMPLE_VO_DEV_UHD  SAMPLE_VO_DEV_DHD0 /* VO's ultra HD device:HD0 */
#define SAMPLE_VO_DEV_HD   SAMPLE_VO_DEV_DHD1 /* VO's HD device:HD1 */
#define SAMPLE_VO_LAYER_VHD0 0
#define SAMPLE_VO_LAYER_VHD1 1
#define SAMPLE_VO_LAYER_VHD2 2
#define SAMPLE_VO_LAYER_PIP  SAMPLE_VO_LAYER_VHD2

#define SAMPLE_AUDIO_EXTERN_AI_DEV 0
#define SAMPLE_AUDIO_EXTERN_AO_DEV 0
#define SAMPLE_AUDIO_INNER_AI_DEV 0
#define SAMPLE_AUDIO_INNER_AO_DEV 0
#define SAMPLE_AUDIO_INNER_HDMI_AO_DEV 1

#define SAMPLE_AUDIO_PTNUMPERFRM   480

#define WDR_MAX_PIPE_NUM        4


#define PAUSE()  do {\
		printf("---------------press Enter key to exit!---------------\n");\
		getchar();\
	} while (0)


#define SAMPLE_PRT(fmt...)   \
	do {\
		printf("[%s]-%d: ", __func__, __LINE__);\
		printf(fmt);\
	} while (0)

#define CHECK_NULL_PTR(ptr)\
	do {\
		if (NULL == ptr) {\
			printf("func:%s,line:%d, NULL pointer\n", __func__, __LINE__);\
			return TS_FAILURE;\
		} \
	} while (0)

typedef TS_S32  (*drawprocess)(TS_VOID **in, TS_VOID **out);

/*******************************************************
 *   enum define
 *******************************************************/

typedef enum hiPIC_SIZE_E {
	PIC_CIF,
	PIC_D1_PAL,    /* 720 * 576 */
	PIC_D1_NTSC,   /* 720 * 480 */
	PIC_360P,	   /* 640 * 360  */
	PIC_720P,      /* 1280 * 720  */
	PIC_1080P,     /* 1920 * 1080 */
	PIC_2304x1296,
	PIC_2560x1440,
	PIC_2592x1520,
	PIC_2592x1536,
	PIC_2592x1944,
    PIC_2688x1860,
	PIC_2716x1524,
	PIC_3840x2160,
	PIC_4096x2160,
	PIC_3000x3000,
	PIC_4000x3000,
	PIC_7680x4320,
	PIC_3840x8640,
	PIC_BUTT
} PIC_SIZE_E;

typedef enum hiSAMPLE_SNS_TYPE_E {
	SONY_IMX327_MIPI_2M_30FPS_12BIT,
	SONY_IMX327_MIPI_2M_30FPS_12BIT_WDR2TO1,
	SONY_IMX327_2L_MIPI_2M_30FPS_12BIT,
	SONY_IMX327_2L_MIPI_2M_30FPS_12BIT_WDR2TO1,
	SONY_IMX307_MIPI_2M_30FPS_12BIT,
	SONY_IMX307_MIPI_2M_30FPS_12BIT_WDR2TO1,
	SONY_IMX335_MIPI_5M_30FPS_12BIT,
	SONY_IMX335_MIPI_5M_30FPS_10BIT_WDR2TO1,
	SONY_IMX335_MIPI_4M_30FPS_12BIT,
	SONY_IMX335_MIPI_4M_30FPS_10BIT_WDR2TO1,
	SONY_IMX458_MIPI_8M_30FPS_10BIT,
	SONY_IMX458_MIPI_12M_20FPS_10BIT,
	SONY_IMX458_MIPI_4M_60FPS_10BIT,
	SONY_IMX458_MIPI_4M_40FPS_10BIT,
	SONY_IMX458_MIPI_2M_90FPS_10BIT,
	SONY_IMX458_MIPI_1M_129FPS_10BIT,
	PANASONIC_MN34220_LVDS_2M_30FPS_12BIT,
	HAPS_ARMISP,
	EVB_SENSOR,//1080P 30fps NV12
	EVB_SENSOR_4K,//4K 30fps NV12
	EVB_SENSOR_4MP,
	EVB_SENSOR_5MP,//2529*1944 30fps NV12
	X86_LOGITECH_HD1080,
	X86_LOGITECH_HD720,
	SAMPLE_SNS_TYPE_BUTT,
} SAMPLE_SNS_TYPE_E;


typedef enum hiSAMPLE_VO_MODE_E {
	VO_MODE_1MUX,
	VO_MODE_2MUX,
	VO_MODE_3MUX,
	VO_MODE_4MUX,
	VO_MODE_8MUX,
	VO_MODE_9MUX,
	VO_MODE_16MUX,
	VO_MODE_25MUX,
	VO_MODE_36MUX,
	VO_MODE_49MUX,
	VO_MODE_64MUX,
	VO_MODE_2X4,
	VO_MODE_BUTT
} SAMPLE_VO_MODE_E;

typedef enum hiSAMPLE_RC_E {
	SAMPLE_RC_CBR = 0,
	SAMPLE_RC_VBR,
	SAMPLE_RC_AVBR,
	SAMPLE_RC_CVBR,
	SAMPLE_RC_QVBR,
	SAMPLE_RC_QPMAP,
	SAMPLE_RC_FIXQP
} SAMPLE_RC_E;

enum SAMPLE_VENC_CONSUME_TYPE {
	SAMPLE_VENC_FILE = 0,
	SAMPLE_VENC_RTSP,
	SAMPLE_VENC_BOTH
};

#if 0
typedef enum SAMPLE_ALGO_TYPE {
	SAMPLE_ALGO_MOTION = 0,
	SAMPLE_ALGO_BODY,
	SAMPLE_ALGO_BOTH,
	SAMPLE_ALGO_OBJ,
	SAMPLE_ALGO_BODYTRACK,
	SAMPLE_ALGO_PCPP,
	SAMPLE_ALGO_NORMAL
}SAMPLE_ALGO_TYPE_E;
#else
typedef enum SAMPLE_ALGO_TYPE {
	SAMPLE_ALGO_MASK_MOTION 		= 0x1,
	SAMPLE_ALGO_MASK_MUTITARGET 	= 0x2,
	SAMPLE_ALGO_MASK_FACEPLAN 		= 0x4,
	SAMPLE_ALGO_MASK_FACE 		    = 0x8,
	SAMPLE_ALGO_MASK_BODYTRACK		= 0x10,
	SAMPLE_ALGO_MASK_PLATERECO 		= 0x20,
	SAMPLE_ALGO_MASK_PLATE		    = 0x40, 
	SAMPLE_ALGO_MASK_BIKE 	        = 0x80, 
	SAMPLE_ALGO_MASK_NORMAL
}SAMPLE_ALGO_TYPE_E;
#endif

/*******************************************************
 *   structure define
 *******************************************************/
typedef struct stALG_RESULT
{
	ALG_MOTION_DETECT_RESULT_S g_algo_motion_result;
	ALG_FACE_SOLUTION_RESULT_S g_algo_faceplan_result;
	ALG_FACE_DETECT_RESULT_S g_algo_face_result;
	ALG_BYTE_TRACK_RESULTS_S g_algo_bodytrack_result;
	ALG_LICENSE_PLATE_DET_RESULT_S g_algo_plate_result;
	ALG_CAR_SOLUTION_RESULT_S g_algo_platereco_result;
	ALG_PBCE_DETECT_RESULT_S g_algo_mutitarget_result;
} ALG_RESULT;

typedef struct tsSAMPLE_VENC_GETSTREAM_PARA_S {
	TS_BOOL bThreadStart;
	VENC_CHN VeChn[VENC_MAX_CHN_NUM];
	TS_S32  s32Cnt;
	enum SAMPLE_VENC_CONSUME_TYPE consume_type;
} SAMPLE_VENC_GETSTREAM_PARA_S;

typedef struct tsmSAMPLE_VENC_CHN_CONFIG_S
{
	PAYLOAD_TYPE_E			enType;
	SIZE_S					stSize;
	SAMPLE_RC_E				enRcMode;
	TS_U32					fixQp;
	TS_U32					u32Gop;
	TS_U32					u32FrameRate;
	TS_U32					u32BitRate;
	TS_U32					u32Profile;
	TS_BOOL					bRcnRefShareBuf;//borrowing for pixel format
	VENC_GOP_ATTR_S			stGopAttr;
} SAMPLE_VENC_CHN_CONFIG_S;

typedef struct tsSAMPLE_VENC_QPMAP_SENDFRAME_PARA_S {
	TS_BOOL  bThreadStart;
	VPSS_GRP VpssGrp;
	VPSS_CHN VpssChn;
	VENC_CHN VeChn[VENC_MAX_CHN_NUM];
	TS_S32   s32Cnt;
	SIZE_S   stSize;
} SAMPLE_VENC_QPMAP_SENDFRAME_PARA_S;


typedef struct tsSAMPLE_VI_DUMP_THREAD_INFO_S {
	VI_PIPE     ViPipe;
	TS_S32      s32Cnt;
	TS_BOOL     bDump;
	TS_CHAR     aszName[128];
	pthread_t   ThreadId;
} SAMPLE_VI_DUMP_THREAD_INFO_S;

typedef struct tsSAMPLE_SENSOR_INFO_S {
	SAMPLE_SNS_TYPE_E   enSnsType;
	TS_S32              s32SnsId;
	TS_S32              s32BusId;
	combo_dev_t           MipiDev;
} SAMPLE_SENSOR_INFO_S;

typedef struct tsSAMPLE_SNAP_INFO_S {
	TS_BOOL  bSnap;
	TS_BOOL  bDoublePipe;
	VI_PIPE    VideoPipe;
	VI_PIPE    SnapPipe;
	VI_VPSS_MODE_E  enVideoPipeMode;
	VI_VPSS_MODE_E  enSnapPipeMode;
} SAMPLE_SNAP_INFO_S;

typedef struct tsSAMPLE_DEV_INFO_S {
	VI_DEV      ViDev;
	WDR_MODE_E  enWDRMode;
} SAMPLE_DEV_INFO_S;

typedef struct tsSAMPLE_PIPE_INFO_S {
	VI_PIPE         aPipe[WDR_MAX_PIPE_NUM];
	VI_VPSS_MODE_E  enMastPipeMode;
	TS_BOOL         bMultiPipe;
	TS_BOOL         bVcNumCfged;
	TS_BOOL         bIspBypass;
	PIXEL_FORMAT_E  enPixFmt;
	TS_U32          u32VCNum[WDR_MAX_PIPE_NUM];
} SAMPLE_PIPE_INFO_S;

typedef struct tsSAMPLE_CHN_INFO_S {
	VI_CHN              ViChn[VI_MAX_CHN_NUM];
	PIXEL_FORMAT_E      enPixFormat;
	DYNAMIC_RANGE_E     enDynamicRange;
	VIDEO_FORMAT_E      enVideoFormat;
	COMPRESS_MODE_E     enCompressMode;
} SAMPLE_CHN_INFO_S;

typedef struct tsSAMPLE_VI_INFO_S {
	SAMPLE_SENSOR_INFO_S    stSnsInfo;
	SAMPLE_DEV_INFO_S       stDevInfo;
	SAMPLE_PIPE_INFO_S      stPipeInfo;
	SAMPLE_CHN_INFO_S       stChnInfo;
	SAMPLE_SNAP_INFO_S      stSnapInfo;
} SAMPLE_VI_INFO_S;

typedef struct tsSAMPLE_VI_CONFIG_S {
	SAMPLE_VI_INFO_S    astViInfo[VI_MAX_DEV_NUM];
	TS_S32              as32WorkingViId[VI_MAX_DEV_NUM];
	TS_S32              s32WorkingViNum;
} SAMPLE_VI_CONFIG_S;

typedef struct tsSAMPLE_VI_FRAME_CONFIG_S {
	TS_U32                  u32Width;
	TS_U32                  u32Height;
	TS_U32                  u32ByteAlign;
	PIXEL_FORMAT_E          enPixelFormat;
	VIDEO_FORMAT_E          enVideoFormat;
	COMPRESS_MODE_E         enCompressMode;
	DYNAMIC_RANGE_E         enDynamicRange;
} SAMPLE_VI_FRAME_CONFIG_S;

typedef struct tsSAMPLE_VI_FRAME_INFO_S {
	VB_BLK             VbBlk;
	TS_U32             u32Size;
	VIDEO_FRAME_INFO_S stVideoFrameInfo;
} SAMPLE_VI_FRAME_INFO_S;

typedef struct tsSAMPLE_VI_FPN_CALIBRATE_INFO_S {
	TS_U32                  u32Threshold;
	TS_U32                  u32FrameNum;
	ISP_FPN_TYPE_E          enFpnType;
	PIXEL_FORMAT_E          enPixelFormat;
	COMPRESS_MODE_E         enCompressMode;
} SAMPLE_VI_FPN_CALIBRATE_INFO_S;

typedef struct tsSAMPLE_VI_FPN_CORRECTION_INFO_S {
	ISP_OP_TYPE_E           enOpType;
	ISP_FPN_TYPE_E          enFpnType;
	TS_U32                  u32Strength;
	PIXEL_FORMAT_E          enPixelFormat;
	COMPRESS_MODE_E         enCompressMode;
	SAMPLE_VI_FRAME_INFO_S  stViFrameInfo;
} SAMPLE_VI_FPN_CORRECTION_INFO_S;

typedef struct tag_SAMPLE_VO_WBC_CONFIG {
	//VO_WBC_SOURCE_TYPE_E    enSourceType;
	DYNAMIC_RANGE_E         enDynamicRange;
	COMPRESS_MODE_E         enCompressMode;
	TS_S32 s32Depth;

	TS_S32                  VoWbc;
	//VO_WBC_ATTR_S           stWbcAttr;
	//VO_WBC_SOURCE_S         stWbcSource;
	//VO_WBC_MODE_E           enWbcMode;

} SAMPLE_VO_WBC_CONFIG;

typedef struct tsSAMPLE_COMM_VO_LAYER_CONFIG_S {
	/* for layer */
	VO_LAYER                VoLayer;
	VO_INTF_SYNC_E          enIntfSync;
	RECT_S                  stDispRect;
	SIZE_S                  stImageSize;
	PIXEL_FORMAT_E          enPixFormat;

	TS_U32                  u32DisBufLen;
	DYNAMIC_RANGE_E         enDstDynamicRange;

	/* for chn */
	SAMPLE_VO_MODE_E        enVoMode;
} SAMPLE_COMM_VO_LAYER_CONFIG_S;

typedef struct tsSAMPLE_VO_CONFIG_S {
	/* for device */
	VO_DEV                  VoDev;
	VO_INTF_TYPE_E          enVoIntfType;
	VO_INTF_SYNC_E          enIntfSync;
	PIC_SIZE_E              enPicSize;
	TS_U32                  u32BgColor;

	/* for layer */
	PIXEL_FORMAT_E          enPixFormat;
	RECT_S                  stDispRect;
	SIZE_S                  stImageSize;
	VO_PART_MODE_E          enVoPartMode;

	TS_U32                  u32DisBufLen;
	DYNAMIC_RANGE_E         enDstDynamicRange;

	/* for chnnel */
	SAMPLE_VO_MODE_E        enVoMode;
} SAMPLE_VO_CONFIG_S;


typedef enum tsTHREAD_CONTRL_E {
	THREAD_CTRL_START,
	THREAD_CTRL_PAUSE,
	THREAD_CTRL_STOP,
} THREAD_CONTRL_E;

typedef struct tsVDEC_THREAD_PARAM_S {
	TS_S32 s32ChnId;
	PAYLOAD_TYPE_E enType;
	TS_CHAR cFilePath[128];
	TS_CHAR cFileName[128];
	TS_CHAR cUrl[256];
	TS_CHAR cProgName[32];
	TS_S32 s32StreamMode;
	TS_S32 s32MilliSec;
	TS_S32 s32MinBufSize;
	TS_S32 s32IntervalTime;
	THREAD_CONTRL_E eThreadCtrl;
	TS_U64  u64PtsInit;
	TS_U64  u64PtsIncrease;
	TS_BOOL bCircleSend;
	TS_BOOL bUsedBuffer;
	TS_S32  s32RecvTotalSize;
} VDEC_THREAD_PARAM_S;

typedef struct tsSAMPLE_VDEC_BUF {
	TS_U32  u32PicBufSize;
	TS_U32  u32TmvBufSize;
	TS_BOOL bPicBufAlloc;
	TS_BOOL bTmvBufAlloc;
} SAMPLE_VDEC_BUF;


typedef struct tsSAMPLE_VDEC_VIDEO_ATTR {
	VIDEO_DEC_MODE_E enDecMode;
	TS_U32              u32RefFrameNum;
	DATA_BITWIDTH_E  enBitWidth;
} SAMPLE_VDEC_VIDEO_ATTR;

typedef struct tsSAMPLE_VDEC_PICTURE_ATTR {
	PIXEL_FORMAT_E enPixelFormat;
	TS_U32         u32Alpha;
} SAMPLE_VDEC_PICTURE_ATTR;


typedef struct tsSAMPLE_VDEC_ATTR {
	PAYLOAD_TYPE_E enType;
	VIDEO_MODE_E   enMode;
	TS_U32 u32Width;
	TS_U32 u32Height;
	TS_U32 u32FrameBufCnt;
	TS_U32 u32DisplayFrameNum;
	union {
		SAMPLE_VDEC_VIDEO_ATTR stSapmleVdecVideo;      /* structure with video ( h265/h264) */
		SAMPLE_VDEC_PICTURE_ATTR stSapmleVdecPicture; /* structure with picture (jpeg/mjpeg )*/
	};
} SAMPLE_VDEC_ATTR;

typedef struct tsSAMPLE_VB_BASE_INFO_S {
	PIXEL_FORMAT_E      enPixelFormat;
	TS_U32              u32Width;
	TS_U32              u32Height;
	TS_U32              u32Align;
	COMPRESS_MODE_E     enCompressMode;
} SAMPLE_VB_BASE_INFO_S;

typedef struct tsSAMPLE_VB_CAL_CONFIG_S {
	TS_U32 u32VBSize;

	TS_U32 u32HeadStride;
	TS_U32 u32HeadSize;
	TS_U32 u32HeadYSize;

	TS_U32 u32MainStride;
	TS_U32 u32MainSize;
	TS_U32 u32MainYSize;

	TS_U32 u32ExtStride;
	TS_U32 u32ExtYSize;
} SAMPLE_VB_CAL_CONFIG_S;

//typedef struct tsALGO_BODY_DETECT_INIT_S{
//    TS_U8   *graph;                       // mode graph address
//    TS_U8   *weight;                      // mode weight address
//    TS_U32   imageType;                   // input image type (RGBA, gray, yuv ...)
//} ALGO_BODY_DETECT_INIT_S;

//typedef struct tsALGO_BODY_BOX_S{
//    // the range of x and y is 0.0f~1.0f
//    TS_FLOAT xmin;
//    TS_FLOAT ymin;
//    TS_FLOAT xmax;
//    TS_FLOAT ymax;
//    TS_FLOAT score;
//} ALGO_BODY_BOX_S;

//typedef struct tsALGO_FACEBOX_S{
//    // the range of x and y is 0.0f~1.0f
//    TS_FLOAT xmin;
//    TS_FLOAT ymin;
//    TS_FLOAT xmax;
//    TS_FLOAT ymax;
//    TS_FLOAT score;
//} ALGO_FACEBOX_S;

//typedef struct tsALGO_IMAGE_S{
//    TS_U32    c;
//    TS_U32    h;
//    TS_U32    w;
//    TS_U32   imageType;                   // input image type (RGBA, gray, yuv ...)
//    TS_U8   *data;
//} ALGO_IMAGE_S;

//typedef struct tsALGO_FACE_IN_S{
//    ALGO_IMAGE_S image;
//    TS_FLOAT thres;
//} ALGO_FACE_IN_S;

typedef struct tsCPM_ADAP_S {
    CPM_GRP_ATTR_S  pCPMAdapGrpAttr;
    CPM_Handle_S    pCPMAdapHandle;
    TS_U32         CPMAdapChnNum;
    CPM_CHN_ATTR_S  CPMAdapChnAttr;
}CPM_ADAP_S;

typedef struct tsCPM_CHN_GETRESULT_PARA_S {
	CPM_CHN CpmChn;
	TS_VOID *data;
} CPM_CHN_GETRESULT_PARA_S;

typedef struct tsSAMPLE_ALGO_GETRESULT_PARA_S {
	TS_BOOL bThreadStart;
	TS_S32  s32Cnt;
	CPM_CHN_GETRESULT_PARA_S stAlgoResult[CPM_MAX_CHN_NUM];
} SAMPLE_ALGO_GETRESULT_PARA_S;

typedef struct tsVPSS_ROTATION_INFO_S
{
	TS_BOOL    bEnable;
	ROTATION_E stVpssChnRotation;
}VPSS_ROTATION_INFO_S;


typedef struct tsSAMPLE_VPSS_EXTEND_ATTR_S
{
    VPSS_CROP_INFO_S stVpssGrpCrop;
    VPSS_CROP_INFO_S stVpssChnCrop;
    VPSS_ROTATION_INFO_S stVpssChnRotation;
}SAMPLE_VPSS_EXTEND_ATTR_S;

/*******************************************************
 *   function announce
 *******************************************************/

TS_VOID *SAMPLE_SYS_IOMmap(TS_U64 u64PhyAddr, TS_U32 u32Size);
TS_S32 SAMPLE_SYS_Munmap(TS_VOID *pVirAddr, TS_U32 u32Size);
TS_S32 SAMPLE_SYS_SetReg(TS_U64 u64Addr, TS_U32 u32Value);
TS_S32 SAMPLE_SYS_GetReg(TS_U64 u64Addr, TS_U32 *pu32Value);

TS_S32 SAMPLE_COMM_SYS_GetPicSize(PIC_SIZE_E enPicSize, SIZE_S *pstSize);
//TS_S32 SAMPLE_COMM_SYS_MemConfig(TS_VOID);
TS_VOID SAMPLE_COMM_SYS_Exit(void);
TS_S32 SAMPLE_COMM_SYS_Init(VB_CONFIG_S *pstVbConfig);
TS_S32 SAMPLE_COMM_SYS_InitWithVbSupplement(VB_CONFIG_S* pstVbConf, TS_U32 u32SupplementConfig);

TS_S32 SAMPLE_COMM_VB_Save2File(VIDEO_FRAME_INFO_S *pVideoFrame, TS_CHAR * pPath, TS_S32 fileIdx);

TS_S32 SAMPLE_COMM_VI_Bind_VO(VI_PIPE ViPipe, VI_CHN ViChn, VO_LAYER VoLayer, VO_CHN VoChn);
TS_S32 SAMPLE_COMM_VI_UnBind_VO(VI_PIPE ViPipe, VI_CHN ViChn, VO_LAYER VoLayer, VO_CHN VoChn);
TS_S32 SAMPLE_COMM_VI_Bind_VPSS(VI_PIPE ViPipe, VI_CHN ViChn, VPSS_GRP VpssGrp);
TS_S32 SAMPLE_COMM_VI_UnBind_VPSS(VI_PIPE ViPipe, VI_CHN ViChn, VPSS_GRP VpssGrp);
TS_S32 SAMPLE_COMM_VI_Bind_VENC(VI_PIPE ViPipe, VI_CHN ViChn, VENC_CHN VencChn);
TS_S32 SAMPLE_COMM_VI_UnBind_VENC(VI_PIPE ViPipe, VI_CHN ViChn, VENC_CHN VencChn);
TS_S32 SAMPLE_COMM_VPSS_Bind_AVS(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, AVS_GRP AvsGrp, AVS_PIPE AvsPipe);
TS_S32 SAMPLE_COMM_VPSS_UnBind_AVS(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, AVS_GRP AvsGrp, AVS_PIPE AvsPipe);
TS_S32 SAMPLE_COMM_VPSS_Bind_VO(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VO_LAYER VoLayer, VO_CHN VoChn);
TS_S32 SAMPLE_COMM_VPSS_UnBind_VO(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VO_LAYER VoLayer, VO_CHN VoChn);
TS_S32 SAMPLE_COMM_VPSS_Bind_VENC(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VENC_CHN VencChn);
TS_S32 SAMPLE_COMM_VPSS_UnBind_VENC(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VENC_CHN VencChn);
TS_S32 SAMPLE_COMM_AVS_Bind_AVS(AVS_GRP AvsSrcGrp, AVS_CHN AvsSrcChn, AVS_GRP AvsDestGrp, AVS_CHN AvsDestChn);
TS_S32 SAMPLE_COMM_AVS_UnBind_AVS(AVS_GRP AvsSrcGrp, AVS_CHN AvsSrcChn, AVS_GRP AvsDestGrp, AVS_CHN AvsDestChn);
TS_S32 SAMPLE_COMM_AVS_Bind_VPSS(AVS_GRP AvsGrp, AVS_CHN AvsChn, VPSS_GRP VpssGrp);
TS_S32 SAMPLE_COMM_AVS_UnBind_VPSS(AVS_GRP AvsGrp, AVS_CHN AvsChn, VPSS_GRP VpssGrp);
TS_S32 SAMPLE_COMM_AVS_Bind_VENC(AVS_GRP AvsGrp, AVS_CHN AvsChn, VENC_CHN VencChn);
TS_S32 SAMPLE_COMM_AVS_UnBind_VENC(AVS_GRP AvsGrp, AVS_CHN AvsChn, VENC_CHN VencChn);
TS_S32 SAMPLE_COMM_AVS_Bind_VO(AVS_GRP AvsGrp, AVS_CHN AvsChn, VO_LAYER VoLayer, VO_CHN VoChn);
TS_S32 SAMPLE_COMM_AVS_UnBind_VO(AVS_GRP AvsGrp, AVS_CHN AvsChn, VO_LAYER VoLayer, VO_CHN VoChn);
TS_S32 SAMPLE_COMM_VDEC_Bind_VPSS(VDEC_CHN VdecChn, VPSS_GRP VpssGrp);
TS_S32 SAMPLE_COMM_VDEC_UnBind_VPSS(VDEC_CHN VdecChn, VPSS_GRP VpssGrp);
TS_S32 SAMPLE_COMM_VO_Bind_VO(VO_LAYER  SrcVoLayer, VO_CHN SrcVoChn, VO_LAYER DstVoLayer, VO_CHN DstVoChn);
TS_S32 SAMPLE_COMM_VO_UnBind_VO(VO_LAYER DstVoLayer, VO_CHN DstVoChn);

TS_VOID SAMPLE_COMM_ISP_Stop(ISP_DEV IspDev);
TS_VOID SAMPLE_COMM_All_ISP_Stop(void);
TS_S32 SAMPLE_COMM_ISP_Run(ISP_DEV IspDev);
TS_S32 SAMPLE_COMM_ISP_BindSns(ISP_DEV IspDev, TS_U32 u32SnsId, SAMPLE_SNS_TYPE_E enSnsType, TS_S8 s8SnsDev);
TS_S32 SAMPLE_COMM_ISP_Sensor_Regiter_callback(ISP_DEV IspDev, TS_U32 u32SnsId);
TS_S32 SAMPLE_COMM_ISP_Sensor_UnRegiter_callback(ISP_DEV IspDev);
TS_S32 SAMPLE_COMM_ISP_Aelib_Callback(ISP_DEV IspDev);
TS_S32 SAMPLE_COMM_ISP_Aelib_UnCallback(ISP_DEV IspDev);
TS_S32 SAMPLE_COMM_ISP_Awblib_Callback(ISP_DEV IspDev);
TS_S32 SAMPLE_COMM_ISP_Awblib_UnCallback(ISP_DEV IspDev);
TS_S32 SAMPLE_COMM_ISP_GetIspAttrBySns(SAMPLE_SNS_TYPE_E enSnsType, ISP_PUB_ATTR_S *pstPubAttr);
TS_S32 SAMPLE_COMM_ISP_SetParamIndex(VI_PIPE ViPipe, TS_U32 paramSetIndex);
TS_S32 SAMPLE_COMM_ISP_GetParamIndex(VI_PIPE ViPipe, TS_U32 *paramSetIndex);

TS_S32 SAMPLE_COMM_VI_GetWDRModeBySensor(SAMPLE_SNS_TYPE_E enMode, WDR_MODE_E *penWDRMode);
TS_S32 SAMPLE_COMM_VI_GetPipeBySensor(SAMPLE_SNS_TYPE_E enMode, SAMPLE_PIPE_INFO_S *pstPipeInfo);
TS_S32 SAMPLE_COMM_VI_GetSizeBySensor(SAMPLE_SNS_TYPE_E enMode, PIC_SIZE_E *penSize);
TS_S32 SAMPLE_COMM_VI_GetFrameRateBySensor(SAMPLE_SNS_TYPE_E enMode, TS_U32 *pu32FrameRate);
//TS_S32 SAMPLE_COMM_VI_StartDev(SAMPLE_VI_INFO_S *pstViInfo);
TS_S32 SAMPLE_COMM_VI_StartChn(VI_CHN ViChn, RECT_S *pstCapRect, SIZE_S *pstTarSize, SAMPLE_VI_CONFIG_S *pstViConfig);
TS_S32 SAMPLE_COMM_VI_StartMIPI(SAMPLE_VI_CONFIG_S *pstViConfig);
TS_S32 SAMPLE_COMM_VI_StartVi(SAMPLE_VI_CONFIG_S *pstViConfig);
TS_S32 SAMPLE_COMM_VI_StopVi(SAMPLE_VI_CONFIG_S *pstViConfig);
TS_S32 SAMPLE_COMM_VI_SetMipiAttr(SAMPLE_VI_CONFIG_S *pstViConfig);
TS_S32 SAMPLE_COMM_VI_GetDevAttrBySns(SAMPLE_SNS_TYPE_E enSnsType, VI_DEV_ATTR_S *pstViDevAttr);
TS_VOID SAMPLE_COMM_VI_GetSensorInfo(SAMPLE_VI_CONFIG_S *pstViConfig);

TS_S32 SAMPLE_COMM_VI_GetDayNight(VI_PIPE ViPipe, TS_S32 * pMode);
TS_S32 SAMPLE_COMM_VI_SetDayNight(VI_PIPE ViPipe, TS_S32 mode);

combo_dev_t SAMPLE_COMM_VI_GetComboDevBySensor(SAMPLE_SNS_TYPE_E enMode, TS_S32 s32SnsIdx);
// TS_S32 SAMPLE_COMM_VI_SaveRaw(VIDEO_FRAME_S* pVBuf, TS_U32 u32Nbit, FILE* pfd);
// TS_VOID* SAMPLE_COMM_VI_DumpRaw(TS_VOID *arg);
// TS_S32 SAMPLE_COMM_VI_StartDumpRawThread(VI_PIPE ViPipe, TS_S32 s32Cnt, const TS_CHAR *pzsName);
// TS_S32 SAMPLE_COMM_VI_StopDumpRawThread(TS_VOID);
TS_S32 SAMPLE_COMM_VI_SetParam(SAMPLE_VI_CONFIG_S *pstViConfig);
TS_S32  SAMPLE_COMM_VI_SwitchMode_StopVI(SAMPLE_VI_CONFIG_S *pstViConfigSrc);
TS_S32  SAMPLE_COMM_VI_SwitchMode(SAMPLE_VI_CONFIG_S *pstViConfigDes);

// TS_S32 SAMPLE_COMM_VI_FpnCalibrateConfig(VI_PIPE ViPipe, SAMPLE_VI_FPN_CALIBRATE_INFO_S *pstViFpnCalibrateInfo);
// TS_S32 SAMPLE_COMM_VI_FpnCorrectionConfig(VI_PIPE ViPipe, SAMPLE_VI_FPN_CORRECTION_INFO_S *pstViFpnCorrectionInfo);
// TS_S32 SAMPLE_COMM_VI_DisableFpnCorrection(VI_PIPE ViPipe, SAMPLE_VI_FPN_CORRECTION_INFO_S *pstViFpnCorrectionInfo);

// TS_S32 SAMPLE_COMM_VI_Load_UserPic(const char *pszYuvFile, VI_USERPIC_ATTR_S *pstUsrPic,
//			SAMPLE_VI_FRAME_INFO_S *pstViFrameInfo);
// TS_VOID SAMPLE_COMM_VI_Release_UserPic(SAMPLE_VI_FRAME_INFO_S *pstViFrameInfo);

TS_S32 SAMPLE_COMM_VI_FF_INIT(
	VI_PIPE pipe,
	VI_CHN chn,
	VI_VPSS_MODE_S *vivpss_mode,
	VI_DEV_ATTR_S *dev_attr,
	VI_DEV_BIND_PIPE_S *dev_bind_pipe,
	VI_PIPE_ATTR_S *pipe_attr,
	VI_CHN_ATTR_S *chn_attr,
	ISP_PUB_ATTR_S *isp_attr);


TS_S32 SAMPLE_COMM_VPSS_Start(VPSS_GRP VpssGrp, TS_BOOL *pabChnEnable, VPSS_GRP_ATTR_S *pstVpssGrpAttr,
						VPSS_CHN_ATTR_S *pastVpssChnAttr);
TS_S32 SAMPLE_COMM_VPSS_Stop(VPSS_GRP VpssGrp, TS_BOOL *pabChnEnable);
TS_S32 SAMPLE_COMM_VPSS_AdvFunStart(VPSS_GRP VpssGrp, TS_BOOL *pabChnEnable, VPSS_GRP_ATTR_S *pstVpssGrpAttr,
						VPSS_CHN_ATTR_S *pastVpssChnAttr);

TS_S32 SAMPLE_COMM_VPSS_FF_INIT(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_GRP_ATTR_S* pstVpssGrpAttr, VPSS_CHN_ATTR_S* pastVpssChnAttr);
TS_S32 SAMPLE_COMM_VPSS_SwitchResolution(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, TS_U32 width, TS_U32 height);


TS_S32 SAMPLE_COMM_VO_GetWH(VO_INTF_SYNC_E enIntfSync, TS_U32 *pu32W, TS_U32 *pu32H, TS_U32 *pu32Frm);
TS_S32 SAMPLE_COMM_VO_MemConfig(VO_DEV VoDev, TS_CHAR *pcMmzName);
TS_S32 SAMPLE_COMM_VO_StartDev(VO_DEV VoDev, VO_PUB_ATTR_S *pstPubAttr);
TS_S32 SAMPLE_COMM_VO_StopDev(VO_DEV VoDev);
TS_S32 SAMPLE_COMM_VO_StartLayer(VO_LAYER VoLayer, const VO_VIDEO_LAYER_ATTR_S *pstLayerAttr);
TS_S32 SAMPLE_COMM_VO_StopLayer(VO_LAYER VoLayer);
TS_S32 SAMPLE_COMM_VO_StartChn(VO_LAYER VoLayer, SAMPLE_VO_MODE_E enMode);
TS_S32 SAMPLE_COMM_VO_StopChn(VO_LAYER VoLayer, SAMPLE_VO_MODE_E enMode);
TS_S32 SAMPLE_COMM_VO_StartWBC(SAMPLE_VO_WBC_CONFIG *pWbcConfig);
TS_S32 SAMPLE_COMM_VO_StopWBC(SAMPLE_VO_WBC_CONFIG *pWbcConfig);
TS_S32 SAMPLE_COMM_VO_GetDefWBCConfig(SAMPLE_VO_WBC_CONFIG *pWbcConfig);
TS_S32 SAMPLE_COMM_VO_BindVpss(VO_LAYER VoLayer, VO_CHN VoChn, VPSS_GRP VpssGrp, VPSS_CHN VpssChn);
TS_S32 SAMPLE_COMM_VO_UnBindVpss(VO_LAYER VoLayer, VO_CHN VoChn, VPSS_GRP VpssGrp, VPSS_CHN VpssChn);
TS_S32 SAMPLE_COMM_VO_BindVi(VO_LAYER VoLayer, VO_CHN VoChn, VI_CHN ViChn);
TS_S32 SAMPLE_COMM_VO_UnBindVi(VO_LAYER VoLayer, VO_CHN VoChn);
TS_S32 SAMPLE_COMM_VO_HdmiStart(VO_INTF_SYNC_E enIntfSync);
TS_S32 SAMPLE_COMM_VO_HdmiStartByDyRg(VO_INTF_SYNC_E enIntfSync, DYNAMIC_RANGE_E enDyRg);
TS_S32 SAMPLE_COMM_VO_HdmiStop(void);
TS_S32 SAMPLE_COMM_VO_GetDefConfig(SAMPLE_VO_CONFIG_S *pstVoConfig);
TS_S32 SAMPLE_COMM_VO_StopVO(SAMPLE_VO_CONFIG_S *pstVoConfig);
TS_S32 SAMPLE_COMM_VO_StartVO(SAMPLE_VO_CONFIG_S *pstVoConfig);
TS_S32 SAMPLE_COMM_VO_StopPIP(SAMPLE_VO_CONFIG_S *pstVoConfig);
TS_S32 SAMPLE_COMM_VO_StartPIP(SAMPLE_VO_CONFIG_S *pstVoConfig);
TS_S32 SAMPLE_COMM_VO_GetDefLayerConfig(SAMPLE_COMM_VO_LAYER_CONFIG_S *pstVoLayerConfig);
TS_S32 SAMPLE_COMM_VO_StartLayerChn(SAMPLE_COMM_VO_LAYER_CONFIG_S *pstVoLayerConfig);
TS_S32 SAMPLE_COMM_VO_StopLayerChn(SAMPLE_COMM_VO_LAYER_CONFIG_S *pstVoLayerConfig);
TS_VOID SAMPLE_COMM_VO_Exit(void);

TS_S32 SAMPLE_COMM_VENC_MemConfig(void);
TS_S32 SAMPLE_COMM_VENC_Creat(VENC_CHN VencChn, PAYLOAD_TYPE_E enType,  PIC_SIZE_E enSize, SAMPLE_RC_E enRcMode,
		TS_U32  u32Profile, TS_BOOL bRcnRefShareBuf, VENC_GOP_ATTR_S *pstGopAttr);
TS_S32 SAMPLE_COMM_VENC_CustomCreat(VENC_CHN VencChn,SIZE_S * pInSize);

TS_S32 SAMPLE_COMM_VENC_Start(VENC_CHN VencChn, PAYLOAD_TYPE_E enType, PIC_SIZE_E enSize, SAMPLE_RC_E enRcMode,
			TS_U32 u32Profile, TS_BOOL bRcnRefShareBuf, VENC_GOP_ATTR_S *pstGopAttr);
TS_S32 SAMPLE_COMM_VENC_CustomStart(VENC_CHN VencChn,SIZE_S * pInSize);
TS_S32 SAMPLE_COMM_VENC_SetSmartH265(VENC_CHN VencChn, TS_U8 u8Enable);
TS_S32 SAMPLE_COMM_VENC_GetSmartH265(VENC_CHN VencChn, TS_U8 *u8Enable);

TS_S32 SAMPLE_COMM_VENC_Stop(VENC_CHN VencChn);
TS_S32 SAMPLE_COMM_VENC_SnapStart(VENC_CHN VencChn, SIZE_S *pstSize, TS_BOOL bSupportDCF);
TS_S32 SAMPLE_COMM_VENC_SnapProcess(VENC_CHN VencChn, TS_U32 SnapCnt, TS_BOOL bSaveJpg, TS_BOOL bSaveThm);
TS_S32 SAMPLE_COMM_VENC_SwitchResolution(VENC_CHN VeChn, TS_U32 width, TS_U32 height);
TS_S32 SAMPLE_COMM_VENC_SwitchEncoder(VENC_CHN VeChn, PAYLOAD_TYPE_E type);
TS_S32 SAMPLE_COMM_VENC_SaveJpeg(VENC_CHN VencChn, TS_U32 SnapCnt);
TS_S32 SAMPLE_COMM_VENC_SnapStop(VENC_CHN VencChn);
TS_S32 SAMPLE_COMM_VENC_StartGetStream(VENC_CHN VeChn[], TS_S32 s32Cnt);
TS_S32 SAMPLE_COMM_VENC_StartGetStreamV2(VENC_CHN VeChn[], TS_S32 s32Cnt, enum SAMPLE_VENC_CONSUME_TYPE type);
TS_S32 SAMPLE_COMM_VENC_StopGetStream(void);
TS_S32 SAMPLE_COMM_VENC_StartGetStream_Svc_t(TS_S32 s32Cnt);
TS_S32 SAMPLE_COMM_VENC_GetGopAttr(VENC_GOP_MODE_E enGopMode, VENC_GOP_ATTR_S *pstGopAttr);
TS_S32 SAMPLE_COMM_VENC_QpmapSendFrame(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VENC_CHN VeChn[],
					TS_S32 s32Cnt, SIZE_S stSize);
TS_S32 SAMPLE_COMM_VENC_StopSendQpmapFrame(void);
TS_S32 SAMPLE_COMM_VENC_Manual_Capture(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VENC_CHN VencChn, TS_BOOL enableJPEG);
TS_S32 SAMPLE_COMM_VENC_StartIDR(VENC_CHN VeChn[],TS_S32 s32Cnt);
TS_S32 SAMPLE_COMM_VENC_StopIDR(void);
TS_S32 SAMPLE_COMM_VENC_StopSendFrame(void);
TS_S32 SAMPLE_COMM_VENC_SendFrame(VENC_CHN VencChn, VIDEO_FRAME_INFO_S * pFrame, TS_S32 s32MilliSec);
TS_VOID SAMPLE_COMM_VENC_GetTestSavePath(TS_VOID);
TS_S32 SAMPLE_COMM_VENC_GetFilePostfix(PAYLOAD_TYPE_E enPayload, char* szFilePostfix);
TS_S32 SAMPLE_COMM_VENC_SaveStream(FILE* pFd, VENC_STREAM_S* pstStream);
TS_VOID openTSingRTSPServer(PAYLOAD_TYPE_E  enType);

int SAMPLE_COMM_VENC_FF_INIT(VENC_CHN chn, VENC_CHN_ATTR_S *VencChnAttr, VENC_RECV_PIC_PARAM_S *RecvParam);


TS_VOID SAMPLE_COMM_VDEC_ExitVBPool(TS_VOID);
TS_S32 SAMPLE_COMM_VDEC_InitVBPool(TS_U32 ChnNum, SAMPLE_VDEC_ATTR *pastSampleVdec);
TS_VOID * SAMPLE_COMM_VDEC_SendStream(TS_VOID *pArgs);
TS_VOID SAMPLE_COMM_VDEC_CmdCtrl(TS_S32 s32ChnNum,VDEC_THREAD_PARAM_S *pstVdecSend, pthread_t *pVdecThread);
TS_VOID SAMPLE_COMM_VDEC_StartSendStream(TS_S32 s32ChnNum, VDEC_THREAD_PARAM_S *pstVdecSend, pthread_t *pVdecThread);
TS_VOID SAMPLE_COMM_VDEC_StopSendStream(TS_S32 s32ChnNum, VDEC_THREAD_PARAM_S *pstVdecSend, pthread_t *pVdecThread);
TS_VOID * SAMPLE_COMM_VDEC_GetPic(TS_VOID *pArgs);
TS_VOID SAMPLE_COMM_VDEC_StartGetPic(TS_S32 s32ChnNum, VDEC_THREAD_PARAM_S *pstVdecGet, pthread_t *pVdecThread);
TS_VOID SAMPLE_COMM_VDEC_StopGetPic(TS_S32 s32ChnNum, VDEC_THREAD_PARAM_S *pstVdecGet, pthread_t *pVdecThread);
TS_S32 SAMPLE_COMM_VDEC_Start(TS_S32 s32ChnNum, SAMPLE_VDEC_ATTR *pastSampleVdec);
TS_S32 SAMPLE_COMM_VDEC_Stop(TS_S32 s32ChnNum);
TS_S32 SAMPLE_COMM_VDEC_StartCPM(CPM_GRP CPMGrp);

TS_S32 SAMPLE_COMM_VI_Bind_CPM(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, CPM_GRP CpmGrp, CPM_PIPE CpmPipe);
TS_S32 SAMPLE_COMM_VI_UnBind_CPM(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, CPM_GRP CpmGrp, CPM_PIPE CpmPipe);
TS_S32 SAMPLE_COMM_VPSS_Bind_CPM(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, CPM_GRP CpmGrp, CPM_PIPE CpmPipe);
TS_S32 SAMPLE_COMM_VPSS_UnBind_CPM(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, CPM_GRP CpmGrp, CPM_PIPE CpmPipe);
TS_S32 SAMPLE_COMM_CPM_Bind_VENC(CPM_GRP CpmGrp, CPM_CHN CpmChn, VENC_CHN VencChn);
TS_S32 SAMPLE_COMM_CPM_UnBind_VENC(CPM_GRP CpmGrp, CPM_CHN CpmChn, VENC_CHN VencChn);
TS_S32 SAMPLE_COMM_CPM_Bind_VO(CPM_GRP CpmGrp, CPM_CHN CpmChn, VO_LAYER VoLayer, VO_CHN VoChn);
TS_S32 SAMPLE_COMM_CPM_UnBind_VO(CPM_GRP CpmGrp, CPM_CHN CpmChn, VO_LAYER VoLayer, VO_CHN VoChn);

// TS_S32 SAMPLE_COMM_CPM_Bind(TS_U32 u32PipeNum);
TS_S32 SAMPLE_COMM_CPM_Bind_ViCpmVenc(TS_U32 u32PipeNum, CPM_GRP CPMGrp, CPM_CHN CPMChn, VI_CHN ViChn, VENC_CHN VencChn);
TS_S32 SAMPLE_COMM_CPM_Start(CPM_ADAP_S *pCPMAdap);
TS_S32 SAMPLE_COMM_CPM_Active(CPM_GRP CPMGrp);
TS_S32 SAMPLE_COMM_CPM_Stop(CPM_GRP CPMGrp);
// TS_S32 SAMPLE_COMM_CPM_UnBind(TS_U32 u32PipeNum);
TS_S32 SAMPLE_COMM_CPM_UnBind_ViCpmVenc(TS_U32 u32PipeNum, CPM_GRP CPMGrp, CPM_CHN CPMChn, VI_CHN ViChn, VENC_CHN VencChn);
TS_S32 SAMPLE_ALGO_CPM_Config(AVS_GRP CPMGrp, int pipeNum, SAMPLE_ALGO_TYPE_E algo_type);
TS_S32 SAMPLE_COMM_CPM_Bind_ViCpm(TS_U32 u32PipeNum, CPM_GRP CPMGrp, CPM_CHN CPMChn, VI_CHN ViChn);
TS_S32 SAMPLE_COMM_CPM_UnBind_ViCpm(TS_U32 u32PipeNum, CPM_GRP CPMGrp, CPM_CHN CPMChn, VI_CHN ViChn);
TS_S32 SAMPLE_COMM_CPM_MULTI_Start(CPM_ADAP_S *pCPMAdap);
TS_S32 SAMPLE_COMM_CPM_Bind_ViVpssCpmVenc(TS_U32 u32PipeNum, VI_CHN ViChn, VPSS_GRP VpssGrp, VPSS_CHN VpssChn, AVS_GRP CPMGrp, AVS_CHN CPMChn, VENC_CHN VencChn);
TS_S32 SAMPLE_COMM_CPM_Bind_ViVpssCpm(TS_U32 u32PipeNum, VI_CHN ViChn, VPSS_GRP VpssGrp, VPSS_CHN VpssChn, AVS_GRP CPMGrp);
TS_S32 SAMPLE_COMM_CPM_UnBind_ViVpssCpmVenc(TS_U32 u32PipeNum, VI_CHN ViChn,  VPSS_GRP VpssGrp, VPSS_CHN VpssChn, AVS_GRP CPMGrp, AVS_CHN CPMChn, VENC_CHN VencChn);
TS_S32 SAMPLE_COMM_CPM_UnBind_ViVpssCpm(TS_U32 u32PipeNum, VI_CHN ViChn, VPSS_GRP VpssGrp, VPSS_CHN VpssChn, AVS_GRP CPMGrp);
TS_S32 SAMPLE_COMM_CPM_UnBind_VpssCpm(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, AVS_GRP CPMGrp);

TS_S32 SAMPLE_COMM_REGION_Create(TS_S32 HandleNum, RGN_TYPE_E enType);
TS_S32 SAMPLE_COMM_REGION_Destroy(TS_S32 HandleNum, RGN_TYPE_E enType);
TS_S32 SAMPLE_COMM_REGION_AttachToChn(TS_S32 HandleNum, RGN_TYPE_E enType, MPP_CHN_S *pstMppChn);
TS_S32 SAMPLE_COMM_REGION_DetachFrmChn(TS_S32 HandleNum, RGN_TYPE_E enType, MPP_CHN_S *pstMppChn);
TS_S32 SAMPLE_COMM_REGION_SetBitMap(RGN_HANDLE Handle, PIXEL_FORMAT_E enPixelFmt);
TS_S32 SAMPLE_COMM_REGION_GetUpCanvas(RGN_HANDLE Handle);
TS_S32 SAMPLE_COMM_REGION_GetMinHandle(RGN_TYPE_E enType);
TS_S32 SAMPLE_REGION_DetachFromChn(RGN_HANDLE Handle, MPP_CHN_S *pstChn);
TS_S32 SAMPLE_REGION_Destroy(RGN_HANDLE Handle);
TS_S32 REGION_MST_LoadBmp(const char *filename, BITMAP_S *pstBitmap, TS_BOOL bFil, TS_U32 u16FilColor,
	PIXEL_FORMAT_E enPixelFormat);

TS_S32 SAMPLE_COMM_ALGO_DrawCallBack(drawprocess cb);
TS_S32 SAMPLE_COMM_ALGO_FACE_CPM_GetResult_Cb(VIDEO_FRAME_INFO_S *in, VIDEO_FRAME_INFO_S *out);
TS_S32 SAMPLE_COMM_ALGO_CPM_GetResult(TS_VOID *pHandle, TS_VOID *pResult);

#if 1
char* SAMPLE_AUDIO_Pt2Str(PAYLOAD_TYPE_E enType);
TS_S32 SAMPLE_COMM_ADEC_Bind_AO(ADEC_CHN AdChn, AUDIO_DEV AoDev, AO_CHN AoChn);
TS_S32 SAMPLE_COMM_AUDIO_CreatTrdAiAo(AUDIO_DEV AiDev, AI_CHN AiChn, AUDIO_DEV AoDev, AO_CHN AoChn);
TS_S32 SAMPLE_COMM_AUDIO_CreatTrdAiAenc(AUDIO_DEV AiDev, AI_CHN AiChn, AENC_CHN AeChn);
TS_S32 SAMPLE_COMM_AUDIO_CreatTrdAencAdec(AENC_CHN AeChn, ADEC_CHN AdChn, FILE *pAecFd);
TS_S32 SAMPLE_COMM_AUDIO_CreatTrdFileAdec(ADEC_CHN AdChn, FILE *pAdcFd);
TS_S32 SAMPLE_COMM_AUDIO_CreatTrdAoVolCtrl(AUDIO_DEV AoDev);
TS_S32 SAMPLE_COMM_AUDIO_DestoryTrdAi(AUDIO_DEV AiDev, AI_CHN AiChn);
TS_S32 SAMPLE_COMM_AUDIO_DestoryTrdAencAdec(AENC_CHN AeChn);
TS_S32 SAMPLE_COMM_AUDIO_DestoryTrdFileAdec(ADEC_CHN AdChn);
TS_S32 SAMPLE_COMM_AUDIO_DestoryTrdAoVolCtrl(AUDIO_DEV AoDev);
TS_S32 SAMPLE_COMM_AUDIO_DestoryAllTrd(void);
TS_S32 SAMPLE_COMM_AUDIO_AoBindAdec(AUDIO_DEV AoDev, AO_CHN AoChn, ADEC_CHN AdChn);
TS_S32 SAMPLE_COMM_AI_Bind_AENC(AI_CHN AiChn, AENC_CHN AencChn);
TS_S32 SAMPLE_COMM_AUDIO_AoUnbindAdec(AUDIO_DEV AoDev, AO_CHN AoChn, ADEC_CHN AdChn);
TS_S32 SAMPLE_COMM_AUDIO_AoBindAi(AUDIO_DEV AiDev, AI_CHN AiChn, AUDIO_DEV AoDev, AO_CHN AoChn);
TS_S32 SAMPLE_COMM_AUDIO_AoUnbindAi(AUDIO_DEV AiDev, AI_CHN AiChn, AUDIO_DEV AoDev, AO_CHN AoChn);
TS_S32 SAMPLE_COMM_AUDIO_AencBindAi(AUDIO_DEV AiDev, AI_CHN AiChn, AENC_CHN AeChn);
TS_S32 SAMPLE_COMM_AUDIO_AencUnbindAi(AUDIO_DEV AiDev, AI_CHN AiChn, AENC_CHN AeChn);
TS_S32 SAMPLE_COMM_AUDIO_CfgAcodec(AIO_ATTR_S *pstAioAttr);
TS_S32 SAMPLE_COMM_AUDIO_StartAi(AUDIO_DEV AiDevId, TS_S32 s32AiChnCnt, AIO_ATTR_S *pstAioAttr,
	TS_BOOL bAiChnEn, AUDIO_SAMPLE_RATE_E enOutSampleRate, TS_BOOL bResampleEn, TS_VOID* pstAiVqeAttr, TS_U32 u32AiVqeType);
TS_S32 SAMPLE_COMM_AUDIO_StopAi(AUDIO_DEV AiDevId, TS_S32 s32AiChnCnt, TS_BOOL bResampleEn, TS_BOOL bVqeEn);
TS_S32 SAMPLE_COMM_AUDIO_StartAo(AUDIO_DEV AoDevId, TS_S32 s32AoChnCnt,
			AIO_ATTR_S *pstAioAttr, AUDIO_SAMPLE_RATE_E enInSampleRate, TS_BOOL bResampleEn);
TS_S32 SAMPLE_COMM_AUDIO_StopAo(AUDIO_DEV AoDevId, TS_S32 s32AoChnCnt, TS_BOOL bResampleEn);
TS_S32 SAMPLE_COMM_AUDIO_StartAenc(TS_S32 s32AencChnCnt, AIO_ATTR_S *pstAioAttr, PAYLOAD_TYPE_E enType);
TS_S32 SAMPLE_COMM_AUDIO_StopAenc(TS_S32 s32AencChnCnt);
TS_S32 SAMPLE_COMM_AUDIO_StartAdec(ADEC_CHN AdChn, PAYLOAD_TYPE_E enType);
TS_S32 SAMPLE_COMM_AUDIO_StopAdec(ADEC_CHN AdChn);

TS_S32 SAMPLE_COMM_VDEC_InitVBPool(TS_U32 ChnNum, SAMPLE_VDEC_ATTR *pastSampleVdec);
TS_VOID SAMPLE_COMM_VDEC_ExitVBPool(void);
TS_VOID SAMPLE_COMM_VDEC_CmdCtrl(TS_S32 s32ChnNum, VDEC_THREAD_PARAM_S *pstVdecSend, pthread_t *pVdecThread);
TS_VOID SAMPLE_COMM_VDEC_StartSendStream(TS_S32 s32ChnNum, VDEC_THREAD_PARAM_S *pstVdecSend, pthread_t *pVdecThread);
TS_VOID SAMPLE_COMM_VDEC_StopSendStream(TS_S32 s32ChnNum, VDEC_THREAD_PARAM_S *pstVdecSend, pthread_t *pVdecThread);
TS_VOID *SAMPLE_COMM_VDEC_SendStream(TS_VOID *pArgs);
TS_VOID SAMPLE_COMM_VDEC_StartGetPic(TS_S32 s32ChnNum, VDEC_THREAD_PARAM_S *pstVdecGet, pthread_t *pVdecThread);
TS_VOID SAMPLE_COMM_VDEC_StopGetPic(TS_S32 s32ChnNum, VDEC_THREAD_PARAM_S *pstVdecGet, pthread_t *pVdecThread);
TS_S32 SAMPLE_COMM_VDEC_Start(TS_S32 s32ChnNum, SAMPLE_VDEC_ATTR *pastSampleVdec);
TS_S32 SAMPLE_COMM_VDEC_Stop(TS_S32 s32ChnNum);
#endif




enum SAMPLE_DETAIL {
	FULL_DETAIL = 0,
	WITHOUT_WH
};


/*
 * 1. add (void)(&dst == &src);\ for compile warning when types are different
 * 2. add COMPILE_ERR_WHILE_TRUE or _Static_assert for compile error when sizes are different
 * 3. NO call limit such as dst++, src++
 */
#define STRUCT_PTR_COPY(dst,src) do {\
	typeof(dst) _dst = (dst);\
	typeof(src) _src = (src);\
	(void)(dst == src);\
	_Static_assert(sizeof(*_dst) == sizeof(*_src), "sizes of struct are different !");\
	memcpy(_dst, _src, sizeof(*_dst) < sizeof(*_src) ? sizeof(*_dst) : sizeof(*_src));\
	} while(0)

#endif /* End of #ifndef __SAMPLE_COMMON_H__ */
