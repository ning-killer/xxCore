#ifndef OVD_DEFINE_H
#define OVD_DEFINE_H

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



//基础类型定义
#if !defined(__cplusplus)
#define bool char
#define true 1
#define false 0
#endif


typedef char OVD_bool;
typedef unsigned int OVD_uint32;
typedef int OVD_int32;
typedef unsigned long long  OVD_uint64;
typedef long long OVD_int64;
typedef char  OVD_char;
typedef unsigned char  OVD_uchar;
typedef void OVD_void;
typedef float  OVD_float;
typedef unsigned short OVD_ushort;



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
    OVD_FHDMODE  =  3, //超高清
}OVDEncodeQuality;

typedef struct
{
    OVDAVCodec codec;
    OVD_int32 quality;
    OVD_uint32 bitrate;
    OVD_ushort width;
    OVD_ushort height;
    OVD_uchar framerate;
    OVD_uchar colorDepth;
    OVD_uchar frameInterval;
    OVD_bool rsk_encrypt;
    OVD_uchar reserve;

}OVDVideoDataFormat;

typedef struct
{
    OVDAVCodec codec;
    OVD_uint32 samplesRate;
    OVD_uint32 bitrate;
    OVD_ushort waveFormat;
    OVD_ushort channelNumber;
    OVD_ushort blockAlign;
    OVD_ushort bitsPerSample;
    OVD_int32 sampleperframe;
    OVD_ushort frameInterval;
    OVD_ushort reserve;
}OVDAudioDataFormat;



typedef struct
{
    OVD_bool OVDDeviceID[MAX_LEN_32];               //设备ID号，必填
    OVD_char OVDHardWareModel[MAX_LEN_32];         //设备型号，必填
    OVD_char OVDSystemVersion[MAX_LEN_32];        //设备固件版本号，必填
    OVD_char wifi_ssid[WIFI_SSID_LEN];            //设备当前连接的wifi的ssid, 该字段空串表示未连接wifi，可选
    OVD_int32  wifi_signal;               //设备当前wifi的信号强度, 0-100, 当wifi_ssid不为空时有效，可选
    OVD_int32  upBandwidth;               //设备探测到的上行最大带宽，单位bps，不存在则表示上行带宽未知，负值表示未知，可选
    OVD_int32  downBandwidth;             //设备探测到的下行最大带宽，单位bps，不存在则表示下行带宽未知，负值表示未知，可选
    OVD_bool ipAddr[MAX_LEN_128];               //IP地址    <局域网IP，支持IPV6>，空值表示未知，可选
    OVD_bool macAddr[MAX_LEN_32]; 	            //MAC地址，空值表示未知，可选,格式大写，以:号相隔
    OVD_int32 battery;  //<可选，整数： 设备当前设备电池电量, 0-100, 该字段不存在表示设备不支持电池供电>
	OVD_int32 cpuLoad;  //<可选，整数，cpu负载率0~100>
	OVD_int32 memoryTotal; //<可选，整数，总内存数，单位为KB>
	OVD_int32 memoryAvailable; //<可选，整数，剩余可用内存，单位为KB>
}OVDDeviceInfo;


typedef struct
{
    OVD_char passDomain[MAX_LEN_64];        //<云服务器的域名>，必填
    OVD_int32  passPort;       //<云服务器的端口>，必填
    OVD_int32  Secure_PassPort;  //<云服务器的加密端口>,必填
    OVD_char p2p_passDomain[MAX_LEN_64];    // <P2P pass 的域名>，必填
    OVD_int32  p2p_passPort;   //<杭研p2p pass的端口>，必填
    OVD_char turnDomain[MAX_LEN_64];        //<P2P turn的域名>，没有置为空
    OVD_int32  turnPort;       //<p2p turn 的端口>，没有置为-1
	OVD_char probeDomain[MAX_LEN_64];        //<软探针服务器的域名>，必填
    OVD_int32  probePort;       //<软探针服务器的端口>，必填
    OVD_int32  probeSecurePort;  //<软探针服务器的加密端口>,必填
    OVD_char hibernationDomain[MAX_LEN_64];        //<休眠服务地址域名>，没有置为空
    OVD_int32  hibernationPort;       //<休眠服务地址端口>，没有置为-1
    OVD_int32  hibernationHBInterval;       //<休眠心跳骤起>，没有置为值10s，单位为秒
    OVD_int32  maxP2PSession;       //设备支持的最多的P2P流的个数，设备固有参数，服务器不能设置
}OVDNetParam;


typedef struct
{
    OVD_char OVDDeviceID[MAX_LEN_32];           //OVD设备ID，必填
    OVD_char OVDLoginPassword[MAX_LEN_64];           //OVD接入密码，必填
    OVD_char OVDMediaEncPassword[MAX_LEN_128];    //OVD视频加密密码，必填
    OVD_char OVDHardWareModel[MAX_LEN_32];      //OVD的硬件型号，必填
    OVD_char OVDSystemVersion[MAX_LEN_32];      //OVD的固件版本号，必填
    OVD_char OVDModelId[MAX_LEN_32];            //modelid,用于区分同一设备型号不同芯片方案的情况
    OVD_char OVDmacaddress[MAX_LEN_32];         //OVD的mac地址,大写，中间以分号相隔
    //int  period;                    //device记录的SDK周期上报的周期，单位：秒，必填
    OVDNetParam netParam;           //网络信息，必填
    OVD_int32 enable_servicschedule;      //是否启用服务调度
    OVD_char servicescheduleurl[MAX_LEN_1024];  //服务调度的URL 
    OVD_char cafilepath[MAX_LEN_256];   //客户端ca文件路径
    OVD_int32 tz;                        //设备时区，默认东8区
}OVDClientParam;

//软探针告警阈值
typedef struct{
	OVD_int32 packetLossRateThreshold;
	OVD_int32 avgRTTThreshold;
	OVD_int32 wifiQualityThreshold;
	OVD_int32 cpuLoadThreshold;
	OVD_int32 memoryLoadThreshold;
	OVD_int32 memoryAvaliable;
	OVD_int32 checkPeriod;
	OVD_int32 reportSwitch;	
}OVDProbeDetector;


/*能力集描述
能力集参考doc 里的cap.txt文件
*/

typedef struct
{
    OVD_bool have_alarms_io;   //<布尔型：是否支持外包报警,默认值为false>,
    OVD_bool have_alarms_face; //<布尔型：是否支持人脸侦测,默认值为false>,
    OVD_int32     support_alarms_face_zone; //<整型:0不支持，1：只支持矩形，n(n>=3):支持不规则n边型>, 注：目前app限制，只需要设备支持矩形即可
    OVD_bool have_alarms_cry; //<布尔型：是否支持哭声侦测,默认值为false>,
    OVD_bool have_alarms_voice; //<布尔型：是否支持声音侦测,默认值为false>,
    OVD_bool have_alarms_motion; //<布尔型：是否支持移动侦测,默认值为false>,
    OVD_int32     support_alarms_motion_zone; //<整型:0不支持，1：只支持矩形，n(n>=3):支持不规则n边型>, 注：目前app限制，只需要设备支持矩形即可
    OVD_bool have_alarms_body;  //<布尔型：是否支持人形侦测,默认值为false>,
    OVD_int32     support_alarms_body_zone; //<整型:0不支持，1：只支持矩形，n(n>=3):支持不规则n边型>, 注：目前app限制，只需要设备支持矩形即可
    OVD_bool have_alarms_cross; //<布尔型：是否支持拌网侦测>,
    OVD_bool have_alarms_pir; //<<布尔型，是否支持pir侦测>,
    OVD_bool have_alarms_pir_staymode;//<<布尔型，是否支持pir逗留模式,门铃>
    OVD_bool have_alarms_lossLock; //<<布尔型，是否支持撬锁侦测>
    OVD_bool have_alarms_alertarea; //<<布尔型，是否支持警戒功能>
    OVD_int32     support_alarms_alertarea_zone; //<整型:0不支持，1：只支持矩形，n(n>=3):支持不规则n边型>, 注：目前app限制，只需要设备支持矩形即可
}OVDAlarmsInfo;


//AI 能力相关
typedef struct
{
    OVD_bool face; //布尔型，设备是否支持上报人脸AI,默认值为false
    OVD_bool faceExposureAdjust; //布尔型，设备是否支持人脸曝光度调节,默认值为false
    OVD_char CaptureModeList[MAX_LEN_256]; //字符串,抓拍模式调节列表选项，若没有，则设备不支持模式切换,默认“quality”:代表质量抓拍，“quick”:代表快速抓拍,用逗号相隔
    OVD_int32 alarm_zone;
}OVDAIFaceCapInfo;

typedef struct
{
	OVD_bool kitchen_detection; 
    OVD_char CaptureModeList[MAX_LEN_256];
	OVD_char ClothesColorList[MAX_LEN_256];
    OVD_int32 alarm_zone;
}OVDAIKitchenCapInfo;

typedef struct
{
	OVD_bool vehicle_detection;
    OVD_char CaptureModeList[MAX_LEN_256];
    OVD_int32 alarm_zone;
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
    OVD_char alertarea[256];  
	OVD_char vehicle_detection[256];
    
}OVDlinkage_mode; //设备sdk 联动方式的支持，每种告警一个元素

typedef struct
{
    OVD_bool preference_call; //布尔型，是否支持呼叫偏好功能；呼叫偏好：可设置在发起呼叫时，普通电话优先或者APP通话优先
    OVD_bool set_calling_number; //布尔型，是否支持呼叫号码设置的功能；呼叫号码：可设置点击呼叫按键后，摄像头呼叫的号码（被叫号码）
    OVD_bool not_disturb_mode; //布尔型，是否支持手机勿扰模式；手机勿扰是控制设备端主叫手机端。可设置设备勿扰开关是否打开，如果打开，可设置勿扰时间
    OVD_bool not_disturb_device_mode; //布尔型，是否支持设备勿扰模式，设备勿扰是控制手机端主叫设备端
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
    OVD_char video_quality[MAX_LEN_256];
     //<字符串列表(中间用,隔开)：支持的视频编码格式列表，列表元素可选值为：h264/h265,默认支持h264>
    OVD_char video_formats_supportlists[MAX_LEN_256];

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

	OVD_char have_support_extension[MAX_LEN_256]; //<字符串列表(中间用,隔开)：支持的extension方法列表，列表元素可选值为：echotest/ccrtc>

}OVDCapInfo;



//日志输出级别依次增高
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
    OVD_void (*pOVDLogOutCallBack)(const char* buff);  //设备提供的日志输出回调，SDK的输出日志可以保存到device的存储文件中，可选，若未空为不支持
}OVDLogParam;

typedef enum
{
    OVD_STATUS_IDLE	          =	  0,  //初始态
    OVD_STATUS_DOWNLOADING	  =	  1,  //安装包下载中
    OVD_STATUS_INSTALLING     =   2,  //安装中
    OVD_STATUS_FAILED         =   3,  //升级失败
    OVD_STATUS_DONE           =   4,  //安装成功
}OVDUpgradeStatus;


//p2p 卡录像机制
typedef struct
{
    OVD_int32 channel;                    //通道号
    OVD_char FileName[MAX_LEN_256];
    OVD_int32  FileType; 			    	//文件类型(0 视频文件， 1 告警文件)
    OVD_uint64 FileStartStamp;		    //录像开始时间（单位为秒）
    OVD_uint64 FileEndStamp;			//录像接收时间（单位为秒）
    OVD_int32  RecordDuration; 			//时长（单位为秒）
    OVD_int32  FileSize; 					//文件大小
}OVDRecordFileInfo;

typedef struct
{
    OVD_int32               fileCount;            //文件数量
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
    OVD_int32  SDExist;		    //0 not, 1 yes, 2 error
    OVD_int32	 SDTotalSize;	    //总容量(M)
    OVD_int32	 SDFreeSize;	    //空闲量
    OVD_char EarlyFileName[MAX_LEN_24]; //当前SD卡最早一个录像文件
}OVDSDInfo;
//p2p 卡录像机制 end
 


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

//p2p 对讲
typedef enum{
    OVD_PCM          = 0,
}OVDAUDIOPLY_TYPE;

typedef struct
{
    OVD_uint32 codec;         //OVDAUDIOPLY_TYPE
    OVD_uint32 samplesRate;
    OVD_uint32 bitWidth;
    OVD_uint32 volume;
    OVD_uint32 priority;      //the biger ,the higher
}OVDAudioOutDataFormat;
//p2p 对讲 end

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
    OVD_char *buf;    			//数据buf
    OVD_int32  size;    			//数据长度
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
    OVD_KITCHEN_MASK = 14,  //厨房行为检测，检测到口罩异常
    OVD_KITCHEN_CAP = 15,  //厨房行为检测，检测到帽子异常
    OVD_KITCHEN_CLOTHES = 16,  //厨房行为检测，检测到工服异常
    OVD_VEHICLE_GEN = 17,  //车型侦测，通用车辆侦测
    OVD_VEHICLE_MOTOR = 18,  //车型侦测，检测到电瓶车
    OVD_VEHICLE_BIKE = 19,  //车型侦测，检测到自行车
    OVD_OTHER,
}OVDAlarmType;

typedef struct
{
    OVD_int32             channel;            //通道号
    OVD_int64    startTimeStamp;	   //报警开始时间戳 (单位为秒)
    OVDAlarmType	AlarmType;     //报警类型
    OVD_char*           desc;               //告警描述
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
    OVD_char ssid[33];
    OVD_int32 ssidLen;
    OVD_char pwd[80];
    OVD_int32 pwdLen;
}OVDSSIDWiFiInfo;

typedef struct
{
    OVD_char ssid[WIFI_SSID_LEN];   //wifi的ssid
    OVD_int32  ssidLen;
    OVD_char pwd[WIFI_PWD_LEN];    //wifi密码
    OVD_int32  pwdLen;
    OVD_char phone[16+1];
    OVD_int32 phoneLen;
    OVD_char extension[256];  //扩展信息字段
    OVD_int32 extensionLen;
}OVDXXSSIDWiFiInfo;

typedef enum
{
    OVD_HIBERNATE_OVC_NOTIFY = 0,   //OVC信令交互，需要保持一段时间不断电
    OVD_HIBERNATE_P2P_NAT = 1,      //P2P开始NAT穿透打洞
    OVD_HIBERNATE_P2P_OPEN = 2,     //正在P2P通话
    OVD_HIBERNATE_OTHER = 3
}OVDHibernateReason;

//p2p 卡录像结构体
typedef struct
   {

       OVD_uint64 StartStamp;           //录像开始时间（单位为秒）
       OVD_uint64 EndStamp;         //录像接收时间（单位为秒）

   }OVD_DMERecordFileInfo;

   typedef struct
   {
       OVD_int32               Count;            //卡录像片段个数
       OVD_DMERecordFileInfo sectioninfo[64];        //卡录像一个连续区间的信息
   }OVD_DMERecordFileListPerPage;

/*
远程配置参考doc/config.txt
*/

//ovd 状态
enum ovd_state {
    OVD_ONLINE = 1, //设备在线
    OVD_UPDATING=2,  //设备升级
    OVD_ABNORMAL=3 //异常
};


//channels 状态
enum ovd_channel_state {
    OVD_CHANNEL_OFFLINE=0, //离线
    OVD_CHANNEL_ONLINE=1, //在线
 
    OVD_CHANNEL_ABNORMAL=3, //异常
    OVD_CHANNEL_DISABLE=4,   //不使能
};


typedef struct{
    OVD_int32 channelnum;
    enum ovd_channel_state state;    
} Simplechannelinfo;

typedef struct{
    enum ovd_state state;  //1：设备在线        2：设备升级        3：异常
    Simplechannelinfo channelarray[64];  //用静态数组，最多支持64个channel,不用malloc
    OVD_int32 channelcount;
} SimpleOVDinfo;


//数据帧信息
typedef struct{
    OVD_bool iskey_frame; //是否是I帧(必填)

    OVD_uchar rev[3];  //预留1

    OVD_ushort width;  

    OVD_ushort height;

    OVD_ushort framerate;  //帧率(必填)

    OVD_ushort gop;   //(必填)

    OVD_uchar rev2[20]; //预留2

    
}OVD_VideoFrameInfo;

typedef struct{

    OVD_uint32 samplesRate; //采样率(必填)

    OVD_ushort channelNumber; //声道数

    OVD_ushort bitsPerSample; //位深

    OVD_uint32 samplePerFrame; //每帧采样点数

    OVD_uchar rev[20]; //预留

}OVD_AudioFrameInfo;


typedef struct{

    OVDContentType frame_type;//帧类型，音频or 视频

    OVDAVCodec codec_type;//编码类型

    OVD_uint64 timestamp; //绝对时间戳，单位为毫秒

    OVD_VideoFrameInfo video_info;

    OVD_AudioFrameInfo audio_info; 

    OVD_uint32 framebuf_len; //帧长度

    OVD_char *frame_buf; //帧内容

    OVD_uchar rev[8]; //预留

}OVD_FrameInfo;

#endif


