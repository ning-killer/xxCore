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

#define MAX_CHANNEL_COUNT  32  //设备支持最多的channel数量，channel编号范围为0～63
#define MAX_STREAM_COUNT   4   //单个视频通道最大码流个数
//为节省sdk，内部先将channel数调整为1

//错误码
#define  RPC_UNIMPLEMENT 101 //未实现回调

#define ALARM_TIME_MIN_INTERVAL 10
#define ALARM_TIME_MAX_INTERVAL 3600
#define OVD_MAX_LOCAL_STORAGE_PATH_LEN 256

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifdef __GNUC__
    #define OVD_EXPORT __attribute__((visibility("default")))
#else
    #define OVD_EXPORT
#endif

/**
 * @brief 重启原因
 */
 typedef enum
 {
    OVD_REBOOT_REASON_DUT_COMMON=1000, //设备侧通用原因重启
    OVD_REBOOT_REASON_DUT_POWER_DOWN=1001, //设备侧断电重启
    OVD_REBOOT_REASON_DUT_CRASH=1002, //设备侧崩溃触发的重启
    OVD_REBOOT_REASON_DUT_HWATCHDOG=1003, //设备侧硬件看门狗触发的重启
    OVD_REBOOT_REASON_DUT_FACTORY_RESET=1004, //设备侧本地恢复出厂设置引发的重启
    OVD_REBOOT_REASON_DUT_ABNORMAL_REBOOT=1005, //设备侧异常重启，可能规避一些异常逻辑而导致的重启

    OVD_REBOOT_REASON_SDK_COMMON=2000, //sdk逻辑触发的通用原因重启
    OVD_REBOOT_REASON_SDK_SCHEDULED_MAINTENANCE=2001,//sdk自动维护触发的重启
    OVD_REBOOT_REASON_SDK_SWATCHDOG=2002, //sdk软看门狗触发的重启

    OVD_REBOOT_REASON_CLOUD_COMMON=3000, //平台触发的通用重启原因
    OVD_REBOOT_REASON_CLOUD_REBOOT=3001, //平台触发的远程重启
    OVD_REBOOT_REASON_CLOUD_UPGRADE=3002, //平台触发的升级重启
    OVD_REBOOT_REASON_CLOUD_FACTORY_RESET=3003, //平台触发的恢复出厂设置
 }ovd_reboot_reason_e;


typedef enum
{
    OVD_RET_STREAM_CONNECT_CHANNEL_BROKEN = -5,		//底层流媒体通道出现断开
    OVD_RET_COMMON_ERROR                  = -1, 	//通用错误码
    OVD_RET_SUCCESS                       =  0, 	//通用成功
    OVD_RET_BADPARAMETER                  =  1, 	//入参错误
    OVD_RET_NOMEM                         =  2, 	//内存错误

    //配网相关错误码
    OVD_NETCONF_SUCCESS=7,         //配网识别数据成功
    OVD_NETCONF_LESS_DATA=9,       //配网缺失数据，数据太长，二维码分页，需要厂商再次输入
    OVD_RET_RPC_UNIMPLEMENT = 101, //该功能模拟未使能或未支持
    DME_RPC_SUBSTREAM_NOTSUPPORT = 107, //性能不足，子码流开启失败

    //srt卡回放机制错误码
    OVD_RET_READ_FRAME_RETRY=201, //读帧重试错误码
    OVD_RET_READ_FRAME_EOF=202,   //卡录像读取完毕，厂商反馈EOF
    OVD_RET_SEEK_NODATA=203,      //seek点无音视频数据

}OVD_ERRNO;




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

#define OVD_FALSE   0
#define OVD_TRUE    1

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
}OVDAIFaceCapInfo;

typedef struct
{
    OVD_bool passenger;                         //布尔型，设备是否支持上报passenger,默认值为false
    OVD_bool passengerExposureAdjust;           //布尔型，设备是否支持人脸曝光度调节,默认值为false
    OVD_char passengerCaptureModeList[MAX_LEN_256]; //字符串,抓拍模式调节列表选项，若没有，则设备不支持模式切换,默认“quality”:代表质量抓拍，“quick”:代表快速抓拍,用逗号相隔
    OVD_int32 passengerAlarmzone;
    OVD_bool support_osd_status;                //布尔型，设备是否支持osd显示客流统计信息
}OVDAIPassengerCapInfo;


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
    //OVD_char CaptureModeList[MAX_LEN_256];
    OVD_int32 alarm_zone;
	OVD_bool vehicle_detect_site;
}OVDAIVehicleCapInfo;

typedef struct
{
	OVD_bool nonmotorvehicle_detection;
    OVD_char CaptureModeList[MAX_LEN_256];
    OVD_int32 alarm_zone;
}OVDAINonmotorvehicleCapInfo;

typedef struct
{
    OVD_bool regionalPeopleStat;                      //布尔型，设备是否支持区域人数统计，0表示不支持，1表示支持，默认不支持
    OVD_bool support_alert_time;                      //布尔型，是否支持设置检测时间， 0表示不支持，1表示支持，默认不支持
    OVD_bool support_osd_status;                      //布尔型，是否支持设置OSD叠加人数， 0表示不支持，1表示支持，默认不支持
    OVD_int32 support_regional_people_count;          //整型，个数，0表示不支持，其他值表示对应配置项“触发预警的人数”的最大值限制，默认不支持
    OVD_int32 support_alarm_report_duration;          //整型，分钟，0表示不支持，其他值表示对应配置项“告警上报间隔时间”的最大值限制，默认不支持
    OVD_int32 support_detect_result_report_duration;  //整型，分钟，0表示不支持，其他值表示对应配置项“区域人数定时上传”的最大值限制，默认不支持
    OVD_int32 detect_plans_num;                       //整型，检测计划的个数，0不支持，最大值10个，默认不支持
    OVD_int32 alarm_area_num;                         //整型，检测区域的个数，0不支持，最大值10个，默认不支持
    OVD_int32 alarm_zone;                             //整型，0不支持，1：只支持矩形，n(n>=3):支持不规则n边形，目前只支持4边形。最大支持10边形，默认不支持
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

//车道线
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
}OVDAICapInfo;

//注：当前和家亲app声光联动只有体现在警戒功能上
typedef struct
{
    OVD_char alertarea[256];
	OVD_char vehicle_detection[256];
    OVD_char nonmotorvehicle_detection[256];
	OVD_char mask_detection[256];
    OVD_char regionalPeopleStat_detection[256];
    OVD_char transgression[256];
}OVDlinkage_mode; //设备sdk 联动方式的支持，每种告警一个元素

typedef struct
{
    OVD_bool preference_call;         //布尔型，是否支持呼叫偏好功能；呼叫偏好：可设置在发起呼叫时，普通电话优先或者APP通话优先
    OVD_bool set_calling_number;      //布尔型，是否支持呼叫号码设置的功能；呼叫号码：可设置点击呼叫按键后，摄像头呼叫的号码（被叫号码）
    OVD_bool not_disturb_mode;        //布尔型，是否支持手机勿扰模式；手机勿扰是控制设备端主叫手机端。可设置设备勿扰开关是否打开，如果打开，可设置勿扰时间
    OVD_bool not_disturb_device_mode; //布尔型，是否支持设备勿扰模式，设备勿扰是控制手机端主叫设备端
    OVD_bool auto_hangup;             //布尔型，是否支持自动挂断功能；自动挂断：可设置检测到访客离开/进门后自行停止呼叫
    OVD_bool voice_calling_number;    //布尔型，是否支持语音呼叫号码功能；如果设备上报支持，则APP新增“语音呼叫号码”配置项，配置完成后，可实现通过语音给指定的号码打电话
}OVDHJKH_mode; //设备上对和家看护功能的功能支持配置


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
    OVD_bool cruise;    //<布尔型，是否支持巡航功能，开启后支持巡航> 1.29新增
    OVD_bool collision;  //<布尔型，是否支持防碰撞功能，开启后支持防碰撞> 1.29新增
    OVD_bool reminder;  //#<布尔型，是否支持提示音功能，开启后支持提示音> 1.34新增
} OVDEBO_mode;

typedef enum{
    OVD_UNLOCKWAYCAP_FP  = (1<<0),     //指纹
    OVD_UNLOCKWAYCAP_PWD = (1<<1),     //密码
    OVD_UNLOCKWAYCAP_CARD= (1<<2),     //门卡
    OVD_UNLOCKWAYCAP_WHITE_SIM=(1<<3), //超级SIM卡(白卡)
    OVD_UNLOCKWAYCAP_ENCRY_SIM=(1<<4), //超级SIM卡(加密)
    OVD_UNLOCKWAYCAP_FINGER_VEIN=(1<<5),//指静脉
    OVD_UNLOCKWAYCAP_IRIS= (1<<6),     //虹膜
    OVD_UNLOCKWAYCAP_TMPPWD=(1<<7), //临时密码
    OVD_UNLOCKWAYCAP_REMOTECALL=(1<<8),//固话按键远程开锁
    OVD_UNLOCKWAYCAP_FACE = (1<<9),    //人脸
    OVD_UNLOCKWAYCAP_OTHER,
}ovd_unlockway_cap_t;

typedef enum{
    OVD_LOCKBATTERYCAP_SINGLE=0,           //单电池方案
    OVD_LOCKBATTERYCAP_PRIMARY_BACKUP=1,   //主备电
    OVD_LOCKBATTERYCAP_DUAL_POWER=2,       //双供电
    OVD_LOCKBATTERYCAP_OTHER,
}ovd_lockbattery_cap_t;
//门锁品类
typedef struct
{
    OVD_int32 unlockway;//门锁支持的开锁方式，用比特位掩码表示支持方式，掩码映射表参考
    ovd_lockbattery_cap_t battery;//门锁电池方案
    OVD_bool  usermanage;//是否支持成员管理
    OVD_bool  tmppwd;//是否支持临时密码
} ovd_lock_cap_params_t;

typedef struct
{
    OVD_int32 support_set_target;
    OVD_bool support_staymode_time;
    OVD_bool support_set_alerttime;
    OVD_int32 support_alarms_alertarea_zone;
} ovd_alertarea_params_t;

typedef struct
{
    OVD_int32 support_set_target;
    OVD_bool support_set_alerttime;
    OVD_bool support_set_statistics_line;
} ovd_transgression_params_t;

typedef struct
{
    OVD_bool have_alarms_io;   				 //<布尔型：是否支持外包报警,默认值为false>,
    OVD_bool have_alarms_face; 				 //<布尔型：是否支持人脸侦测,默认值为false>,
    OVD_int32 support_alarms_face_zone; 	 //<整型:0不支持，1：只支持矩形，n(n>=3):支持不规则n边型>, 注：目前app限制，只需要设备支持矩形即可

    OVD_bool have_alarms_cry;                             //<布尔型：是否支持哭声侦测,默认值为false>,
    OVD_bool support_crying_pacify;                       //<布尔型：是否啼哭安抚功能>,
    OVD_int32 crying_pacify_audio_playing_count;          //<整型：啼哭安抚音频播放次数的最大值限制>,
    OVD_bool support_crying_pacify_audio_playing_volumn;  //<布尔型：是否支持设置啼哭安抚音频播放音量>,
    OVD_int32 crying_pacify_audio_playing_type;           //<整型：支持设置啼哭安抚音频的方式>,

    OVD_bool have_alarms_voice; 			 //<布尔型：是否支持声音侦测,默认值为false>,
    OVD_bool have_alarms_motion; 			 //<布尔型：是否支持移动侦测,默认值为false>,
    OVD_int32 support_alarms_motion_zone;    //<整型:0不支持，1：只支持矩形，n(n>=3):支持不规则n边型>, 注：目前app限制，只需要设备支持矩形即可
    OVD_bool have_alarms_body;  			 //<布尔型：是否支持人形侦测,默认值为false>,
    OVD_int32 support_alarms_body_zone; 	 //<整型:0不支持，1：只支持矩形，n(n>=3):支持不规则n边型>, 注：目前app限制，只需要设备支持矩形即可
    OVD_bool have_alarms_cross;				 //<布尔型：是否支持拌网侦测>,
    OVD_bool have_alarms_pir; 				 //<<布尔型，是否支持pir侦测>,
    OVD_bool have_alarms_pir_staymode;		 //<<布尔型，是否支持pir逗留模式,门铃>
    OVD_bool have_alarms_lossLock; 			 //<<布尔型，是否支持撬锁侦测>
    OVD_bool have_alarms_alertarea;          //<<布尔型，是否支持警戒功能>
    OVD_bool have_alertarea_expel;           //<布尔型，是否支持智能驱离功能（针对带云台的警戒设备）>,1.34新增
    OVD_int32 hava_alarms_alertarea_zone;    //<整型:0不支持，1：只支持矩形，n(n>=3):支持不规则n边型>, 注：目前app限制，只需要设备支持矩形即可
    ovd_alertarea_params_t alertarea_params; //1.33新增，区域入侵参数
    OVD_bool have_alarms_transgression;      //<<1.33 新增 布尔型：是否支持越界侦测功能，默认值为false>
    ovd_transgression_params_t transgression_params; //1.33新增，越界侦测参数

}OVDAlarmsInfo;

typedef enum
{
    DEVICE_TYPE_IPC = 0,    // 普通IPC设备，单个物理视频通道 see cap_ipc_t
    DEVICE_TYPE_LOWPOWER_REPEATER,  // 低功耗中继型设备 see cap_ipc_t
    DEVICE_TYPE_LOWPOWER,   // 低功耗非中继型设备 see cap_ipc_t
    DEVICE_TYPE_NVR,    // NVR设备 see cap_nvr_t
    DEVICE_TYPE_MIPC,    // 多通道IPC设备
}ovd_device_type_e;


//@breif 视频码流加密映射表
typedef enum
{
    OVD_MEDIA_NO_ENCRYPT        = 0,                // 无加密
    OVD_MEDIA_BASE_ENCRYPT      = 1,                // 基础加密
    OVD_MEDIA_ADVANCED_ENCRYPT  = 2,                // 钥匙链加密
    OVD_MEDIA_ENCRPYT_TYPE_OTHER,
}ovd_media_encrypt_type_e;

/* 智能夜视支持的侦测类型掩码 */
#define OVD_NIGHTVISION_DETECT_HUMAN   (1 << 0)
#define OVD_NIGHTVISION_DETECT_MOTION  (1 << 1)


typedef struct OVDCapInfo_s
{
    ovd_device_type_e device_type; // 整型,0代表普通IPC，1代表低功耗中继型设备，2代表低功耗非中继型设备,1.21版本新增
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

    OVD_bool multi_stream; //<布尔型，是否支持主子码流功能，默认值为false> 1.30版本新增

    OVD_bool have_battery;      //<布尔型：是否电池供电,主要用于门铃设备,默认值为false>,
    OVD_bool have_audio_out;    //<布尔型：是否支持音乐输出,默认值为false>
    OVD_bool have_voice_out;    //<布尔型：是否支持对讲输出,默认值为false>
    OVD_bool have_ivrs;         //<布尔型：是否支持ivrs协议，默认值为false，表示云存只支持ivr协议>,
    OVD_bool have_trace;        //<布尔型：是否支持移动跟踪,默认值为false>

    OVDAICapInfo  ovdAICapInfo;      //AI相关
    OVDAlarmsInfo ovdCapInfo_alarms; //alarms 告警上报相关能力
    OVDlinkage_mode ovdlinkage_mode; //联动方式的支持

    OVD_bool log_upload_https;       //<布尔型：日志文件上传是否支持HTTPS，若不支持，则OVC下发的日志上传指令中的http URL为http协议，默认值为false>,
    OVD_bool firmware_download_https;//<布尔型：固件下载是否支持HTTPS，若不支持，则OVC下发的升级指令中的http URL为http协议，默认值为false>,
    OVD_bool video_encrypt;          //<布尔型：是否支持视频加密，默认值为false>

    OVD_bool have_switch;            //<布尔型：是否支持通道使能开关，默认值为false>
    OVD_bool have_sd;                //<布尔型：是否支持SD卡/TF卡，默认值为true>
    OVD_bool have_led;               //<布尔型：是否支持led灯开关，默认值为false>
    OVD_bool light_supplement_lamp;  //<布尔型：是否支持补光灯进行照明功能用,默认值为false>
    OVD_bool have_screen;            //<布尔型：是否支持带屏,默认值为false>
    OVD_bool have_support_softprobe; //<布尔型：是否支持软探针模块，默认值为false>
    OVD_bool have_auto_reboot;       //<布尔型：是否支持设备自动维护，默认值为false>
    OVD_bool have_timed_dormancy;       //<布尔型：是否支持定时休眠开关，默认值为false>
    OVD_bool yard_light_brightness;//<布尔型：是否支持庭院灯灯光控制,默认false>
    OVD_bool yard_light_manual_control;//<布尔型：是否支持手动控制,默认false>
    OVD_int32 yard_light_timed_schedule;//<整型，是否支持计划控制，0为不支持，非0数字表示支持几个计划控制时间段，默认为2，最大值为5>

    OVD_bool dashcam; //<布尔型：是否支持行车记录仪功能，默认不支持>
    OVD_bool smoke_sensor;//<布尔型：是否支持四合一烟雾传感器功能，默认不支持>
    OVD_bool AIGC_cloud_record;//是否支持AI云存

	//<字符串列表(中间用,隔开)：支持的视频清晰度选项列表，列表元素可选值为：ld/sd/hd/fhd，分别代表低清/标清/高清/全高清，默认支持sd和hd>
    OVD_char video_quality[MAX_LEN_24];
    //<字符串列表(中间用,隔开)：支持的视频编码格式列表，列表元素可选值为：h264/h265,默认支持h264>
    OVD_char video_formats_supportlists[MAX_LEN_24];
    //<布尔型，是否支持平台切换视频h264,h265格式>
    OVD_bool support_set_video_formats;
    //<bool型，静态场景码率自适应，详细要求参考入库规范相关功能说明，默认值为不支持>
    OVD_bool support_static_scence_ABR;

    /*
        OVD_MEDIA_NO_ENCRYPT  该设备不支持码流加密功能
        OVD_MEDIA_BASE_ENCRYPT  只支持基础加密方案，适用于所有设备
        OVD_MEDIA_ADVANCED_ENCRYPT 用于设备性能富裕的设备，说明设备能够支持aes_cb_256加密及基础加密方案
    */
    ovd_media_encrypt_type_e stream_encryption_mode;

    ovd_lock_cap_params_t lock_params;//1.34新增，门锁品类相关能力集

    OVD_bool have_voiceout_volume; //<布尔型： 设备是否支持对讲音量调节，默认值为false>
    OVD_bool have_doorbell_volume;
    OVD_bool support_set_normal_nightvision_mode;//<布尔型: 普通摄像头是否支持设备切换夜视模式，夜视模式分为开启/关闭/自动>
    OVD_bool support_set_color_nightvision_mode;//<布尔型：全彩摄像头是否支持设备切换夜视模式，夜视模式分为黑白夜视/全彩夜视/智能夜视>
    OVD_int64 support_detect_nightvision_mask; // <整形：智能夜视模式可支持的侦测类型掩码，比特位为1表示支持,共64位。参考 OVD_NIGHTVISION_，默认值0x0000000000000000>

    //osd相关
    OVD_bool osd_text; //布尔型，是否支持持文本水印
    OVD_bool osd_logo; //布尔型，是否支持水印图片
    OVD_bool enable_hjkh; //布尔型，设备是否集成了和家固话，默认值为false
    OVDHJKH_mode hjkh_mode;// 设备和家看护相关的功能
    //机器人
    OVD_bool enable_EBO; //<布尔型，设备是否支持机器人指令，默认值为false>
    OVDEBO_mode EBO_mode;
    OVD_char robot_ctrl[MAX_LEN_32];//<字符串类型，支持的机器人控制类型>

    OVD_char have_support_extension[MAX_LEN_256]; //<字符串列表(中间用,隔开)：支持的extension方法列表，列表元素可选值为：echotest/ccrtc>

    //喂食相关
    OVD_bool enable_petfeeder; //布尔型，是否支持众鼎宠物喂食器相关能力集，1.29版本新增
    OVD_int32 petfeeder_plan_num; //int型，数量为支持的最大计划条数，最大值为10
    OVD_int32 petfeeder_feed_num; //int型，数量为支持的喂食份数，值必须大于0

    // 多通道设备
    OVD_int32 camera_channel_max;  // 多通道设备最大支持的通道个数，单通道设备不用填写该字段
    OVD_int32 robot_housekeeper;  //1.33新增，整型，位运算0x0F。设备是否支持机器人管家相关功能。最低位表示"天气预报" 0x01，第二位表示"吃药提醒" 0x02，默认0表示不支持
    OVD_int32 camera_lens_type;  //<1.34.0新增，整型，位运算 0x0F。设备支持的摄像头镜头形态。最低位表示"单目" 0x01，第二位表示"双目拼接" 0x02，第三位表示"单目枪球联动" 0x04，第四位表示"双目拼接枪球联动" 0x08，第五位表示"全景拼接枪球联动" ，默认 0x01>
} OVDCapInfo;

typedef struct OVD_ChannelCap_s
{
    OVD_bool have_switch;            //<布尔型：是否支持通道使能开关，默认值为false>
    OVD_int32 audiosamplerate; //枚举整型，0（默认值）：代表8K， 1：代表16K, 1.27版本新增

    OVD_bool have_ptz;          //<布尔型：是否支持云台控制能力,默认值为false>
    OVD_bool support_ptz_preset;//<布尔型：是否支持云台预置位,默认值为false>

    OVD_bool zoomcontrol; //<布尔型，是否支持用户手动调节倍率，默认值为false>
    OVD_bool focuscontrol; //<布尔型，是否支持用户手动调节焦距，默认值为false>

    OVD_bool multi_stream; //<布尔型，是否支持主子码流功能，默认值为false> 1.30版本新增

    OVDAICapInfo ovdAICapInfo;      //AI相关
    OVDAlarmsInfo ovdCapInfo_alarms; //alarms 告警上报相关能力
    OVDlinkage_mode ovdlinkage_mode; //联动方式的支持

    OVD_bool video_encrypt;          //<布尔型：是否支持视频加密，默认值为false>

	//<字符串列表(中间用,隔开)：支持的视频清晰度选项列表，列表元素可选值为：ld/sd/hd/fhd，分别代表低清/标清/高清/全高清，默认支持sd和hd>
    OVD_char video_quality[MAX_LEN_24];
    //<字符串列表(中间用,隔开)：支持的视频编码格式列表，列表元素可选值为：h264/h265,默认支持h264>
    OVD_char video_formats_supportlists[MAX_LEN_24];
    //<bool型，静态场景码率自适应，详细要求参考入库规范相关功能说明，默认值为不支持>
    OVD_char support_static_scence_ABR;


    /*
        OVD_MEDIA_NO_ENCRYPT  该设备不支持码流加密功能
        OVD_MEDIA_BASE_ENCRYPT  只支持基础加密方案，适用于所有设备
        OVD_MEDIA_ADVANCED_ENCRYPT 用于设备性能富裕的设备，说明设备能够支持aes_cb_256加密及基础加密方案
    */
    ovd_media_encrypt_type_e stream_encryption_mode;//设备最强性能支持的加密方式

    ovd_lock_cap_params_t lock_params;//1.34新增，门锁品类相关能力集

    //<布尔型，是否支持平台切换视频h264,h265格式>
    OVD_bool support_set_video_formats;

    OVD_bool support_set_normal_nightvision_mode;//<布尔型: 普通摄像头是否支持设备切换夜视模式，夜视模式分为开启/关闭/自动>
    OVD_bool support_set_color_nightvision_mode;//<布尔型：全彩摄像头是否支持设备切换夜视模式，夜视模式分为黑白夜视/全彩夜视/智能夜视>
    OVD_int64 support_detect_nightvision_mask; // <整形：智能夜视模式可支持的侦测类型掩码，比特位为1表示支持,共64位。bit0: 人形侦测，bit1：移动侦测，默认值0x0000000000000000>

    //osd相关
    OVD_bool osd_text; //布尔型，是否支持持文本水印
    OVD_bool osd_logo; //布尔型，是否支持水印图片
}OVD_ChannelCap_t;


typedef enum
{
    OVD_CONTENT_NONE =   0,
    OVD_Video   =	1,          //视频
    OVD_Audio	=	2,          //音频
    OVD_Private	=	3,          //私有数据：该类型只在SDK内部使用
}OVDContentType;

typedef enum
{
    OVD_HIGH_STREAM  =   1,   //主码流
    OVD_LOW_STREAM   =   2    //子码流
} OVDCodeStream;

typedef enum
{
    source_STREAM_NO_CODEC = 0,
    source_STREAM_CODEC_H264,
    source_STREAM_CODEC_H265,
    source_STREAM_VIDEO_CODEC_MAX = 15,
    source_STREAM_CODEC_AAC_WITH_ADTS = 16,
    source_STREAM_CODEC_AAC,                //当前sdk音频格式只支持aac with adts
    source_STREAM_AUDIO_CODEC_MAX = 30,
    source_STREAM_CODEC_PRIVATE = 31,
    source_STREAM_CODEC_MAX
}OVDAVCodec;


typedef enum
{

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
    OVDAVCodec codec;     //编解码类型，例如h264/h265/aac

    OVD_int32 quality;    //暂未使用，编码质量CQS
    OVD_uint32 bitrate;   //暂未使用，码率
    OVD_ushort width;     //暂未使用，分辨率宽
    OVD_ushort height;    //暂未使用，分辨率高
    OVD_uchar framerate;  //暂未使用，帧率
    OVD_uchar colorDepth; //暂未使用，像素位数，例如8位像素为256色
    OVD_uchar frameInterval;  //暂未使用，帧间隔
    OVD_uchar reserve;    //暂未使用，保留字段

}OVDVideoDataFormat;

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
}OVDAudioDataFormat;



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
    OVD_bool macAddr[MAX_LEN_32]; 	            //MAC地址，空值表示未知，可选,格式大写，以:号相隔
    OVD_int32 battery;  //<可选，整数： 设备当前设备电池电量, 0-100, 该字段不存在表示设备不支持电池供电>
	OVD_int32 cpuLoad;  //<可选，整数，cpu负载率0~100>
	OVD_int32 memoryTotal; //<可选，整数，总内存数，单位为KB>
	OVD_int32 memoryAvailable; //<可选，整数，剩余可用内存，单位为KB>
}OVDDeviceInfo;

/**
 * OVD 网络信息
 */
typedef struct
{
    OVD_char passDomain[MAX_LEN_64];		//<云服务器的域名>，必填
    OVD_int32 passPort;        				//<云服务器的端口>，必填
    OVD_int32 Secure_PassPort;  			//<云服务器的加密端口>,必填
    OVD_char p2p_passDomain[MAX_LEN_64];    // <P2P pass 的域名>，必填
    OVD_int32 p2p_passPort;   				//<杭研p2p pass的端口>，必填
    OVD_char turnDomain[MAX_LEN_64];        //<P2P turn的域名>，没有置为空
    OVD_int32 turnPort;      				//<p2p turn 的端口>，没有置为-1
	OVD_char probeDomain[MAX_LEN_64];       //<软探针服务器的域名>，必填
    OVD_int32 probePort;      				//<软探针服务器的端口>，必填
    OVD_int32 probeSecurePort;  			//<软探针服务器的加密端口>,必填
    OVD_char hibernationDomain[MAX_LEN_64]; //<休眠服务地址域名>，没有置为空
    OVD_int32 hibernationPort;       		//<休眠服务地址端口>，没有置为-1
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
	/** 设备硬件型号，必填 */
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
    OVD_char OVDmacHash[MAX_LEN_64];           //macHash,防克隆校验信息，需要烧录固化到设备里，且恢复出产设置时不能擦除。
    OVDNetParam netParam;           			//网络信息，厂商无需填写
    OVD_int32 enable_servicschedule;      		//是否启用服务调度,厂商需填写1
    OVD_char servicescheduleurl[MAX_LEN_1024];  //服务调度的URL
    OVD_char cafilepath[MAX_LEN_256];   		//客户端ca文件路径,暂不启用
    OVD_int32 tz;                        		//设备时区，默认东8区
    OVD_char local_storage_path[MAX_LEN_256];   //卡录像挂载路径,暂不启用
    OVD_char ovd_data_path[MAX_LEN_256];        //数据文件保存路径，保存flash，不设置会失败
    OVD_char ovd_log_path[MAX_LEN_256];         //本地日志保存路径，保存内存，需可读写，eg:/tmp,如不设置，默认写在/tmp目录下

}OVDClientParam;

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
}OVDLogLevel;

//日志输出位置
typedef enum
{

    OVD_LOGSTD_NO = 0,    //不输出
    OVD_LOGSTD_OUT = 1,   //输出到标准输出
    OVD_LOGSTD_ERR  = 2,  //输出到标准出错
}OVDLogSTD;

typedef struct
{
    OVDLogLevel logLevel;           //日志输出级别，详细见枚举值LogLevel，可选
    OVDLogSTD   logSTD;             //日志输出位置，可选，详细见枚举值LogSTD，可选
    OVD_void(*pOVDLogOutCallBack)(const char
                                  *buff);   //设备提供的日志输出回调，SDK的输出日志可以保存到device的存储文件中，必选
} OVDLogParam;

typedef struct
{
    ovd_reboot_reason_e reason;     //重启原因
    OVD_uint64 ts;                  //重启的时间戳（绝对时间戳）
    OVD_char message[256];          //辅助描述性字符串，厂商可在此简要描述重启原因
}ovd_reboot_param_t;            //厂商传入上次重启的具体信息

typedef enum
{
    OVD_STATUS_IDLE =0,             //初始态
    OVD_STATUS_DOWNLOADING =1,      //安装包下载中
    OVD_STATUS_DOWNLOAD_FINISH=2,  //下载成功
    OVD_STATUS_DOWNLOAD_FAIL=3,    //下载失败
    OVD_STATUS_INSTALLING =4,      //安装中
    OVD_STATUS_FAILED =5,          //升级失败
    OVD_STATUS_DONE  =6,           //安装成功
    OVD_FAILED_SD_NOT_FOUND = 7,   //升级失败原因：未检测到SD卡，请插入SD卡
    OVD_FAILED_LOW_BATTERY_LESS_30  = 8, // 升级失败原因：设备电量过低，当设备在充电桩上时，电量应该超过30%。
    OVD_FAILED_LOW_BATTERY_LESS_70 = 9,  // 升级失败原因：设备电量过低，当设备不在充电桩时，电量应该超过70%。

    OVD_STATUS_MAX
}OVDUpgradeStatus;


/** 升级 */
typedef struct {
    OVD_char        firewarm;          /**< 版本号 */
    OVD_int32      time;      /**< 上次升级时间 */
} OVDFirmInfo;

//p2p 卡录像机制
typedef struct
{
    OVD_int32 channel;                  //通道号
    OVD_char FileName[MAX_LEN_256];
    OVD_int32  FileType; 			    //文件类型(0 视频文件， 1 告警文件)
    OVD_uint64 FileStartStamp;		    //录像开始时间（单位为秒）
    OVD_uint64 FileEndStamp;			//录像接收时间（单位为秒）
    OVD_int32  RecordDuration; 			//时长（单位为秒）
    OVD_int32  FileSize; 			    //文件大小
}OVDRecordFileInfo;

typedef struct
{
    OVD_int32               fileCount;            //文件数量
    OVDRecordFileInfo fileinfo[MAX_LEN_24];       //文件列表
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
    OVC_PTZ_FOCUS_DOWN         = 24, //焦距调小
    OVC_PTZ_FOCUS_UP           = 25, //焦距调大
    OVC_PTZ_ZOOM_RESET         = 26, //恢复默认倍率
    OVC_PTZ_FOCUS_DOWN_STEP    = 27, //焦距单步调小
    OVC_PTZ_FOCUS_UP_STEP      = 28, //焦距单步调大
}OVCPTZControlCmd;

typedef enum
{
    OVC_EBO_MV_STEP      = 1,   //步进指令
    OVC_EBO_MV_CONT      = 2,   //持续移动指令
    OVC_EBO_NOFALL_TEST  = 3,   //防跌落
    OVC_EBO_LED_TEST   = 4,     //补光灯
    OVC_EBO_MV_FORWARD   = 5,   //向前猛冲
    OVC_EBO_RECHARGE     = 6,   //回充
    OVC_EBO_CIRCLE       = 7,   //转圈
    OVC_EBO_SHAKE        = 8,   //抖动
    OVC_EBO_SELF_TEST      = 9,  //启动自检
    OVC_EBO_REPORT      = 10,   //报告查看
    OVC_EBO_REPORT_LEN      = 11,   //报告返回长度
    OVC_EBO_MV_START     = 12,  //移动开始指令
    OVC_EBO_RESERVE   =13, //保留指令

}OVCEBOControlCmd;


/** EBO转动位置坐标信息 */
typedef struct
{
    OVD_int32 pan;        /**< 水平方向坐标，整型，范围是[-100,100],只有步进指令或持续移动指令带有这个字段，左为负，右为正 */
    OVD_int32 tilt;       /**< 垂直方向坐标，整型，范围是[-100,100],只有步进指令或持续移动指令带有这个字段，下为负，上为正 */
    OVD_int32 seq;       //指令顺序
} ovd_ebo_space_t;

typedef struct
{
    OVD_char *buf;    			//数据buf
    OVD_int32  size;    			//数据长度
}ovd_common_info_t;


typedef struct
{
    OVCEBOControlCmd  ebo_cmd;
    union
    {
        ovd_ebo_space_t  ebo_space;
        ovd_common_info_t ebo_data;
    };
}ovd_ebo_content_t;

typedef struct
{
    OVD_char type[MAX_LEN_64];
    OVD_char action[MAX_LEN_64];
    OVD_char *data;
}ovd_robot_content_t;

typedef enum
{
    OVC_ROBOT_OWL      = 0,   //猫头鹰
    OVC_ROBOT_TGP      = 1,
}ovd_robot_cmd;

typedef struct{
    OVD_float longitude; //经度信息
    OVD_float latitude; //纬度信息
}ovd_gps_info_t;

//p2p 对讲
typedef enum
{
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
    OVD_OUTTER  =   2,       //外部告警
    OVD_MOTIOM	= 	3,       //移动侦测
    OVD_CROSS	= 	4,       //拌网侦测
    OVD_CRY		=	5,       //哭声侦测
    OVD_FACE	=	6,       //脸部识别
    OVD_VOICE	=	7,       //声音侦测
    OVD_LOW_BATTERY	=	8,   //低电告警
    OVD_LOSS_LOCK	=	9,   //撬锁告警
    OVD_BELL	=	10,      //按铃事件
    OVD_MAN     =    11,     //人形事件
    OVD_PIR     =    12,     //pir移动侦测
    OVD_ALERTAREA =   13,    //警戒功能告警
    OVD_KITCHEN_MASK = 14,   //厨房行为检测，检测到口罩异常
    OVD_KITCHEN_CAP = 15,    //厨房行为检测，检测到帽子异常
    OVD_KITCHEN_CLOTHES = 16,//厨房行为检测，检测到工服异常
    OVD_VEHICLE_GEN = 17,    //车型侦测，通用车辆侦测
    OVD_VEHICLE_MOTOR = 18,  //车型侦测，检测到电瓶车
    OVD_VEHICLE_BIKE = 19,   //车型侦测，检测到自行车
    OVD_FACE_MASK = 20,      //口罩检测，检测到人脸未戴口罩
    OVD_REGIONAL_PEOPLE_STAT  = 21,      //区域人数统计
    OVD_PARABOLIC_AERIAL      = 22,      //高空抛物
    OVD_BABY_CRYING           = 23,      //婴儿啼哭，哭声侦测因安防平台原因，合并到婴儿啼哭
    OVD_OFF_DUTY              = 24,      //离岗检测
    OVD_FEEDER                = 25,      //喂食检测
    OVD_PET                   = 26,      //宠物出现
    OVD_PARKING_NOTICE        = 27,      //行车记录仪：停车通知
    OVD_DRIVING_COLLISION      = 28,      //行车记录仪：行车碰撞告警
    OVD_PARKING_COLLISION      = 29,      //行车记录仪：停车碰撞告警
    OVD_VOICE_CAPTURE         = 30,      //行车记录仪：语音抓拍
    OVD_TRANSGRESSION         = 31,      //越界侦测
    OVD_AUTOCHECK             = 32,      //四合一烟雾传感器：设备自检
    OVD_ANTI_DISMANTLING      = 33,      //四合一烟雾传感器：防拆告警
    OVD_SMOKE                 = 34,      //四合一烟雾传感器：烟雾告警
    OVD_TEMPERATURE           = 35,      //四合一烟雾传感器：温度告警
    OVD_HUMIDITY              = 36,      //四合一烟雾传感器：湿度告警
    OVD_FAULT                 = 37,      //四合一烟雾传感器：故障告警
    OVD_OFFLINE               = 38,      //四合一烟雾传感器：烟雾模块失联告警
    OVD_FIRE                  = 39,      //四合一烟雾传感器：火焰识别告警

    OVD_OTHER,
}OVDAlarmType;

typedef struct
{
    OVD_int32             channel; //通道号
    OVD_int64    startTimeStamp;   //报警开始时间戳 (单位为秒)
    OVDAlarmType    AlarmType;     //报警类型
    OVD_char*           desc;      //告警描述
    OVDImageInfo    ImageInfo;     //背景图信息
    OVD_char *extension;         //gps位置信息,json格式：{“gps”:{“longitude”: 经度坐标信息 “latitude”:纬度坐标信息}}
}OVDUpLoadAlarmInfo;

typedef enum
{
    ROBOT_ALARM_EXAMPLE = 0,
    ROBOT_ALARM_MAX,
}RobotAlarmType;

typedef struct
{
    OVD_int64           timeStamp;          //告警时间戳 (单位为秒)
    RobotAlarmType      AlarmType;          //报警类型
    OVD_char*           desc;               //告警描述
}OVDRobotAlarmInfo;

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

       OVD_uint64 StartStamp;       //录像开始时间（单位为秒）
       OVD_uint64 EndStamp;         //录像接收时间（单位为秒）

   }OVD_DMERecordFileInfo;

   typedef struct
   {
       OVD_int32               Count;            //卡录像片段个数
       OVD_DMERecordFileInfo sectioninfo[64];    //卡录像一个连续区间的信息
   }OVD_DMERecordFileListPerPage;

/*
远程配置参考doc/config.txt
*/

//ovd 状态
enum ovd_state
{
    OVD_ONLINE = 1, //设备在线
    OVD_UPDATING=2,  //设备升级
    OVD_ABNORMAL=3, //异常
    OVD_LOWPOWER_OFFLINESLEEP=4  //不支持远程唤醒的低功耗设备，在进入休眠时需要上报该状态，app借此展示为休眠状态而非离线状态。
};

enum ovd_power_mode
{
    OVD_POE_POWER = 1,      //POE供电
    OVD_5V_POWER  = 2,      //5V供电
    OVD_BATTERY_POWER = 3,  //电池供电
};


//channels 状态
enum ovd_channel_state
{
    OVD_CHANNEL_OFFLINE=0, //离线
    OVD_CHANNEL_ONLINE=1, //在线
    OVD_CHANNEL_ABNORMAL=3, //异常
    OVD_CHANNEL_DISABLE=4,   //不使能（休眠）
    OVD_CHANNEL_UNUSE=5,  // 通道未启用
};


typedef struct
{
    OVD_int32 channelnum;
    enum ovd_channel_state state;
} Simplechannelinfo;

typedef struct
{
    enum ovd_state state;  //1：设备在线        2：设备升级        3：异常
    Simplechannelinfo channelarray[MAX_CHANNEL_COUNT];  //用静态数组，最多支持64个channel,不用malloc
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
}OVD_VideoFrameInfo;

typedef struct
{
    OVD_uint32 samplesRate;    //采样率(必填)
    OVD_ushort channelNumber;  //声道数
    OVD_ushort bitsPerSample;  //位深
    OVD_uint32 samplePerFrame; //每帧采样点数
    OVD_uchar rev[20];         //预留
}OVD_AudioFrameInfo;


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
}OVD_FrameInfo;


typedef enum {
    OVD_OTA_CMD_SET_UPGRADE_MODEL   = 0,  //目标版本号保存
    OVD_OTA_CMD_SET_UPGRADE_TIME    = 1,  //升级时间保存
    OVD_OTA_CMD_SET_UPGRADE_STATE   = 2,  //升级状态保存
    OVD_OTA_CMD_GET_UPGRADE_MODEL   = 3,  //目标版本号获取
    OVD_OTA_CMD_GET_UPGRADE_TIME    = 4,  //升级时间获取
    OVD_OTA_CMD_GET_UPGRADE_STATE   = 5,  //升级状态获取
}ovd_content_e;


/** 存储设备状态 */
typedef enum {
    OVD_LOCAL_STORAGE_STATUS_READY,             /**< 设备外部存储(如TFCard)插入，且状态正常 */
    OVD_LOCAL_STORAGE_STATUS_NOT_READY,                 /**< 设备无外部存储   */
    OVD_LOCAL_STORAGE_STATUS_ABNORMAL,                  /**< 设备外部存储异常 */
    OVD_LOCAL_STORAGE_FILE_SYSTEM_NOT_SUPPORT,          /**< 不支持的文件系统 */
} ovd_local_storage_status_e;

/** 存储设备类型 */
typedef enum {
    OVD_LOCAL_STORAGE_TYPE_TFCARD,
} ovd_local_storage_type_e;

/** 存储设备状态和路径信息 */
typedef struct {
    ovd_local_storage_type_e   storage_type;                    /**< 存储类型 */
    ovd_local_storage_status_e storage_status;                  /**< 存储设备状态 */
    OVD_char storage_path[OVD_MAX_LOCAL_STORAGE_PATH_LEN];    /**< 存储设备路径，storage_type=OVD_LOCAL_STORAGE_STORAGE_STATUS_READY 时有效 */
} ovd_local_storage_info_t;


/** 设置云存套餐信息 */
typedef enum
{
    ///无云存套餐
    OVD_CMD_SET_CLOUD_NONE,
    ///云存全天套餐
    OVD_CMD_SET_CLOUD_ALL_DAY,
    //AIGC事件智能套餐
    OVD_CMD_SET_CLOUD_AIGC_EVENT,
    ///云存事件套餐
    OVD_CMD_SET_CLOUD_EVENT,
} OVD_SetCloudInfo_e;




/*******************  软探针相关  **********************/
/* ping和traceroute结构体 */
typedef struct
{
    OVD_char   url[256];  //下发ping和traceroute地址
    OVD_int32     delay_time; //ping和traceroute延时
    OVD_int32     result_data; // ping成功率/traceroute跳数
} ovd_probe_data_t;

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
    /// 获取芯片厂家。结构体ovd_common_info_t。
    OVD_CMD_GET_CHIP_SUPPLIER,
    /// 获取芯片型号。结构体ovd_common_info_t。
    OVD_CMD_GET_CHIP_MODEL,
    /// 获取和家智话SDK版本号。结构体ovd_common_info_t。
    OVD_CMD_GET_HJZH_SDKVER,
    /// 获取Andlink SDK版本号。结构体ovd_common_info_t。
    OVD_CMD_GET_ANDLINK_SDKVER,
    /// 下发ping地址，返回ping 5次的平均延时ms和成功率（丢包率）（要求3s内返回回调结果）。结构体ovd_probe_data_t。
    OVD_CMD_GET_PING,
    /// 下发Traceroute地址，返回traceroute跳到目的地跳了多少次，跳到目的地用了多少时间（ms）（要求10min内返回回调结果）。结构体ovd_probe_data_t
    OVD_CMD_GET_TRACEROUTE,
    /// 获取设备4G信号强度
    OVD_CMD_GET_RSSI_4G,
    /// 获取设备供电方式。int型，具体类型参考枚举ovd_power_mode
    OVD_CMD_GET_POWER_MODE
} ovd_probe_devrunning_info_e;

//异常场景
typedef enum
{
    /// 服务器连接状态
    /// 设备和服务器建立TCP连接出错  \n 回调指针类型：NULL
    OVD_RUNNING_STATUS_TCP_ERROR,
    /// 设备分配服务器出错  \n 回调指针类型：NULL
    OVD_RUNNING_STATUS_ASSIGN_ERROR,
    /// 设备分配的服务器为空  \n 回调指针类型：NULL
    OVD_RUNNING_STATUS_ASSIGN_NULL,
    /// 设备连接服务器时域名解析失败  \n 回调指针类型：NULL
    OVD_RUNNING_STATUS_HOST_RESOLVE_ERROR,
    /// 设备连接服务器时证书校验失败  \n 回调指针类型：NULL
    OVD_RUNNING_STATUS_CA_ERROR,
    /// 设备和服务器建立加密连接出错  \n 回调指针类型：NULL
    OVD_RUNNING_STATUS_SSL_ERROR,
    /// 设备和服务器的心跳出错  \n 回调指针类型：NULL
    OVD_RUNNING_STATUS_HEARTBEAT_ERROR,
    /// 设备从服务器接收数据超时  \n 回调指针类型：NULL
    OVD_RUNNING_STATUS_READ_TIMEOUT,
    /// 设备从服务器接收数据出错  \n 回调指针类型：NULL
    OVD_RUNNING_STATUS_READ_ERROR,
    /// 设备向服务器发送数据超时  \n 回调指针类型：NULL
    OVD_RUNNING_STATUS_WRITE_TIMEOUT,
    /// 设备向服务器发送数据出错  \n 回调指针类型：NULL
    OVD_RUNNING_STATUS_WRITE_ERROR,
    /// 设备系统时间出错  \n 回调指针类型：NULL
    OVD_RUNNING_STATUS_SYS_TIME_ERROR,
    /// 设备向服务器校验失败  \n 回调指针类型：NULL
    OVD_RUNNING_STATUS_AUTH_ERROR,
    /// 码流状态
    /// 检测到没有视频流  \n 回调指针类型：NULL
    OVD_RUNNING_STATUS_NO_VIDEO,
    /// 检测到没有音频流  \n 回调指针类型：NULL
    OVD_RUNNING_STATUS_NO_AUDIO,
    /// 检测到帧大小超过限制  \n 回调指针类型：NULL
    OVD_RUNNING_STATUS_LARGE_FRAME,
    /// 检测到音视频存在不同步  \n 回调指针类型：NULL
    OVD_RUNNING_STATUS_VIDEO_AUDIO_UNSYNC,
    /// 流数据缓存目录路径IO出错  \n 回调指针类型：NULL
    OVD_RUNNING_STATUS_STREAM_DATA_PATH_ERROR,
    /// 视频模糊  \n 回调指针类型：NULL
    OVD_RUNNING_STATUS_VIDEO_BLUR,
    /// SD卡异常，停止录制  \n  回调指针类型： NULL
    OVD_RUNNING_STATUS_SDCARD_ABNORMAL,
    /// 镜头遮挡  \n  回调指针类型： NULL
    OVD_RUNNING_STATUS_CAMERA_BLOCK,
     /// senser异常  \n  回调指针类型： NULL
    OVD_RUNNING_STATUS_SENSER_ABNORMAL,
    //// 第一个I帧时间 \n 回调指针类型：NULL
    OVD_RUNNING_STATUS_IFRAME_TIME,
    /// 供电模式转变 \n 回调指针类型：NULL
    OVD_RUNNING_STATUS_PWOERMODE_CHANGE,

    /// 直播时延汇报
    OVD_LIVEPUBLISH_REPORT,
    ///直播拉流失败汇报
    OVD_LIVEPUBLISH_FAIL_REPORT,
    ///配网成功相关指标汇报
    OVD_BIND_REPORT,
    ///配网失败
    OVD_BIND_FAIL_REPORT,
    ///告警失败
    OVD_ALARM_FAIL_REPORT,
    ///告警指标
    OVD_ALARM_REPORT,
    ///重启信息上报
    OVD_REBOOT_REPORT,
} ovd_probe_exception_type_e;


typedef enum
{
    OVD_PROBE_SYNC_MFR  =   1,   //同步接口，回调给设备
    OVD_PROBE_ASYNC_MFR    =   2,  //异步接口，回调给设备
    OVD_PROBE_SYNC_NOMFR  =   3,   //同步接口，不回调给设备
    OVD_PROBE_ASYNC_NOMFR    =   4,  //异步接口，不回调给设备
} ovd_probe_sync_type_e;

typedef struct {
    ovd_probe_exception_type_e  exception_type; //异常事件详细类型
    OVD_int64 createTime;    //产生异常的时间,单位s，有需要就传值，不需要传0，软探针内部获取时间
    OVD_char content[1024]; //详细信息,比如关键打印
    OVD_char module[64]; //调用模块,比如关键打印
    ovd_probe_sync_type_e synctype; //接口同异步选择
} ovd_probe_except_info_t;

/**
 * @brief 休眠服务器信息结构体
 */
typedef struct{
    OVD_char hdomain[256];  //休眠服务器域名
    OVD_int32 hport;        //休眠服务器端口
    OVD_int32 hb_interval;  //休眠心跳间隔
    OVD_char  hb_token[17]; //休眠心跳口令
}ovd_hserverinfo_t;







#endif
