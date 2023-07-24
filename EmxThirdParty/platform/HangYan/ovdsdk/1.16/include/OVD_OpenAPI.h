#ifndef OVDOPENAPI_OPENAPI_H
#define OVDOPENAPI_OPENAPI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <inttypes.h>
#include <time.h>
#include <stdarg.h>

#define MAX_LEN_24  24
#define MAX_LEN_32  32
#define MAX_LEN_64  64
#define MAX_LEN_128  128
#define MAX_LEN_256  256
#define MAX_LEN_1024  1024

#define WIFI_SSID_LEN  33
#define WIFI_PWD_LEN  80

#define MAX_CHANNEL_COUNT  64   //设备支持最多的channel数量，channel编号范围为0～63

//错误码
#define  RPC_UNIMPLEMENT 101 //未实现回调

typedef enum {
    OVD_RET_STREAM_CONNECT_CHANNEL_BROKEN = -5, //底层流媒体通道出现断开

    OVD_RET_COMMON_ERROR = -1,    //通用错误码
    OVD_RET_SUCCESS = 0,           //通用成功
    OVD_RET_BADPARAMETER=1,        //入参错误
    OVD_RET_NOMEM=2,               //内存错误

    //配网相关错误码
    OVD_NETCONF_SUCCESS=7,         //配网识别数据成功         
    OVD_NETCONF_LESS_DATA=9,       //配网缺失数据，数据太长，二维码分页，需要厂商再次输入
    
    OVD_RET_RPC_UNIMPLEMENT = 101, //该功能模拟未使能或未支持

    //srt卡回放机制错误码
    OVD_RET_READ_FRAME_RETRY=201, //读帧重试错误码
    OVD_RET_READ_FRAME_EOF=202,   //卡录像读取完毕，厂商反馈EOF
    OVD_RET_SEEK_NODATA=203, //seek点无音视频数据
    
}OVD_ERRNO;



#if !defined(__cplusplus)
#define bool char
#define true 1
#define false 0
#endif



typedef char OVD_bool; 
typedef enum
{
    OVD_Video   =	0,          //视频
    OVD_Audio	=	1,          //音频
}OVDContentType;

/*
    source_STREAM_CODEC_H264,
    source_STREAM_CODEC_AAC_WITH_ADTS,
    source_STREAM_CODEC_AAC,
    source_STREAM_NO_CODEC,
    source_STREAM_CODEC_H265
*/

typedef enum {
    source_STREAM_CODEC_H264 = 0,
    source_STREAM_CODEC_AAC_WITH_ADTS = 1,
//    source_STREAM_CODEC_AAC =2,          当前sdk音频格式只支持aac with adts
    source_STREAM_NO_CODEC = 3,
    source_STREAM_CODEC_H265 = 4
}OVDAVCodec;


typedef enum {
 
    Probe_status_EXIT=-2,     //Probe模块退出      
    Probe_status_UNSTART=-1, //Probe模块未启动
    Probe_status_PAUSED=0, //Probe模块暂停
    Probe_status_STARTING=1, //Probe模块启动
}OVD_Probe_status;
typedef enum
{
    OVD_1DMODE  =   0, //低清
    OVD_SDMODE  =   1, //标清
    OVD_HDMODE  =   2, //高清
    OVD_FHDMODE  =  3, //全高清
}OVDEncodeQuality;

typedef struct
{
    OVDAVCodec codec;
    int quality;
    unsigned int bitrate;
    unsigned short width;
    unsigned short height;
    unsigned char framerate;
    unsigned char colorDepth;
    unsigned char frameInterval;
    OVD_bool rsk_encrypt;
    unsigned char reserve;

}OVDVideoDataFormat;

typedef struct
{
    OVDAVCodec codec;
    unsigned int samplesRate;
    unsigned int bitrate;
    unsigned short waveFormat;
    unsigned short channelNumber;
    unsigned short blockAlign;
    unsigned short bitsPerSample;
    int sampleperframe;
    unsigned short frameInterval;
    unsigned short reserve;
}OVDAudioDataFormat;



typedef struct
{
    char OVDDeviceID[MAX_LEN_32];               //设备ID号，必填
    char OVDHardWareModel[MAX_LEN_32];         //设备型号，必填
    char OVDSystemVersion[MAX_LEN_32];        //设备固件版本号，必填
    char wifi_ssid[WIFI_SSID_LEN];            //设备当前连接的wifi的ssid, 该字段空串表示未连接wifi，可选
    int  wifi_signal;               //设备当前wifi的信号强度, 0-100, 当wifi_ssid不为空时有效，可选
    int  upBandwidth;               //设备探测到的上行最大带宽，单位bps，不存在则表示上行带宽未知，负值表示未知，可选
    int  downBandwidth;             //设备探测到的下行最大带宽，单位bps，不存在则表示下行带宽未知，负值表示未知，可选
    char ipAddr[MAX_LEN_128];               //IP地址    <局域网IP，支持IPV6>，空值表示未知，可选
    char macAddr[MAX_LEN_32]; 	            //MAC地址，空值表示未知，可选,格式大写，以分号相隔
    int battery;  //<可选，整数： 设备当前设备电池电量, 0-100, 该字段不存在表示设备不支持电池供电>
	int cpuLoad;  //<可选，整数，cpu负载率0~100>
	int memoryTotal; //<可选，整数，总内存数，单位为KB>
	int memoryAvailable; //<可选，整数，剩余可用内存，单位为KB>
}OVDDeviceInfo;


typedef struct
{
    char passDomain[MAX_LEN_64];        //<云服务器的域名>，必填
    int  passPort;       //<云服务器的端口>，必填
    int  Secure_PassPort;  //<云服务器的加密端口>,必填
    char p2p_passDomain[MAX_LEN_64];    // <P2P pass 的域名>，必填
    int  p2p_passPort;   //<杭研p2p pass的端口>，必填
    char turnDomain[MAX_LEN_64];        //<P2P turn的域名>，没有置为空
    int  turnPort;       //<p2p turn 的端口>，没有置为-1
	char probeDomain[MAX_LEN_64];        //<软探针服务器的域名>，必填
    int  probePort;       //<软探针服务器的端口>，必填
    int  probeSecurePort;  //<软探针服务器的加密端口>,必填
    char hibernationDomain[MAX_LEN_64];        //<休眠服务地址域名>，没有置为空
    int  hibernationPort;       //<休眠服务地址端口>，没有置为-1
    int  hibernationHBInterval;       //<休眠心跳骤起>，没有置为值10s，单位为秒
    int  maxP2PSession;       //设备支持的最多的P2P流的个数，设备固有参数，服务器不能设置
}OVDNetParam;


typedef struct
{
    char OVDDeviceID[MAX_LEN_32];           //OVD设备ID，必填
    char OVDLoginPassword[MAX_LEN_64];           //OVD接入密码，必填
    char OVDMediaEncPassword[MAX_LEN_128];    //OVD视频加密密码，必填
    char OVDHardWareModel[MAX_LEN_32];      //OVD的硬件型号，必填
    char OVDSystemVersion[MAX_LEN_32];      //OVD的固件版本号，必填
    char OVDModelId[MAX_LEN_32];            //modelid,用于区分同一设备型号不同芯片方案的情况
    char OVDmacaddress[MAX_LEN_32];         //OVD的mac地址,大写，中间以分号相隔
    //int  period;                    //device记录的SDK周期上报的周期，单位：秒，必填
    OVDNetParam netParam;           //网络信息，必填
    int enable_servicschedule;      //是否启用服务调度
    char servicescheduleurl[MAX_LEN_1024];  //服务调度的URL 
    char cafilepath[MAX_LEN_256];   //客户端ca文件路径
    int tz;                        //设备时区，默认东8区
}OVDClientParam;

//软探针告警阈值
typedef struct{
	int packetLossRateThreshold;
	int avgRTTThreshold;
	int wifiQualityThreshold;
	int cpuLoadThreshold;
	int memoryLoadThreshold;
	int memoryAvaliable;
	int checkPeriod;
	int reportSwitch;	
}OVDProbeDetector;


/*能力集描述
{
  "ptz": <布尔型：是否支持云台控制能力，默认值为false>,
  "support_ptz_preset":<布尔型：是否支持云台预置位，默认值false>,
  "battery": <布尔型：是否电池供电，，默认值为false>,
  "media_protocol": <字符串；流媒体传输协议，目前可选值为：rtmp、srt，默认值为rtmp>, 
  "audio_out": <布尔型：是否支持音乐输出，默认值为false>,
  "voice_out": <布尔型：是否支持对讲输出，默认值为false>,
  "cloud_record": <布尔型：是否支持录像切片直存，默认值为false>,
  "ivrs": <布尔型：是否支持ivrs协议，默认值为false，表示云存只支持ivr协议>,
  "trace": <布尔型：是否支持移动跟踪，默认值为false>,
  "alarms": { // 告警上报相关能力
      "io": <布尔型：是否支持外包报警， 默认值为false>,
      "face": <布尔型：是否支持人脸侦测，默认值为false>,
      "face_zone":<整型:0不支持，1：只支持矩形，n(n>=3):支持不规则n边型>
      "cry": <布尔型：是否支持哭声侦测，默认值为false>,
      "voice": <布尔型：是否支持声音侦测，默认值为false>,
      "motion": <布尔型：是否支持移动侦测，默认值为false>,
      "motion_zone":<整型:0不支持，1：只支持矩形，n(n>=3):支持不规则n边型>,
      "motion_defense_strategy":<布尔型：是否支持移动区域声光联动（防御报警）配置
      "body": <布尔型：是否支持人形侦测，默认值为false>,
      "body_zone":<整型:0不支持，1：只支持矩形，n(n>=3):支持不规则n边型>,
      "body_defense_strategy":<布尔型：是否支持人形区域声光联动（防御报警）配置
      "cross": <布尔型：是否支持拌网侦测，默认值为false>,
      "cross_zone":<布尔型：是否支持伴网侦测区域设置，默认值为false>,
      "pir": <布尔型：是否支持pir红外物体移动侦测，默认值为false>,
      "lossLock":<布尔型：是否支持撬锁告警，默认值为false>,
  },
  "pic_upload_https": <布尔型：截图上传是否支持HTTPS，若不支持，则OVC下发的截图指令中的http URL为http协议，默认值为false>,
  "log_upload_https": <布尔型：日志文件上传是否支持HTTPS，若不支持，则OVC下发的日志上传指令中的http URL为http协议，默认值为false>,
  "firmware_download_https": <布尔型：固件下载是否支持HTTPS，若不支持，则OVC下发的升级指令中的http URL为http协议，默认值为false>,
  "video_encrypt": <布尔型：是否支持视频加密，默认值为false>,
  "switch": <布尔型：是否支持通道使能开关，默认值为false>,
  "led": <布尔型：是否支持led灯开关，默认值为false>,
  "SD": <布尔型： 是否支持SD卡/TF卡，默认值为true>,
  "auto_reboot": <布尔型：是否支持设备自动维护，默认值为false>
  "video_quality": <字符串列表(中间用,隔开)：支持的视频清晰度选项列表，列表元素可选值为：ld/sd/hd/fhd，分别代表低清/标清/高清/全高清，默认支持sd和hd>
  "video_formats_supportlists": <字符串列表(中间用,隔开)：支持的视频编码格式列表，列表元素可选值为：h264/h265,默认支持h264>
  "support_set_video_formats":<布尔型，是否支持平台切换视频h264,h265格式>
  
  "voiceout_volume": <布尔型： 设备是否支持对讲音量调节，默认值为false>
  "doorbell_volume": <布尔型： 是否支持设备支持门铃音量设置,默认值为false>
  "support_set_normal_nightvision_mode":<布尔型: 普通摄像头是否支持设备切换夜视模式，夜视模式分为开启/关闭/自动>
  "support_set_color_nightvision_mode":<布尔型：全彩摄像头是否支持设备切换夜视模式，夜视模式分为黑白夜视/全彩夜视/智能夜视>
  "support_alarmlight": <布尔型： 是否支持告警灯功能,默认值为false>
  "support_buzzer": <布尔型: 是否支持蜂鸣器，默认值为false>
  "support_defense_alarm": <布尔型： 是否支持防御告警时产生声音,默认值为false>
  "support_edit_defanse_alarm_content"：<布尔型，是否支持编辑防御告警时产生的声音内容，默认值为false>
  
}

*/



typedef struct
{
    OVD_bool have_alarms_io;   //<布尔型：是否支持外包报警,默认值为false>,
    OVD_bool have_alarms_face; //<布尔型：是否支持人脸侦测,默认值为false>,
    int     support_alarms_face_zone; //<整型:0不支持，1：只支持矩形，n(n>=3):支持不规则n边型>, 注：目前app限制，只需要设备支持矩形即可
    OVD_bool have_alarms_cry; //<布尔型：是否支持哭声侦测,默认值为false>,
    OVD_bool have_alarms_voice; //<布尔型：是否支持声音侦测,默认值为false>,
    OVD_bool have_alarms_motion; //<布尔型：是否支持移动侦测,默认值为false>,
    int     support_alarms_motion_zone; //<整型:0不支持，1：只支持矩形，n(n>=3):支持不规则n边型>, 注：目前app限制，只需要设备支持矩形即可
    OVD_bool have_alarms_body;  //<布尔型：是否支持人形侦测,默认值为false>,
    int     support_alarms_body_zone; //<整型:0不支持，1：只支持矩形，n(n>=3):支持不规则n边型>, 注：目前app限制，只需要设备支持矩形即可
    OVD_bool have_alarms_cross; //<布尔型：是否支持拌网侦测>,
    OVD_bool have_alarms_pir; //<<布尔型，是否支持pir侦测>,
    OVD_bool have_alarms_pir_staymode;//<<布尔型，是否支持pir逗留模式,门铃>
    OVD_bool have_alarms_lossLock; //<<布尔型，是否支持撬锁侦测>
    OVD_bool have_alarms_alertarea; //<<布尔型，是否支持警戒功能>
    int     support_alarms_alertarea_zone; //<整型:0不支持，1：只支持矩形，n(n>=3):支持不规则n边型>, 注：目前app限制，只需要设备支持矩形即可
}OVDAlarmsInfo;


//AI 能力相关
typedef struct
{
    OVD_bool face; //布尔型，设备是否支持上报人脸AI,默认值为false
    OVD_bool faceExposureAdjust; //布尔型，设备是否支持人脸曝光度调节,默认值为false
    char CaptureModeList[MAX_LEN_256]; //字符串,抓拍模式调节列表选项，若没有，则设备不支持模式切换,默认“quality”:代表质量抓拍，“quick”:代表快速抓拍,用逗号相隔
    int alarm_zone;
}OVDAIFaceCapInfo;

typedef struct
{
	OVD_bool kitchen_detection; 
    char CaptureModeList[MAX_LEN_256];
	char ClothesColorList[MAX_LEN_256];
    int alarm_zone;
}OVDAIKitchenCapInfo;

typedef struct
{
	OVD_bool vehicle_detection;
    char CaptureModeList[MAX_LEN_256];
    int alarm_zone;
}OVDAIVehicleCapInfo;


typedef struct
{
   OVDAIFaceCapInfo AIface;
   OVDAIKitchenCapInfo AIkitchen;
   OVDAIVehicleCapInfo AIvehicle;
}OVDAICapInfo;

//注：当前和家亲app声光联动只有体现在警戒功能上
typedef struct
{
    char alertarea[256];  
	char vehicle_detection[256];
    
}OVDlinkage_mode; //设备sdk 联动方式的支持，每种告警一个元素

typedef struct
{
    OVD_bool preference_call; //布尔型，是否支持呼叫偏好功能；呼叫偏好：可设置在发起呼叫时，普通电话优先或者APP通话优先
    OVD_bool set_calling_number; //布尔型，是否支持呼叫号码设置的功能；呼叫号码：可设置点击呼叫按键后，摄像头呼叫的号码（被叫号码）
    OVD_bool not_disturb_mode; //布尔型，是否支持电话勿扰模式；电话勿扰：可设置设备勿扰开关是否打开，如果打开，可设置勿扰时间
    OVD_bool auto_hangup; //布尔型，是否支持自动挂断功能；自动挂断：可设置检测到访客离开/进门后自行停止呼叫
}OVDHJKH_mode; //设备上对和家看护功能的功能支持配置

typedef struct
{
    OVD_bool have_ptz;//<布尔型：是否支持云台控制能力,默认值为false>
    OVD_bool support_ptz_preset;//<布尔型：是否支持云台预置位,默认值为false>
    OVD_bool have_battery; //<布尔型：是否电池供电,主要用于门铃设备,默认值为false>,

    OVD_bool have_audio_out; //<布尔型：是否支持音乐输出,默认值为false>
    OVD_bool have_voice_out; //<布尔型：是否支持对讲输出,默认值为false>
    OVD_bool have_ivrs;//<布尔型：是否支持ivrs协议，默认值为false，表示云存只支持ivr协议>,
    OVD_bool have_trace; //<布尔型：是否支持移动跟踪,默认值为false>

    OVDAICapInfo  ovdAICapInfo; //AI相关
    //alarms 告警上报相关能力
    OVDAlarmsInfo ovdCapInfo_alarms; 


    //联动方式的支持
    OVDlinkage_mode ovdlinkage_mode; 
    OVD_bool log_upload_https;//<布尔型：日志文件上传是否支持HTTPS，若不支持，则OVC下发的日志上传指令中的http URL为http协议，默认值为false>,
    OVD_bool firmware_download_https;//<布尔型：固件下载是否支持HTTPS，若不支持，则OVC下发的升级指令中的http URL为http协议，默认值为false>,
    OVD_bool video_encrypt; //是否支持视频加密，默认值为false, 视频加密分两个值与，一个是厂商是否支持,主要考虑厂商的性能等因素是否支撑的了视频加密，第2个是设备sdk是否支持视频加密实现
    OVD_bool have_switch; //是否支持通道使能开关，默认值为false>,
    OVD_bool have_sd;   //是否支持SD卡/TF卡，默认值为true>,
    OVD_bool have_led; //是否支持led灯开关，默认值为false>,
    OVD_bool have_support_softprobe; //是否支软探针，默认值为false>,
    OVD_bool have_auto_reboot;
    //支持的视频清晰度选项列表，列表元素可选值为：ld/sd/hd/fhd，分别代表低清/标清/高清/全高清，默认支持sd和hd>
    char video_quality[MAX_LEN_256];
     //<字符串列表(中间用,隔开)：支持的视频编码格式列表，列表元素可选值为：h264/h265,默认支持h264>
    char video_formats_supportlists[MAX_LEN_256];

    //<布尔型，是否支持平台切换视频h264,h265格式>
    OVD_bool support_set_video_formats;

    OVD_bool have_voiceout_volume; //<布尔型： 设备是否支持对讲音量调节，默认值为false>
    OVD_bool have_doorbell_volume;
    OVD_bool support_set_normal_nightvision_mode;//<布尔型: 普通摄像头是否支持设备切换夜视模式，夜视模式分为开启/关闭/自动>
    OVD_bool support_set_color_nightvision_mode;//<布尔型：全彩摄像头是否支持设备切换夜视模式，夜视模式分为黑白夜视/全彩夜视/智能夜视>

    //osd相关
    OVD_bool osd_text; //布尔型，是否支持持文本水印
    OVD_bool osd_logo; //布尔型，是否支持水印图片
    OVD_bool enable_hjkh; //布尔型，设备是否集成了和家固话，默认值为false
    OVDHJKH_mode hjkh_mode;// 设备和家看护相关的功能

}OVDCapInfo;



//日志输出基本依次增高
typedef enum{
    OVD_LOGLEVEL_TRACE = 0,
    OVD_LOGLEVEL_DEBUG = 1,
    OVD_LOGLEVEL_INFO  = 2,
    OVD_LOGLEVEL_WARN  = 3,
    OVD_LOGLEVEL_ERROR = 4,
    OVD_LOGLEVEL_FATAL = 5,
}OVDLogLevel;

//日志输出位置
typedef enum{

    OVD_LOGSTD_NO = 0,  //不输出
    OVD_LOGSTD_OUT = 1,  //输出到标准输出
    OVD_LOGSTD_ERR  = 2,  //输出到标准出错
}OVDLogSTD;

typedef struct
{
    OVDLogLevel logLevel;           //日志输出级别，详细见枚举值LogLevel，可选
    OVDLogSTD   logSTD;             //日志输出位置，可选，详细见枚举值LogSTD，可选
    void (*pOVDLogOutCallBack)(const char* buff);  //设备提供的日志输出回调，SDK的输出日志可以保存到device的存储文件中，可选，若未空为不支持
}OVDLogParam;

typedef enum
{
    OVD_STATUS_IDLE	          =	  0,  //空闲态
    OVD_STATUS_DOWNLOADING	  =	  1,  //安装包下载中
    OVD_STATUS_INSTALLING     =   2,  //安装中
    OVD_STATUS_FAILED         =   3,  //升级失败
    OVD_STATUS_DONE           =   4,  //安装成功
}OVDUpgradeStatus;


typedef struct
{
    int channel;                    //通道号
    char FileName[MAX_LEN_256];
    int  FileType; 			    	//文件类型(0 视频文件， 1 告警文件)
    unsigned long long FileStartStamp;		    //录像开始时间（单位为秒）
    unsigned long long FileEndStamp;			//录像接收时间（单位为秒）
    int  RecordDuration; 			//时长（单位为秒）
    int  FileSize; 					//文件大小
}OVDRecordFileInfo;

typedef struct
{
    int               fileCount;            //文件数量
    OVDRecordFileInfo fileinfo[MAX_LEN_24];        //文件列表
}OVDRecordFileListPerPage;


typedef enum
{
    OVD_CONTINUE	=	0,  //继续播放
    OVD_PAUSE	    =	1,  //暂停
    OVD_STOP        =   2,  //停止
    OVD_FAST        =   3,  //快进
    OVD_SLOW        =   4,  //慢放
    OVD_JUMP        =   5,  //拖动  ms
}OVDCONTROLTYPE;

typedef struct
{
    int  SDExist;		    //0 not, 1 yes, 2 error
    int	 SDTotalSize;	    //总容量(M)
    int	 SDFreeSize;	    //空闲量
    char EarlyFileName[MAX_LEN_24]; //当前SD卡最早一个录像文件
}OVDSDInfo;

typedef enum
{
    OVC_PTZ_MV_UP        = 0,   //向上
    OVC_PTZ_MV_DOWN      = 1,   //向下
    OVC_PTZ_MV_LEFT      = 2,   //向左
    OVC_PTZ_MV_RIGHT     = 3,   //向右
    OVC_PTZ_MV_UPLEFT    = 4,   //左上
    OVC_PTZ_MV_UPRIGHT   = 5,   //右上
    OVC_PTZ_MV_DOWNLEFT  = 6,   //左下
    OVC_PTZ_MV_DOWNRIGHT = 7,   //右下
    OVC_PTZ_ZOOM_IN      = 8,   //拉近
    OVC_PTZ_ZOOM_OUT     = 9,   //拉远
    OVC_PTZ_MV_STOP      = 10,    //停止运动
    OVC_PTZ_GOTO_PRESET  = 11,   //跳转预置位
    OVC_PTZ_SET_PRESET   = 12,   //设置预置位点
    OVC_PTZ_CLEAR_PRESET = 13,   //清除预置位点
    OVC_PTZ_MV_UP_STEP   = 14,   //单步上
    OVC_PTZ_MV_DOWN_STEP   = 15, //单步下
    OVC_PTZ_MV_LEFT_STEP   = 16,  //单步左
    OVC_PTZ_MV_RIGHT_STEP  = 17,  //单步右
    OVC_PTZ_MV_UPLEFT_STEP  = 18,  //单步左上
    OVC_PTZ_MV_UPRIGHT_STEP  = 19,  //单步右上
    OVC_PTZ_MV_DOWNLEFT_STEP  = 20,  //单步左下
    OVC_PTZ_MV_DOWNRIGHT_STEP  = 21,  //单步右下
    OVC_PTZ_ZOOM_IN_STEP       = 22,  //单步拉近
    OVC_PTZ_ZOOM_OUT_STEP      = 23,  //单步拉远
}OVCPTZControlCmd;

typedef enum{
    OVD_PCM          = 0,
}OVDAUDIOPLY_TYPE;

typedef struct
{
    unsigned int codec;         //OVDAUDIOPLY_TYPE
    unsigned int samplesRate;
    unsigned int bitWidth;
    unsigned int volume;
    unsigned int priority;      //the biger ,the higher
}OVDAudioOutDataFormat;

typedef enum
{
    MP3_CLOSE	=	0,  	 //停止播放
    MP3_PAUSE	=	1,  	 //暂停播放
    MP3_RESUME	= 	2,    //恢复播放
    MP3_OTHER,
}OVDMp3PlayCtrl;

typedef enum
{
    OVD_PLAY_STOPPED		=	0,  //停止播放状态
    OVD_PLAY_PAUSING		=	1,  //暂停状态
    OVD_PLAY_PLAYING		=	2,  //播放中
}OVDAudioPlayStatus;

typedef struct
{
    char *buf;    			//数据buf
    int  size;    			//数据长度
    //char		  ImageUrl[1024];	//目前没用到,可填空
}OVDImageInfo;

typedef enum
{
    OVD_OUTTER  =   2,      //外部告警
    OVD_MOTIOM	= 	3,      //移动侦测
    OVD_CROSS	= 	4,      //拌网侦测
    OVD_CRY		=	5,      //哭声侦测
    OVD_FACE	=	6,      //脸部识别
    OVD_VOICE	=	7,      //声音侦测
    OVD_LOW_BATTERY	=	8,      //低电告警
    OVD_LOSS_LOCK	=	9,      //撬锁告警
    OVD_BELL	=	10,      //按铃事件
    OVD_MAN     =    11,     //人形事件
    OVD_PIR     =    12,    //pir移动侦测
    OVD_ALERTAREA =   13,   //警戒功能告警
    OVD_OTHER,
}OVDAlarmType;

typedef struct
{
    int             channel;            //通道号
    long long    startTimeStamp;	   //报警开始时间戳 (单位为秒)
    OVDAlarmType	AlarmType;     //报警类型
    char*           desc;               //告警描述
    OVDImageInfo	ImageInfo;	   //背景图信息
}OVDUpLoadAlarmInfo;

typedef enum
{
    OVD_WIFI = 0,//IWiFi
    OVD_SSID_WIFI = 1,//ssid和WIFI密码
    OVD_PHONE = 2,//bindid
    OVD_STRING = 3,
    OVD_XX_SSID_WIFI = 7, //数据读取完整(包含ssid，密码及phoneid/bindid)
    OVD_XX_LESS_DATA = 9 //厂商继续输入新的图片或者字符串
} OVDWifiInfoType;

typedef struct
{
    char ssid[33];
    int ssidLen;
    char pwd[80];
    int pwdLen;
}OVDSSIDWiFiInfo;

typedef struct
{
    char ssid[WIFI_SSID_LEN];   //wifi的ssid
    int  ssidLen;
    char pwd[WIFI_PWD_LEN];    //wifi密码
    int  pwdLen;
    char phone[16+1];
}OVDXXSSIDWiFiInfo;

typedef enum
{
    OVD_HIBERNATE_OVC_NOTIFY = 0,   //OVC信令交互，需要保持一段时间不断电
    OVD_HIBERNATE_P2P_NAT = 1,      //P2P开始NAT穿透打洞
    OVD_HIBERNATE_P2P_OPEN = 2,     //正在P2P通话
    OVD_HIBERNATE_OTHER = 3
}OVDHibernateReason;

typedef struct
   {

       long long StartStamp;           //录像开始时间（单位为秒）
       long long EndStamp;         //录像接收时间（单位为秒）

   }OVD_DMERecordFileInfo;

   typedef struct
   {
       int               Count;            //卡录像片段个数
       OVD_DMERecordFileInfo sectioninfo[64];        //卡录像一个连续区间的信息
   }OVD_DMERecordFileListPerPage;

/*
 *设备配置信息定义
 *json格式
 *
 {
 "channels": {
 "通道号": {           //通道号（非负整数）作为键
   "on": <可选，可读可写,布尔型：通道使能开关，通道关闭后，应停止该通道的流媒体采集、告警、云台操作等相关功能，若OVD不具备逻辑使能能力，该字段不存在>
   "video_encoding":{    //视频相关编码配置     
     "encoder": <必填，可读可写，字符串：视频编码格式，目前支持h264、h265>
     "quality": <必填，可读可写，字符串；可选值为：ld、sd、hd、fhd，分别代表低清，标清，高清，全高清>
     "fps": <可选，只读，整形：每秒帧数>
     "bitrate": <可选，只读，整形：码流比特率>
     "width": <可选，只读，整形：图像宽度像素>
     "height": <可选，只读，整形：图像高度像素>
     "gop": <可选，只读，整形：码流gop,单位帧>
     "encrypt": <可选，客端可写，布尔型：是否对视频码流进行加密，默认为false>
   }
   "audio_encoding":{      //音频相关编码配置    
     "encoder": <必填，可读可写，字符串：音频编码器名称，目前仅支持aac>
     "sample_rate": <可选，只读，整形：采样率，即每秒钟采用数目，合法值8000/16000/32000/44100/48000>
     "bitrate": <可选，只读，整形：码流比特率>
     "bits_per_sample": <可选，只读，整形：位宽，即每个sample的比特数>
     "sample_per_frame": <可选，只读，整形：每一帧中包含的sample数，AAC算法标准固定为1024>
     "channel": <可选，只读，整形：声道数>   
   }       
   "image":{       //图像相关配置   
     "horflip":  <必填，可读可写, 布尔型：水平翻转>
     "verflip":  <必填，可读可写, 布尔型：垂直翻转>     
   }
   "AI":{   //设备是否支持AI能力
     “face”: //人脸AI上报
             {
         "on"://<必填，布尔型，是否上报AI平台>
       "exposure_on": <必填，可读可写，布尔型，人脸曝光度调节功能开关
       "exposure_adjust":<必填，可读可写，整型，人脸曝光度调节，0~100>
       "mode":<可选，可读可写，字符串，人脸抓拍模式，“quality”:代表质量抓拍，“quick”:代表快速抓拍
       "quality_value":<可选，可读写，整形，选择质量抓拍时，人脸质量抓拍的阈值设置，0~100>
     }
   }
   "alarms":{
     "io": {           //外部报警配置，若OVD不具备该能力，该字段不存在
       "on":  <必填，可读可写,布尔型：使能开关>
       "sensitivity":  <必填，可读可写,整型：探测灵敏度， 0 - 100>
     }
     "face": {          //人脸识别配置，若OVD不具备该能力，该字段不存在
       "on":  <必填，可读可写,布尔型：使能开关>
       "sensitivity":  <必填，可读可写,整型：探测灵敏度， 0 - 100>
 
       "zone": [  //若支持区域配置则有该配置，将图形10000等分
                         {//每个区域由各个顶点组成，顶点需要按照图像的顺时针或逆时针排序
                  "areaId":  //areaid 从0开始
                "point": [
                 {
                     "pointId":   //顶点id  从0开始
                   "x”:
                   "y":
                },
                ...
                ]
            },
            ...         
                 ]
       
     }
     "cry": {           //哭声侦测配置，若OVD不具备该能力，该字段不存在
       "on":  <必填，可读可写,布尔型：使能开关>
       "sensitivity":  <必填，可读可写,整型：探测灵敏度， 0 - 100>  
     }       
     "voice": {         //声音侦测配置，若OVD不具备该能力，该字段不存在
       "on":  <必填，可读可写,布尔型：使能开关>
       "sensitivity":  <必填，可读可写,整型：探测灵敏度， 0 - 100>    
     }
     "motion": {        //移动侦测配置，若OVD不具备该能力，该字段不存在
       "on":  <必填，可读可写,布尔型：使能开关>
       "sensitivity":  <必填，可读可写,整型：探测灵敏度， 0 - 100>   
       "zone": [  //若支持区域配置则有该配置，将图形10000等分
                         {//每个区域由各个顶点组成，顶点需要按照图像的顺时针或逆时针排序
                  "areaId":  //areaid 从0开始
                "point": [
                 {
                     "pointId":   //顶点id  从0开始
                   "x”:
                   "y":
                },
                ...
                ]
            },
            ...         
                 ]
 
     }
     "cross": {         //拌网配置，若OVD不具备该能力，该字段不存在
       "on":  <必填，可读可写,布尔型：使能开关>
       "sensitivity":  <必填，可读可写,整型：探测灵敏度， 0 - 100>
       "zone": [  //若支持区域配置则有该配置，将图形10000等分
                         {//每个区域由各个顶点组成，顶点需要按照图像的顺时针或逆时针排序
                  "areaId":  //areaid 从0开始
                "point": [
                 {
                     "pointId":   //顶点id  从0开始
                   "x”:
                   "y":
                },
                ...
                ]
            },
            ...         
                 ]
     }
     "body": {         //人形侦测配置，若OVD不具备该能力，该字段不存在
       "on":  <必填，可读可写,布尔型：使能开关>
       "sensitivity":  <必填，可读可写,整型：探测灵敏度， 0 - 100>
       "zone": [  //若支持区域配置则有该配置，将图形10000等分
                         {//每个区域由各个顶点组成，顶点需要按照图像的顺时针或逆时针排序
                  "areaId":  //areaid 从0开始
                "point": [
                 {
                     "pointId":   //顶点id  从0开始
                   "x”:
                   "y":
                },
                ...
                ]
            },
            ...         
                 ]
       "linkage_strategy":  //告警联动策略，若不支持则无此选项
       {         
          "speech":
          {
                     "on":<可选，可读可写，布尔型，是否联动语音播报>
                     "vol":<可选，可读可写，整形，语音播放音量>
                     "repeat":<可选，可读可写，整形，语音播放次数>
                     "url":<可选，可读可写，字符串，语音内容的获取url>
         }
 
         
        "light":
         {
                 "on":<可选，可读可写,布尔型，是否联动警灯>
                 "mode":<可选，可读可写，整形，告警灯亮灯模式，0：常亮  1：闪烁>
                 "dur":<可选，可短可写，整形，告警灯单次亮灯时长，单位：秒>
         }
         
              "buz":
          {
                     "on":<可选，可读可写，布尔型，是否联动蜂鸣器>
                     "dur":<可选，蜂鸣器报警时长，单位为秒>
          }
             }
     }
     
     "alertarea": {         //警戒区域告警，若OVD不具备该能力，该字段不存在
       "on":  <必填，可读可写,布尔型：使能开关>
       "sensitivity":  <必填，可读可写,整型：探测灵敏度， 0 - 100, 目前app无该配置项>
       "alerttime":<可选，警戒时间段，可读可写>
       {
                     "on":<可选，可读可写，布尔型，是否开启警戒时间段的配置>
         "starttime":<可选，可读可写，字符串，“hh:mm:ss” 格式>
         "endtime":<可选，可读可写，字符串，“hh:mm:ss” 格式> //当endtime早于starttime，就是跨天
         "repeat": <必填，字符串数组: 计划重复的星期号，数组内的的字符串枚举值可选为Mon/Tue/Wed/Thu/Fri/Sat/Sun>
       }
       "zone": [  //若支持区域配置则有该配置，将图形10000等分
                         {//每个区域由各个顶点组成，顶点需要按照图像的顺时针或逆时针排序
                  "areaId":  //areaid 从0开始
                "point": [
                 {
                     "pointId":   //顶点id  从0开始
                   "x”:
                   "y":
                },
                ...
                ]
            },
            ...         
                 ]
       "linkage_strategy":  //告警联动策略，若不支持则无此选项
       {         
          "speech":
          {
                     "on":<可选，可读可写，布尔型，是否联动语音播报>
                     "vol":<可选，可读可写，整形，语音播放音量>
                     "repeat":<可选，可读可写，整形，语音播放次数>
                     "url":<可选，可读可写，字符串，语音内容的获取url>
         }
 
         
        "light":
         {
                 "on":<可选，可读可写,布尔型，是否联动警灯>
                 "mode":<可选，可读可写，整形，告警灯亮灯模式，0：常亮  1：闪烁>
                 "dur":<可选，可短可写，整形，告警灯单次亮灯时长，单位：秒>
         }
         
              "buz":
         {
                     "on":<可选，可读可写，布尔型，是否联动蜂鸣器>
                     "dur":<可选，蜂鸣器报警时长，单位为秒>
         }
         }
     }
     
     "pir": {   //红外物体移动侦测，若OVD不具备该能力，该字段不存在
         "on": <必填，可读可写,布尔型：使能开关>
       "sensitivity": <必填，可读可写,整型：探测灵敏度， 0 - 100> 
       "staymode_time":<必填，可读可写，整形，单位为秒，若ovd不具备逗留模式设置，则该字段不存在>
     }
     "losslock"{ //防拆告警
         "on":<必填，可读可写，布尔型，使能开关>
     }
   }
   "audio_out_volume": <可选，可读可写，整数：扬声器输出音量，0-100，若该字段不存在表示设备不支持音量调节>
   "doorbell_volume": <可选，可读可写，整数：扬声器输出音量，0-100，若该字段不存在表示设备不支持门铃音量调节>
   "trace":  <可选，可读可写,布尔型：移动跟踪, 若该字段不存在，则表示设备不支持移动追踪>
   "switch_schedule": {   //可选，通道定时使能计划，若设备不支持，该字段不存在
     "on":  <必填，可读可写,布尔型：定时使能计划开关>
     "start_time": <必填，可读可写,字符串型：通道使能时间，格式“HH:MM:SS”>
     "shutdown_time": <必填，可读可写,字符串型：通道关闭时间，格式“HH:MM:SS”>
     "repeat": <必填，字符串数组: 计划重复的星期号，数组内的的字符串枚举值可选为Mon/Tue/Wed/Thu/Fri/Sat/Sun>
   }
   "osd": {   //可选， 通道视频的OSD配置，若设备不支持OSD，该字段不存在
     "text": <必填，可读可写, 字符串类型：OSD文本内容，若为空串，则当前没有OSD显示>
     "font": <必填，可读可写，整型：字号大小，1为小字号，2为大字号>
     "pos": <必填，可读可写，字符串枚举型：OSD位置，可选值为top-left/top-right/bottom-left/bottom-right
   }
   "normal_nightvision_mode": <可选，可读可写，整形，0：自动，1：开启，2：关闭，若OVD不具备普通摄像头夜视模式配置功能，则该字段不存在> 
   "color_nightvision_mode": <可选，可读可写，整形，0：黑白夜视，1：全彩夜视，2：智能夜视，若OVD不具备彩色摄像头夜视模式配置功能，则该字段不存在>
   "extension": {
     <设备厂家扩展内容，厂家可自定义该对象字段内容>
   }
 }
 ...
 }
 "log_level": <必填，可读可写，整数：输出日志级别，0:trace/1:debug/2:info/3:warn/4:error/5:fatal>
 "tz": <必填，可读可写，整数：时区号，东为正数，西为负数。例如东八区为+8>
 "auto_reboot": {   //自动维护（重启）能力，若OVD不具备该能力，则该字段不存在
     "on": <必填，可读可写,布尔型：使能开关>
     "cycle": <必填，可读可写,整型：自动维护（重启）的最短周期，单位秒，例如7天自动重启，可以设置为604800>    
     "start": <必填，可读可写,字符串：自动维护（重启）开始时间，格式“HH:MM:SS”>    
     "end": <必填，可读可写,字符串：自动维护（重启）结束时间，格式“HH:MM:SS”>   
     "last_reboot": <必填，只读,字符串型：上次重启时间，格式”YY-MM-DDTHH:MM:SS”>     
 }
 "led": <可选，可读可写,布尔型：Led灯是否打开。若OVD不具备LED灯控制能力，该字段不存在>
 "softprobe_on":<可选，可读可写，布尔型，软探针模块开关，固化到设备>
 "dutMTU": <可选，可读可写，整形，设备出口网卡MTU设置，主要为了应付部分网络情景下，大包被网络节点拦截导致通讯失败>
 "extension": {
     <设备厂家扩展内容，厂家可自定义该对象字段内容>
 }
 
 }

 *
 */


/*简单的ovd信息
"state": <必填，整数；OVD的状态，1：在线，2：升级中，3：异常>
  "channels": [
    {
      "channel": <必填，整数>,
      "state": <必填，整数；该channel的状态，0：离线，1：在线， 3：异常，4：不使能>,
    }
*/

//ovd 状态
enum ovd_state {
    OVD_ONLINE = 1, //设备在线
    OVD_UPDATING=2,  //设备升级
    OVD_ABNORMAL=3 //异常
};


//channels ??
enum ovd_channel_state {
    OVD_CHANNEL_OFFLINE=0, //离线
    OVD_CHANNEL_ONLINE=1, //在线
 
    OVD_CHANNEL_ABNORMAL=3, //异常
    OVD_CHANNEL_DISABLE=4,   //不使能
};


typedef struct{
    int channelnum;
    enum ovd_channel_state state;    
} Simplechannelinfo;

typedef struct{
    enum ovd_state state;  //1：设备在线        2：设备升级        3：异常
    Simplechannelinfo channelarray[64];  //用静态数组，最多支持64个channel,不用malloc
    int channelcount;
} SimpleOVDinfo;


//数据帧信息
typedef struct{
    OVD_bool iskey_frame; //是否是I帧

    unsigned char rev[3];  //预留1

    unsigned short width;  

    unsigned short height;

    unsigned short framerate;  //帧率

    unsigned short gop;   

    unsigned char rev2[20]; //预留2

    
}OVD_VideoFrameInfo;

typedef struct{

    unsigned int samplesRate; //采样率

    unsigned short channelNumber; //声道数

    unsigned short bitsPerSample; //位深

    int samplePerFrame; //每帧采样点数

    unsigned char rev[20]; //预留

}OVD_AudioFrameInfo;


typedef struct{

    OVDContentType frame_type;//帧类型，音频or 视频

    OVDAVCodec codec_type;//编码类型

    long long timestamp; //绝对时间戳

    OVD_VideoFrameInfo video_info;

    OVD_AudioFrameInfo audio_info; 

    unsigned int framebuf_len; //帧长度

    char *frame_buf; //帧内容

    unsigned char rev[8]; //预留

}OVD_FrameInfo;




typedef struct{

    /*
     * 调用时机：OVC获取OVD设备信息时调用
     * 功能介绍：根据结构体OVDDeviceInfo的定义，返回相应的设备参数信息，包括设备ID、软硬件版本信息、wifi信息、ip\mac等信息
     *
    **参数说明:
    **    [out]deviceInfo:    需要设备返回的信息，详细见结构体OVDDeviceInfo
    **
    **返回值：
    **    成功：0
    **    失败：-1
    */
    int (*OVD_GetOVDDeviceInfo)(OVDDeviceInfo *deviceInfo);


    /*
    *  ####接口描述：
        OVC可以通过该命令获取OVD的配置。
        RPC ovc-->ovd
    *  ####参数说明：
       [out] output_ovdconfig: 字符串，厂商负责分配内存malloc,由设备sdk负责释放
       [out] output_size:  字符串长度
    
    *   ###返回值：
    *       成功：0 
    *       失败：其他值
    */

    int (*OVD_GetOVDConfigureInfo)(char** output_ovdconfig, int *output_size);


    /*
     * 调用时机：OVC对相应的参数进行修改时，通过此接口下发需要修改的参数
     * 功能介绍：OVD根据收到的参数信息，启用相应配置的参数。没有携带的信息，设备保持原参数配置
     *
     *
    **参数说明:
    **    [in]in_ovdconfig:    需要设置的信息，json格式信息，见具体反馈信息如上“设备配置信息定义”说明
    **
    **返回值：
    **    成功：0
    **    失败：-1
    */
    int (*OVC_SetOVDConfigureInfo)(char* in_ovdconfig);


    /*
    * 调用时机：OVC相应的地址发生变动时，通过此接口通知OVD
    * 功能介绍：OVD需要持久化配置的服务器地址信息，并根据配置信息，断开原来的连接，根据新的服务器地址建立新的连接；其中地址为空的为未返回数据，OVD继续使用原配置
    *
    **参数说明：
    *     [out]netParam:     OVC反馈的服务信息，详细见结构体OVDNetParam定义，其中地址为空的为未返回数据，此时设备继续使用原配置
    **返回值：
    *    成功：0
    *    失败：-1
     */
    int (*OVD_SetServerInfo)(OVDNetParam *netParam);


    /*
     * 调用时机：当SDK与OVC之间的连接状态发生变化时（连接上/连接断开），通过此接口通知OVD
     * 功能介绍：OVD根据状态的通知，决定相应处理
                要求厂商处理成异步，不能阻塞
     *
    **参数说明:
    **    [in]connectStatus:    //0:连接成功     -1:连接失败   1:重连中
    **
    **返回值：
    **    无
    */
    void (*OVD_OVCConnectStatus)(int connectStatus);


    /*
     * 调用时机：当用户通过APP远程控制channel重启时，调用此接口
     * 功能介绍：OVD根据入参，重新启动相应的channel；若OVD不支持单channel重启，那么就重启设备
     *
    **参数说明:
    **    [in]channel:    若重启channel，则为需要重启的channel。 注：如设备不支持单独重启channel，则直接重启设备。
    **
    **返回值：
    **    成功：0
    **    失败：-1
    */
    int (*OVD_ReBootChannel)(int channel);


    /*
     * 调用时机：当用户通过APP远程控制设备重启时，调用此接口
     * 功能介绍：OVD重启设备
     *
    **参数说明:
    **    无
    **
    **返回值：
    **    成功：0
    **    失败：-1
    */
    int (*OVD_ReBootDevice)();


    /*
     * 调用时机：OVC可以通过该方法阻止设备（及指定通道）在指定时间内休眠，或者唤醒设备（及指定通道）当前所有休眠的部件
     * 功能介绍：OVD收到此指令后，应该在指定的过期时间内，保证设备（及指定通道）能够完全正常上电工作。收到此命令expired秒后，由设备自行决定是否进入休眠状态
     *
    ** 参数说明：
    **    [in]channel:           需要保持不休眠的channel
    **    [in]notAllowHibernate:    是否允许进入休眠状态，0：允许进入休眠，进入休眠时间根据expired参数确定；1：不允许进入休眠，此种情况下，不管expired设置为多少，设备都要保持上电状态，不休眠，直至收到下一个休眠指令
    **    [in]expired:           从收到命令起，到expired的时间内，保持不休眠
    **    [in]reason:            设置不休眠的原因，详见枚举值定义OVDHibernateReason
    ** 返回值：
    **    成功：0
    **    失败：-1
    */
    int (*OVD_KeepAwakenUtilExpired)(int channel, int notAllowHibernate, int expired, OVDHibernateReason reason);


    /*
     * 调用时机：当OVC决定设备恢复出厂配置时，调用此接口
     * 功能介绍：设备收到该请求后，应该将所有配置恢复到出厂状态（包括wifi配置），但不能断开当前网络连接，并返回成功应答。 OVC稍后会再下发一个重启指令将设备重启，默认配置生效。
     *
    **参数说明:
    **   
    **
    **返回值：
    **    成功：0
    **    失败：-1
    */
    int (*OVD_ResetConfiguration)();


    /*
     * (p2p接口)
     * 调用时机：OVC查看OVD上SD卡的录像文件信息列表时，调用此接口
     * 功能介绍：OVD根据入参的配置，查询SD卡中的文件列表，并返回文件列表信息
     *
    **参数说明:
    **    [in]channelmask1:   需要查询的通道mask，对应低位号的通道         （弃用)
    **    [in]channelmask2:   需要查询的通道mask，对应高位号的通道 弃用
    **    [in]recordTypemask:     文件类型（第0位：视频文件 第1位：告警文件） 弃用
    **    [in]startStamp:     录像查询的起始时间戳(单位为秒)
    **    [in]endStamp:       录像查询的结束时间戳(单位为秒)
    **    [in]page:           查询的页码
    **    [in]numInPage:      每页的条数
    **    [out]fileInPage:    录像文件列表信息，详细可见结构体描述OVDRecordFileListPerPage
    **
    **返回值：
    **    成功：0
    **    失败：-1
    **
    **其他说明:
    **    假设查询的录像文件数有200个，numInPage=10，则最大页码Page为20。若传进的参数numInPage=10，page=2，则fileInPage应该返回第10个到第20个录像的信息;若传进的参数numInPage=10，page=21，则fileInPage返回空录像信息(录像个数为0)
    */
    int (*OVD_QueryRecordPage)(unsigned int channelmask1,unsigned int channelmask2,unsigned int recordType, unsigned long long StartStamp,unsigned long long EndStamp,int Page,int numInPage,OVDRecordFileListPerPage *FilePage);


    /*
     * (p2p接口)
     * 调用时机：OVC根据查询到的SD卡文件列表信息，决定查看一个文件详细信息，调用此接口
     * 功能介绍：OVD根据入参的文件名称，读取SD卡上相应文件的音视频数据参数信息及录像时长，反馈到出参
     *
    **参数说明:
    **    [in]channel:          通道号
    **    [in]recordname:       录像文件名称
    **    [out]videoInfo:       视频信息,详细可见结构体描述OVDVideoDataFormat
    **    [out]audioInfo:       音频信息,详细可见结构体描述OVDAudioDataFormat
    **    [out]fileTotalTime:   该录像的时长(秒)
    **
    **返回值：
    **    成功：0
    **    失败：-1
    */
    int (*OVD_OpenRecordFile)(int channel,char* recordname,OVDVideoDataFormat* videoInfo,OVDAudioDataFormat* audioInfo,int* fileTotalTime);


    /*
     * (p2p接口)
     * 调用时机：OVC根据查询到的SD卡文件列表信息，控制相应音视频文件的播放、停止、暂停等信息
     * 功能介绍：OVD根据控制信息，控制文件的内容是否通过OVD_SendRecordAVContent上传文件内容
     *
    **参数说明:
    **    [in]channel:         通道号
    **    [in]controlType:     播放控制，详细可见枚举类型OVDCONTROLTYPE
    **    [in]value:           额外值，目前只有视频拖动时会用到，代表要跳至的视频时间戳(ms)
    **
    **返回值：
    **    成功：0
    **    失败：-1
    */
    int (*OVD_RecordCotrol)(int channel,OVDCONTROLTYPE controlType,int value);

    //录像文件删除不使用
    /*
     * (p2p接口)
     *
    **参数说明:
    **    [in]recordname:      待删除的录像名称
    **
    **返回值：
    **    成功：0
    **    失败：-1
    */
    //int (*OVD_RecordFileDelete)(void *userData,char* recordname);


    //srt方案 卡录像机制,卡录像信令切换到DME（websocket)
   


    /*
    ###接口描述：
        RecordSearch
        OVC可以通过该指令后去获取指定时间区间的卡录像记录
        RCP OVC-->OVD
    ### 参数说明
    [in] channel: 通道
    [in] starttime: 查询的起始时间,单位为秒
    [in] endtime: 查询的结束时间，单位为秒
    [in] page: 查询的页面索引，若一次查询区间不够，sdk会回调第二次
    [in] numinpage: 该页面最大的区间数量
    [out] fileinpage: 厂商回填区间信息
    
    */
    int (*OVD_DMEAPI_callback_RecordSearch)(int channel,long long starttime, long long endtime, int page, int numInPage, OVD_DMERecordFileListPerPage *fileinpage  );

 #if 0
    /*
    ### 回调接口描述：
         RecordStart
         OVC可以通过该方法请求OVD建立一条媒体流通道到指定URL，用于后续卡录像视频传输
         厂商需知道某一channel下出现一个卡录像会话。
    ### 参数说明
         [in] channel: 通道
         
         [in] session_id: 字符串，用来标识这串流
    ### 返回值：
     成功：0
     失败：-1
     */

    int (*OVD_DMEAPI_callback_RecordStart)(int channel,  char *session_id);
    
    


    /*
   ### 回调接口描述：
       RecordStop
       OVC可以通过该方法请求OVD 关闭卡录像媒体传输流
   ### 参数说明
       [in] session_id: 字符串，用来标识这串流
   ###返回值：
       成功：0
       失败：-1
       */

    int(*OVD_DMEAPI_callback_RecordStop)(char* session_id);



    /*
    ### 回调接口描述：
        RecordPlay
    控制设备播放指定时间戳的卡录像
    ### 参数说明
    [in] session_id: 字符串，用来标识流ID
    [in] timestamp: 64位长整数，单位为秒，播放的绝对时间戳，0代表恢复上次暂停的播放
    ### 返回值：
    成功：0
    指定时间戳内无录像：-1
    其他失败：-2
    */

    int(*OVD_DMEAPI_callback_RecordPlay)(char *session_id, long long timestamp);
 

    /*
    ### 接口描述：
    RecordPause
    控制设备卡录像暂停播放
    ### 参数说明
    [in] session_id: 字符串
    */
    int(*OVD_DMEAPI_callback_RecordPause)(char *session_id);
#endif  //第一套srt 卡录像接口弃用

    //第二套卡录像机制

    /*
    
        ### 回调接口描述：
    
          OVD_DMEAPI_callback_RecordOpen
    
          通知厂商打开一条某一channel下的卡录像会话。
    
        ### 参数说明
    
          [in] channel: 通道
    
         
    
        ### 返回值：
    
            失败： NULL
    
             成功： 返回卡录像会话厂商侧的上下文（context）标识，ctx
    
      */

     void* (*OVD_DMEAPI_callback_RecordOpen)(int channel);


     /*
     
        ### 回调接口描述：
     
        OVD_DMEAPI_callback_RecordSeek
     
        将指定卡录像会话游标定位到某一时间点，
     
        注：该调用后，sdk下一次读取音视频数据帧时，厂商需从相近时间点的I帧（优先向前）开始返回数据。
     
          若对应时间点没有录像数据则返回错误。
     
        ### 参数说明
     
        [in] ctx:厂商卡录像会话上下文标识
     
        [in] timestamp: 绝对时间戳，seek的绝对时间点,单位为秒
     
        ###返回值：
     
        成功：0
     
        失败：-1 通用错误
     
             OVD_RET_SEEK_NODATA 对应时间点没有录像数据
     
        */

    int(*OVD_DMEAPI_callback_RecordSeek)(void* ctx, long long timestamp);

    
    /*
    ###接口描述:
        OVD_DMEAPI_callback_RecordReadFrame
    ###参数说明:
        [in] ctx: 厂商卡录像会话上下文
        [in] pframe_info： 帧数据信息
            注：pframe_info 结构体内存由sdk分配, 由sdk 负责释放
                pframe_info里的frame_buf 由厂商分配，也由厂商释放;建议厂商在下次read或者会话结束释放内存；
     ##返回值：
           OVD_RET_COMMON_ERROR = -1,    //通用错误码
    			OVD_RET_SUCCESS = 0,           //成功 
    			OVD_RET_READ_FRAME_RETRY=201, //读帧重试错误码
    			OVD_RET_READ_FRAME_EOF=202,   //卡录像读取完毕，厂商反馈EOF

          若收到通用错误，则sdk会调用销毁卡录像会话资源机制
    */
    int (*OVD_DMEAPI_callback_RecordReadFrame)(void* ctx, OVD_FrameInfo *pframe_info);




    /*
    ###接口描述：
        OVD_DMEAPI_callback_RecordClose
    ###参数说明：
        [in] ctx： 厂商卡录像会话上下文
    ### 返回值：
        0：代表成功
        其他：代表失败
    */
    int (*OVD_DMEAPI_callback_RecordClose)(void *ctx);

    //srt 卡录像机制

    /*
     * 调用时机：OVC根据配置及OVD当前的版本，触发OVD进行固件升级时调用
     * 功能介绍：OVD对比要升级的固件版本号和当前自身的固件版本号，若版本号不一致，那么到相应的URL去下载升级固件
     *
    **参数说明:
    **    [in]firmware_model:  要升级的固件的版本号
    **    [in]upgradeURL:      升级固件的远程url，由设备主动去下载、升级
    **
    **返回值：
    **    成功：0
    **    失败：-1
    */
    int (*OVD_FirmwareUpgrade)(char *firmware_model, char *upgradeURL);


    /*
     * 调用时机：OVC下发升级版本的命令后，会周期性的查询OVD的升级状态及进度
     * 功能介绍：OVD根据当天升级状态及进度，反馈相应的值
     *
    **参数说明:
    **    [out]upgradeStatus:    升级状态，详细见枚举值OVDUpgradeStatus
    **    [out]upgradeProgress:  升级进度，整数值 0-100
    **    [out]version:          //当前系统版本，version所占内存由sdk分配
    **    [in]version_len:      //sdk分配version内存的最大长度
    **    [out]last_upgrade_time:  //<字符串，最近一次处理升级指令的时间，格式”YY-MM-DDTHH:MM:SS”>,sdk 已分配内存
    **    [in]time_len:          //sdk分配的last_upgrade_time的最大长度
    **
    **返回值：
    **    成功：0
    **    失败：-1
    */
    int (*OVD_QueryFirmwareUpgradeStatus)(OVDUpgradeStatus *upgradeStatus, int *upgradeProgress,char *version, int version_len, char *last_upgrade_time,int time_len);


    /*
     * 调用时机：OVC需要与设备同步时间时调用
     * 功能介绍：OVD根据传入的时间及偏差，决定是否根据下发的参数修改时间
     *
    **参数说明:
    **    [in] datetime  <必填，字符串；格式yyyy-MM-ddTHH:mm:ss，例子：2016-12-05T02:15:32>,
    **    [in] tz        <可选，整数：时区数，东为正数，西为负数。例如东八区为+8，默认为+8>
    **    [in] offset: <必填，整数；可接受的偏差，单位秒，若摄像机时间与上面给定的时间的偏差在offset秒之内，则摄像机无需同步时间>
    **    成功：0
    **    失败：-1
    */
    int (*OVD_SyncTime)(char *datetime,int tz, int offset);


    /*
     * 调用时机：OVC需要获取设备时间时调用
     * 功能介绍：OVD查询本地时间，反馈参数
     *
    *  ####参数说明：
        [out] datetime  <必填，字符串；格式YY-MM-DDTHH:MM:SS，例子：2016-12-05T02:15:32>,sdk分配内存
        [in] time_len  datatime 分配的长度
        [in] tz  时区
    *   ###返回值：
    *       成功：0 
    *       失败：其他值

    */
    int (*OVD_QueryTime)(char* out_datetime, int time_len, int *tz);


    /*
     * (p2p接口，目的为兼容老设备)（已弃用）
     * 调用时机：OVC需要获取SD卡信息时调用
     * 功能介绍：OVD查询本地SD卡相关信息（OVDSDInfo定义的内容），反馈信息
     *
    **参数说明:
    **    [out]sdInfo:   存储卡信息,详细可见结构体描述OVDSDInfo
    **
    **返回值：
    **    成功：0
    **    失败：-1
    */
    int (*OVD_GetSDInfo)(OVDSDInfo *sdInfo);


    /*
     * 调用时机：OVC需要格式化SD卡时调用
     * 功能介绍：OVD格式化SD卡
     *
    **参数说明:
    **    无
    **
    **返回值：
    **    成功：0
    **    失败：-1
    */
    int (*OVD_SetSDCardFormat)();


    /*
     * 调用时机：APP端控制OVD进行转动时，调用此接口
     * 功能介绍：OVD根据控制命令，执行动作
     *
    **参数说明:
    **    [in]channel:         通道号
    **    [in]ptzcmd:          控制命令,详细可见枚举类型OVCPTZControlCmd
    **    [in]ptzvalue:        参考下图   
    
    | op | 操作 | value |
    | --- | --- | --- |
    | up | 上 | 可选，整数，速度，0-100，0最慢，100最快，默认50 |
    | down | 下 | 同上 |
    | left | 左 | 同上 |
    | right | 右 | 同上 |
    | upleft | 左上 | 同上 |
    | upright | 右上 | 同上 |
    | downleft | 左下 | 同上 |
    | downright | 右下 | 同上 |
    | zoomin | 拉近 | 同上 |
    | zoomout | 拉远 | 同上 |
    | stop | 停止 | 可选，整数，但数值没有意义 |
    | goto_preset | 跳转预置位 | 预置位ID， 0-255 |
    | set_preset | 设置预置位 | 同上 |
    | clear_preset | 清除预置位 | 同上 |
    | up_step | 单步上 | 可选，整数，单步步长，0-100，0最小，100最大，默认0 |
    | down_step | 单步下 | 同上 |
    | left_step | 单步左 | 同上 |
    | right_step | 单步右 | 同上 |
    | upleft_step | 单步左上 | 同上 |
    | upright_step | 单步右上 | 同上 |
    | downleft_step | 单步左下 | 同上 |
    | downright_step | 单步右下 | 同上 |
    | zoomin_step | 单步拉近 | 同上 |
    | zoomout_step | 单步拉远 | 同上 |
    
    **返回值：
    **    成功：0
    **    失败：-1
    */
    int (*OVD_PTZCmd)(int channel,OVCPTZControlCmd ptzcmd,int ptzvalue);


    /*
     * 调用时机：OVC获取OVD预置点列表信息
     * 功能介绍：OVD返回前期配置的预置点信息
     *
    **参数说明:
    **    [in]channel:         通道号
    **    [out]presetList:     返回的预置点列表,sdk已分配内存
    **    [in]array_malloc_num:  sdk内部分配的预置点数组大小
    **    [out]count:          预置点实际个数
    **
    **返回值：
    **    成功：0
    **    失败：-1
    */
    int (*OVD_GetPresetList)(int channel,int *presetList,int array_malloc_num, int *count);


    /*
     * (p2p接口)
     * 调用时机：APP端发起对讲时，调用此接口打开OVD对讲
     * 功能介绍：OVD打开OVD对讲功能
     *
    **参数说明:
    **    [in]channel:         通道号
    **    [in]andioFormat:     对讲音频数据的格式信息，见结构体定义OVDAudioOutDataFormat
    **
    **返回值：
    **    成功：0
    **    失败：-1
    */
    int (*OVD_AudioPlayStart)(int channel, OVDAudioOutDataFormat andioFormat);


    /*
     * (p2p接口)
     * 调用时机：OVC把对讲的音频内容，传输给OVD时调用
     * 功能介绍：OVD接收数据，并在音箱播放出来
     *
    **参数说明:
    **    [in]channel:   通道号
    **    [in]buf:      音频数据指针
    **    [in]size:     音频数据大小
    **
    **返回值：
    **    成功：0
    **    失败：-1
    */
    int (*OVD_AudioPlayProGress)(int channel,unsigned char* buf, int size);


    /*
     * (p2p接口)
     * 调用时机：APP关闭对讲时通知OVD
     * 功能介绍：OVD关闭对讲功能
     *
    **参数说明:
    **    [in]channel:         通道号
    **
    **返回值：
    **    成功：0
    **    失败：-1
    */
    int (*OVD_AudioPlayStop)(int channel);


    /*
     * (p2p接口，目的为兼容老设备,弃用)
     * 调用时机：OVC通知OVD切换视频清晰度时调用
     * 功能介绍：OVD根据配置切换到相应的视频清晰度，并返回切换后的视频参数
     *  该接口已弃用，分辨率切换放置于get/set config 里的调用里做
     *
    **参数说明:
    **    [in]channel:         通道号
    **    [in]quality:         要设置的清晰度，详细参考枚举值OVDEncodeQuality
    **    [out]vedioInfo:      清晰度调整后的视频参数，详细参考结构体OVDVideoDataFormat
    **
    **返回值：
    **    成功：0
    **    失败：-1
    */
    int (*OVD_VedioSwitchQuality)(int channel, OVDEncodeQuality quality, OVDVideoDataFormat *vedioInfo);


    /*
     * 调用时机：SDK在收到上传视频、P2P播放等命令时，为了服务器/客户端能够最短时间内播放视频，调用此接口强制OVD出一个I帧
     * 功能介绍：OVD收到后，在此channel上的数据流上强制出一个I帧，并继续上传数据流
     *
    **参数说明:
    **    [in]channel:         通道号
    **
    **返回值：
    **    成功：0
    **    失败：-1
    */
    int (*OVD_ForceIFrame)(int channel);


    /*
     * 调用时机：OVC通知OVD截取channel上的当前图片
     * 功能介绍：OVD截取channel上的当前图片，反馈截图内容及信息。若截图大小大于maxImageSize，则返回-2并在OVDImageInfo.size中带回所需要图片大小
     *
    **参数说明:
    **    [in]channel:         通道号
    **    [out]ImageInfo;	   截取的图片信息
    **
    **返回值：
    **    成功：0
    **    截图失败：-1
    **    空间不足：-2,  maxImageSize空间不足，并在OVDImageInfo.size中带回所需要图片大小
    */
    int (*OVD_Snapshot)(int channel,OVDImageInfo *imageInfo, int maxImageSize);


    /*
     * 调用时机：OVC通知OVD播放音乐时调用,暂不启用
     * 功能介绍：OVD去url下载音乐内容，并在channel的音箱上播放
     *
    **参数说明:
    **    [in]channel:      通道号
    **    [in]url:          歌曲下载的url
    **
    **返回值：
    **    成功：0
    **    失败：-1
    */
    int (*OVD_SetAudioOutPlay)(int channel,char *url);


    /*
     * 调用时机：OVC控制OVD的音乐播放，暂不启用
     * 功能介绍：OVD根据控制信息，控制音乐的播放 停止、暂停、继续
     *
    **参数说明:
    **    [in]channel:      通道号
    **    [in]ctrl:         播放控制,详细可见枚举类型OVDMp3PlayCtrl
    **
    **返回值：
    **    成功：0
    **    失败：-1
    */
    int (*OVD_AudioOutPlayCtrl)(int channel,OVDMp3PlayCtrl ctrl);


    /*
     * 调用时机：OVC查询OVD上当前的音乐播放情况，暂不启用
     * 功能介绍：OVD根据当前的播放情况，反馈播放音乐的URL及播放状态（播放 停止、暂停、继续）；若未播放任何音乐，则URL为空
     *
    **参数说明:
    **    [in]channel:         通道号
    **    [out]status:         播放状态,见OVDAudioPlayStatus枚举值定义
    **    [out]url:            正在播放的歌曲的下载url，该域不存在或者空串表示当前未播放，url最长1024字节
    **
    **返回值：
    **    成功：0
    **    失败：-1
    */
    int (*OVD_GetAudioOutPlayStatus)(int channel, int* status, char* out_url);

    //从ovd设备中获取时间戳函数，精确度毫秒， 暂不启用
    int (*OVD_gettime)(long long *out_time);


    /*
        回调时机：重新调度时，调度服务器时间同步
        参数说明：
        [in]: input_time 设置的时间戳，单位为毫秒
        [in]:tolerance_value 设置的容忍值，单位为秒,即当前系统时间戳和预设置的时间相差小于容忍值，则不需要设置系统时间
        返回值：
        成功：0
        失败：-1
    */
    int (*OVD_settime)(long long input_time, int tolerance_value);

	/*同步软探针模块阈值
		packetLossRateThreshold：丢包率阈值（高于）
		avgRTTThreshold：平均RTT阈值 单位毫秒（高于）
 		wifiQualityThreshold：WiFi信号周期内平均信号强度阈值（低于）
		cpuLoadThreshold：CPU使用率阈值（高于）
		memoryLoadThreshold：内存使用率阈值（高于）
		memoryAvaliable：剩余内存 单位kB （低于）
		checkPeriod：检测周期（默认2分钟  单位秒）
		reportSwitch：上报开关配置（默认开  不固化到设备） 1：开关打开，0：开关关闭
	*/
	int (*OVD_syncProbeDetector)(int avgRTTThreshold,int checkPeriod, int cpuLoadThreshold, 
	int memoryLoadThreshold,int memoryAvaliable,int packetLossRateThreshold,int reportSwitch,int wifiQualityThreshold);

	/*
    *  ####接口描述：
        同步软探针配置项softprobe_on
	####功能介绍：
		返回结果为softprobe_on的值，返回1sdk会开启软探针模块，返回0则不开启软探针模块。默认开启。
    *  ####参数说明：
       无
    *   ###返回值：
    *       成功：0/1 
    *       失败：其他值
    */
	int (*OVD_ProbeByval)();

    /*
        ##接口描述
        回调时机：服务启动初期
        ##参数说明
        [out] out_simpleovd sdk已分配内存
        channel_count不能少于1
    */
    int (*OVD_getsimpleovdinfo)(SimpleOVDinfo *out_simpleovdinfo);

    /*
    *  ####接口描述：
        获取sd卡状态


    *  ####参数说明：
        out_state: <必填，整数；sd卡状态，0：正常，1：未插卡，2：卡异常,3格式化中>,
        "out_total": <必填，整数, sd卡总容量，单位为MB>,
        "out_free":  <必填, 整数, sd卡剩余容量，单位为MB> 
    *   ###返回值：
    *       成功：0 
    *       失败：其他值
    */
    int(*OVD_GetDiskInfo)(int *out_state, int *out_total, int *out_free);

    /*
    *  ####接口描述：
        OVD下发软探针命令触发软探针实时上报
	####功能介绍：
		管理平台信令下发上报探针指标指令，设备接收到指令后无需和阈值做比较而是立刻调用SDK指标上报接口（OVD_Probe_StatusReport）上报当前设备各项指标数据。注意实时上报OVD_Probe_StatusReport中key传1

    *  ####参数说明：
       无
    *   ###返回值：
    *       成功：0 
    *       失败：其他值
    */
	int(*OVD_TriggerSoftProbe)();

    /*
    *  ####接口描述：
        LogUploadAsync
        OVC可以通过该命令控制OVD上传相关本地日志，供调试分析使用。OVD收到该指令后，通过异步方式将指定时间范围日志上传到相应的URL。 若指定范围的日志已经滚动删除，则使用尽力原则上传剩余的日志。若指定范围没有任何日志记录，应上传一个长度为0的日志文件。 日志文件通过PUT方法上传。
    
    *  ####参数说明：
        "trans_id": <必填，字符串；标记此日志上传的任务ID>
        "start": <必填，字符串；日志记录开始时间，格式yyyy-MM-ddTHH:mm:ss，例子：2016-12-05T02:15:32>,
        "end": <必填，字符串；日志记录结束时间，格式yyyy-MM-ddTHH:mm:ss，例子：2016-12-06T02:15:32>,
        "url": <必填，字符串；日志上传的URL，设备通过PUT方法上传相应的日志文件>

    *   ###返回值：
    *       成功：0 
    *       失败：其他值
*/
    int(*OVD_LogUploadAsync)(char *trans_id, char *start, char* end,char* url);
    

}OVD_CallBackFunList;




////接口定义

/*
### 接口描述
    将capinfo进行默认值初始化。
    默认值的数值参考对接文档
### 参数说明：
[in] capinfo:  厂商设备能力集

### 返回值：
0：成功
其他： 失败
*/
extern int OVD_CapInit(OVDCapInfo *capinfo);

/*
SDK初始化
### 接口描述
        设备上电后调用
### 参数说明：
[in]capinfo            设备能力集
[in]clientParam:       设备连接服务器信息，见OVDClientParam结构体
[in]logParam:          输出日志配置信息，见LogParam结构体
[in]callBackFunList:   提供给服务器端调用的回调函数，以相应服务器端的请求，见OVD_CallBackFunList。   注：若未提供相关的回调函数，则相关请求被丢弃，及设备端不提供对应的功能。
[in]jsonParam:         json格式化后的字符串，提供动态的参数配置。目前提供 1) snapshotSize: int型数值，设备截图的默认大小；2) buffDuration: int型数值，设备缓存的默认时长，单位为秒；
     json格式信息参考如下：
         {
             "snapshotSize":<可选，整数： 设备默认清晰度下的截图大小。若不设置，则SDK中默认设置为500k>
             "buffDuration":<可选，整数： 设备从上电到连接到平台时，需要SDK缓存的音视频内容的时长，单位为秒。若不设置，则SDK中默认设置为0s，建议低功耗设备设置此值，一般设置为一个数据分片的大小（约10s）>
             "debug_ts":<可选，布尔型，是否开启媒体数据输出ts开关
         }
### 返回值：
成功：0
失败：-1
 */
extern int OVD_Init(OVDCapInfo *capinfo,OVDClientParam *clientParam, OVDLogParam *logParam, OVD_CallBackFunList *callBackFunList, char *jsonParam);


/*
开启服务
### 接口描述
        设备服务准备好后调用，即设备连接网络成功、音视频接口准备完毕等工作之后再调用
        该接口为不阻塞接口
### 参数说明：
无
### 返回值：
0:成功
-1：失败
 */
 extern int OVD_ServiceStart();

/*
关闭服务
### 接口描述
        设备服务停止时后调用
### 参数说明：
无
### 返回值：
无
*/
 extern int OVD_ServiceStop();


/*
关闭信令服务客户端
### 接口描述
    关闭信令服务客户端该接口会被阻塞一定时间
### 参数说明
### 返回值：
   0：成功
   其他： 失败
*/
extern int OVD_DMEStop();

/*
启用信令服务客户端
### 接口描述
    启用信令服务器客户端
###参数说明
### 返回值：
    0： 成功
    其他： 失败
*/
extern int OVD_DMEStart();

/*
 告警开始
### 接口描述
        设备检测到告警条件，触发告警
### 参数说明：
[in]alarmInfo:    报警信息结构体，详细可见结构体描述OVD_UpLoadAlarmInfo
### 返回值：
//return 0: 成功上报且信息缓存；
//return -1: 异常
//return 1: 服务器连接异常但告警被缓存
*/
extern int OVD_AlarmInfoStart(OVDUpLoadAlarmInfo *alarmInfo);

/*
告警结束
### 接口描述
        设备告警结束
### 参数说明：
[in]channel;        通道号
[in]alarmType:      告警类型，详细可见枚举OVD_AlarmType
[in]endTimeStamp:   报警结束时间戳(绝对时间戳)
### 返回值：
成功：0
失败：-1
 */
extern int OVD_AlarmInfoEnd(int channel, OVDAlarmType alarmType, long long endTimeStamp);

/*
### 接口描述
        设备准备喂流，先输入音视频格式
### 参数说明：
[in]channel:         通道号
[in]videoinfo:       视频信息,详见结构体OVDVideoDataFormat，若无视频则为空
[in]audeoinfo:       视频信息,详见结构体OVDAudeoDataFormat，若无音频则为空
### 返回值：
成功：0
失败：-1
 */
extern int OVD_AVPushStart(int channel,OVDVideoDataFormat *videoinfo,OVDAudioDataFormat *audeoinfo);


/*
音视频参数修改
### 接口描述
        已经启动音视频传送后，若设备的音视频参数修改，则调用此接口通知sdk变动的参数
### 参数说明：
[in]channel:         通道号
[in]videoinfo:       视频信息,详见结构体OVDVideoDataFormat，若无视频则为空，若无修改也需要携带原来参数
[in]audeoinfo:       视频信息,详见结构体OVDAudeoDataFormat，若无音频则为空，若无修改也需要携带原来参数
### 返回值：
成功：0
失败：-1
 */
extern int OVD_AVParamModify(int channel,OVDVideoDataFormat *videoinfo,OVDAudioDataFormat *audeoinfo);

/*
视频加密接口
### 接口描述
    视频加密接口。设备调用OVD_AVPushData前先调用OVD_enc_frame进行视频加密。
    注意加密接口返回成功，OVD_AVPushData的入参为加密后的数据，加密接口返回失败，OVD_AVPushData的入参为加密前的数据。
### 参数说明：
[in]channel:         通道号,如果是摄像头产品，则通道号为0，如果是nvr等产品，channel则为前端摄像头的索引
[in]isIFrame:        是否是I帧  0：不是 1：是
[in]contentData:     待加密帧內容
[in]videoDataLen:    待加密数据长度
[in]enc_buf:       加密结果內容
[in]enc_buf_len:       加密后数据内存分配大小 (enc_buf比contentData内存大128)
[in]enc_len:       加密结果长度
[in]codec_enc:    视频格式，1表示h264，2表示h265
### 注意：
	enc_buf比contentData内存大128
### 返回值：
成功：0
失败：-1
 */
extern int OVD_enc_frame(int channel,char isIFrame,unsigned char* contentData,int videoDataLen, unsigned char* enc_buf,int enc_buf_len ,int *enc_len, int codec_enc);

/*
音视频内容推送接口
### 接口描述
        设备向SDK推动音视频内容
### 参数说明：
[in]channel:         通道号,如果是摄像头产品，则通道号为0，如果是nvr等产品，channel则为前端摄像头的索引，
[in]contentType:     准备传送的内容，详见枚举值OVD_ContentType 音频、视频
[in]isIFrame:        是否是I帧  0：不是 1：是
[in]contentData:     发送数据的首字节指针
[in]videoDataLen:    本次发送数据的长度
[in]timestamp:       该帧时间戳(ms)
### 返回值：
成功：0
失败：-1
 */
extern int OVD_AVPushData(int channel,OVDContentType contentType,char isIFrame,unsigned char* contentData,int dataLen, long long timestamp);


/*
音视频内容传送结束接口
该接口是同步接口，在弱网下因需要上传云存数据而阻塞一段时间。
 ### 参数说明：
[in]channel:         通道号
### 返回值：
成功：0
失败：-1
*/
extern int OVD_AVPushEnd(int channel);



/*
SD卡录像内容回放推送接口
### 接口描述
        APP打开相关录像文件后，设备推送相关内容
        *录像内容查询、打开录像文件、录像文件控制、录像删除等功能，由回调函数定义，详见（SDK初始化）的参数定义*
### 参数说明：
[in]channel:         通道号
[in]contentType:     准备传送的内容，详见枚举值OVD_ContentType 音频、视频、音视频
[in]isIFrame:        是否是I帧  0：不是 1：是
[in]contentData:     发送数据的首字节指针
[in]dataLen:    本次发送数据的长度
[in]timestamp:       该帧时间戳(ms)，相对于对应的逻辑卡录像文件头
### 返回值：
成功：0
失败：-1
 */
extern int OVD_SendRecordAVContent(int channel,OVDContentType contentType,char isIFrame,void* contentData,int dataLen,long long timestamp);

/*
SD卡录像内容回放推送完成
### 接口描述
    通知客户端录像文件播放完毕
### 参数说明：
[in]  channel [in] - 通道号
### 返回值：
成功：0
失败：-1
 */

extern int OVD_RecordAVContentSendOver(int channel);

//srt卡录像接口
/*
SD卡录像机制设置session 音视频格式
### 接口描述
       设置卡录像指定session id 的音视频格式
###参数
[in] session_id: 会话id
[in] vf： 视频格式
[in] af:  音频格式
### 返回值：
成功：0
失败：-1
 */
extern int OVD_tfRecord_SetAVFormat(char* session_id, OVDVideoDataFormat *vf, OVDAudioDataFormat *af);


/*
SD卡录像内容回放推送接口
### 接口描述
        APP打开相关录像文件后，设备推送相关内容
### 参数说明：
[in]session_id:         指定会话
[in]contentType:     准备传送的内容，详见枚举值OVD_ContentType 音频、视频、音视频
[in]isIFrame:        是否是I帧  0：不是 1：是
[in]buff:     发送数据的首字节指针
[in]len:    本次发送数据的长度
[in]timestamp:       该帧绝对时间的时间戳(ms)，
注：当厂商推卡录像过程中遇到卡录像空洞，厂商应自动跳过空洞继续播放下一个卡录像区间的文件
### 返回值：
成功：0
失败：-1
 */
extern int OVD_tfRecord_PushData(char* session_id, int type, bool iskey, unsigned char* buff, int len, long long timestamp);

/*
SD卡录像播放完成
###接口描述
    指定session下，卡录像播放完毕
    
    注：厂商通知该接口告知平台音频文件播放完，但厂商推流线程建议不退出，因为app/用户可能会下发拖动指令继续播放，
		厂商推流线程退出应该在卡录像终止播放的回调里做。
###参数说明：
[in]session_id: 指定会话
[in]endTimestamp: 最后一帧的绝对时间的时间戳，单位为秒
###返回值：
成功：0
失败：-1
*/

extern int OVD_tfRecord_PushDataEndFlag(char* session_id, long long endTimestamp);


/*

厂商因卡录像异常，主动关闭卡录像会话，正常情况不该调用此接口

###接口描述

    厂商因卡录像异常，主动关闭卡录像会话，SDK在回调内开始回收底层会话资源

###参数说明：

[in]session_id: 指定会话

[in]closeReason: 异常原因描述

###返回值：

成功：0

失败：-1

*/

extern int OVD_tfRecord_closeSession(char* session_id, char *closeReason);


/*
 声波初始化
### 接口描述
        设备检测到无网络配置信息，判断若使用声波配网，则调用此接口，传入声波参数
### 参数说明：
[in]sampleRate:      采样率
[in]bitWidth:        位宽(8/16bit)
### 返回值：
成功：返回声波句柄
        失败：NULL
*/
extern void* OVD_SoundWaveInit(int sampleRate,int bitWidth);


typedef void (*RecognizerStart)(void);
typedef void (*RecognizerFinish)(int type, void *info, int infoLen);

/*
开始声波识别
### 接口描述
        设备开始声波配网后，获取到音频文件，发送到SDK识别；声波识别模块识别出数据后，调用回调end_cbfunc返回给上层
### 参数说明：
[in]recognizer:      声波句柄
[in]starFunc:        识别开始回调函数；回调函数定义为 void (*RecognizStart)(void);
[in]endFunc:         识别结束回调函数（此函数返回wifi信息）；回调函数定义为 void (*RecognizerFinish)(struct OVDWiFiInfo info);
### 返回值：
成功：0
失败：-1
 */
extern int OVD_SoundWaveStart(void *recognizer,RecognizerStart starFunc,RecognizerFinish endFunc);


/*
采集到的声波数据，写入识别器
### 接口描述
        设备开始声波配网后，把声波数据传入SDK的识别器
### 参数说明：
[in]recognizer:      声波句柄
[in]data:            声波数据的首字节指针
[in]len:             声波数据长度
### 返回值：
成功：成功写入的数据长度
        失败：小于0的值
 */
extern int OVD_SoundWaveWriteData(void *recognizer,const void *data,unsigned long long len);


/*
 停止声波识别
### 接口描述
        识别完后，调用此接口,回收资源
### 参数说明：
[in]recognizer:      声波句柄
### 返回值：
成功：0
失败：-1
 */
extern int OVD_SoundWaveStop(void *recognizer);


/*
 二维码配网开始
### 接口描述
        设备检测到无网络配置信息，判断若使用二维码配网，则调用此接口
### 参数说明：
    [in]cbfun:      成功的回调函数
    [in]reverse:    图片是否翻转
### 返回值：
    成功：返回二维码识别句柄
    失败：NULL
 */

/* 方式一（已弃用）： 使用看护平台设备sdk中编译的zbar库       OVD_QRInit、OVD_QRStart、OVD_QRDestroy*/

extern void *OVD_QRInit(RecognizerFinish cbfun,int reverse);

/*
 二维码配网识别
### 接口描述
        传入图片进行识别
### 参数说明：
    [in]handle:      二维码识别句柄
    [in]raw:         二维码内容
    [in]width:       宽
    [in]height:      高
    [in]bitCount:    位宽
### 返回值：
    成功：0
    失败：-1
 */
extern int OVD_QRStart(void *handle, void *raw, int width, int height, unsigned int bitCount);


/*
 二维码配网结束
### 接口描述
        设备停止二维码配网
### 参数说明：
    [in]handle:      二维码识别句柄
### 返回值：
    成功：0
    失败：-1
 */
extern int OVD_QRDestroy(void *handle);



/* 方式二：不使用看护平台设备sdk中编译的zbar库 OVD_QRString_init、OVD_QRString*/

/*
    二维码配网输入字符串流程初始化

### 返回值：
   OVD_RET_COMMON_ERROR   -1 内部错误
   OVD_RET_SUCCESS  0   初始化成功
*/
extern int OVD_QRString_init();

/*
 * 二维码配网输入字符串
 * ### 接口描述
 *      设备读取字符串,返回解析结果
 * ### 参数说明：
        [out] info,厂商分配内存的指针，识别成功后，识别出的数据会存入这个内存
        [in] qr_str，具体一张二维码图片厂商解析出的字符串
        [in] qr_len, 具体一张二维码图片厂商解析出的字符串长度
 * ### 返回值：
        OVD_RET_RPC_UNIMPLEMENT  101 未集成字符串解析模块
        OVD_RET_BADPARAMETER     1 入参错误
        OVD_RET_COMMON_ERROR    -1 本次输入的字符串无法解析或者最终解析到数据有误，此时厂商可继续输入数据继续识别。
        OVD_NETCONF_SUCCESS   7 识别成功
        OVD_NETCONF_LESS_DATA  9  配网缺失数据，数据太长，二维码分页，需要厂商再次输入
 */
//extern int OVD_QRString(OVDXXSSIDWiFiInfo *info, unsigned char *qr_str,unsigned int qr_len);
extern int OVD_QRString(void *info, unsigned char *qr_str,unsigned int qr_len);



/*
## 接口描述
  设备上报绑定信息。
### 参数说明：
[in]bindid:      绑定的app账号信息
### 返回值：
成功：0
失败：-1
 */
extern int OVD_DeviceBindInfo(char* bindid);

/*
通知OVC日志文件上传完成
### 参数说明：
[in]
  "trans_id": <必填，字符串；标记此日志上传的任务ID，与LogUploadAsync保持一致>
  result: 0 代表success / 1代表fail
  "start": <必填，字符串；日志记录实际开始时间，格式yyyy-MM-ddTHH:mm:ss，例子：2016-12-05T02:15:32>,
  "end": <必填，字符串；日志记录实际结束时间，格式yyyy-MM-ddTHH:mm:ss，例子：2016-12-06T02:15:32>,
  "url": <必填，字符串；日志上传的URL>
  "size": <必填，长整性；上传日志文件的大小字节数>
### 返回值：
成功：0
失败：-1
*/
extern int OVD_LogDone(char* trans_id, int result, char* start, char *end, char *url,unsigned long long size);

/*
 设置SDK与云服务器之间的心跳周期，暂不需启用）
### 接口描述
        一般不用配置
### 参数说明：
[in]interval:     心跳周期，单位为秒
### 返回值：
成功：0
失败：-1
 */
extern int OVD_SetKeepaliveIntervel(int interval);

/*
接口描述：从平台获取token，同步接口，暂不需启用
设备上传bindid后，app识别bindid存token，设备通过平台取token
[in]serviceurl :平台地址
[in]userId:bindid
[in]ovdId:设备序列号
[in]ovdPassword：设备密码
[in]io_timeout:连接服务器超时时间
[in]betoken:用于存放获取到的token值，自行分配内存

*/
extern int OVD_GetIotToken(char* serviceurl,char* userId,char* ovdId,char * ovdPassword,int io_timeout,char * betoken);

/*
 OVD电池电量变化通知
### 接口描述
        OVD可以通过该方法向OVC上报当前的电量，一般电量百分比变化时可以发送该通知事件。设备可以决定每隔多少百分比上报一次电量，比如每隔10%上报一次。
### 参数说明：
[in]battery:     设备当前的电量百分比，整数： 0-100
### 返回值：
成功：0
失败：-1
 */
extern int OVD_BatteryChange(int battery);


//sdk日志打印函数
extern void log_line(const char* tag, int level,const char* format,...);

/*
设备获取休眠地址后回调

    [in] "hdomain": ovh 域名
    [in] "hport": ovh 端口
    [in] "hb_interval": <可选，整数：设备心跳间隔，单位秒，该字段不存在则默认为10秒>,
    "hb_token": <可选， 字符串：休眠服务的令牌，长度不超过16个字符。该字段不存在表示休眠服务不需要提供token>
*/
typedef int (*OVD_GetHServerInfo_handle)(char *hdomain, int hport, int hb_interval,char *hb_token);

int OVD_GetHserverInfo_trigger(OVD_GetHServerInfo_handle func);


/*
    调用时机：设备不重启的情况下更新版本号
*/
extern int OVD_update_systemversion(char *version);


/*
    注册时机：若厂商 系统命令gettimeofday不支持或者有差异实现，可注册第3方实现
*/
typedef int (*OVD_callback_thirdparty_gettimeofday)(struct timeval *tv, struct timezone *tz);

extern int OVD_register_thirdparty_gettimeofday(OVD_callback_thirdparty_gettimeofday func);


#ifdef CONFIG_MODULE_softprobe
/* 
软探针模块详述
1、能力集：设备具备软探针能力集。
2、上线阈值获取：设备开机会向软探针监测平台发起请求，获取到token和软探针监测阈值。监测阈值包括6项指标阈值（丢包率、平均RTT、WiFi信号强度、CPU使用率、内存使用率、剩余内存）、自检周期、阈值上报开关。
3、指标自检：设备按照获取阈值中的自检周期进行自检，默认10分钟一次。
4、软探针开关：阈值上报开关（不固化到设备）为0或管理平台软探针开关（化到设备）为0则不进行自检。
5、异常告警上报：自检时与监测指标进行比较，当指标中有一项未达标则触发异常告警上报，调用SDK指标上报接口进行上报，两次异常上报时间间隔>=20分钟（SDK可配置）。
6、实时上报：管理平台信令下发上报探针指标指令，设备接收到指令后调用SDK指标上报接口上报当前设备各项指标数据。
7、复用信令服务设备信息查询接口DevInfo，增加当前需求中的指标数据。
*/

/*
###软探针指标上报
软探针指标上报接口
### 接口描述：
异常告警上报和实时上报都调用此接口。区别在两个方面，一方面异常告警上报key传0，实时上报key传1，另一方面异常告警上报需要判断阈值，实时上报不需要判断阈值。
### 参数说明：
int packetLossRate;  //整数：0-100 ,丢包率 例如：10， 表示丢包率为10%。针对丢包率，设备端使用ifconfig指令来获取网卡的数据传输情况。packetLossRate=(drop_rx+drop_tx)/(rx+tx)
int maxRTT;   //整数： 0-100, 最大RTT       例如：10，表示周期内网络时延最大rtt为10毫秒。通过sdk接口获取
int minRTT;   //整数： 0-100, 最小RTT       例如：10，表示周期内网络时延最小rtt为10毫秒。通过sdk接口获取
int avgRTT;   //整数： 0-100, 平均RTT       例如：10，表示周期内（默认5分钟）网络时延平均rtt为10毫秒。通过sdk接口获取
int wifiQuality;   //整数：负值（dBm） 例如：-80 ，表示WiFi信号周期内平均信号强度为-80
int cpuLoad;   //整数：0-100, cpu占有率 例如：80，表示cpu单核占用率为80%，使用TOP指令，获取设备CPU占用情况。阈值大致为90%
int memoryLoad;   // 整数：0-100, 内存占有率 例如：80，表示当前内存占用率为80%。使用TOP指令，获取设备整个内存占用情况。监测buff字段，阈值大致为1024K
int memoryAvailable;   //剩余内存（kB）
int key; //是否使用上传时间的标志位。设备主动上报，置0，采用上报时间间隔。平台下发上报，置1，不采用上报时间。


### 返回值：
成功：0
失败：-1


*/
	
extern int OVD_Probe_StatusReport(int packetLossRate,int maxRTT,int minRTT,int avgRTT,int wifiQuality,int cpuLoad,int memoryLoad,int memoryAvailable,int key);

/*
*	####接口描述:
	改变软探针模块状态
*	####参数说明：
	[in]new_status：要改变成的模块状态      
    Probe模块未启动(停止软探针模块)：0 
    Probe模块启动(开始软探针模块)：1
*   ####返回值：
    成功：0
    失败：其他值
*   ####调用方法：
	收到配置参数softProbe_on为0，调用OVD_Probe_status_change(0)；
	收到配置参数softProbe_on为1，调用OVD_Probe_status_change(1)；	
*/
extern int OVD_Probe_status_change(int new_status);


#endif



//设置日志级别
int OVD_setloglevel(OVDLogLevel value);



#if 0
//ovd 状态
enum DME_ovd_state {
    DME_OVD_ONLINE = 1, //设备在线
    DME_OVD_UPDATING=2,  //设备升级
    DME_OVD_ABNORMAL=3 //异常
};


//channels 状态
enum DME_channel_state {
    DME_CHANNEL_OFFLINE=0, //离线
    DME_CHANNEL_ONLINE=1, //在线
 
    DME_CHANNEL_ABNORMAL=3, //异常
    DME_CHANNEL_DISABLE=4,   //不使能
};
#endif
//设备同步设备状态
//state值选项: 1：设备在线        2：设备升级        3：异常
//主要场景：a.设备升级      b:设备异常
int OVD_updateOVDstate(int state);


//设备同步channel状态
//channelstate 值选项：0：离线           1：在线    3：异常    4：不使能
int OVD_updatechannelstate(int channel, int channelstate);



//媒体调试相关
/*
开始直播抓包模式 （涉及直播，对讲，云存）         
    [in] "filepath": 文件输出路径
    [in] "filepre": 文件输出前缀

     OVD_AVPushData 接口会将音视频数据分别dump到两个文件
 输出文件为 音频：{path}/{filepre}_liveaudio.adts
            视频：{path}/{filepre}_live.video
            

 该接口为不可重入函数接口，抓包模式下不建议切换h264,h265
 返回值： 0代表成功
        非0 代表失败
*/
int OVD_StartLiveCapture(char *filepath, char* filepre);

/*
关闭直播抓包模式
返回值 0代表成功
       非0 代表失败
*/
int OVD_StopLiveCapture();

/*
开始卡回放抓包模式(涉及卡回放)
[in] "filepath"： 文件输出路径
[in] "filepre":   文件输出前缀
OVD_SendRecordAVContent 接口会将音视频数据分别dump到两个文件
输出文件为 音频：{path}/{filepre}_recordaudio.adts
            视频：{path}/{filepre}_record.video
  
*/
int OVD_StartRecordCapture(char *filepath, char* filepre);

/*
关闭卡回放抓包模式
*/
int OVD_StopRecordCapture();


/*
*  ####接口描述：
    OVD_get_RTTInfo
    获取DME RTT 统计信息, 每次获取，会将sdk 内部的统计信息清0
*  ####参数说明：
    [out] 最大RTT
    [out] 最小RTT
    [out] 平均RTT
    [out] 统计次数
          若输出值为0，则表明对应指标为无效值。
*   ###返回值：
*       成功：0 
*       失败：其他值
*/
int OVD_get_RTTInfo(long long *max_RTT_value, long long *min_RTT_value, long long* mean_RTT_value, int *count);

#ifdef __cplusplus
}
#endif

#endif //OVDOPENAPI_OPENAPI_H
