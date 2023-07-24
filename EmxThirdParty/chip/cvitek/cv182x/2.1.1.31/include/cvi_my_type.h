//
// Created by tfh on 2021/9/4.
//

#ifndef EP2DHJW_CVI_MY_TYPE_H
#define EP2DHJW_CVI_MY_TYPE_H
typedef enum {
    VI_DEV_ID_0,
    VI_DEV_ID_1,
    VI_DEV_ID_MAX,
} ViDev_e;

typedef enum {
    VI_PIPE_ID_0,
    VI_PIPE_ID_1,
    VI_PIPE_ID_2,
    VI_PIPE_ID_3,
    VI_PIPE_ID_MAX,
} ViPipe_e;

typedef enum {
    VI_CHN_ID_0,
    VI_CHN_ID_1,
    VI_CHN_ID_2,
    VI_CHN_ID_3,
    VI_CHN_ID_MAX,
} ViChn_e;

typedef enum {
    VPSS_DEV_ID_0,
    VPSS_DEV_ID_1,
    VPSS_DEV_ID_MAX,
} VpssDev_e;

typedef enum {
    VPSS_GRP_ID_0,
    VPSS_GRP_ID_1,
    VPSS_GRP_ID_2,
    VPSS_GRP_ID_3,
    VPSS_GRP_ID_4,
    VPSS_GRP_ID_5,
    VPSS_GRP_ID_6,
    VPSS_GRP_ID_7,
    VPSS_GRP_ID_8,
    VPSS_GRP_ID_9,
    VPSS_GRP_ID_10,
    VPSS_GRP_ID_11,
    VPSS_GRP_ID_12,
    VPSS_GRP_ID_13,
    VPSS_GRP_ID_14,
    VPSS_GRP_ID_15,
    VPSS_GRP_ID_MAX,
} VpssGrp_e;

typedef enum {
    VPSS_CHN_ID_0,
    VPSS_CHN_ID_1,
    VPSS_CHN_ID_2,
    VPSS_CHN_ID_MAX,
} VpssChn_e;

typedef enum {
    VENC_CHN_ID_0,
    VENC_CHN_ID_1,
    VENC_CHN_ID_2,
    VENC_CHN_ID_3,
    VENC_CHN_ID_4,
    VENC_CHN_ID_5,
    VENC_CHN_ID_6,
    VENC_CHN_ID_7,
    VENC_CHN_ID_8,
    VENC_CHN_ID_9,
    VENC_CHN_ID_10,
    VENC_CHN_ID_11,
    VENC_CHN_ID_12,
    VENC_CHN_ID_13,
    VENC_CHN_ID_14,
    VENC_CHN_ID_15,
    VENC_CHN_ID_MAX,
} VencChn_e;

typedef enum {
    VB_POOL_ID_0,
    VB_POOL_ID_1,
    VB_POOL_ID_2,
    VB_POOL_ID_3,
    VB_POOL_ID_4,
    VB_POOL_ID_5,
    VB_POOL_ID_6,
    VB_POOL_ID_7,
    VB_POOL_ID_8,
    VB_POOL_ID_MAX,
} VbPool_e;

typedef enum {
    AI_TYPE_0_MD,
    AI_TYPE_1_PD,
    AI_TYPE_2_FD,
} AiType_e;

/*智能侦测所使用的数据源组*/
#ifndef CVI_AI_SRC_BIND_GROUP
#define CVI_AI_SRC_BIND_GROUP VPSS_GRP_ID_0
#endif

/*主子码流对应使用的vpss group*/
int g_MEDIA_VENC_VPSS_GRP(int chn);

/*主子码流对应使用的vpss chn*/
int g_MEDIA_VENC_VPSS_CHN(int chn);

/*获取fd使用的vb*/
int cvi_get_fd_vb_id();

/*isp效果文件路径*/
#define IQ_PARAM_DAY        "/root/firmware/sensor/sdr_day.bin"
#define IQ_PARAM_IR        "/root/firmware/sensor/sdr_ir.bin"
#define IQ_PARAM_WHITE        "/root/firmware/sensor/sdr_white.bin"

#define IQ_CONFIGS_DAY       "/root/configs/sdr_day.bin"
#define IQ_CONFIGS_IR        "/root/configs/sdr_ir.bin"
#define IQ_CONFIGS_WHITE     "/root/configs/sdr_white.bin"

/*LDC标定参数*/
#define IQ_CONFIGS_LDC_MESH     "/root/firmware/sensor/ldc_mesh.bin"

/*snesor 0 的子码流分辨率*/
#ifndef SENSOR0_SUB_STREAMING_SIZE_W
#define SENSOR0_SUB_STREAMING_SIZE_W 1280
#endif
#ifndef SENSOR0_SUB_STREAMING_SIZE_H
#define SENSOR0_SUB_STREAMING_SIZE_H 720
#endif

/*snesor 1 的子码流分辨率*/
#ifndef SENSOR1_SUB_STREAMING_SIZE_W
#define SENSOR1_SUB_STREAMING_SIZE_W 1280
#endif
#ifndef SENSOR1_SUB_STREAMING_SIZE_H
#define SENSOR1_SUB_STREAMING_SIZE_H 720
#endif

/*人形侦测分辨率*/
#ifndef CVI_PD_SRC_WIDTH
#define CVI_PD_SRC_WIDTH 896
#define CVI_PD_SRC_HEIGHT 512
#endif
/*人形侦测所使用的数据源组*/
#ifndef CVI_PD_BIND_GROUP
#define CVI_PD_BIND_GROUP VPSS_GRP_ID_0
#define CVI_PD_BIND_CHN VPSS_CHN_ID_1
#endif
/*ivp 人形侦测使用的 vpss grp and chn*/
#define PD_SRC_VPSSGRP VPSS_GRP_ID_3
#define PD_SRC_VPSSCHN VPSS_CHN_ID_0


/*人脸侦测分辨率*/
#ifndef CVI_FD_SRC_WIDTH
#define CVI_FD_SRC_WIDTH 768
#define CVI_FD_SRC_HEIGHT 432
#endif

/*人脸侦测所使用的数据源组*/
#ifndef CVI_FD_BIND_GROUP
#define CVI_FD_BIND_GROUP VPSS_GRP_ID_0
#define CVI_FD_BIND_CHN VPSS_CHN_ID_1
#endif
/*ivp 人脸侦测使用的 vpss grp and chn*/
#define FD_SRC_VPSSGRP VPSS_GRP_ID_5
#define FD_SRC_VPSSCHN VPSS_CHN_ID_0
/*人脸扣图使用的 vpss grp and chn*/
#define FD_CUT_VPSSGRP CVI_FD_BIND_GROUP
#define FD_CUT_VPSSCHN VPSS_CHN_ID_0

/*移动侦测分辨率*/
#ifndef CVI_MD_SRC_WIDTH
#define CVI_MD_SRC_WIDTH 640
#define CVI_MD_SRC_HEIGHT 360
#endif
/*移动侦测所使用的数据源组*/
#ifndef CVI_MD_BIND_GROUP
#define CVI_MD_BIND_GROUP VPSS_GRP_ID_0
#define CVI_MD_BIND_CHN VPSS_CHN_ID_1
#endif
/*移动侦测使用的 vpss grp and chn*/
#define MD_SRC_VPSSGRP VPSS_GRP_ID_4
#define MD_SRC_VPSSCHN VPSS_CHN_ID_0

/*sensor0 帧率*/
#ifndef CVI_SENSOR0_FRAME_RATE
#define CVI_SENSOR0_FRAME_RATE 15
#endif
/*sensor1 帧率*/
#ifndef CVI_SENSOR1_FRAME_RATE
#define CVI_SENSOR1_FRAME_RATE 15
#endif
/*夜视 帧率*/
#ifndef CVI_NIGHT_FRAME_RATE
#define CVI_NIGHT_FRAME_RATE 15
#endif

/*判断智能侦测使用的sensor数据*/
#if CVI_AI_SRC_BIND_GROUP
#define CVI_VIDEO_SBU_WIDTH SENSOR1_SUB_STREAMING_SIZE_W
#define CVI_VIDEO_SBU_HEIGHT SENSOR1_SUB_STREAMING_SIZE_H
#define CVI_AI_DETECTION_FRAME_RATE CVI_SENSOR1_FRAME_RATE
#else
#define CVI_VIDEO_SBU_WIDTH SENSOR0_SUB_STREAMING_SIZE_W
#define CVI_VIDEO_SBU_HEIGHT SENSOR0_SUB_STREAMING_SIZE_H
#define CVI_AI_DETECTION_FRAME_RATE CVI_SENSOR0_FRAME_RATE
#endif
#ifndef CVI_FD_GROUP_WIDTH
#define CVI_FD_GROUP_WIDTH CVI_VIDEO_SBU_WIDTH
#define CVI_FD_GROUP_HEIGHT CVI_VIDEO_SBU_HEIGHT
#endif
#define CVITEK_SDK_VER 21129


#endif //EP2DHJW_CVI_MY_TYPE_H
