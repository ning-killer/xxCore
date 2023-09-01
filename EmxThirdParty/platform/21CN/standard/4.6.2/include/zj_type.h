#ifndef _ZJ_TYPE_H_
#define _ZJ_TYPE_H_
#include <time.h>
#ifdef __cplusplus
extern "C" {
#endif

#ifdef ZJ_EXPORTS
#define _ZJ_API  __declspec(dllexport)
#else
#define _ZJ_API
#endif

#define ZJ_HANDLE   void*

#define IOT_POLICYNAME_MOTION       (char*)"MOTION"              // 运动侦测报警策略名
#define IOT_POLICYNAME_VOICE        (char*)"VOICEALARM"          // 声音检测报警策略名
#define IOT_POLICYNAME_FALL         (char*)"FALLALARM"           // 跌倒检测报警策略名
#define IOT_POLICYNAME_FLAME        (char*)"FLAMEALARM"          // 火焰检测报警策略名
#define IOT_POLICYNAME_SMOKEY       (char*)"SMOKEYALARM"         // 烟雾检测报警策略名
#define IOT_POLICYNAME_CRY          (char*)"CRYALARM"            // 哭声检测报警策略名
#define IOT_POLICYNAME_HUMANCOUNT   (char*)"HUMANCOUNTALARM"     // 客流预警策略名
#define IOT_POLICYNAME_PET          (char*)"PETALARM"            // 宠物检测报警策略名
#define IOT_POLICYNAME_BATTERYBIKE  (char*)"BATTERYBIKEALARM"    // 电瓶车检测报警策略名
#define IOT_POLICYNAME_HIGHPARABOLIC (char*)"HIGHPARABOLICALARM" // 高空抛物检测报警策略名
#define IOT_POLICYNAME_MASK         (char*)"MASKALARM"           // 口罩检测报警策略名
#define IOT_POLICYNAME_FLAMEDETECT  (char*)"FLAMEDETECT"         // 烟火检测报警策略名

typedef enum enum_zj_os_type
{
    EN_ZJ_OS_TYPE_LINUX     = 1,
    EN_ZJ_OS_TYPE_IOS       = 2,
    EN_ZJ_OS_TYPE_ANDRIOD   = 3,
    EN_ZJ_OS_TYPE_ANDRIODTV = 4,
    EN_ZJ_OS_TYPE_WINDOWS   = 5,
    EN_ZJ_OS_TYPE_RTOS      = 6
}EN_ZJ_OS_TYPE;

/**
  * 设备初始化状态。分别为：初始化、配置加载、网络初始化、注册、登录、绑定、运行。
**********************************************/
typedef enum enum_ZJ_SERVER_STATUS
{
    EN_ZJ_SERVER_STATUS_INIT     = 0,
    EN_ZJ_SERVER_STATUS_GETADDR,
    EN_ZJ_SERVER_STATUS_CONNECT,        //网络连接完成
    EN_ZJ_SERVER_STATUS_REGIST,         //设备注册
    EN_ZJ_SERVER_STATUS_WAITBIND,       //设备等待绑定
    EN_ZJ_SERVER_STATUS_PROCESS,        //设备完成上线
    EN_ZJ_SERVER_STATUS_EXITGROUP       //设备解绑退组
}EN_ZJ_SERVER_STATUS;

/*
 * 设备内存富余级别
 */
typedef enum enum_ZJ_DEVICE_MEM_ABILITY
{
   EN_ZJ_DEVICE_MEM_ABILITY_RICH = 0, //内存余量可以存储3个以上完整GOP
   EN_ZJ_DEVICE_MEM_ABILITY_MID  = 1, //内存余量适中，可以存储1个以上完整GOP
   EN_ZJ_DEVICE_MEM_ABILITY_POOR = 2  //内存余量不足，只可以存在一个完整帧
}EN_ZJ_DEVICE_MEM_ABILITY;

typedef enum enum_ZJ_CFG_ITEM
{
    EN_ZJ_CFG_ITEM_ALL            = 0X00,
    EN_ZJ_CFG_ITEM_DEVICE         = 0X01,
    EN_ZJ_CFG_ITEM_CAMERA         = 0X02,
    EN_ZJ_CFG_ITEM_PTZ            = 0X04,
    EN_ZJ_CFG_ITEM_INIOT          = 0X08,
    EN_ZJ_CFG_ITEM_IOTHUB         = 0X10,
    EN_ZJ_CFG_ITEM_TIMEPOLICY     = 0X20,
    EN_ZJ_CFG_ITEM_ALARMPOLICY    = 0X40,
    EN_ZJ_CFG_ITEM_SYSTEM         = 0X80,
    EN_ZJ_CFG_ITEM_SERVERSET      = 0X100,
    EN_ZJ_CFG_ITEM_SCENEPOLICY    = 0X200,
    EN_ZJ_CFG_ITEM_SCENEMODE      = 0X400,
    EN_ZJ_CFG_ITEM_AI             = 0X800,

    EN_ZJ_CFG_ITEM_GROUPINF       = 0x10000
}EN_ZJ_CFG_ITEM;

/*
 * 设备升级状态。
 */
typedef enum enum_ZJ_DEVICE_UPDATE_STATUS
{
    EN_ZJ_DEVICE_UPDATE_STATUS_NOUPDATE     = 0, //不需要升级
    EN_ZJ_DEVICE_UPDATE_STATUS_NEEDUPDATE   = 1, //有新版本需要升级
    EN_ZJ_DEVICE_UPDATE_STATUS_MUSTUPDATE   = 2  //强制升级，系统版本不兼容需要强制升级
}EN_ZJ_DEVICE_UPDATE_STATUS;

// 网络类型
typedef enum enum_ZJ_NETWORK_TYPE{
    EN_ZJ_NETWORK_TYPE_NONET                = 0x00,     // 无网络
    EN_ZJ_NETWORK_TYPE_WIRED                = 0x01,     // 有线连接
    EN_ZJ_NETWORK_TYPE_WIFI                 = 0x02,     // WIFI连接
    EN_ZJ_NETWORK_TYPE_AP                   = 0x04,     // AP热点
    EN_ZJ_NETWORK_TYPE_SIM                  = 0x08,     // 移动网络
    EN_ZJ_NETWORK_TYPE_NONSENSE             = 0x10,     // 无感陪网
}EN_ZJ_NETWORK_TYPE;

// 定义WIFI列表元素
typedef struct stru_ZJ_WIFI_INFO{
    int     iSigStrength;       //信号强度
    int     iConnectFlag;       //是否当前连接 1.是；0.不是
    char    acWifiSSID[32];     //最长31字节WiFiSSID
    int     iSecurity;          //安全性，0：无，1.WEP，2.WPA-PSK，3.WPA-PSK/WPA2-PSK，4.WPA2-PSK，5.其他
}ST_ZJ_WIFI_INFO;

// 定义当前网络详情结构
typedef struct stru_ZJ_NETWORK_INFO{
    int             iNetType;               // 网络类型 参考:EN_ZJ_NETWORK_TYPE
    int             iSignalType;            // 信号类型 2: 2G 3: 3G 4: 4G 
    int             iSigStrength;           // 信号强度
    unsigned char   aucNetmask[64];         // 子网掩码
    unsigned char   aucGateway[64];         // 默认网关
    unsigned char   aucIPAddr[64];          // 设备IP地址
    unsigned char   aucMacAddr[64];         // 设备MAC地址
    unsigned char   aucUpMacAddr[64];       // 上级路由MAC地址
    unsigned char   aucWIFISSID[32];        // WIFI名称
}ST_ZJ_NETWORK_INFO;

// WIFI连接状态
typedef enum enum_ZJ_NETWORK_ERR_STATUS{
    EN_ZJ_NETWORK_ERR_STATUS_PWDERR         = 0x01, //WIFI密码错误
    EN_ZJ_NETWORK_ERR_STATUS_NOSIGNAL       = 0x02  //信号中断
}EN_ZJ_NETWORK_ERR_STATUS;


// 支持OTA升级的模式
typedef enum enum_ZJ_OTA_ABILITY{
    EN_ZJ_OTA_ABILITY_SDCARD                = 0x01,     // SD卡升级，用户将升级包拷贝到SD卡，然后将SD卡插入设备完成升级；
    EN_ZJ_OTA_ABILITY_LOCALUPDATE           = 0x02,     // 本地升级，用户与设备连接在同一个局域网，完成升级；
    EN_ZJ_OTA_ABILITY_REMOTEUPDATE          = 0x04      // 远程升级，用户与设备不在同一个局域网，完成升级；设备需要更多的空间存放升级包；
}EN_ZJ_OTA_ABILITY;

// 支持WIFI配置模式
typedef enum enum_ZJ_SETWIFI_ABILITY{
    EN_ZJ_SETWIFI_ABILITY_AP                   = 0x01,  //支持AP配网
    EN_ZJ_SETWIFI_ABILITY_SOUND                = 0x02,  //支持声波配网
    EN_ZJ_SETWIFI_ABILITY_SMARTLINK            = 0x04,  //支持SMARTLINK配网
    EN_ZJ_SETWIFI_ABILITY_QRCODE               = 0x08,  //支持二维码配网
    EN_ZJ_SETWIFI_ABILITY_WIRED                = 0x10   //支持有线配网
}EN_ZJ_SETWIFI_ABILITY;

// 设备休眠唤醒能力
typedef enum enum_ZJ_AWAKE_ABILITY{
    EN_ZJ_AWAKE_ABILITY_NOTSUPPORT          = 0x00, //不支持休眠
    EN_ZJ_AWAKE_ABILITY_LOCALAWAKE          = 0x01, //支持本地唤醒
    EN_ZJ_AWAKE_ABILITY_REMOTEAWAKE         = 0x02  //支持远程唤醒
}EN_ZJ_AWAKE_ABILITY;

// 声音对讲支持能力
typedef enum enum_ZJ_VOICEPLAY_ABILITY{
    EN_ZJ_VOICEPLAY_ABILITY_NOSUPPORT   = 0x00, // 不支持对讲
    EN_ZJ_VOICEPLAY_ABILITY_SIMPLEX     = 0x01, // 单工
    EN_ZJ_VOICEPLAY_ABILITY_DUPLEX      = 0x02  // 双工
}EN_ZJ_VOICEPLAY_ABILITY;

// 摄像机PTZ支持能力
typedef enum enum_ZJ_CAMERA_PTZ_ABILITY{
    EN_ZJ_CAMERA_PTZ_ABILITY_P          = 0x01,  //支持P操作
    EN_ZJ_CAMERA_PTZ_ABILITY_T          = 0x02,  //支持T操作
    EN_ZJ_CAMERA_PTZ_ABILITY_Z          = 0x04,  //支持Z操作
    EN_ZJ_CAMERA_PTZ_ABILITY_AUTOCHECK  = 0x08   //支持云台自检
}EN_ZJ_CAMERA_PTZ_ABILITY;

// 摄像机PTZ转动
typedef enum enum_ZJ_CAMERA_PTZ_CONTROL{
    EN_ZJ_CAMERA_PTZ_CONTROL_STOP       = 0,
    EN_ZJ_CAMERA_PTZ_CONTROL_UP         = 1,
    EN_ZJ_CAMERA_PTZ_CONTROL_DOWN       = 2,
    EN_ZJ_CAMERA_PTZ_CONTROL_LEFT       = 3,
    EN_ZJ_CAMERA_PTZ_CONTROL_RIGHT      = 4,
}EN_ZJ_CAMERA_PTZ_CONTROL;

typedef struct str_ZJ_PRESET_POINT  //PTZ ABS MOVE
{
    int    iX;                      //预置位x轴坐标    （底层）
    int    iY;                      //预置位y轴坐标    （底层）
}ST_ZJ_PRESET_POINT;

// 巡航预置点定义
typedef struct stru_ZJ_CAMERA_CRUISE_PRESET{
    int         Idx;            //预置点序号
    int         iX;             //预置点位移X
    int         iY;             //预置点位移Y
    int         iDwellTime;     //停留时间 单位 s
    int         iSpeed;         //云台转速
}ST_ZJ_CAMERA_CRUISE_PRESET;

// 摄像机分辨率支持能力集
typedef enum enum_ZJ_CAMERA_RESOLUTION_ABILITY{
    EN_ZJ_CARERA_RESOLUTION_ABILITY_360P        = 0x0001, //支持640*360分辨率
    EN_ZJ_CARERA_RESOLUTION_ABILITY_480P        = 0x0002, //支持640*480分辨率
    EN_ZJ_CARERA_RESOLUTION_ABILITY_720P        = 0x0004, //支持1280*720分辨率
    EN_ZJ_CARERA_RESOLUTION_ABILITY_960P        = 0x0008, //支持1280*960分辨率
    EN_ZJ_CARERA_RESOLUTION_ABILITY_1080P       = 0x0010, //支持1920*1080分辨率
    EN_ZJ_CARERA_RESOLUTION_ABILITY_1200P       = 0x0020, //支持1600*1200分辨率
    EN_ZJ_CARERA_RESOLUTION_ABILITY_300W        = 0x0040, //支持2048*1536分辨率（支持高清套餐）
    EN_ZJ_CARERA_RESOLUTION_ABILITY_400W        = 0x0080, //支持2500*1600分辨率（支持高清套餐）
    EN_ZJ_CARERA_RESOLUTION_ABILITY_500W        = 0x0100, //支持2560*1920分辨率（支持高清套餐）
    EN_ZJ_CARERA_RESOLUTION_ABILITY_4K          = 0x0200, //支持3840*2160分辨率 800W像素（支持高清套餐）
    EN_ZJ_CARERA_RESOLUTION_ABILITY_8K          = 0x0400, //支持7680*4320分辨率 3200W像素
    EN_ZJ_CAMERA_RESOLUTION_ABILITY_1440P       = 0x0800, //支持2560*1440分辨率（支持高清套餐）
    EN_ZJ_CAMERA_RESOLUTION_ABILITY_600W        = 0x1000, //支持3072*2048分辨率（支持高清套餐）
}EN_ZJ_CARERA_RESOLUTION_ABILITY;

// 摄像机编码码率定义
typedef enum enum_ZJ_CAMERA_BITRATE_TYPE{
    EN_ZJ_CAMERA_BITRATE_TYPE_96K       = 1,
    EN_ZJ_CAMERA_BITRATE_TYPE_128K      = 2,
    EN_ZJ_CAMERA_BITRATE_TYPE_196K      = 3,
    EN_ZJ_CAMERA_BITRATE_TYPE_256K      = 4,
    EN_ZJ_CAMERA_BITRATE_TYPE_384K      = 5,
    EN_ZJ_CAMERA_BITRATE_TYPE_512K      = 6,
    EN_ZJ_CAMERA_BITRATE_TYPE_768K      = 7,
    EN_ZJ_CAMERA_BITRATE_TYPE_1024K     = 8,
    EN_ZJ_CAMERA_BITRATE_TYPE_1536K     = 9,
    EN_ZJ_CAMERA_BITRATE_TYPE_2048K     = 10,
    EN_ZJ_CAMERA_BITRATE_TYPE_2560K     = 11,
    EN_ZJ_CAMERA_BITRATE_TYPE_3072K     = 12,
    EN_ZJ_CAMERA_BITRATE_TYPE_4096K     = 13
}EN_ZJ_CAMERA_BITRATE_TYPE;

// 红外灯支持能力
typedef enum enum_ZJ_CAMERA_IRLED_ABILITY{
    EN_ZJ_CAMERA_IRLED_ABILITY_NOSUPPORT        = 0x00, //不支持红外灯
    EN_ZJ_CAMERA_IRLED_ABILITY_NOCONTROL        = 0x01, //不支持红外灯开关控制，只能通过感光器自动开关控制
    EN_ZJ_CAMERA_IRCUR_ABILITY_CANCONTROL       = 0x02, //支持红外灯命令开关控制
}EN_ZJ_CAMERA_IRLED_ABILITY;

// 摄像机视频编码类型
typedef enum enum_ZJ_VIDEOENC_TYPE{
    EN_ZJ_VIDEOENC_TYPE_JPEG        = 0x01, //JPEG图片编码模式
    EN_ZJ_VIDEOENC_TYPE_H264        = 0x02, //H264编码类型
    EN_ZJ_VIDEOENC_TYPE_H265        = 0x04  //H265编码类型
}EN_ZJ_VIDEOENC_TYPE;

// 音频编码类型
typedef enum enum_ZJ_AUDIOENC_TYPE{
    EN_ZJ_AUDIOENC_TYPE_PCM16       = 0x01,
    EN_ZJ_AUDIOENC_TYPE_G711A       = 0x02,
    EN_ZJ_AUDIOENC_TYPE_G711U       = 0x04,
    EN_ZJ_AUDIOENC_TYPE_AAC         = 0x08,
    EN_ZJ_AUDIOENC_TYPE_MP3         = 0x10
}EN_ZJ_AUDIOENC_TYPE;

typedef enum enum_ZJ_MEDIA_TYPE{
    EN_ZJ_MEDIA_VIDEO   = 0X01,
    EN_ZJ_MEDIA_AUDIO   = 0X02,
    EN_ZJ_MEDIA_AV      = 0X03
}EN_ZJ_STREAMER_TYPE;


// 镜头类型
typedef enum enum_ZJ_CAMERA_LENS_TYPE{
    EN_ZJ_CAMERA_LENS_TYPE_NORMAL       = 0x00,
    EN_ZJ_CAMERA_LENS_TYPE_360          = 0x01,
    EN_ZJ_CAMERA_LENS_TYPE_720          = 0x02
}EN_ZJ_CAMERA_LENS_TYPE;

// 视频编码参数
typedef struct stru_ZJ_VIDEO_PARAM{
    unsigned int uiResolution;    //分辨率，见分辨率定义 EN_ZJ_CARERA_RESOLUTION_ABILITY
    unsigned int uiWidth;         //分辨率  回调设置可以忽略
    unsigned int uiHeight;        //分辨率  回调设置可以忽略
    unsigned int uiEncodeType;    //编码格式    EN_ZJ_VIDEOENC_TYPE
    unsigned int uiSmartEncFlag;  //0.不支持Smart编码；1.支持Smart编码
    unsigned int uiQuality;       //编码质量
    unsigned int uiBitrate;       //码率，见码率定义 EN_ZJ_CAMERA_BITRATE_TYPE
    unsigned int uiFramerate;     //帧率 
    unsigned int uiFrameInterval; //I帧间隔 50帧、100帧、200帧
    unsigned int uiRateType;      // 0 CBR 1 VBR 
}ST_ZJ_VIDEO_PARAM;

// 音频参数描述
typedef struct stru_ZJ_AUDIO_PARAM{
    unsigned int uiEncodeType;    //编码格式    EN_ZJ_AUDIOENC_TYPE
    unsigned int uiSampleRate;    //采样率
    unsigned int uiChannel;       //声道数
    unsigned int uiDepth;         //深度
}ST_ZJ_AUDIO_PARAM;

// 广角镜头圆心半径信息描述
typedef struct str_ZJ_VIDEO_CIRCLE
{
    unsigned int uiRadius;   // 半径
    double   doubleAngle;    // 角度
    unsigned int uiCc1x;     // 圆心坐标X
    unsigned int uiCc1Y;     // 圆心坐标Y
    unsigned int uiCc2x;     // 720度双目
    unsigned int uiCc2Y;     // 720度双目
}ST_ZJ_VIDEO_CIRCLE;

// 扭曲度描述 
typedef struct str_ZJ_VIDEO_DISTORTION
{
    double fx;    // 镜头光轴与图像圆心的水平偏移
    double fy;    // 镜头光轴与图像圆心的垂直偏移
    double a;     // 桶形畸变校正参数
    double b;     // 枕形畸变校正参数
    double scale; // 图像缩放因子
}ST_ZJ_VIDEO_DISTORTION;

//视频编码帧类型
typedef enum enum_ZJ_VIDEO_FRAME_TYPE{
    EN_ZJ_VIDEO_FRAME_TYPE_UNKNOW       = 0, //未知帧类型，需要去检测
    EN_ZJ_VIDEO_FRAME_TYPE_I            = 1, //I帧
    EN_ZJ_VIDEO_FRAME_TYPE_P            = 2, //P帧
}EN_ZJ_VIDEO_FRAME_TYPE;


// 图片类型定义。普通图片，中等大小图片，小图，ICON图片；
typedef enum enum_ZJ_PICTURE_TYPE{
    EN_ZJ_PICTURE_NORMAL                = 0X01,   //码流同等尺寸
    EN_ZJ_PICTURE_MIDDLE                = 0X02,   //码流缩放一倍尺寸
    EN_ZJ_PICTURE_SMALL                 = 0X04,   //320*240尺寸
    EN_ZJ_PICTURE_ICON                  = 0X08    //64*48尺寸
}EN_ZJ_PICTURE_TYPE;


//  IR模式。自动模式，红外模式，全彩模式。
typedef enum enum_ZJ_IRMODE{
    EN_ZJ_IRMODE_AUTO                   = 0,  //自动模式
    EN_ZJ_IRMODE_IR                     = 1,  //红外模式
    EN_ZJ_IRMODE_FULLCOLOR              = 2   //全彩模式
}EN_ZJ_IRMODE;

// 0 正常 、1 卡不可用、 2 无卡、 3格式化中 
typedef enum enum_ZJ_SDCARD_STATUS{
    EN_ZJ_SDCARD_NORMAL                 = 0,  // SDcard正常
    EN_ZJ_SDCARD_USELESS                = 1,  // SDcard不可用
    EN_ZJ_SDCARD_NOCARD                 = 2,  // 没有插入SDcard
    EN_ZJ_SDCARD_FORMATING              = 3   // SDcard格式化中
}EN_ZJ_SDCARD_STATUS;

// 关键帧编码质量参数
typedef enum enum_ZJ_KEYFRAME_QUALITY{
    EN_ZJ_KEYFRAME_QUALITY_NORMAL       = 1, // 标准质量
    EN_ZJ_KEYFRAME_QUALITY_LOW          = 2, // 低质量
}EN_ZJ_KEYFRAME_QUALITY;

//定义AIIoT类型，包括报警器、信号采集检测器、控制器；
typedef enum enum_ZJ_AIIOT_TYPE{
    EN_ZJ_AIIOT_TYPE_REMOTECTRLER               = 0,        //遥控器
    EN_ZJ_AIIOT_TYPE_DOORSWITCH                 = 1,        //门磁
    EN_ZJ_AIIOT_TYPE_SMOKETRANSDUCER            = 2,        //烟雾传感器
    EN_ZJ_AIIOT_TYPE_GASSENSOR                  = 3,        //燃气 探测 传感器
    EN_ZJ_AIIOT_TYPE_LIGHTSWITCH                = 4,        //电灯
    EN_ZJ_AIIOT_TYPE_CURTAIN                    = 5,        //窗帘
    EN_ZJ_AIIOT_TYPE_JACK                       = 6,        //插座
    EN_ZJ_AIIOT_TYPE_PIR                        = 7,        //人体 红外 
    EN_ZJ_AIIOT_TYPE_WATER                      = 8,        //水浸
    EN_ZJ_AIIOT_TYPE_ERG                        = 9,        //紧急
    EN_ZJ_AIIOT_TYPE_ALARMBEER                  = 10,       //警号
    
    EN_ZJ_AIIOT_TYPE_JCAKHVAC                   = 11,       //空调插座
    EN_ZJ_AIIOT_TYPE_MULTISWITCH                = 12,       //多路开关
    EN_ZJ_AIIOT_TYPE_SHUTTERMOTOR               = 13,       //卷帘 电动机
    EN_ZJ_AIIOT_TYPE_GLASSBROKENSENSOR          = 20,       //玻璃破碎 传感器
    EN_ZJ_AIIOT_TYPE_INFRAREDSENSOR             = 23,       //红外 对射 
    EN_ZJ_AIIOT_TYPE_BATTERYVALVE               = 24,       //电磁阀门
    EN_ZJ_AIIOT_TYPE_AIRFLOWSENSOR              = 25,       //气流传感器
    EN_ZJ_AIIOT_TYPE_MULTICTR                   = 32,       //多功能控制
    EN_ZJ_AIIOT_TYPE_INTELLOCK                  = 33,       //智能锁
    EN_ZJ_AIIOT_TYPE_DOORBELL                   = 34,       //门铃
    EN_ZJ_AIIOT_TYPE_COSENSOR                   = 40,       //一氧化碳探测器
    
    EN_ZJ_AIIOT_TYPE_MOTION                     = 1000,      // 运动检测
    EN_ZJ_AIIOT_TYPE_INNER_DOORBELL             = 1001,      // 内置 门铃
    EN_ZJ_AIIOT_TYPE_RECORD                     = 1002,      // 录像 不需要注册IOT
    EN_ZJ_AIIOT_TYPE_INNER_PIR                  = 1003,      // 内置人体红外探测器
    EN_ZJ_AIIOT_TYPE_VOICEALARMDETECT           = 1004,      // 声音报警检测
    EN_ZJ_AIIOT_TYPE_SNAPSHORT                  = 1005,      // SnapShot截图  不需要注册IOT
    EN_ZJ_AIIOT_TYPE_PTZ                        = 1006,      // PTZ 不需要用注册IOT
    EN_ZJ_AIIOT_TYPE_INNER_LAMP                 = 1007,      // 内置白光灯
    EN_ZJ_AIIOT_TYPE_INNER_STATELAMP            = 1008,      // 内置状态指示灯
    EN_ZJ_AIIOT_TYPE_BUZZER                     = 1009,      // 扬声器
    EN_ZJ_AIIOT_TYPE_CLOUDRECORD                = 1010,      // 云端录像
    EN_ZJ_AIIOT_TYPE_CLOUDSNAP                  = 1011,      // 云端截图
    EN_ZJ_AIIOT_TYPE_CAMERA                     = 1012,      // 摄像机
    EN_ZJ_AIIOT_TYPE_EVENT                      = 1013,      // 事件记录推送 或 看家事件推送能力
    EN_ZJ_AIIOT_TYPE_DNSET                      = 1014,      // 强制白天晚上,和自动
    EN_ZJ_AIIOT_TYPE_FORCEREMOVE                = 1015,      // 强拆
    EN_ZJ_AIIOT_TYPE_STAY                       = 1016,      // 逗留
    EN_ZJ_AIIOT_TYPE_COLLISION                  = 1017,      // 碰撞
    EN_ZJ_AIIOT_TYPE_CLOUDFACE                  = 1018,      // 云端人脸检测功能
    EN_ZJ_AIIOT_TYPE_TIMERCLOUD                 = 1019,      // 定时触发事件
    EN_ZJ_AIIOT_TYPE_LINKAGEALARM               = 1022,      // 平台设备联动报警
    EN_ZJ_AIIOT_TYPE_BROADCAST                  = 1023,      // 广播
    EN_ZJ_AIIOT_TYPE_FALLDOWN                   = 1024,      // 跌倒检测
    EN_ZJ_AIIOT_TYPE_FLAMEDETECT                = 1025,      // 烟火检测
    EN_ZJ_AIIOT_TYPE_SMOKEDETECT                = 1026,      // 烟雾检测
    EN_ZJ_AIIOT_TYPE_CRYALARM                   = 1027,      // 哭声侦测
    EN_ZJ_AIIOT_TYPE_HUMANCOUNT                 = 1028,      // 客流预警
    EN_ZJ_AIIOT_TYPE_PETDETET                   = 1029,      // 宠物检测报警
    EN_ZJ_AIIOT_TYPE_CLICKBUTTON                = 1030,      // IPC按键报警
    EN_ZJ_AIIOT_TYPE_BATTERYBIKE                = 1031,      // 电瓶车识别
    EN_ZJ_AIIOT_TYPE_HIGHPARABOLIC              = 1032,      // 高空抛物识别
    EN_ZJ_AIIOT_TYPE_MASK                       = 1033       // 口罩识别
}EN_ZJ_AIIOT_TYPE;

// IoT设备默认IoTID
typedef enum enum_zj_default_iot_id_type
{
    EN_ZJ_DEFAULT_IOTID   = 0,    // IoT设备默认IoTID:0
}EN_ZJ_DEFAULT_IOT_ID_TYPE;

// IoT设备默认事件ID
typedef enum enum_zj_default_iot_event_type
{
    EN_ZJ_DEFAULT_IOT_EVENTID   = 0,    // IoT设备默认事件ID:0
}EN_ZJ_DEFAULT_IOT_EVENT_TYPE;

// 区分MOTION中不同的检测类型
typedef enum enum_zj_motion_event_type
{
    EN_ZJ_MOTION_EVENT_MOTION                   = 0X00,         // 移动侦测事件类型
    EN_ZJ_MOTION_EVENT_HUMAN                    = 0X01,         // 人形侦测事件类型
    EN_ZJ_MOTION_EVENT_FACE                     = 0X02,         // 人脸侦测事件类型
    EN_ZJ_MOTION_EVENT_FENCE_MOTION_IN          = 0X03,         // 电子围栏移动侦测进入事件
    EN_ZJ_MOTION_EVENT_FENCE_MOTION_OUT         = 0X04,         // 电子围栏移动侦测离开事件
    EN_ZJ_MOTION_EVENT_FENCE_HUMAN_IN           = 0X05,         // 电子围栏移动人形进入事件
    EN_ZJ_MOTION_EVENT_FENCE_HUMAN_OUT          = 0X06,         // 电子围栏移动人形离开事件
    EN_ZJ_MOTION_EVENT_FENCE_FACE_IN            = 0X07,         // 电子围栏移动人脸进入事件
    EN_ZJ_MOTION_EVENT_FENCE_FACE_OUT           = 0X08,         // 电子围栏移动人脸离开事件
    EN_ZJ_MOTION_EVENT_CAR_DETECT               = 0X09,         // 车辆侦测报警事件
    EN_ZJ_MOTION_EVENT_CARNUM_DISCERN           = 0X10,         // 车牌识别报警事件
    EN_ZJ_MOTION_EVENT_MOTION_CROSSLINE         = 0X11,         // 越线移动侦测报警事件
    EN_ZJ_MOTION_EVENT_HUMAN_CROSSLINE          = 0X12,         // 越线人形侦测报警事件
    EN_ZJ_MOTION_EVENT_FACE_CROSSLINE           = 0X13,         // 越线人脸检测报警事件
    EN_ZJ_MOTION_EVENT_FENCE_MOTION_STAY        = 0X14,         // 逗留移动侦测报警事件
    EN_ZJ_MOTION_EVENT_FENCE_HUMAN_STAY         = 0X15,         // 逗留人形侦测报警事件
    EN_ZJ_MOTION_EVENT_FENCE_FACE_STAY          = 0X16,         // 逗留人脸检测报警事件
    EN_ZJ_MOTION_EVENT_FENCE_MOTOR_IN           = 0X17,         // 电子围栏机动车进入事件
    EN_ZJ_MOTION_EVENT_FENCE_MOTOR_OUT          = 0X18,         // 电子围栏机动车离开事件
    EN_ZJ_MOTION_EVENT_FENCE_MOTOR_STAY         = 0X19,         // 逗留机动车报警事件
    EN_ZJ_MOTION_EVENT_FENCE_NONMOTOR_IN        = 0X20,         // 电子围栏非机动车进入事件
    EN_ZJ_MOTION_EVENT_FENCE_NONMOTOR_OUT       = 0X21,         // 电子围栏非机动车离开事件
    EN_ZJ_MOTION_EVENT_FENCE_NONMOTOR_STAY      = 0X22,         // 逗留非机动车报警事件    
}EN_ZJ_MOTION_EVENT_TYPE;

// 0x01 、0x08表示原生方向
typedef enum enum_ZJ_ROTATE_TYPE {
    EN_ZJ_ROTATE_TYPE_VERTICAL_UP           = 0x01,  //画面朝上
    EN_ZJ_ROTATE_TYPE_VERTICAL_DOWN         = 0x02,  //画面朝下
    EN_ZJ_ROTATE_TYPE_MIRROR_ENABLE         = 0x04,  //镜像翻转
    EN_ZJ_ROTATE_TYPE_MIRROR_DISABLE        = 0x08,  //取消镜像翻转
}EN_ZJ_ROTATE_TYPE;

typedef enum enum_ZJ_CAMERA_EVENT_TYPE
{
    EN_ZJ_CAMERA_EVENT_LOWPOWER             = 1,  // 低电报警
    EN_ZJ_CAMERA_EVENT_POWER_OK             = 2,  // 低电恢复
    EN_ZJ_CAMERA_EVENT_PLUGINTFCARD         = 3,  // 插入TF卡
    EN_ZJ_CAMERA_EVENT_PLUGOUTTFCARD        = 4   // 拔出TF卡
}EN_ZJ_CAMERA_EVENT_TYPE;

typedef enum enum_ZJ_OSD_POSITION_TYPE
{
    EN_ZJ_OSD_POSITION_DEFAULT              = 0,  // 默认
    EN_ZJ_OSD_POSITION_LT                   = 1,  // 左上
    EN_ZJ_OSD_POSITION_LD                   = 2,  // 左下
    EN_ZJ_OSD_POSITION_RT                   = 3,  // 右上
    EN_ZJ_OSD_POSITION_RD                   = 4   // 右下
}EN_ZJ_OSD_POSITION_TYPE;

typedef enum enum_ZJ_CTRLDEVICEID_TYPE{

    EN_ZJ_CTRLDID_NODE                      = 0,
    EN_ZJ_CTRLDID_CLEAR                     = 1,
    EN_ZJ_CTRLDID_QUERY                     = 2,
    EN_ZJ_CTRLDID_EXITGROUP                 = 3
}EN_ZJ_CTRLDEVICEID_TYPE;

// 设置设备类型?1.摄像机；2.分体门铃；3.单体门铃；4.NVR
typedef enum enum_ZJ_DEV_TYPE
{
    EN_ZJ_DEV_TYPE_CAMERA                   = 0X01,     // 摄像机
    EN_ZJ_DEV_TYPE_DOORBELL_SPLIT           = 0X02,     // 分体门铃
    EN_ZJ_DEV_TYPE_DOORBELL_SINGLE          = 0X03,     // 单体门铃
    EN_ZJ_DEV_TYPE_NVR                      = 0X04,     // NVR
}EN_ZJ_DEV_TYPE;

//  铃声类型： 1 门铃提示音  2 门铃 4 警戒音,0X07  三种声音
typedef enum enum_ZJ_RING_TYPE{
    EN_ZJ_RING_PROMPT_TONE                  = 0X01,   // 门铃提示音
    EN_ZJ_RING_DOORBELL                     = 0X02,   // 门铃/自定义铃声
    EN_ZJ_RING_ALARM                        = 0X04,   // 警戒音
    // EN_ZJ_RING_MASK                      = 0X08,   // 口罩识别警告音
    // EN_ZJ_RING_BETTRYBIKE                = 0X10    // 电瓶车警戒音
}EN_ZJ_RING_TYPE;

// 扬声器类型,区分门铃、中继
typedef enum enum_ZJ_SPEAKER_TYPE{
    EN_ZJ_SPEAKER_DOORBELL                  = 0,  // 门铃
    EN_ZJ_SPEAKER_REPEATER                  = 1   // 中继器
}EN_ZJ_SPEAKER_TYPE;

typedef struct stru_ZJ_TRIGGER_INFO{
    unsigned int        uiIotType;
    unsigned long long  lluIotId;
    unsigned int        uiEventId;
    unsigned int        uiDuration;
    time_t              tCreateTime;
    void                *pstHandler;  // 保留
}ST_ZJ_TRIGGER_INFO;

typedef struct stru_ZJ_SOUNDFILE_INFO
{
    unsigned char aucFileName[64];
    unsigned int  uiFileSize;
    unsigned int  uiFileFormat;           //EN_ZJ_FILE_FORMAT
    unsigned char aucFileContent[64];     //中文描述
    unsigned int  uiFileCategory;         //EN_ZJ_RING_TYPE 
    struct stru_ZJ_SOUNDFILE_INFO *pstNextNode;
}ST_ZJ_SOUNDFILE_INFO;

// 声音文件格式
typedef enum enum_ZJ_FILE_FORMAT
{
    EN_ZJ_FILE_FORMAT_WAV               = 0X01,   //wav格式
}EN_ZJ_FILE_FORMAT; 

// 默认策略的结构体封装 (丢弃)
typedef struct str_ZJ_DEFAULT_POLICY
{
    unsigned int  uiIotType;             //添加的iot类型
    unsigned int  uiOpenFlag;            //iot开关，1 开 0 关闭
    unsigned int  uiEventId;             //事件id,0 1 2 3 ...
    unsigned int  uiTraceFlag;           //追踪开关 1 开 0 关闭，只适用于iot 1000
    unsigned int  uiSenstive;            //灵敏度设置，0-100，只适用于iot 1000
    unsigned int  uiBuzzerFlag;          //给添加的iot配置"扬声器"输出，1 配置 0 不配置
    unsigned int  uiRecordFlag;          //给添加的iot配置"录像"输出，1 配置 0 不配置
    unsigned int  uiSnapFlag;            //给添加的iot配置"抓图"输出，1 配置 0 不配置
    unsigned int  uiEventFlag;           //给添加的iot配置"事件上报"输出，1 配置 0 不配置
    unsigned int  uiLampFlag;            //给添加的iot配置"白光灯"输出，1 配置 0 不配置
    unsigned int  uiEventInterval;       //事件上报的间隔，单位s
    unsigned int  uiDuration;            //事件录像时长，单位s
    unsigned int  uiActiveTime;          //侦测触发时长设置，单位s
    unsigned int  uiSoundType;           //扬声器声音类型设置，EN_ZJ_RING_TYPE
    unsigned char *pucSoundFile;         //扬声器声音文件名
}ST_ZJ_DEFAULT_POLICY;

// IoT告警类型信息
typedef struct stru_ZJ_Iot_Inf
{
    unsigned int        uiIoTType;      // IoT设备类型
    unsigned long long  lluIoTId;       // IoT设备ID
    unsigned int        uiIoTEventId;   // IoT事件ID
}ST_ZJ_IOT_INF;

// IoT告警策略信息
typedef struct stru_ZJ_IotPolicy_Info
{
    unsigned int        uiInIoTType;        // 告警IoT设备类型
    unsigned long long  lluInIoTId;         // 告警IoT设备ID
    unsigned int        uiInIoTEventId;     // 告警IoT事件ID

    unsigned int        uiOpenFlag;         // 策略启动标志  
    unsigned char       aucPolicyName[32];  // 策略名称

    unsigned int        uiStartTime;        // 策略布防开始时间(当天的秒数)
    unsigned int        uiEndTime;          // 结束时间戳(当天的秒数)
    unsigned int        uiSpanFlag;         // 是否跨天        
    unsigned int        uiWeekFlag;         // 周几 掩码代表一周中的一天或多天 0x7F:一周
                                            // 0x01:周一 0x02:周二 0x04:周三 0x08:周四 0x10:周五 0x20:周六 0x40:周日
}ST_ZJ_IOT_POLICY_INFO;

// 设备状态回调接口； 告诉设备当前状态；
typedef int (*ZJ_PFUN_DEVICE_STATUS)(EN_ZJ_SERVER_STATUS enDeviceStatus, int iErrCode);

// 重启摄像机回调接口； 告诉设备需要重新启动
typedef int (*ZJ_PFUN_DEV_REBOOT)();

// 进程退出回调接口； 告诉设备需要进程需要退出
typedef int (*ZJ_PFUN_DEV_EXIT)();

// 设置时间时区回调接口， 告诉设备按参数设置时间时区；
// iSyncFlag :设备是否自动ntp同步utc时间，1 自动同步 0 不自动同步
// iZone :设备当地时区，距离0时区的时差，单位s
// pcDatetime :设备当地时间，2019-10-28 23:23:24
// pucDst :夏令时，CET-1CEST,M3.5.0,M10.5.0/3
// 一般参数1、2、4 联合使用
typedef int (*ZJ_PFUN_DEVICE_SETTIMEZONE)(int iSyncFlag, int iZone, char* pcDatetime, char *pucDst);

// 时间时区信息获取回调接口，将设备时间时区信息通过该接口参数设置下去；
// piZone :设备当地时区，距离0时区的时差，单位s
// pucTime:设备当地时间，2019-10-28 23:23:24
//piSyncFlag:查询是否同步过互联网时间 0.不同步；1.同步
typedef int (*ZJ_PFUN_DEVICE_GETTIMEANDZONE)(int *piZone,unsigned char *pucTime,int *piSyncFlag);

// 设置默认时间和时区 iSetZoneFlag 时区是否有效, 1 有效 时区为iZone,iGMTSecond: utc标准时单位s, 0 无效。
typedef int (*ZJ_PFUN_DEVICE_SETDEFAULTZONETIME)(int iSetZoneFlag,int iZone,long long iGMTSecond);

// 设备恢复出厂设置回调接口，告诉设备需要将参数配置恢复出厂配置状态
typedef int (*ZJ_PFUN_DEVICE_RESTOREFACTORYSETTING)();

// 无感配网接口回调接口,1-成功，-1-失败
typedef int (*ZJ_PFUN_DEVICE_AUTOCONN_RESULT)(int iResult);
// 设置WIFI回调接口，告诉设备当前设置的WIFI模式，WIFI的参数（SSID, 密码，加密类型）；
typedef int (*ZJ_PFUN_DEVICE_SETWIFI)(EN_ZJ_NETWORK_TYPE enWifiType, char* pcSSID, char* pcPwd, int iEncrytType);
// 获取WIFI列表回调接口，获取信号最强的，最多16个WIFI列表；
typedef int (*ZJ_PFUN_DEVICE_GETWIFI)(ST_ZJ_WIFI_INFO stWifiInfo[16], unsigned int *puiWifiCount);
// 获取当前网络详情
typedef int (*ZJ_PFUN_DEVICE_GETNETINFO)(ST_ZJ_NETWORK_INFO* pstNetInfo);

typedef int (*ZJ_PFUN_DEVICE_RECVCUSTOMDATA)(const unsigned char* pucPeerID, unsigned char* pucData, int iLen);

// 设备进入休眠回调接口， 设备在该接口,收到等待秒数后，进入休眠状态，-1.为不可以进入休眠； 0.立刻进入休眠； > 0为等待对应的秒数后进入休眠；
typedef int (*ZJ_PFUN_STARTSLEEP)(int iWaitSecond);

// 音频编码开关回调， 音频开始编码/关闭编码 0. 结束； 1.开始
typedef int (*ZJ_PFUN_AUDIO_SWITCH)(int iState); 

// 视频编码开关回调， 该路视频开始编码/结束编码 0. 结束； 1.开始
typedef int (*ZJ_PFUN_VIDEO_SWITCH)(int iStreamID, int iState);

// 流媒体需要一个I帧回调接口，告知设备当前流需要一个关键帧； EN_ZJ_KEYFRAME_QUALITY
typedef int (*ZJ_PFUN_VIDEO_NEEDIFRAME)(int iStreamID,unsigned int uiKeyQuality);

// 红外灯开关回调接口，告诉设备红外灯切换模式 见：EN_ZJ_IRMODE
typedef int (*ZJ_PFUN_IRLED_SWITCH)(int iState);

// 设置画面翻转回调接口， 告诉设备画面进行翻转，取值参考：EN_ZJ_ROTATE_TYPE
typedef int (*ZJ_PFUN_IMAGEINVERSION)(int iState);

// 设置摄像机打开关闭回调接口，告诉设备关闭音频和视频； 0. 关闭； 1.打开
typedef int (*ZJ_PFUN_CAMERA_SWITCH)(int iState);

// 设置抓取图片回调接口，告诉设备抓取一幅实时画面的图片, 获取对应尺寸的图片，并存入到传入的空间中。 If success, the return value is the jpeg buf length.
typedef int (*ZJ_PFUN_VIDEO_GETJPEG)(EN_ZJ_PICTURE_TYPE enPicType, unsigned char** ppucJpegBuf);

// 摄像头PTZ回调接口，告诉设备进行PTZ操作;PTZ操作方法；速度，1~7；1最低；7最快；PTZStep：移动步长，一个步长为1度。360表示一直移动到头一圈
typedef int (*ZJ_PFUN_CAMERA_ONPTZ)(EN_ZJ_CAMERA_PTZ_CONTROL enPTZControl, int iSpeed, int iPTZStep);
// 摄像头当前位置获取，获取X,Y点值；
typedef int (*ZJ_PFUN_CAMERA_PTZ_GETPOINT)(int* piX, int* piY);
// 摄像机移动到预置点位置
typedef int (*ZJ_PFUN_CAMERA_PTZ_GOTOPOINT)(int iX, int iY);
// 摄像机自动检测，当预置点位置因人为转动电机导致错乱时，需要重新设置，设置前需要先自检获得初始位置；
typedef int (*ZJ_PFUN_CAMERA_PTZ_AUTOCHECK)();
// 0 静止, 1 转动中
typedef int (*ZJ_PFUN_CAMERA_PTZ_STATUS)(int *piPtzStatus);
// 摄像机巡航开始回调接口，告诉设备按定义的预置点巡航；
typedef int (*ZJ_PFUN_CAMERA_CRUISE_START)(int iPresetPointNum, ST_ZJ_CAMERA_CRUISE_PRESET* pstPresets);
// 摄像机停止巡航
typedef int (*ZJ_PFUN_CAMERA_PTZ_STOP)();

// 通知摄像机开始执行智能巡航  iPointCnt 坐标点个数、iDwellTime 每个点驻留的时间。
typedef int (*ZJ_PFUN_CAMERA_SMARTCRUISE_START)(int iPointCnt,int iDwellTime);

//iSpeakerId:门铃扬声器id。网关类门铃：0.外机，1.网关内机；单机版默认为0
//音频音量调节回调接口，告诉设备要将音量大小，设置音频采集音量; 0~100。 0为静音；100为最大声音
typedef int (*ZJ_PFUN_AUDIO_VOLUMN_ADJUST)(int iMicId,int iVolumn);

// 本地录像状态回调接口，告诉设备本地录像是否进行。 0.未录像；1.录像
typedef int (*ZJ_PFUN_CAMERA_RECORD_STATUS)(int iStatus);

//自定义水印展示模式，0,画面上只能展示一个角落的自定义osd  1，画面上可以同时展示四个角落的自定义osd
typedef int (*ZJ_PFUN_CAMERA_CUSTOMOSDMODESETTING)(int iDisplayMode);
/* 摄像头OSD水印添加回调接口，告诉设备对当前码流增加水印 iPosition:EN_ZJ_OSD_POSITION_TYPE 1.左上；2.左下；3.右上；4.右下；0.默认
当 openfalg 是 0 的时候 , 忽略 iPosition 和 pcOSDName 两个参数
*/
typedef int (*ZJ_PFUN_CAMERA_OSDSETTING)(int iPosition, char* pcOSDName);
// osd 水印的 开关显示
typedef int (*ZJ_PFUN_CAMERA_CTRLOSDSHOWFLAG)(int iOpenFlag);


// 摄像头有逆向流传入回调接口，告诉设备有逆向媒体流传入； 1. Start; 0. Stop; iStreamType:EN_ZJ_STREAMER_TYPE
typedef int (*ZJ_PFUN_CAMERA_MEDIA_TOPLAY)(const char* pcPeerID, ZJ_HANDLE hPlayHandle, int iStreamType, int iStatus);

//日志收集回调接口，通过该接口将所有日志打包成pcDesInfo文件；
typedef int (*ZJ_PFUN_COLLECTLOGFILES)(unsigned char *pucPeerId,unsigned int iSessionId,unsigned char *pcDesInfo);

//SD卡格式化回调接口，通过该接口通知设备重构
typedef int (*ZJ_PFUN_CAMERA_FORMATSDCARD)();

// 获取SD卡容量和状态回调接口，通过该接口获取SD卡容量信息 单位 M 和 sdcard状态 EN_ZJ_SDCARD_STATUS
typedef int (*ZJ_PFUN_CAMERA_GETSDCARDSIZE)(unsigned int* piTotalSize, unsigned int* piFreeSize);

// SD卡自检测回调接口，通过该接口让SD卡进行检测修复，返回错误信息。 0 为无错误；
typedef int (*ZJ_PFUN_CAMERA_CHECKSDCARD)(int* piErrCode);

// 设置摄像机GAT1400开关
typedef int (*ZJ_PFUN_SET_GA1400SWITCH)(unsigned int uiGa1400Switch);

// 设置GA1400设备信息
typedef int (*ZJ_PFUN_SET_GA1400INFO)(unsigned char *pucGa1400ID, unsigned char *pucDomain);

// 通知设备GA1400在线状态
typedef int (*ZJ_PFUN_SET_GA1400STATUS)(unsigned int uiGa1400Status);

// AIiOT 设备开始工作回调接口， 通过该接口启动AIIOT设备信号采集；
typedef int (*ZJ_PFUN_AIIOT_START)(unsigned int uiAIIoTType, unsigned long long uiAIIoTID);

// AIiOT 设备停止工作回调接口， 通过该接口通知设备停止信号采集；
typedef int (*ZJ_PFUN_AIIOT_STOP)(unsigned int uiAIIoTType, unsigned long long lluAIIoTID);

/*************************************************************************************
查询AIiOT 设备当前事件或信号量回调接口， 通过该接口返回设备当前的信号量和事件, 
返回输入信号类型和对应的信号Json字串返回下来
{"DNFlag":"1"} // 1 白天 2 晚上
*************************************************************************************/
typedef int (*ZJ_PFUN_AIIOT_GETINPUT)(unsigned int uiAIIoTType, unsigned long long uiAIIoTID,unsigned char aucSignalValue[256]);

// 向AIiOT 设备输出信号回调接口，通过该接口，将输出信号类型和对应的JSON描述字符串输出到AIiOT设备；CONTRL 函数
typedef int (*ZJ_PFUN_AIIOT_OUTPUT)(unsigned int uiAIIoTType, unsigned long long uiAIIoTID, unsigned char* pSignalValue,ST_ZJ_TRIGGER_INFO* pstTriggerInf);

// 向AIIoT设备设置属性回调接口，通过该接口，将新的属性值设置给AIIoT设备，让设备以设置的属性值进行工作；属性以一个JSON字符串传递；
// return :0 成功，1 算法不支持，2 算法互斥，3 内存不足，4 算力不足，5 其他错误
// 如果设备支持AI开关返回错误，则厂家需要调用ZJ_SetAISwitchReturnErrAbility接口设置能力
typedef int (*ZJ_PFUN_AIIOT_SETPROP)(unsigned int uiAIIoTType, unsigned long long uiAIIoTID, unsigned char* pstProp); // changed type

// 检测当前输入信号对应的事件类型回调接口，通过该接口，判定当前输入信号的事件类型，以返回值返回；
typedef int (*ZJ_PFUN_AIIOT_CHECKEVENT)(unsigned int uiAIIoTType, unsigned long long uiAIIoTID, unsigned char* pSignalValue);

// 设置设备名称
typedef int (*ZJ_PFUN_SET_DEVICE_NAME)(unsigned char *pucDeviceName);

// 强拆设备触发报警开关
typedef int (*ZJ_PFUN_SET_DISMANTABLE_ALARM)(unsigned int uiOpenFlag);

// 逗留报警开关
typedef int (*ZJ_PFUN_SET_STAY_ALARM)(unsigned int uiOpenFlag);

// 设置摄像机宽动态开关
typedef int (*ZJ_PFUN_SET_WIDE_DYNAMIC_CAM)(unsigned int uiOpenFlag);

// 配置变化 通知 
typedef int (*ZJ_PFUN_CFGITEM_CHANGE)(unsigned int uiCfgItem);


// 音频采集参数配置的回调接口,参数配置放在pstAudioParm里边；
typedef int (*ZJ_PFUN_SET_AUDIO_PARM)(ST_ZJ_AUDIO_PARAM *pstAudioParm);

// 这是设置视频采集参数的回调函数
typedef int (*ZJ_PFUN_SET_VIDEO_PARM)(int iStreamID,ST_ZJ_VIDEO_PARAM *pstVideoParm);

// 这是设置PTZ 参数的回调接口
typedef int (*ZJ_PFUN_SET_PTZ_PARM)(int iOpenFlag);

/****************************************************************
升级回调函数
***************************************************************/
// 设备新版本回调接口，告诉设备有新版本
typedef int (*ZJ_PFUN_NEWVERSON_NOTICE)(unsigned char *pucNewVersion,unsigned int uiFileSize);

// 设备镜像下载回调接口，sdk下载下来的数据通过此回调透传给设备，uiEndFlag : 1 表示下载完成   0 表示下载中
typedef int (*ZJ_PFUN_NEWVERSON_DATADOWN)(unsigned char *pucPackage,unsigned int uiPacklen,unsigned int uiEndFlag);

// 停止下载文件
typedef int (*ZJ_PFUN_STOPUPGRADE)();

// 1 解签成功，通知固件覆盖镜像、 0 解签不成功，不覆盖
typedef int (*ZJ_PFUN_COVERIMAGE_NOTICE)(unsigned int uiCoverFlag);

/****************************************************************
***************************************************************/
// 切换摄像头
typedef int (*ZJ_PFUN_SWITCH_LEN)(int iLenId);

/***************************************************************
自定义报警声音
***************************************************************/
// 删除声音文件
typedef int (*ZJ_PFUN_DELSOUNDFILE)(unsigned char *pucSoundName);
// 获取声音文件列表 ,第三个参数内存固件层开辟，sdk释放,piTotalSize，piFreeSize单位kb
typedef int (*ZJ_PFUN_GETSOUNDFILES)( unsigned int *piTotalSize, unsigned int *piFreeSize, ST_ZJ_SOUNDFILE_INFO **pstHeadNode, EN_ZJ_RING_TYPE enSoundType);

/***************************************************************
中继设备
***************************************************************/
// 唤醒、休眠
typedef int (*ZJ_PFUN_SETRELAYDEVAWAKE)(unsigned int iAwakeStatus);

// 获取cpu占用率、内存使用率回调接口，通过该接口返回系统CPU占用率和内存使用率
typedef int (*ZJ_PFUN_CAMERA_CPU_RAM_USAGE)(unsigned int* piCpuUsage, unsigned int* piRamUsage);

/***************************************************************
ai图片相关
pucDes 结构：
{"Age":"","Sex":"","Race":"","HaveStubble":"","HaveGlasses":"","HaveFaceMask":""}
***************************************************************/
typedef struct stru_ZJ_AIPIC_NODE
{
    unsigned int  uiPicLen;             // ai图片长度
    unsigned int  uiSimilarity;         // 相似度 必填
    unsigned char aucCarNum[16];        // 车牌号
    unsigned char aucLabelID[64];       // ai图片id(电信labelid和布控id复用)
    unsigned char *pucPicBuf;           // ai图片buf
    unsigned char *pucDes;              // json 描述信息 年龄、是否佩戴眼镜、是否佩戴口罩、有没有胡子、皮肤种族
    double dPointX;                     // 0 - 1  左上角 是 00 右下角 是11  (可选)
    double dPointY;
    double dWidth;                      // 抓拍图宽(原图扣出来的宽) 必填
    double dHeight;                     // 抓拍图高(原图扣出来的高) 必填 
    unsigned int  uiLeftTopX;           // 抓拍图左上角坐标X(基于背景图宽) 必填
    unsigned int  uiLeftTopY;           // 抓拍图左上角坐标Y(基于背景图高) 必填
    unsigned int  uiRightBtmX;          // 抓拍图右上角坐标X(基于背景图宽) 必填
    unsigned int  uiRightBtmY;          // 抓拍图右上角坐标Y(基于背景图高) 必填
    struct stru_ZJ_AIPIC_NODE *pstNextNode;
}ST_ZJ_AIPIC_NODE;

typedef struct stru_ZJ_AIPIC_EVENT_INFO
{
    unsigned int       uiBgJpgLen;      // 背景图大小
    unsigned char     *pucBgJpgBuff;    // 背景图
    ST_ZJ_AIPIC_NODE  *pstAiPicHead;    // ai图片（人脸、牌照） ST_ZJ_AIPIC_NODE
    double             dBgWidth;        // 背景图宽 必填 ZJ_SetAiPicEventEx2 接口要求背景图宽:1920
    double             dBgHeight;       // 背景图高 必填 ZJ_SetAiPicEventEx2 接口要求背景图高:1080
    unsigned long long lluTimeStamp;    // 秒时间戳
}ST_ZJ_AIPIC_EVENT_INFO;

// zip方式上传
typedef struct stru_ZJ_AI_FACEFILE_INFO
{
    unsigned char aucCarNum[16];                        // 车牌号
    unsigned char aucFaceFileName[32];                  // face或者车牌抓拍图的文件名 名字规范：三位数字.jpg  例如：001.jpg  不能重复
    struct stru_ZJ_AI_FACEFILE_INFO *pstNextNode;
}ST_ZJ_ZIP_AIFACEFILE_INFO;

typedef struct stru_ZJ_AI_EVENT_NODE
{
    unsigned char aucBgFileName[32];                    // 背景图拍图的文件名 名字规范：三位数字.jpg  例如：000.jpg  不能重复
    unsigned long long lluTimeStamp;                    // 毫秒时间戳
    ST_ZJ_ZIP_AIFACEFILE_INFO  *pstZipFaceFileHead;     // ST_ZJ_ZIP_AIFACEFILE_INFO
    struct stru_ZJ_AI_EVENT_NODE *pstNextNode;
}ST_ZJ_ZIP_AIBGFILE_NODE;

typedef struct stru_ZJ_AI_ZIP_EVENT_SIGNAL
{
    unsigned char aucFilePath[128];                 // 绝对路径,压缩包sdk删除，压缩包命名格式：UID_yyyyMMddHHmmss.zip
    ST_ZJ_ZIP_AIBGFILE_NODE *pstZipBgFileHead;      // ST_ZJ_ZIP_AIBGFILE_NODE
}ST_ZJ_AI_ZIP_EVENT_SIGNAL;

// AI告警事件上传图片和视频信息
typedef struct stru_ZJ_AI_AlARM_UPLOAD_INF
{
    unsigned char  ucPicPath[256];             // AI事件图片本地文件路径 UID_AI_yyyymmddhhmmss.jpg
    unsigned char  ucVideoPath[256];           // AI事件视频本地文件路径 UID_AI_yyyymmddhhmmss.ps
    unsigned long long lluTimeStamp;          // AI事件触发时间戳(秒)
    unsigned char  aucExpandField[1024];      // AI事件上报拓展字段,例如{"carcode":"1"},当前版本置空
}ST_ZJ_AI_AlARM_UPLOAD_INF;

typedef enum enum_PS_CONTENT_TYPE
{
    EN_PS_CONTENT_TYPE_NONE           = 0X00,
    EN_PS_CONTENT_TYPE_HIGHPARABOLIC  = 0x01   // 高空抛物
}EN_PS_CONTENT_TYPE;

typedef enum enum_ZJ_FREE_BEFORE_STARTUP_CACHE_TYPE
{
    EN_ZJ_FREE_BEFORE_STARTUP_CACHE_AIALARMPV   = 0X01, // 启动前清理 ZJ_IotAIEventPVInPut 产生的缓存
    EN_ZJ_FREE_BEFORE_STARTUP_CACHE_AIZIP       = 0x02  // 启动前清理 ZJ_SetAiPicEventEx 产生的缓存
}EN_ZJ_FREE_BEFORE_STARTUP_CACHE_TYPE;

// 创建识别底库集,一个pucLabelID可对应多张picid ,iLableType 人脸  2 牌照
typedef int (*ZJ_PFUN_CREAT_AILABLE)(unsigned char *pucLabelID,int iLableType);

// 更改图片归属的集合
typedef int (*ZJ_PFUN_MODIFY_PICINF)(unsigned char *pucPicID,unsigned char *pucNewLableID);

#if 0
// 向底库集中添加新的图片
typedef int (*ZJ_PFUN_ADDAIPICCB)(unsigned char *pucLabelID,unsigned char *pucPicID,unsigned char *pucPicBuff,unsigned int uiPicLen);
#else
// 向底库集中添加新的图片 包括特征值提取
typedef int (*ZJ_PFUN_ADDAIPICCB)(unsigned char *pucLabelID, unsigned char *pucPicID, unsigned char *pucAIPicFile);
#endif

// 删除底库中某一张图片 包括特征值
typedef int (*ZJ_PFUN_DELAIPICSCB)(unsigned char *pucLabelID,unsigned char *pucPicID);

// 删除底库集合
typedef int (*ZJ_PFUN_DELAILABLE)(unsigned char *pucLabelID);

// 释放上报人脸图和背景图的缓存
typedef int (*ZJ_PFUN_FREEAIPICCACHE)(unsigned char *pucPicID, unsigned char *pucFaceData, unsigned char *pucBgData);

// 自动录入开关 uiAutoInPutFlag 自动录入开关1 打开 0 关闭,uiSimiliarity 相似度 0-100
// uiAutoInPutFlag==1，当识别出来相似度小于uiSimiliarity，设备上报一个抓拍信号到sdk
// uiAutoInPutFlag==0，当识别出来相似度大于uiSimiliarity，设备上报一个识别信号到sdk,小于相似度则不上报。
typedef int (*ZJ_PFUN_SETAUTOINPUTFLAG)(unsigned int uiAutoInPutFlag,unsigned int uiSimiliarity);

// 释放上报IOT AI告警事件的视频图片的缓存
typedef int (*ZJ_PFUN_FREEUPLOADAIALARMPVCACHE)(unsigned char *ucPicPath, unsigned char *ucVideoPath);

// 释放启动前可能存在的缓存 参考: EN_ZJ_FREE_BEFORE_STARTUP_CACHE_TYPE
typedef int (*ZJ_PFUN_FREEBEFORESTARTUPCACHE)(EN_ZJ_FREE_BEFORE_STARTUP_CACHE_TYPE iFreeStartUpCacheType);

// 创建PS MUXER句柄
_ZJ_API int PsMuxer_CreateHandle(unsigned char *pucFilePath, EN_PS_CONTENT_TYPE ucContentType);

// 销毁PS MUXER句柄
_ZJ_API int PsMuxer_DestoryHandle(int iPsMuxerId);

// 写视频数据
_ZJ_API int PsMuxer_WriteVideo(int iPsMuxerId, char *ptDatabuff, unsigned int uiDatabuffLen, unsigned int uiTimeStamp, unsigned int bIFrame, unsigned char aucPesPriData[128]);

// 写音频数据
_ZJ_API int PsMuxer_WriteAudio(int iPsMuxerId, char *ptDatabuff, unsigned int uiDatabuffLen, unsigned int uiTimeStamp);

/************************************************************************
** 人流量统计
************************************************************************/
// uiInterval 设置设备统计的时间间隔,每经过间隔时间就要上报一次此期间统计的人数
// uiOpenFlag 开关 1 打开  0 关闭
typedef int (*ZJ_PFUN_SETHUMANCOUNT_PARAM)(unsigned int uiOpenFlag,unsigned int uiInterval);

//pucRegions:格式 "Regions":[{"x": 0.100,"y": 0.001},{"x": 0.100,"y": 0.001},{"x": 0.100,"y": 0.001}, {"x": 0.100,"y": 0.001},]
typedef int (*ZJ_PFUN_SETHUMANCOUNT_REGIONS)(unsigned char *pucRegions);

/************************************************************************
** 小喇叭
************************************************************************/
// 把音频编码参数回调给设备，并且通知设备开始下载音频文件,uiPlayVolume 播放音量0-100,uiPlayRate 播放速率0-2
typedef int (*ZJ_PFUN_START_CUSTOM_AUDIO)(ST_ZJ_AUDIO_PARAM *pstCustomAudioParm, unsigned int uiPlayVolume,unsigned int uiPlayRate);

// 小喇叭音频数据回调
typedef int (*ZJ_PFUN_CUSTOM_AUDIOD_TRANS)(unsigned char *pucPackage,unsigned int uiPacklen);

// 结束下载 
typedef int (*ZJ_PFUN_STOP_CUSTOM_AUDIO)();

// 播放自定义字符串
typedef int (*ZJ_PFUN_BROADCAST_CUSTOM_STRING)(unsigned char *pStrBuff);

// rtos设备 需要实现ping操作
typedef int (*ZJ_PFUN_DEVICE_PINGINF)(unsigned char *pucIp, unsigned int uiPingNum, unsigned int *puiDelayMin, unsigned int *puiDelayMax, unsigned int *puiDelayAvg, unsigned int *puiMissRate);

/************************************************************************
** 云化摄像头
************************************************************************/
// 通知云化sdk去平台下载AI算法
typedef int (*ZJ_PFUN_CLOUDCAMERA_NOTICE)();

// 获取流水号回调函数定义
typedef int (*ZJ_PFUN_CLOUDCAM_GETEVENTNUM)(unsigned int uiIotType,unsigned char *pucEventNum);

// iot属性变化通知回调
typedef int (*ZJ_PFUN_CLOUDCAM_SETPROP)(unsigned int uiAIIoTType, unsigned long long uiAIIoTID, unsigned char* pstProp);

/************************************************************************
* common file download
************************************************************************/
// notice app have data to down    uiFileType : EN_ZJ_FILE_FORMAT    uiFileSize :Byte
typedef int (*ZJ_PFUN_FILEDOWN_NOTICE)(unsigned int uiFileType,unsigned int uiFileSize,unsigned char *pucFileName);

// trans data CB ,uiEndFlag : 0 download  ing  1 download  endflag 
// app savefile ok  return 0 ,else 1
typedef int (*ZJ_PFUN_FILEDOWN_TRANS)(unsigned char *pucPackage,unsigned int uiPacklen,unsigned int uiEndFlag);

// stop save downloaded file,data is not usefull
typedef int (*ZJ_PFUN_FILEDOWN_STOP)();

// AI事件上报标志回调
typedef int (*ZJ_PFUN_SET_EVENT_PUSHFLAG)(unsigned int uiMothionPushFlag, unsigned int uiHumanPushFlag);

#ifdef __cplusplus
}
#endif


#endif
