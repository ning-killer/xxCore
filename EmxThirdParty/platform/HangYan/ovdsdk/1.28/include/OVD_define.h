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

#define MAX_CHANNEL_COUNT  1  //设备支持最多的channel数量，channel编号范围为0～63
//为节省sdk，内部先将channel数调整为1

//错误码
#define  RPC_UNIMPLEMENT 101 //未实现回调

#define ALARM_TIME_MIN_INTERVAL 10
#define ALARM_TIME_MAX_INTERVAL 3600

typedef enum
{
    OVD_RET_STREAM_CONNECT_CHANNEL_BROKEN = -5,     //底层流媒体通道出现断开
    OVD_RET_COMMON_ERROR                  = -1,     //通用错误码
    OVD_RET_SUCCESS                       =  0,     //通用成功
    OVD_RET_BADPARAMETER                  =  1,     //入参错误
    OVD_RET_NOMEM                         =  2,     //内存错误

    //配网相关错误码
    OVD_NETCONF_SUCCESS = 7,       //配网识别数据成功
    OVD_NETCONF_LESS_DATA = 9,     //配网缺失数据，数据太长，二维码分页，需要厂商再次输入
    OVD_RET_RPC_UNIMPLEMENT = 101, //该功能模拟未使能或未支持

    //srt卡回放机制错误码
    OVD_RET_READ_FRAME_RETRY = 201, //读帧重试错误码
    OVD_RET_READ_FRAME_EOF = 202, //卡录像读取完毕，厂商反馈EOF
    OVD_RET_SEEK_NODATA = 203,    //seek点无音视频数据

} OVD_ERRNO;


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

/**
 * OVD 设备能力集
 * 能力集参考doc 里的cap.txt文件
 */

/*AI 能力相关 */
typedef struct
{
    OVD_bool face;                         //布尔型，设备是否支持上报人脸AI,默认值为false
    OVD_bool faceExposureAdjust;           //布尔型，设备是否支持人脸曝光度调节,默认值为false
    OVD_char CaptureModeList[MAX_LEN_256]; //字符串,抓拍模式调节列表选项，若没有，则设备不支持模式切换,默认“quality”:代表质量抓拍，“quick”:代表快速抓拍,用逗号相隔
    OVD_int32 alarm_zone;
    OVD_bool mask_detection;               //布尔型，口罩检测,默认值为false
} OVDAIFaceCapInfo;

typedef struct
{
    OVD_bool passenger;                         //布尔型，设备是否支持上报人脸AI,默认值为false
    OVD_bool passengerExposureAdjust;           //布尔型，设备是否支持人脸曝光度调节,默认值为false
    OVD_char passengerCaptureModeList[MAX_LEN_256]; //字符串,抓拍模式调节列表选项，若没有，则设备不支持模式切换,默认“quality”:代表质量抓拍，“quick”:代表快速抓拍,用逗号相隔
    OVD_int32 passengerAlarmzone;
} OVDAIPassengerCapInfo;


typedef struct
{
    OVD_bool kitchen_detection;
    OVD_char CaptureModeList[MAX_LEN_256];
    OVD_char ClothesColorList[MAX_LEN_256];
    OVD_int32 alarm_zone;
} OVDAIKitchenCapInfo;

typedef struct
{
    OVD_bool vehicle_detection;
    //OVD_char CaptureModeList[MAX_LEN_256];
    OVD_int32 alarm_zone;
    OVD_bool vehicle_detect_site;
} OVDAIVehicleCapInfo;

typedef struct
{
    OVD_bool nonmotorvehicle_detection;
    OVD_char CaptureModeList[MAX_LEN_256];
    OVD_int32 alarm_zone;
} OVDAINonmotorvehicleCapInfo;

typedef struct
{
    OVD_bool regionalPeopleStat;                         //布尔型，设备是否支持上报区域人数统计,默认值为false
    OVD_int32 regionalPeopleStatAlarmzone;
} OVDAIRegionalPeopleStatCapInfo;

typedef struct
{
    OVD_bool parabolicAerial;                         //布尔型，设备是否支持上报高空抛物,默认值为false
    OVD_int32 detectZoneNum;           //检测区域最大个数
    OVD_int32 detectZone;              //检测区域绘制多边形最大边数, 0不支持，1：只支持矩形，n(n>=3):支持不规则n边形
    OVD_int32 shieldZoneNum;           //屏蔽区域最大个数
    OVD_int32 shieldZone;              //屏蔽区域绘制多边形最大边数, 0不支持，1：只支持矩形，n(n>=3):支持不规则n边形
    OVD_int32 floorConfNum;            //楼层设置最大个数
} OVDAIParabolicAerialCapInfo;


typedef struct
{
    OVD_bool  laneLine;                 //布尔型，设备是否支持上报AI,默认值为false
    OVD_int32 laneLineNum;
    OVD_int32 laneLineAlarmzone;
} OVDAILaneLineCapInfo;

typedef struct
{
    OVD_bool offDuty_detection;                         //布尔型，设备是否支持离岗检测,默认值为false
    OVD_bool support_alert_time;
    OVD_int32 support_on_duty_count;
    OVD_int32 support_off_duty_durtion;
    OVD_int32 detect_plan_num;
    OVD_int32 alarm_area_num;
    OVD_int32 alarm_zone;
    
} OVDAIOffDutyCapInfo;



typedef struct
{
    OVDAIFaceCapInfo AIface;
    OVDAIKitchenCapInfo AIkitchen;
    OVDAIVehicleCapInfo AIvehicle;
    OVDAINonmotorvehicleCapInfo AInonmotorvehicle;
    OVDAIPassengerCapInfo AIpassenger;
    OVDAIRegionalPeopleStatCapInfo AIregionalPeopleStat;
    OVDAIParabolicAerialCapInfo AIparabolicAerial;
    OVDAILaneLineCapInfo AILaneLine;
    OVDAIOffDutyCapInfo AIOffDuty;
} OVDAICapInfo;

//注：当前和家亲app声光联动只有体现在警戒功能上
typedef struct
{
    OVD_char alertarea[256];
    OVD_char vehicle_detection[256];
    OVD_char nonmotorvehicle_detection[256];
    OVD_char mask_detection[256];
    OVD_char regionalPeopleStat_detection[256];
} OVDlinkage_mode; //设备sdk 联动方式的支持，每种告警一个元素

typedef struct
{
    OVD_bool preference_call;         //布尔型，是否支持呼叫偏好功能；呼叫偏好：可设置在发起呼叫时，普通电话优先或者APP通话优先
    OVD_bool set_calling_number;      //布尔型，是否支持呼叫号码设置的功能；呼叫号码：可设置点击呼叫按键后，摄像头呼叫的号码（被叫号码）
    OVD_bool not_disturb_mode;        //布尔型，是否支持手机勿扰模式；手机勿扰是控制设备端主叫手机端。可设置设备勿扰开关是否打开，如果打开，可设置勿扰时间
    OVD_bool not_disturb_device_mode; //布尔型，是否支持设备勿扰模式，设备勿扰是控制手机端主叫设备端
    OVD_bool auto_hangup;             //布尔型，是否支持自动挂断功能；自动挂断：可设置检测到访客离开/进门后自行停止呼叫
    OVD_bool voice_calling_number;    //布尔型，是否支持语音呼叫号码功能；如果设备上报支持，则APP新增“语音呼叫号码”配置项，配置完成后，可实现通过语音给指定的号码打电话
} OVDHJKH_mode; //设备上对和家看护功能的功能支持配置


typedef struct
{
    OVD_bool move;    //布尔型，是否支持左右前后移动
    OVD_bool nofall;  //布尔型，是否支持防跌落
    OVD_bool laser;   //布尔型，是否支持激光
    OVD_bool onrush;  //布尔型，是否支持向前猛冲
    OVD_bool recharge;//布尔型，是否支持回充
    OVD_bool shake;   //布尔型，是否支持抖动
    OVD_bool spin;    //布尔型，是否支持转圈
    OVD_bool track;    //<布尔型，是否支持移动追踪，开启后  默认值是： 人形追踪> 1.27新增
    OVD_bool body_track;    //<布尔型，是否支持人形追踪追踪，开启后支持人形追踪> 1.27新增
    OVD_bool pets_track;    //<布尔型，是否支持宠物追踪追踪，开启后支持宠物追踪> 1.27新增
} OVDEBO_mode;

typedef struct
{
    OVD_bool have_alarms_io;                 //<布尔型：是否支持外包报警,默认值为false>,
    OVD_bool have_alarms_face;               //<布尔型：是否支持人脸侦测,默认值为false>,
    OVD_int32 support_alarms_face_zone;      //<整型:0不支持，1：只支持矩形，n(n>=3):支持不规则n边型>, 注：目前app限制，只需要设备支持矩形即可
    OVD_bool have_alarms_cry;                //<布尔型：是否支持哭声侦测,默认值为false>,
    OVD_bool have_alarms_voice;              //<布尔型：是否支持声音侦测,默认值为false>,
    OVD_bool have_alarms_motion;             //<布尔型：是否支持移动侦测,默认值为false>,
    OVD_int32 support_alarms_motion_zone;    //<整型:0不支持，1：只支持矩形，n(n>=3):支持不规则n边型>, 注：目前app限制，只需要设备支持矩形即可
    OVD_bool have_alarms_body;               //<布尔型：是否支持人形侦测,默认值为false>,
    OVD_int32 support_alarms_body_zone;      //<整型:0不支持，1：只支持矩形，n(n>=3):支持不规则n边型>, 注：目前app限制，只需要设备支持矩形即可
    OVD_bool have_alarms_cross;              //<布尔型：是否支持拌网侦测>,
    OVD_bool have_alarms_pir;                //<<布尔型，是否支持pir侦测>,
    OVD_bool have_alarms_pir_staymode;       //<<布尔型，是否支持pir逗留模式,门铃>
    OVD_bool have_alarms_lossLock;           //<<布尔型，是否支持撬锁侦测>
    OVD_bool have_alarms_alertarea;          //<<布尔型，是否支持警戒功能>
    OVD_int32 support_alarms_alertarea_zone; //<整型:0不支持，1：只支持矩形，n(n>=3):支持不规则n边型>, 注：目前app限制，只需要设备支持矩形即可
} OVDAlarmsInfo;

typedef struct
{
    OVD_int32 device_type; // 整型,0代表普通IPC，1代表低功耗中继型设备，2代表低功耗非中继型设备,1.21版本新增
    OVD_int32 audiosamplerate; //枚举整型，0（默认值）：代表8K， 1：代表16K, 1.27版本新增
    OVD_bool enable_low;//布尔型，是否支持低功耗相关功能,1.25版本新增
    OVD_bool support_awaked;//布尔型，是否支持低功耗唤醒，默认值为false,不支持低功耗唤醒 1.21版本新增
    OVD_bool support_awaked_switch;//布尔型，是否支持低功耗唤醒开关,1.25版本新增
    OVD_bool support_4G;//布尔型，是否支持4G,1.25版本新增
    OVD_bool smart_mode;//布尔型，是否智能模式,1.25版本新增
    OVD_bool have_ptz;          //<布尔型：是否支持云台控制能力,默认值为false>
    OVD_bool support_ptz_preset;//<布尔型：是否支持云台预置位,默认值为false>
    OVD_bool stopalarm; //<布尔型，是否支持消控能力，默认值为false>
    OVD_bool zoomcontrol; //<布尔型，是否支持用户手动调节倍率，默认值为false>
    OVD_bool focuscontrol; //<布尔型，是否支持用户手动调节焦距，默认值为false>

    OVD_bool have_battery;      //<布尔型：是否电池供电,主要用于门铃设备,默认值为false>,
    OVD_bool have_audio_out;    //<布尔型：是否支持音乐输出,默认值为false>
    OVD_bool have_voice_out;    //<布尔型：是否支持对讲输出,默认值为false>
    OVD_bool have_ivrs;         //<布尔型：是否支持ivrs协议，默认值为false，表示云存只支持ivr协议>,
    OVD_bool have_trace;        //<布尔型：是否支持移动跟踪,默认值为false>

    OVDAICapInfo  ovdAICapInfo;      //AI相关
    OVDAlarmsInfo ovdCapInfo_alarms; //alarms 告警上报相关能力
    OVDlinkage_mode ovdlinkage_mode; //联动方式的支持

    OVD_bool have_switch;            //<布尔型：是否支持通道使能开关，默认值为false>
    OVD_bool have_sd;                //<布尔型：是否支持SD卡/TF卡，默认值为true>
    OVD_bool have_led;               //<布尔型：是否支持led灯开关，默认值为false>
    OVD_bool have_screen;            //<布尔型：是否支持带屏,默认值为false>
    OVD_bool have_support_softprobe; //<布尔型：是否支持软探针模块，默认值为false>
    OVD_bool have_auto_reboot;       //<布尔型：是否支持设备自动维护，默认值为false>

    //庭院摄像头相关
    OVD_bool yard_light_brightness;//<布尔型，是否支持庭院灯灯光控制,默认不支持>
    OVD_bool yard_light_manual_control;//<布尔型，是否支持手动控制，默认不支持>
    OVD_bool yard_light_timed_schedule;//<整形，是否支持计划控制，0为不支持，非0数字表示支持几个计划控制时间段，默认为2，最大值为5>

    //<字符串列表(中间用,隔开)：支持的视频清晰度选项列表，列表元素可选值为：ld/sd/hd/fhd，分别代表低清/标清/高清/全高清，默认支持sd和hd>
    OVD_char video_quality[MAX_LEN_24];
    //<字符串列表(中间用,隔开)：支持的视频编码格式列表，列表元素可选值为：h264/h265,默认支持h264>
    OVD_char video_formats_supportlists[MAX_LEN_24];
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
    OVD_bool enable_EBO; //<布尔型，设备是否支持机器人指令，默认值为false>
    OVDEBO_mode  EBO_mode;
    OVD_char have_support_extension[MAX_LEN_256]; //<字符串列表(中间用,隔开)：支持的extension方法列表，列表元素可选值为：echotest/ccrtc>

} OVDCapInfo;



typedef enum
{
    OVD_Video   =   0,          //视频
    OVD_Audio   =   1,          //音频
} OVDContentType;


typedef enum
{
    source_STREAM_CODEC_H264 = 0,
    source_STREAM_CODEC_AAC_WITH_ADTS = 1,
    //    source_STREAM_CODEC_AAC =2,          当前sdk音频格式只支持aac with adts
    source_STREAM_NO_CODEC = 3,
    source_STREAM_CODEC_H265 = 4
} OVDAVCodec;


typedef enum
{

    Probe_status_EXIT = -2,   //Probe模块退出
    Probe_status_UNSTART = -1, //Probe模块未启动
    Probe_status_PAUSED = 0, //Probe模块暂停
    Probe_status_STARTING = 1, //Probe模块启动
} OVD_Probe_status;


typedef enum
{
    OVD_1DMODE  =   0, //低清
    OVD_SDMODE  =   1, //标清
    OVD_HDMODE  =   2, //高清
    OVD_FHDMODE  =  3, //超高清
} OVDEncodeQuality;

typedef struct
{
    OVDAVCodec codec;     //编解码类型，例如h264/h265/aac
    OVD_int32 quality;    //暂未使用，编码质量CQS
    OVD_uint32 bitrate;   //暂未使用，码率
    OVD_ushort width;     //暂未使用，分辨率宽
    OVD_ushort height;    //暂未使用，分辨率高
    OVD_uchar framerate;  //暂未使用，帧率
    OVD_uchar colorDepth; //暂未使用，像素位数，例如8位像素为256色
    OVD_uchar frameInterval;  //暂未使用，帧间隔
    OVD_uchar reserve;    //暂未使用，保留字段

} OVDVideoDataFormat;

typedef struct
{
    OVDAVCodec codec;         //编解码类型，例如h264/h265/aac
    OVD_uint32 samplesRate;   //采样率，例如8000/16000
    OVD_uint32 bitrate;       //暂未使用，码率
    OVD_ushort waveFormat;    //暂未使用，音频文件格式
    OVD_ushort channelNumber; //声道数
    OVD_ushort blockAlign;    //暂未使用，每帧采样的字节数
    OVD_ushort bitsPerSample; //暂未使用，采样位数
    OVD_int32 sampleperframe; //暂未使用，每帧采样点个数
    OVD_ushort frameInterval; //暂未使用，帧间隔
    OVD_ushort reserve;       //暂未使用，保留字段
} OVDAudioDataFormat;



typedef struct
{
    OVD_bool OVDDeviceID[MAX_LEN_32];               //设备ID号，必填
    OVD_char OVDHardWareModel[MAX_LEN_32];         //设备型号，必填
    OVD_char OVDSystemVersion[MAX_LEN_32];        //设备固件版本号，必填
    OVD_char wifi_ssid[WIFI_SSID_LEN];            //设备当前连接的wifi的ssid, 该字段空串表示未连接wifi，可选
    OVD_int32  wifi_signal;               //设备当前wifi的信号强度, 0-100, 当wifi_ssid不为空时有效，可选
    OVD_int32
    upBandwidth;               //设备探测到的上行最大带宽，单位bps，不存在则表示上行带宽未知，负值表示未知，可选
    OVD_int32
    downBandwidth;             //设备探测到的下行最大带宽，单位bps，不存在则表示下行带宽未知，负值表示未知，可选
    OVD_bool ipAddr[MAX_LEN_128];               //IP地址    <局域网IP，支持IPV6>，空值表示未知，可选
    OVD_bool macAddr[MAX_LEN_32];               //MAC地址，空值表示未知，可选,格式大写，以:号相隔
    OVD_int32 battery;  //<可选，整数： 设备当前设备电池电量, 0-100, 该字段不存在表示设备不支持电池供电>
    OVD_int32 cpuLoad;  //<可选，整数，cpu负载率0~100>
    OVD_int32 memoryTotal; //<可选，整数，总内存数，单位为KB>
    OVD_int32 memoryAvailable; //<可选，整数，剩余可用内存，单位为KB>
} OVDDeviceInfo;

/**
 * OVD 网络信息
 */
typedef struct
{
    OVD_char passDomain[MAX_LEN_64];        //<云服务器的域名>，必填
    OVD_int32 passPort;                     //<云服务器的端口>，必填
    OVD_int32 Secure_PassPort;              //<云服务器的加密端口>,必填
    OVD_char p2p_passDomain[MAX_LEN_64];    // <P2P pass 的域名>，必填
    OVD_int32 p2p_passPort;                 //<杭研p2p pass的端口>，必填
    OVD_char turnDomain[MAX_LEN_64];        //<P2P turn的域名>，没有置为空
    OVD_int32 turnPort;                     //<p2p turn 的端口>，没有置为-1
    OVD_char probeDomain[MAX_LEN_64];       //<软探针服务器的域名>，必填
    OVD_int32 probePort;                    //<软探针服务器的端口>，必填
    OVD_int32 probeSecurePort;              //<软探针服务器的加密端口>,必填
    OVD_char hibernationDomain[MAX_LEN_64]; //<休眠服务地址域名>，没有置为空
    OVD_int32 hibernationPort;              //<休眠服务地址端口>，没有置为-1
    OVD_int32 hibernationHBInterval;        //<休眠心跳骤起>，没有置为值10s，单位为秒
    OVD_char logDomain[MAX_LEN_64];        //<实时日志服务器地址>，没有置为空
    OVD_int32 maxP2PSession;                //设备支持的最多的P2P流的个数，设备固有参数，服务器不能设置
} OVDNetParam;



typedef struct
{
    /**
     * 设备ID，必填
     * 说明：物料清单里的设备序列号,需要烧录固化到设备里。
     *       16位，1-2位代表device vendor，3-6位代表device model，后10位代表生产流水号
     */
    OVD_char OVDDeviceID[MAX_LEN_32];
    /**
     * 设备接入密码，必填
     * 说明：物料清单里登录密码用于服务器鉴权，需要烧录固化到设备里。
     */
    OVD_char OVDLoginPassword[MAX_LEN_64];
    /**
     *设备视频加密密码，必填
     * 说明：物料清单里的视频加密密码，用于对视频流进行加密，需要烧录固化到设备里。
     */
    OVD_char OVDMediaEncPassword[MAX_LEN_128];
    /** 设备型号，必填 */
    OVD_char OVDHardWareModel[MAX_LEN_32];
    /**
     * 设备固件版本号，必填
     * 说明：固件版本号命名规则:需满足 3 段及 3 段以上，且总长不要超过 10 位纯数字，不可使用中划线等其他符号。
     */
    OVD_char OVDSystemVersion[MAX_LEN_32];
    /**
     * 设备芯片ID，必填
     * 说明：杭研分配，主要区分同一设备型号不同芯片方案的情况。
     */
    OVD_char OVDModelId[MAX_LEN_32];
    /**
     * 设备mac地址，必填
     * 说明：
     * (1)设备首次连接平台，平台会记录mac地址，区分大小写。用英文:分隔符分开。
     * (2)设备侧上报的mac地址格式需与包材的mac地址格式保持一致
     */
    OVD_char OVDmacaddress[MAX_LEN_32];
    OVDNetParam netParam;                       //网络信息，必填
    OVD_int32 enable_servicschedule;            //是否启用服务调度
    OVD_char servicescheduleurl[MAX_LEN_1024];  //服务调度的URL
    OVD_char cafilepath[MAX_LEN_256];           //客户端ca文件路径,暂不启用
    OVD_int32 tz;                               //设备时区，默认东8区
} OVDClientParam;

/**
 * OVD 日志相关，注意要做到日志隔离
 */

//日志输出级别依次增高
typedef enum
{
    OVD_LOGLEVEL_TRACE = 0,
    OVD_LOGLEVEL_DEBUG = 1,
    OVD_LOGLEVEL_INFO  = 2,
    OVD_LOGLEVEL_WARN  = 3,
    OVD_LOGLEVEL_ERROR = 4,
    OVD_LOGLEVEL_FATAL = 5,
} OVDLogLevel;

//日志输出位置
typedef enum
{

    OVD_LOGSTD_NO = 0,    //不输出
    OVD_LOGSTD_OUT = 1,   //输出到标准输出
    OVD_LOGSTD_ERR  = 2,  //输出到标准出错
} OVDLogSTD;

typedef struct
{
    OVDLogLevel logLevel;           //日志输出级别，详细见枚举值LogLevel，可选
    OVDLogSTD   logSTD;             //日志输出位置，可选，详细见枚举值LogSTD，可选
    OVD_void(*pOVDLogOutCallBack)(const char
                                  *buff);   //设备提供的日志输出回调，SDK的输出日志可以保存到device的存储文件中，必选
} OVDLogParam;

typedef enum
{
    OVD_STATUS_IDLE           =   0,  //初始态
    OVD_STATUS_DOWNLOADING    =   1,  //安装包下载中
    OVD_STATUS_INSTALLING     =   2,  //安装中
    OVD_STATUS_FAILED         =   3,  //升级失败
    OVD_STATUS_DONE           =   4,  //安装成功
} OVDUpgradeStatus;


//p2p 卡录像机制
typedef struct
{
    OVD_int32 channel;                  //通道号
    OVD_char FileName[MAX_LEN_256];
    OVD_int32  FileType;                //文件类型(0 视频文件， 1 告警文件)
    OVD_uint64 FileStartStamp;          //录像开始时间（单位为秒）
    OVD_uint64 FileEndStamp;            //录像接收时间（单位为秒）
    OVD_int32  RecordDuration;          //时长（单位为秒）
    OVD_int32  FileSize;                //文件大小
} OVDRecordFileInfo;

typedef struct
{
    OVD_int32               fileCount;            //文件数量
    OVDRecordFileInfo fileinfo[MAX_LEN_24];       //文件列表
} OVDRecordFileListPerPage;


typedef enum
{
    OVD_CONTINUE    =   0,  //继续播放
    OVD_PAUSE       =   1,  //暂停
    OVD_STOP        =   2,  //停止
    OVD_FAST        =   3,  //快进
    OVD_SLOW        =   4,  //慢放
    OVD_JUMP        =   5,  //拖动  ms
} OVDCONTROLTYPE;


typedef struct
{
    OVD_int32  SDExist;         //0 not, 1 yes, 2 error
    OVD_int32    SDTotalSize;       //总容量(M)
    OVD_int32    SDFreeSize;        //空闲量
    OVD_char EarlyFileName[MAX_LEN_24]; //当前SD卡最早一个录像文件
} OVDSDInfo;
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
    OVC_PTZ_FOCUS_DOWN         = 24, //焦距调小
    OVC_PTZ_FOCUS_UP           = 25, //焦距调大
    OVC_PTZ_ZOOM_RESET         = 26, //恢复默认倍率
    OVC_PTZ_FOCUS_DOWN_STEP    = 27, //焦距单步调小
    OVC_PTZ_FOCUS_UP_STEP      = 28, //焦距单步调大
} OVCPTZControlCmd;

/** EBO转动位置坐标信息 */
typedef struct
{
    OVD_int32 pan;        /**< 水平方向坐标，整型，范围是[-100,100],只有步进指令或持续移动指令带有这个字段，左为负，右为正 */
    OVD_int32 tilt;       /**< 垂直方向坐标，整型，范围是[-100,100],只有步进指令或持续移动指令带有这个字段，下为负，上为正 */
} OVD_Space_st;

typedef enum
{
    OVC_EBO_MV_STEP      = 1,   //步进指令
    OVC_EBO_MV_CONT      = 2,   //持续移动指令
    OVC_EBO_RESERVE      = 3,   //预留指令
    OVC_EBO_RESERVE2     = 4,   //预留指令
    OVC_EBO_MV_FORWARD   = 5,   //向前猛冲
    OVC_EBO_RECHARGE     = 6,   //回充
    OVC_EBO_CIRCLE       = 7,   //转圈
    OVC_EBO_SHAKE        = 8,   //抖动
} OVCEBOControlCmd;

/* ping和traceroute结构体 */
typedef struct
{
    OVD_char   info[256];//下发ping和traceroute地址
    OVD_int64     data;  //ping和traceroute延时，int64
    OVD_int32     data2;// ping成功率/traceroute跳数，int32
} OVDProbeDataInfo;


//p2p 对讲
typedef enum
{
    OVD_PCM          = 0,
} OVDAUDIOPLY_TYPE;

typedef struct
{
    OVD_uint32 codec;         //OVDAUDIOPLY_TYPE
    OVD_uint32 samplesRate;
    OVD_uint32 bitWidth;
    OVD_uint32 volume;
    OVD_uint32 priority;      //the biger ,the higher
} OVDAudioOutDataFormat;
//p2p 对讲 end

typedef enum
{
    MP3_CLOSE   =   0,       //停止播放
    MP3_PAUSE   =   1,       //暂停播放
    MP3_RESUME  =   2,    //恢复播放
    MP3_OTHER,
} OVDMp3PlayCtrl;

typedef enum
{
    OVD_PLAY_STOPPED        =   0,  //停止播放状态
    OVD_PLAY_PAUSING        =   1,  //暂停状态
    OVD_PLAY_PLAYING        =   2,  //播放中
} OVDAudioPlayStatus;

typedef struct
{
    OVD_char *buf;              //数据buf
    OVD_int32  size;                //数据长度
    //char        ImageUrl[1024];   //目前没用到,可填空
} OVDImageInfo;

typedef enum
{
    OVD_OUTTER  =   2,       //外部告警
    OVD_MOTIOM  =   3,       //移动侦测
    OVD_CROSS   =   4,       //拌网侦测
    OVD_CRY     =   5,       //哭声侦测
    OVD_FACE    =   6,       //脸部识别
    OVD_VOICE   =   7,       //声音侦测
    OVD_LOW_BATTERY           = 8,     //低电告警
    OVD_LOSS_LOCK             = 9,     //撬锁告警
    OVD_BELL                  = 10,      //按铃事件
    OVD_MAN                   = 11,      //人形事件
    OVD_PIR                   = 12,      //pir移动侦测
    OVD_ALERTAREA             = 13,      //警戒功能告警
    OVD_KITCHEN_MASK          = 14,      //厨房行为检测，检测到口罩异常
    OVD_KITCHEN_CAP           = 15,      //厨房行为检测，检测到帽子异常
    OVD_KITCHEN_CLOTHES       = 16,      //厨房行为检测，检测到工服异常
    OVD_VEHICLE_GEN           = 17,      //车型侦测，通用车辆侦测
    OVD_VEHICLE_MOTOR         = 18,      //车型侦测，检测到电瓶车
    OVD_VEHICLE_BIKE          = 19,      //车型侦测，检测到自行车
    OVD_FACE_MASK             = 20,      //口罩检测，检测到人脸未戴口罩
    OVD_REGIONAL_PEOPLE_STAT  = 21,      //区域人数统计
    OVD_PARABOLIC_AERIAL      = 22,      //高空抛物
    OVD_BABY_CRYING           = 23,      //婴儿啼哭，哭声侦测因安防平台原因，合并到婴儿啼哭
    OVD_OFF_DUTY              = 24,      //离岗检测

    OVD_OTHER,
} OVDAlarmType;

typedef struct
{
    OVD_int32             channel; //通道号
    OVD_int64    startTimeStamp;   //报警开始时间戳 (单位为秒)
    OVDAlarmType    AlarmType;     //报警类型
    OVD_char           *desc;      //告警描述
    OVDImageInfo    ImageInfo;     //背景图信息
} OVDUpLoadAlarmInfo;

typedef enum
{
    OVD_WIFI = 0,//IWiFi
    OVD_SSID_WIFI = 1,//ssid和WIFI密码
    OVD_PHONE = 2,        //bindid
    OVD_STRING = 3,
    OVD_XX_SSID_WIFI = 7, //数据读取完整(包含ssid，密码及phoneid/bindid)
    OVD_XX_LESS_DATA = 9  //厂商继续输入新的图片或者字符串
} OVDWifiInfoType;

typedef struct
{
    OVD_char ssid[33];
    OVD_int32 ssidLen;
    OVD_char pwd[80];
    OVD_int32 pwdLen;
} OVDSSIDWiFiInfo;

typedef struct
{
    OVD_char ssid[WIFI_SSID_LEN];   //wifi的ssid
    OVD_int32  ssidLen;
    OVD_char pwd[WIFI_PWD_LEN];    //wifi密码
    OVD_int32  pwdLen;
    OVD_char phone[16 + 1];
    OVD_int32 phoneLen;
    OVD_char extension[256];  //扩展信息字段
    OVD_int32 extensionLen;
} OVDXXSSIDWiFiInfo;

typedef enum
{
    OVD_HIBERNATE_OVC_NOTIFY = 0,   //OVC信令交互，需要保持一段时间不断电
    OVD_HIBERNATE_P2P_NAT = 1,      //P2P开始NAT穿透打洞
    OVD_HIBERNATE_P2P_OPEN = 2,     //正在P2P通话
    OVD_HIBERNATE_OTHER = 3
} OVDHibernateReason;

//p2p 卡录像结构体
typedef struct
{

    OVD_uint64 StartStamp;       //录像开始时间（单位为秒）
    OVD_uint64 EndStamp;         //录像接收时间（单位为秒）

} OVD_DMERecordFileInfo;

typedef struct
{
    OVD_int32               Count;            //卡录像片段个数
    OVD_DMERecordFileInfo sectioninfo[64];    //卡录像一个连续区间的信息
} OVD_DMERecordFileListPerPage;

/*
远程配置参考doc/config.txt
*/

//ovd 状态
enum ovd_state
{
    OVD_ONLINE = 1, //设备在线
    OVD_UPDATING = 2, //设备升级
    OVD_ABNORMAL = 3, //异常
    OVD_LOWPOWER_OFFLINESLEEP = 4 //不支持远程唤醒的低功耗设备，在进入休眠时需要上报该状态，app借此展示为休眠状态而非离线状态。
};


//channels 状态
enum ovd_channel_state
{
    OVD_CHANNEL_OFFLINE = 0, //离线
    OVD_CHANNEL_ONLINE = 1, //在线
    OVD_CHANNEL_ABNORMAL = 3, //异常
    OVD_CHANNEL_DISABLE = 4, //不使能（休眠）
};


typedef struct
{
    OVD_int32 channelnum;
    enum ovd_channel_state state;
} Simplechannelinfo;

typedef struct
{
    enum ovd_state state;  //1：设备在线        2：设备升级        3：异常
    Simplechannelinfo channelarray[64];  //用静态数组，最多支持64个channel,不用malloc
    OVD_int32 channelcount;
} SimpleOVDinfo;


//数据帧信息
typedef struct
{
    OVD_bool iskey_frame;  //是否是I帧(必填)
    OVD_uchar rev[3];      //预留1
    OVD_ushort width;
    OVD_ushort height;
    OVD_ushort framerate;  //帧率(必填)
    OVD_ushort gop;        //(必填)
    OVD_uchar rev2[20];    //预留2
} OVD_VideoFrameInfo;

typedef struct
{
    OVD_uint32 samplesRate;    //采样率(必填)
    OVD_ushort channelNumber;  //声道数
    OVD_ushort bitsPerSample;  //位深
    OVD_uint32 samplePerFrame; //每帧采样点数
    OVD_uchar rev[20];         //预留
} OVD_AudioFrameInfo;


typedef struct
{
    OVDContentType frame_type;       //帧类型，音频or 视频
    OVDAVCodec codec_type;           //编码类型
    OVD_uint64 timestamp;            //绝对时间戳，单位为毫秒
    OVD_VideoFrameInfo video_info;
    OVD_AudioFrameInfo audio_info;
    OVD_uint32 framebuf_len;        //帧长度
    OVD_char *frame_buf;            //帧内容
    OVD_uchar rev[8];               //预留
} OVD_FrameInfo;


/** 获取设备运行状态信息回调 */
typedef enum
{
    /// 获取设备丢包率，单位%。int型。设备端使用ifconfig指令来获取网卡的数据传输情况。packetLossRate=(drop_rx+drop_tx)/(rx+tx)
    OVD_CMD_GET_PACKET_LOSSRATE,
    /// 获取设备WIFI强度，单位dB(负值)。int型。
    OVD_CMD_GET_WIFI_QUALITY,
    /// 获取设备CPU占用率，单位%。int型。
    OVD_CMD_GET_CPU_LOAD,
    /// 获取设备内存占用率，单位%。int型。
    OVD_CMD_GET_MEMORY_LOAD,
    /// 获取设备剩余内存，单位KB。int型。
    OVD_CMD_GET_MEMORY_AVAILABLE,
    /// 获取设备运行时长，单位s。int型。
    OVD_CMD_GET_RUNTIME,
    /// 获取设备运行总内存，单位KB。int型。
    OVD_CMD_GET_MEMORY_TOTAL,
    /// 获取设备运行总闪存，单位KB。int型。
    OVD_CMD_GET_FLASH_TOTAL,
    /// 获取芯片厂家，char[64]。
    OVD_CMD_GET_CHIP_SUPPLIER,
    /// 获取芯片型号，char[64]。
    OVD_CMD_GET_CHIP_MODEL,
    /// 获取和家智话SDK版本号，char[64]。
    OVD_CMD_GET_HJZH_SDKVER,
    /// 获取Andlink SDK版本号，char[64]，不支持返回空。
    OVD_CMD_GET_ANDLINK_SDKVER,
    ///下发ping地址，返回ping 5次的平均延时ms和成功率（丢包率）（要求10min内返回回调结果），详见OVDProbeDataInfo
    OVD_CMD_GET_PING,
    ///下发Traceroute地址，返回traceroute跳到目的地跳了多少次，跳到目的地用了多少时间（ms）（要求10min内返回回调结果），详见OVDProbeDataInfo
    OVD_CMD_GET_TRACEROUTE,
    ///获取设备4G信号强度。int型。
    OVD_CMD_GET_RSSI_4G,
} OVD_GetDevRunningInfo_e;


/** 设置云存套餐信息 */
typedef enum
{
    ///无云存套餐
    OVD_CMD_SET_CLOUD_NONE,
    ///云存全天套餐
    OVD_CMD_SET_CLOUD_ALL_DAY,
    ///云存事件套餐
    OVD_CMD_SET_CLOUD_EVENT,
} OVD_SetCloudInfo_e;


#endif
