#ifndef _ASJSTD_H__
#define _ASJSTD_H__

#ifdef WIN32 //windows

#else //LINUX

#include <stdlib.h>
#include <sys/vfs.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <mntent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <errno.h>
#include <pthread.h>
#include "AsjType.h"
#endif

#ifndef ALOGD
    #define ALOGD(fmt, args...) 
#endif
#ifndef ALOGI
    #define ALOGI(fmt, args...) 
#endif
#ifndef ALOGE
    #define ALOGE(fmt, args...) 
#endif

#define CUSTOM_ENABLE		1//
#define APP_MONITOR_ENABLE
#define IPC_BROADCAST_SUPPORT

#define MAX_CLIENT_NUMBER    10
#define MAX_AVCHANNEL        16
#define MAX_SIZE_IOCTRL_BUF  1024
#define MAXRECORDFILECOUNTS  50


#define ETH0_CONNECTED         (unsigned char) (1<<0)
#define WLAN0_DISCONNECT          (unsigned char) (1<<1)
#define WLAN0_ONDISCONNECT            (unsigned char) (1<<2)
#define WLAN0_CONNECT         (unsigned char) (1<<3)
#define WLAN0_ONCONNECTED       (unsigned char) (1<<4)
#define NET_CHANGE         (unsigned char) (1<<5)
#define AP_USED     (unsigned char) (1<<6)
#define NET_LP_CONNECTED         (unsigned char) (1<<7)

#define NET_DHCP_IDLE 	0
#define NET_DHCP_ETH0 	1
#define NET_DHCP_WLAN0 	2

#define BASE_YEAR				1900

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define IN
#define OUT

#ifdef __cplusplus
extern "C" {
#endif


#define LOGINFOPATH "/opt"	 // /tmp"
#define LOGINFONAME "LogInfo"

//====================================================module define===================================================================================
//sensor
typedef enum{
	SENSOR_NULL,
	SENSOR_F23A,
	SENSOR_F23_DVP,
	SENSOR_F37,
	SENSOR_F37_DVP,
	SENSOR_SC2335,
	SENSOR_K05,
	SENSOR_Q03,
	SENSOR_GC2053,
	SENSOR_SC3335,
	SENSOR_Q03_DVP,
	SENSOR_IMX410,
	SENSOR_IMX415,
	SENSOR_GC5603,
	SENSOR_K04,
	SENSOR_K06,
	SENSOR_K17
}SENSORTYPE;

//chip
typedef enum{
	CHIP_NULL,
	CHIP_HI3516EV100,
	CHIP_HI3516EV200,
	CHIP_HI3516EV300,
}CHIPTYPE;

typedef enum _NETTYPE{
	NET_NULL = 0,
	NET_RJ45,
	NET_WIFI,
	NET_RJ45WIFI,
	NET_RJ45WIFI_NOAP,
	NET_4G_LP,
	NET_WIFI_LP
}NETTYPE;

typedef enum {
	WIFI_STA = 0,
	WIFI_AP,
	WIFI_NONE
}WIFI_MODE;

//下面设备类型是公司统一的设备类型定义
typedef enum
{
	ASJ_DEVICE_TYPE_DVR = 1,
	ASJ_DEVICE_TYPE_NVR = 2,
	ASJ_DEVICE_TYPE_POENVR = 3,
	ASJ_DEVICE_TYPE_WIRELESSNVR = 4,
	ASJ_DEVICE_TYPE_POEIPC = 5,
	ASJ_DEVICE_TYPE_BOLTIPC = 6,
	ASJ_DEVICE_TYPE_IPC = 7,
	ASJ_DEVICE_TYPE_KAPIANJI=8,
	ASJ_DEVICE_TYPE_FISHEYE =9,
	ASJ_DEVICE_TYPE_PTZIPC =10,
	ASJ_DEVICE_TYPE_C611 =	11,
	ASJ_DEVICE_TYPE_C612 = 	12,
	ASJ_DEVICE_TYPE_C199 = 	13,
	ASJ_DEVICE_TYPE_C190 = 	14,
	ASJ_DEVICE_TYPE_C199PRO = 15,
	ASJ_DEVICE_TYPE_C115 = 	16,
	ASJ_DEVICE_TYPE_C289 = 	17,
	ASJ_DEVICE_TYPE_C308 = 	18,
	ASJ_DEVICE_TYPE_C513 = 	20,
	ASJ_DEVICE_TYPE_C308_5M = 	21,
	ASJ_DEVICE_TYPE_C190Pro = 	22,
	ASJ_DEVICE_TYPE_C287 = 	23,
	ASJ_DEVICE_TYPE_C516 = 	24,
	ASJ_DEVICE_TYPE_C188 = 	25,
	ASJ_DEVICE_TYPE_C2894G = 	26,
	ASJ_DEVICE_TYPE_C296 = 	27,
	ASJ_DEVICE_TYPE_C183 = 	28,
	ASJ_DEVICE_TYPE_C518 = 	29,
	ASJ_DEVICE_TYPE_C290 = 	30,
	ASJ_DEVICE_TYPE_C182 = 	31,
	ASJ_DEVICE_TYPE_C225 = 	32,
	ASJ_DEVICE_TYPE_C296POE = 	33,
	ASJ_DEVICE_TYPE_C298 =	34,
	ASJ_DEVICE_TYPE_C299 =	35,
	ASJ_DEVICE_TYPE_C186 =	36, //黑光
	ASJ_DEVICE_TYPE_C521 =	37,//双目带屏
	ASJ_DEVICE_TYPE_C291 =	38,//低功耗 wifi
	ASJ_DEVICE_TYPE_C2914G =	39,//低功耗  4G	
}ASJ_DEVICE_TYPE;


enum {
	mod_status_ide = 0,
	mod_status_running,
	mod_status_quit,
	mod_status_quited
};

typedef enum
{
   AUTO_MODE	=0x0,  //自动帧率
   HIGH_FRAME	=0x1,  //25帧率
   MEDIUM_FRAME =0X2,  //20帧率
   LOW_FRAME	=0X3,  //15帧率
}FRAME_MODE;

typedef enum _TIME_FORMAT_E {
	HOURS_12 = 0,
	HOURS_24,
} TIME_FORMAT;
//====================================================================================================================================================

//视频制式
#define DMS_HH1080P_FORMAT_WIDTH			1920
#define DMS_HH1080P_FORMAT_HEIGHT			1080
#define NTSC								0	//60HZ
#define PAL									1	//50HZ
#define VIDEOFAMAT
//====================================================================================================================================================
typedef enum tagVS_STREAM_TYPE_E
{
    VS_STREAM_TYPE_MAIN = 0,    /**< 主码流 */
    VS_STREAM_TYPE_NETSUB,  /**< 网络子码流 */
    VS_STREAM_TYPE_MAIN_1,    /**<sensor 2主码流 */
    VS_STREAM_TYPE_NETSUB_1,  /**<sensor 2子码流 */    
    VS_STREAM_TYPE_ALEXA264,
    VS_STREAM_TYPE_SNAP,    /**< 抓拍码流 */
    VS_STREAM_TYPE_MOBILE,  /**< 手机码流 */
  //  VS_STREAM_TYPE_IP,      /**< IP码流*/
    VS_STREAM_TYPE_BUTT     /**< 非法值*/
}VS_STREAM_TYPE_E;

enum
{
    COMPT_H264    = 96,
    COMPT_H265    = 265
};

enum
{
	FMT_PIC_CIF,
	FMT_PIC_360P,	   /* 640 * 360 */
	FMT_PIC_VGA,	   /* 640 * 480 */
	FMT_PIC_640x360,
	FMT_PIC_D1_PAL,    /* 720 * 576 */
	FMT_PIC_D1_NTSC,   /* 720 * 480 */
	FMT_PIC_720P,	   /* 1280 * 720  */
	FMT_PIC_1080P	   /* 1920 * 1080 */
};

typedef enum tagVS_FRAME_TYPE_E
{
    VS_FRAME_I, //<I帧
    VS_FRAME_P, //<P帧
    VS_FRAME_B, //<B帧
    VS_FRAME_A, //<音频帧
    VS_FRAME_BUTT //<保留值

}VS_FRAME_TYPE_E;

typedef enum{
	RECTYPE_NULL,
	RECTYPE_ALLTIME,  //全时录像
	RECTYPE_ALARM,	  //报警录像
	RECTYPE_MOTION,   //移动侦测
	RECTYPE_VIDEOLOST,//视频丢失
	RECTYEP_SCHEMA,   //规划特定时间录像
}RECTYPE;

typedef enum _IMG_SET_CMD
{
	IMG_NULL,
	IMG_BRIGHTNESS,   	//亮度
	IMG_HUE,		  	//色度色调
	IMG_SATURATION,   	//饱和度，颜色
	IMG_CONTRAST,	  	//对比度
	IMG_SCENEMODE,		//模式，室内或室外
	IMG_MIRROR,			//镜像
	IMG_SETBAW,			//黑白和彩色相互切换
}IMG_SET_CMD;

typedef struct _DWD_TIME
{
  DWORD    dwYear;
  DWORD    dwMonth;
  DWORD    dwDay;
  DWORD    dwHour;
  DWORD    dwMinute;
  DWORD    dwSecond;
}DWD_TIME, *PDWD_TIME;
typedef enum
{
	Sunday,
	Monday,
	Tuesday,
	Wednesday,
	Thursday,
	Friday,
	Saturday,
	Everyday,
}dateTime_t;

typedef struct
{
    int             year;
    int             month;
    dateTime_t      week;
    int             day;
    int             hour;
    int             minute;
    int             second;

    int             result;
}sysTime_t;

typedef struct _SYSTEMSET{
	 int					nTimeZone;		//时区
}SYSTEMSET;

typedef struct _ASJSYSINFO
{
	char UID[34];				//uid
	char UserName[32];			//login name
	char Password[32];			//login pwd
}ASJSYSINFO;


typedef struct _ASJNETINFO
{

}ASJNETINFO;

typedef struct _ASJDEVICEINFO
{

	ASJSYSINFO SysInfo;
	ASJNETINFO NetInfo;

}ASJDEVICEINFO;
typedef enum tagDMS_MBUF_POS_POLICY_E
{
    DMS_MBUF_POS_CUR_READ = 0,
		//<当前读指针的位置
    DMS_MBUF_POS_LAST_READ_nIFRAME,
		//<离当前读指针最近的第n个I帧所在的块的起始位置
    DMS_MBUF_POS_CUR_WRITE,
		//<当前写指针的位置
    DMS_MBUF_POS_LAST_WRITE_nBLOCK,
		//<离当前写指针最近的第n个块的起始位置
    DMS_MBUF_POS_LAST_WRITE_nIFRAME,
		//<离当前写指针最近的第n个I帧所在的块的起始位置, n=0表示最近的I帧
    DMS_MBUF_POS_BUTT
}DMS_MBUF_POS_POLICY_E;
typedef enum __STREAM_TYPE_E
{
	DMS_MAIN_STREAM = 0, 	// 主码流
	DMS_SECOND_STREAM, 		// 子码流
	DMS_MAIN_STREAM_1, 		// 主码流(sensor 2)
	DMS_SECOND_STREAM_1, 	// 子码流(sensor 2)
	DMS_ALEXA264_STREAM, 	// alexa
	DMS_MOBILE_STREAM, 		// 
	DMS_MAX_STREAM_TYPE_NUM
}STREAM_TYPE_E;

#define NAME_MAX_LEN 32
#define MAX_SHELTER_RECT 5
#define MD_STRIDE_WIDTH     44
#define MD_STRIDE_HEIGHT    36
#define MD_STRIDE_SIZE      ((MD_STRIDE_WIDTH+4)*MD_STRIDE_HEIGHT/8) // 216 bytes  1行6 bytes 表示 44 列，或略最后4列

typedef struct {
    UINT8 Mirror;		//0~3,close ,up down,left right, up down and left right
    UINT8 SceneMode;		//indoor or out door
    UINT8 Hue;
	UINT8 Staturation;
	UINT8 Contrast;
	UINT8 Brightness;
	UINT8 CDSDelay;
	UINT8 CDSType;//0-软光敏 1-硬光敏
	UINT8 VideoStandard;			//N:60Hz,P:50Hz
}VIDEO_INFO;
typedef struct {
	float X;
	float Y;
	float Width;
	float Height;
	LONG   Color;
}SHELTER_RECT;
typedef struct {
	UINT8 Enable;
	UINT8 Channel;
	SHELTER_RECT ShelterRect[MAX_SHELTER_RECT];
}SHELTER_INFO;
typedef struct{
	UINT8 Type;			// 0:disable, 1: motion, 2: person ,3 : motion + per ,4: pnv ,5:motion + pnv ,6: per + pnv, 7: motion + per + pnv
	UINT8 Channel;
	UINT8 Sensitive;
}MOTION_DETECT;
typedef struct{
	UINT8 Type;
	UINT8 Channel;
	UINT8 ConTh[16];
	UINT32 Resolution;
	UINT32 Model_version;
}CAR_DETECT;
typedef struct{
	UINT32 DrawBoxEn;
	UINT32 DrawCarBoxEn;
	UINT32 DrawPersonBoxEn;
}DRAWBOXEN;
typedef struct{
	UINT32 HumDetBinden;
	UINT32 CarDetBinden;
	UINT32 PetDetBinden;
	UINT32 FaceDetBinden;
}DETBINDENABLE;
typedef struct{
	UINT32 CarMoveThreshold;
	UINT32 HumMoveThreshold;
	UINT32 FaceMoveThreshold;
}DETSLEEP;
typedef struct{
	UINT32 SoundDetEnable;
	UINT32 SoundDetThreshold;
}DETENABLE;
typedef struct {
	UINT8 Channel;
	UINT8 EnableTime;
	UINT8 EnableName;
	UINT8 EnableLogo;
	UINT8 SubEnableLogo;
	TIME_FORMAT TimeFormat;
	UINT32 EnFormat;
	float TimeTopLeftX;
	float TimeTopLeftY;
	float NameTopLeftX;
	float NameTopLeftY;
	float LogoTopLeftX;
	float LogoTopLeftY;
	char LogoFile[NAME_MAX_LEN];
	char LogoFile720p[NAME_MAX_LEN];
	float SubLogoTopLeftX;
	float SubLogoTopLeftY;
	char SubLogoFile[NAME_MAX_LEN];
	char 	OsdName[NAME_MAX_LEN];
}OSD_INFO;
typedef struct {
	char token[NAME_MAX_LEN];
	UINT8 OsdHandle;
	UINT8 OsdEnable;
	UINT8 OsdType;
	UINT8 PostFlag;
	UINT8 PosType;
	UINT8 TextStrFlag;
	UINT8 TextStrType;
	UINT8 TextDateFlag;
	char TextDateType[NAME_MAX_LEN];
	UINT8 TextTimeFlag;
	char TextTimeType[NAME_MAX_LEN];
	UINT8 FontSize;
	UINT32 OsdX;
	UINT32 OsdY;
	UINT8 PlainFlag;
	char OsdText[NAME_MAX_LEN];
}OVIF_OSD_INFO;
typedef struct {
	UINT8  EncodeVideo;
	UINT8  EncodeAudio;
	UINT32 CompressionType;
	UINT32 Framerate;
	UINT32 DyncFramerate;
	UINT32 StreamFormat;
	UINT32 Height;
	UINT32 Width;
	UINT32 RateType;
	UINT32 BitRate;
	UINT32 MaxKeyInterval;
	UINT32 ImageQuality;
	UINT32 FrameMode;
}STREAM_ENCODEINFO;
typedef struct {
    VIDEO_INFO VideoInfo;
	SHELTER_INFO ShelterInfo;
	MOTION_DETECT MotionDetect;
	CAR_DETECT CarDetect;
	DRAWBOXEN DrawBoxen;
	DETBINDENABLE DetBindenable;
	DETSLEEP DetSleep;
	DETENABLE DetEnable;
	OSD_INFO OsdInfo;
	OVIF_OSD_INFO OvifOsdInfo;
	OVIF_OSD_INFO OvifOsdInfo_1;
	OVIF_OSD_INFO OvifOsdInfo_2;
	OVIF_OSD_INFO OvifOsdInfo_3;
	STREAM_ENCODEINFO MainStream;
	STREAM_ENCODEINFO SecondStream;
	STREAM_ENCODEINFO MainStream_1;
	STREAM_ENCODEINFO SecondStream_1;
	STREAM_ENCODEINFO ThirdStream;
}VIDEO_CFG;
//====================================================================================================================================================
// Alarm
//====================================================================================================================================================
typedef struct _ALARM_STRUCT_{        
    UINT8  Alarm_Type;     					//告警类型0: disable, 1: motion, 2, per ， 3，person + motion
    UINT8  Alarm_Level;       				//告警级别0:关闭   1：强烈声光告警   2：轻微声光告警   3:强烈声告警        4:轻微声告警//5:强烈光告警                                                                 
    UINT8  Alarm_Man_En;   					//人为告警0:关闭1:开启
    UINT8  Night_Mode;      				//夜视模式: 0:全彩夜视1:黑白夜视2:智能夜视
    int    Output_Vol;                 		//音量大小[-57,6]
    int    Input_Vol;                 		//咪头音量大小[0,20]
    UINT8  WaterflowLed_En;
}ALARM_STRUCT, *pALARM_STRUCT;

typedef struct _IVP_ALARM_STRUCT_{        
    UINT8 EnAlarm;//保留						// 人形侦测报警		0:disable, 0x01: person	 0xf: 忽略整个结构体
    UINT8 Sensibility;//侦测灵敏度 0-7
    UINT8 ivp_flag;// 0全部1画框2斜线
    UINT16 ivp_x;
    UINT16 ivp_y;
    UINT16 ivp_w;
    UINT16 ivp_h;
}IVP_ALARM_STRUCT, *pIVP_ALARM_STRUCT;

typedef struct _CUSTOM_STRUCT_{        
    int Start_x;
    int Start_y;
    int End_x;
    int End_y;  
    int Entersou;	                			// 进去铃声 无:0   打开:1
    int Leavesou;                  			// 离开铃声 无:0   打开:1
    int  start_time;               			//时间戳
    int  end_time;                 			//时间戳
    unsigned char week;							// 星期几111 1111 ->127
    unsigned char EnAlarm;						// 自定义报警使能关闭:0   打开:1
    unsigned char Direction;                  	//方向
}CUSTOM_STRUCT, *pCUSTOM_STRUCT;

	
typedef struct {
	UINT32 EnAlarm;							// APP 设置移动报警消息推送使能。 0: 关闭，1： 打开
	UINT32 Sensibility;   
	ALARM_STRUCT AlarmStruct;  				// 报警信息体
	IVP_ALARM_STRUCT IVP_AlarmStruct; 			//人形侦测
	UINT32 EnAiAlarm;                       //ai传图使用
    char  AiUrl[256];  
    char  etk[256];  
	int    Ai_time;	
	UINT32 Intervaltime;
}ALARM_INFO; 

typedef struct {
    ALARM_INFO alarmInfo;
	CUSTOM_STRUCT CUSTOM_Custom;       //自定义报警
}ALARM_CFG;

typedef struct _TUK_MOTION_AREA	
{	
    UINT16 gridWidth;
    UINT16 gridHeight;
    char areaData[216/*MD_STRIDE_SIZE*/];		//  216 bytes     
}TUK_MOTION_AREA;

#ifdef __cplusplus
}
#endif

#endif  //#ifndef _DMS_TYPE_H__

