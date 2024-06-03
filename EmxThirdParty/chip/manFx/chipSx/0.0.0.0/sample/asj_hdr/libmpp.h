/******************************************************************************
  Hisilicon base Hi3516EV100 COMM programs head file.

  Copyright (C), 2019-2022, ASJ Tech. Co., Ltd.
 ******************************************************************************
  Modification:  2019-7 Created by wusongbai
******************************************************************************/

#ifndef __MPPLIB_H__
#define __MPPLIB_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "AsjStd.h"

#ifdef eeasy823
#define JPEG_MAX_SIZE 1024*200
#elif defined eeasy820
#define JPEG_MAX_SIZE 1024*100
#elif defined eeasy822
#define JPEG_MAX_SIZE 1024*100
#elif defined eeasy823v2
#define JPEG_MAX_SIZE 1024*100
#elif defined xs
#define JPEG_MAX_SIZE 1024*50
#else
#define JPEG_MAX_SIZE 1024*200
#endif

//unsigned char jpeg_pVideoBuf[3][JPEG_MAX_SIZE];
//int jpeg_bufSize[3];
/******************************************************************************
* function : Basc type definition area
******************************************************************************/
typedef unsigned char           HI_U8;
typedef unsigned short          HI_U16;
typedef unsigned int            HI_U32;

typedef signed char             HI_S8;
typedef short                   HI_S16;
typedef int                     HI_S32;

#ifndef _M_IX86
    typedef unsigned long long  HI_U64;
    typedef long long           HI_S64;
#else
    typedef __int64             HI_U64;
    typedef __int64             HI_S64;
#endif

typedef char                    HI_CHAR;
#define HI_VOID                 void

typedef HI_U32                  HI_HANDLE;

typedef enum {
    HI_FALSE = 0,
    HI_TRUE  = 1,
} HI_BOOL;

#ifndef NULL
#define NULL                    0L
#endif

#define HI_NULL                 0L
#define HI_SUCCESS              0
#define HI_FAILURE              (-1)

/******************************************************************************
* function : User type definition area
******************************************************************************/
typedef enum FRAME_TYPE_USER_DEFINE{
    FRAME_I = 0,
    FRAME_P,
    FRAME_B,
    FRAME_A,
    FRAME_BUTT
}FRAME_TYPE;

//typedef enum APP_PIC_SIZE_T
//{
//	PIC_CIF,
//	PIC_360P,		/* 640 * 360 */
//	PIC_VGA,
//	PIC_640x360,
//	PIC_D1_PAL,		/* 720 * 576 04 */
//	PIC_D1_NTSC,	/* 720 * 480 05 */
//	PIC_720P,		/* 1280 * 720  06 */
//	PIC_1080P,		/* 1920 * 1080 07 */
//	PIC_1296P,		/* 2304 * 1296 08 */
//	PIC_1440P,		/* 2560 * 1620 09 */
//	PIC_1620P,		/* 2880 * 1620 10 */
//	PIC_1944P,		/* 2560 * 1944 11 */
//	PIC_2160P,		/* 3840 * 2160 12 */
//	PIC_DI,
//} PIC_SIZE_T;

typedef enum APP_PIC_SIZE_ASJ_T
{
	PIC_CIF_ASJ,
	PIC_360P_ASJ,		/* 640 * 360 */
	PIC_VGA_ASJ,
	PIC_640x360_ASJ,
	PIC_D1_PAL_ASJ,		/* 720 * 576 04 */
	PIC_D1_NTSC_ASJ,	/* 720 * 480 05 */
	PIC_720P_ASJ,		/* 1280 * 720  06 */
	PIC_1080P_ASJ,		/* 1920 * 1080 07 */
	PIC_1296P_ASJ,		/* 2304 * 1296 08 */
	PIC_1440P_ASJ,		/* 2560 * 1620 09 */
	PIC_1620P_ASJ,		/* 2880 * 1620 10 */
	PIC_1944P_ASJ,		/* 2560 * 1944 11 */
	PIC_2160P_ASJ,		/* 3840 * 2160 12 */
	PIC_DI_ASJ,
} PIC_SIZE_ASJ_T;

typedef enum _osd_veralign_t_{
   veraligntop = 0,
   veralignbottom,
   veraligncenter,
}osd_veralign_t;

typedef enum _osd_horalign_t_{
   horalignleft = 0,
   horalignright,
   horaligncenter,
}osd_horalign_t;


typedef struct _videoosd_{
	unsigned int x;/*显示区域x偏移位*/
	unsigned int y;/*显示区域y偏移位*/
    osd_horalign_t horalign;
    osd_veralign_t veralign;
	unsigned short color;//保留
	PIC_SIZE_ASJ_T resolution;
	unsigned char DateMode;/*日期格式，0为MM-DD-YY，1为YY-MM-DD*/
	unsigned char TimeMode;/*时间格式，0为12小时制，1为24小时制*/
	char chnname[48];/*通道名称*/
	char alarmstate[10];//保留
	int    TVMode; //保留
}videoosd_t;

typedef enum BUF_POSITION_POLICY_X{
    /*current reader pointer pos*/
    BUF_POSITION_CUR_READ = 0,
    /*The starting position of the block where
     * the nth I frame closest to the current
     * read pointer is located*/
    BUF_POSITION_LAST_READ_nIFRAME,
    /*Current write pointer pos*/
    BUF_POSITION_CUR_WRITE,
    /*The starting position of the nth block
     * closest to the current write pointer*/
    BUF_POSITION_LAST_WRITE_nBLOCK,
    /*The starting position of the block where
     * the nth I frame closest to the current
     * write pointer is located,
     * n=0 indicates the nearest I frame*/
    BUF_POSITION_LAST_WRITE_nIFRAME,
    BUF_POSITION_BUTT
}BUF_POSITION_POLICY;

typedef enum __MOTION_DETECT_TYPE
{
    MOTION_DETECT_MOTION = 0,
    MOTION_DETECT_PER,
    MOTION_DETECT_PNV,
    MOTION_DETECT_FACE,
    MOTION_DETECT_PET,
    MOTION_DETECT_NOISE,
    MOTION_DETECT_CUSTOM,
    MOTION_DETECT_MAX
}MOTION_DETECT_TYPE;

typedef enum __MID_OSD_HIDE_TYPE
{
    MID_OSD_SHOW = 0,
    MID_OSD_HIDE
}MID_OSD_HIDE_TYPE;

typedef enum __MID_OSD_TYPE
{
    MID_OSD_TYPE_CHN_SET = 0,
    MID_OSD_TYPE_TIME_SET,
    MID_OSD_TYPE_LOGO_SET
}MID_OSD_TYPE;

typedef enum __MID_OSD_TYPE_G
{
    MID_OSD_TYPE_CHN_GET = 0,
    MID_OSD_TYPE_TIME_GET,
    MID_OSD_TYPE_LOGO_GET
}MID_OSD_TYPE_G;

typedef enum __MID_AREA_TYPE
{
    MID_AREA_TYPE_MOTION = 0,
    MID_AREA_TYPE_PERSION
}MID_AREA_TYPE;

//typedef struct _videoadj_{
//    int mainkbrate;
//    int subkbrate;
//    int fps;
//    int gop;
//    int resolution;//0-原分辨率 1-720P
//}videoadj_t;

typedef struct _videoadj_{
    int chn;
    int bitrate;
    int fps;
    int gop;
    int resolution;//0-原分辨率 1-720P
}videoadj_t;

typedef unsigned char uint8_t;

typedef void (*CallBack_Log)(uint8_t level, const char *tag, const char *file, const char *func,const long line, const char *format, ...);

/******************************************************************************
*初始化
******************************************************************************/
HI_S32 MID_Mpp_Init(VIDEO_CFG *VideoCfgInfo, ALARM_CFG *AlarmCfgInfo, CallBack_Log elog);
/******************************************************************************
*音频相关
******************************************************************************/
HI_S32 MID_Audio_Speaker_Open(HI_VOID);
HI_S32 MID_Audio_Speaker_SetVol(HI_S32 audioVolume);
HI_S32 MID_Audio_Speaker_Bydir(HI_CHAR *path);
HI_S32 MID_Audio_Speaker_Close(HI_VOID);
HI_S32 MID_Audio_WriteFrame(HI_CHAR *pAudioBuf, HI_U32 frameSize);
/******************************************************************************
*码流设置及获取
******************************************************************************/
int MID_Add_Reader(int chn, int *pReaderId);
int MID_Del_Reader(int readerId);
int MID_Get_OneFrame(HI_S32 nReaderId, BUF_POSITION_POLICY enPolicy, int s32N, \
char **pVideoBuf, int *bufSize, FRAME_TYPE *frameType, unsigned long long *timeTick);
HI_S32 MID_Mpp_RequestIDR(int chn, int bInstant);//请求关键帧
int MID_Video_Adj(videoadj_t para);//调整帧率、码率、GOP、分辨率
int MID_VideoBitrate_Adj(VIDEO_CFG *VideoCfgInfo, ALARM_CFG *AlarmCfgInfo,videoadj_t para);//保留不用实现
/******************************************************************************
*图像设置
******************************************************************************/
int MID_ISP_SetImgParam(int chn,IMG_SET_CMD Cmd, int Val);
/******************************************************************************
*捕捉图像
******************************************************************************/
HI_S32 MID_Mpp_Capture_YUV_Frame(int chn, char *path);
/******************************************************************************
*OSD设置
******************************************************************************/
int MID_OSD_Create(int chn, VIDEO_CFG * VideoCfgInfo,const char *imagePath,const char *fontpath);
int MID_OSD_Destroy(int chn, VIDEO_CFG *VideoCfgInfo);
int MID_OSD_Set(int chn, MID_OSD_HIDE_TYPE hide, MID_OSD_TYPE type, videoosd_t *pPara, int id);
int MID_OSD_Text_Reset(int chn);

int MID_OSD_Chn_Set(int stream, videoosd_t *pPara);//保留不用实现
int MID_OSD_Chn_Close(int stream);//保留不用实现
int MID_OSD_Time_Set(int stream, videoosd_t *pPara);//保留不用实现
int MID_OSD_Time_Close(int stream);//保留不用实现
int MID_OSD_Logo_Set(int stream, videoosd_t *pPara);//保留不用实现
int MID_OSD_Logo_Close(int stream);//保留不用实现
int MID_OSD_Enformt(int DateFormat,TIME_FORMAT TimeFormat);//保留不用实现
int MID_OSD_Chn_Open(int stream, videoosd_t *pPara);//保留不用实现
int MID_OSD_Chn_Destroy(int stream);//保留不用实现
/******************************************************************************
*AI设置及获取
******************************************************************************/
int MID_DrawBoxEnable(void);//使能画框 人形框/车型框使能、禁止
int MID_AREA_Detect_SET(IVP_ALARM_STRUCT para);//检测区域设置 用于人形/车形/宠物...
int MID_Cover_Set_Attr(SHELTER_INFO ShelterInfo);//隐私遮挡设置
int MID_Get_Motion_Detect(MOTION_DETECT_TYPE type, BOOL *val);//获取各种报警状态

int MID_Motion_Set_Area(char *OutArea);//设置移动区域 暂不实现，需要沟通

int MID_Motion_Get_Area(char *InArea, char *OutArea);//保留不用实现
int MID_Smd_Get_Area(char *InArea, char *OutArea);//保留不用实现
int MID_Smd_Set_Area(char *InArea, char *OutArea);//保留不用实现
int MID_Set_Md_Sensitivity(int mode);//保留不用实现
/******************************************************************************
*其他
******************************************************************************/
int MID_Get_Isp_Cds(void);//获取光敏值
int MID_ISP_SetAeAttr(int Val);//设置强光抑制的权重 不一定实现
int MID_SetSnapNub(int num);//设置捕获图像序号  还需调整
int MID_Set_Ir_cut(int state);//保留不用实现
int MID_WTD_Feed(void);
int MID_WTD_Init(void);

int MID_UTILS_EXEC_CMD_EX(const char *cmd, const char *expStr, int *isOK);//保留不用实现
#ifdef __cplusplus
}
#endif

#endif
