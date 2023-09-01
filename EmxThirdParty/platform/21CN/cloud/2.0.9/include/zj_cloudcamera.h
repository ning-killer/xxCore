#ifndef _ZJ_CLOUDCAMERA_H_
#define _ZJ_CLOUDCAMERA_H_


#ifdef __cplusplus
extern "C" {
#endif

//芯片信息
typedef struct stru_ZJ_AI_CHIP_INFO {
	unsigned char aucProduct[64];//设备型号
	unsigned char aucChipModel[64];//芯片型号
	unsigned char aucChipManufacturer[64];//芯片制造商
	unsigned char aucSystem[64];//芯片运行的系统环境
	unsigned char aucMacAddr[32];//设备的mac地址
	float chipComputingPower;//芯片算力,TOPS计算
	int iChipMemory;//芯片的内存大小，单位KB
	int iFlashSize;//芯片的flash大小，单位KB
	char cEnergyType;//供电类型,0.不带电池；1.带电池
	int iChipSdkVersion;//芯片版本，1，2，3这种自然数
} ST_ZJ_AI_CHIP_INFO;

//芯片运行状态
typedef struct stru_ZJ_AI_CHIP_STATUS {
	int iFreeMemory;//芯片剩余内存，单位KB
	int iUsedMemory;//芯片使用内存，单位KB
	float fFreePower;//芯片剩余算力
	float fUsedPower;//芯片使用的算力
	int iFreeFlash;//Flash空闲量，单位KB
	int iUsedFlash;//Flash已用量，单位KB
} ST_ZJ_AI_CHIP_STATUS;

//支持的音频播放信息
typedef enum enum_ZJ_AI_AUDIO_SAMPLERATE_TYPE{
    EN_ZJ_AI_AUDIO_SAMPLERATE_8K       = 0x00,//只支持8k
    EN_ZJ_AI_AUDIO_SAMPLERATE_8K16K    = 0x01,//支持8K和16K
}EN_ZJ_AI_AUDIO_SAMPLERATE_TYPE;

//可设置的日志打印级别
typedef enum enum_ZJ_AI_LOG_LEVEL
{
   EN_ZJ_AI_LOG_LVL_NONE  = 0x0000,
   EN_ZJ_AI_LOG_LVL_ERR   = 0x0001,
   EN_ZJ_AI_LOG_LVL_WARN  = 0x0002,
   EN_ZJ_AI_LOG_LVL_INFO  = 0x0004,
   EN_ZJ_AI_LOG_LVL_DBG   = 0x0008,
   EN_ZJ_AI_LOG_LVL_ALL   = 0x00FF, 
}EN_ZJ_AI_LOG_LEVEL;

//SDK初始化接口；传入一个模型文件存储路径，需要确保路径可访问，如不可访问则会返回对应的错误状态信息。
int ZJ_AI_Init(char * pcModelPath);
//启动云摄像头SDK相关线程并分配资源，需要在Init后执行
int ZJ_AI_Start();
//停止云摄像头SDK运行，相关线程会被删除
int ZJ_AI_Stop();	
//云摄像头SDK实例销毁接口
int ZJ_AI_Destroy();

//设置支持的音频采样率接口
int ZJ_AI_SetAudioSampleRate(int iSampleRateType);
//设置云化SDK运行工作环境，1、生产环境，0、测试环境
int ZJ_AI_SetCloudCameraSdkRunMode(int iSdcSdkRunFlag); 

//设置日志等级，在sdk初始化前完成设置
int ZJ_AI_SetDebugLevel(EN_ZJ_AI_LOG_LEVEL enDebugLevel);


//获取芯片信息回调函数
typedef int (*ZJ_PFUN_AI_GETCHIPINFO)(ST_ZJ_AI_CHIP_INFO* ai_chip_info);
//获取芯片状态回调函数
typedef int (*ZJ_PFUN_AI_GETCHIPSTATUS)(ST_ZJ_AI_CHIP_STATUS* ai_chip_status);
//直播流画框回调函数
typedef int (*ZJ_PFUN_AI_DRAWPOLYLINES)(unsigned char* pucRegions);
//设置共享内存与画面参数回调函数
typedef int (*ZJ_PFUN_AI_SETVIDEOPARAM)(unsigned char* pucVideoParam);
//设置算法日志打捞路径回调函数
typedef int (*ZJ_PFUN_AI_SETLOGPATH)(unsigned char* pucLogPath);
//设置启动实时跟踪框绘制回调函数
typedef int (*ZJ_PFUN_AI_STARTTRACK)(int iKey,int iChannel);
//设置停止实时跟踪框绘制回调函数
typedef int (*ZJ_PFUN_AI_STOPTRACK)(int iKey);

//设置获取芯片信息回调接口
int ZJ_AI_SetGetChipInfoCB (ZJ_PFUN_AI_GETCHIPINFO  pfunGetChipInfo);

//设置获取芯片状态回调接口
int ZJ_AI_SetGetChipStatusCB (ZJ_PFUN_AI_GETCHIPSTATUS  pfunGetChipStatus);

//设置直播流画框回调接口
int ZJ_AI_SetDrawPolylinesCB (ZJ_PFUN_AI_DRAWPOLYLINES  pfunDrawPolylines);

//设置共享内存与画面参数回调接口
int ZJ_AI_SetVideoParamCB (ZJ_PFUN_AI_SETVIDEOPARAM pfunSetVideoParam);
//设置算法日志打捞回调接口
int ZJ_AI_SetCollectLogPathCB (ZJ_PFUN_AI_SETLOGPATH pfunSetLogPath);
//设置实时跟踪框启停回调
int ZJ_AI_SetRealTimeTrackCB (ZJ_PFUN_AI_STARTTRACK pfunStartTrack, ZJ_PFUN_AI_STOPTRACK pfunStopTrack);

#ifdef __cplusplus
}
#endif

#endif

