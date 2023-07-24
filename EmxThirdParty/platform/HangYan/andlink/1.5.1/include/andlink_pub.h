/*
* Copyright (c) 2019,中移(杭州)信息技术有限公司
* All rights reserved.
*
* 文件名称：andlink_pub.h
* 说 明：andlinkSDK 对外接口信息
*        !!!强烈注意:此文件由SDK维护,厂商只可引用,不可修改.!!!
* 初始版本：1.0
* 作 者：罗武通
* 完成日期：2019年4月29日
*
* 取代版本： 1.4
* 原作者 ：wuhao
* 完成日期： 2020年02月01日
*/


#ifndef __ANDLINK_PUB_H
#define __ANDLINK_PUB_H

#include "cJSON.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
启动的SOFT AP要求满足:
热点名称:CMQLINK-{$deviceType}-****,devieType 为设备在开发者门户申请的设备类型码;****为设备产生的随机四位码，一般是 MAC 地址后缀
通道网络配置:支持 DHCP,设备地址为 192.168.188.1
通道热点认证方式:广播、 开放式接入
*/


//SOFT AP 默认地址;AP_IP_ADDR,支持DHCP
#define AP_IP_ADDR "192.168.188.1"

//设备属性相关的宏
#define MAX_FUNCSTR_LEN 		32  // 管控命令function长度
#define MAX_DEV_ID_LEN			64  // 设备ID长度
#define MAX_SEQID_LEN 		    32  // 管控流水号长度
#define MAX_GW_TOKEN_LEN        32  // andlinkGWToken长度
#define MAX_ANDLINK_TOKEN_LEN   32  // andlinkToken长度
#define MAX_TOKEN_LEN			512 // productToken长度
#define MAX_DEV_EXT_LEN			512 // 设备扩展信息最大长度
#define MAX_CMDDATA_LEN			512 // 数据上报或管控数据的最大长度
#define MAX_DM_TOKEN_LEN	    128 // dmToken最大长度

#define MAX_AUTH_ID_LEN	    	32 		// authID最大长度
#define MAX_AUTH_KEY_LEN	    128 	// authKEY最大长度
#define MAX_AUTH_CODE_LEN	    1024 	// authCode最大长度




// wifi控制相关的宏
#define MAX_SSID_LEN     32
#define MAX_PASSWD_LEN   32
#define MAX_ENCRYPT_LEN  16


// WiFi控制选项
typedef enum 
{
	WIFI_OPT_STA_START  =	1, // 表示打开STA模式
	WIFI_OPT_STA_STOP   =	2, // 表示关闭STA模式
	
	WIFI_OPT_AP_START   =	3, // 表示打开AP模式
	WIFI_OPT_AP_STOP    =	4  // 表示关闭AP模式
}WIFI_CTRL_OPT_e;


// 配网模式
typedef enum 
{
	NETWOKR_MODE_WIRED  =	0x01,  // 表示有线配网设备
	NETWOKR_MODE_WIFI	=   0x02,  // 表示WIFI配网设备,sdk自主选择采用qlink配网还是soft配网方式
	NETWOKR_MODE_4G, 			// 表示4G配网设备
	NETWOKR_MODE_BT, 			// 表示蓝牙配网设备
	NETWOKR_MODE_OTHERS, 		// 表示其他配网设备

	NETWOKR_MODE_WIFI_NOTNEED_ADLCFGNET = 0x12, // 表示无需使用andlink配网功能的wifi设备
	NETWOKR_MODE_MAX
}CFG_NET_MODE_e;

// 下行控制命令响应模式
typedef enum 
{
	NORSP_MODE   =	0, // 无需响应
	ASYNC_MODE   =	1, // 异步响应,采用devDataReport进行响应
	SYNC_MODE    =	2, // 同步响应,下行管控函数的输出参数进行响应,一般用户设备接入本地网关,不常用
}RESP_MODE_e;

//设备的andlink状态
typedef enum 
{
	ADL_INIT,
	ADL_CFGNET    ,		// 配网状态
	ADL_CFGNET_SUC,		// 配网成功状态(收到工作热点信息)
	ADL_CFGNET_FAIL, 	// 配网失败状态

	ADL_BOOTSTRAP    ,  // 设备开始注册状态
	ADL_BOOTSTRAP_SUC,  // 设备注册成功状态
	ADL_BOOTSTRAP_FAIL, // 设备注册失败状态
	
	ADL_BOOT,           // 设备开始上线状态
	ADL_BOOT_SUC,       // 设备上线成功状态
	ADL_BOOT_FAIL,      // 设备上线失败状态
	
	ADL_ONLINE,         // 设备在线状态
	ADL_RESET,          // 设备复位状态
	ADL_OFFLINE,        // 设备离线状态
	ADL_STATE_MAX
} ADL_DEV_STATE_e;


//设备对外开放查询的属性信息
typedef enum
{
	ADL_AUTH_CODE,		 // 获取一机一密生成的设备工作秘钥; 
	ADL_AUTH_DEVICE_ID,	 // 获取一机一密生成的设备唯一ID;
	ADL_USER_KEY,		 // 获取userkey,默认值为16个0
	ADL_BIND_STATUS,	 // 获取设备的绑定状态,1,绑定;非1,为非绑定;
    ADL_MONITOR_USER_ID, // 获取安防类设备的安防平台userid;for单独无感配网的SDK
    ADL_AUTH_MODE        // 获取SDK是否支持一机一密,1支持
}EXPORT_DEVICE_ATTRS_e;

//ANDLINK的功能点
typedef enum
{
	//最多32个元素,每个元素表示一个功能
	ADL_APP_SEARCH_SERVICE = 0x01,		  // APP发现服务; 
	ADL_OFFLINE_UNBIND_AUTO_REBOOTSTRAP = 0x02,// 离线解绑再次自动注册功能
	ADL_XX1 = 0x04,
	ADL_XX2 = 0x08,

}ADL_FUNCS_e;



// wifi控制接口
typedef struct 
{
  char ssid[MAX_SSID_LEN];           
  char password[MAX_PASSWD_LEN];
  char encrypt[MAX_ENCRYPT_LEN];
  int channel;
} wifi_cfg_info_t;


// 下行控制指令帧结构
typedef struct 
{
  char function[MAX_FUNCSTR_LEN]; 
  char deviceId[MAX_DEV_ID_LEN];           
  char childDeviceId[MAX_DEV_ID_LEN];
  char seqId[MAX_SEQID_LEN];
  int  dataLen;
  char *data;
} dn_dev_ctrl_frame_t;


// 设备属性
typedef struct 
{
	CFG_NET_MODE_e  cfgNetMode;
	char *deviceVendor;
	char *deviceType;
	char *deviceMac; // 厂商可以填mac或sn,即设备唯一标识
	char *andlinkToken; 	// 平台对接 非必选;
	char *productToken; 	// 平台对接 非必选;
	char *firmWareVersion;
	char *softWareVersion;
	char *dmKey;			// V1.4.3及之后版本,DM迁移到andlink平台后,废弃;
	char *cfgPath;
	char *extInfo;
}adl_dev_attr_t;


//回调接口,用户注册、实现,杭研调用
typedef struct 
{
	// 按需实现;不需要使用andlink配网的wifi设备或有线设备,不必实现;
	// 控制WIFI(连接、断开热点;启动、关闭热点);  
	int (*ctrl_wifi_callback)(WIFI_CTRL_OPT_e opt, wifi_cfg_info_t *wificfg, char *outMsg, int msgBufSize);
	
	// 按需实现;通知设备状态
	int (*set_led_callback)(ADL_DEV_STATE_e state);

	// 按需实现,平台对接的设备不必实现;下行管控;
	int (*dn_send_cmd_callback)(RESP_MODE_e mode, dn_dev_ctrl_frame_t *ctrlFrame, char *eventType, char *respData, int respBufSize);

	// 按需实现,平台对接的设备不必实现;
	// 通知设备参数同步;sdk上线成功后调用;
	int (*dev_paramsSync_callback)();

	// 按需实现,平台对接的设备不必实现;
	// OTA1:下载并升级版本,childDevId为空表示父设备,否则表示子设备;
	int (*download_upgrade_version_callback)(char *childDevId, char *downloadurl, char *filetype, int chkfilesize);

    // 按需实现;
    // OTA2:设备仅升级版本,childDevId为空表示父设备,否则表示子设备;
	int (*upgrade_version_callback)(char *childDevId, char *filename, char *filetype);

	// 必须实现;注意入参可能其中之一为空,sdk需要哪个参数,哪一项才会传入地址,可参考示例代码
	// 获取设备IP;sdk以此判断设备是否联网;
	int (*get_device_ipaddr)(char *ip, char *broadAddr);
	// 按需实现;复位设备IP;平台对接的设备不必实现;
	int (*reset_device_Ipaddr)(void);

	// 按需实现; 若不实现此接口,sdk使用内部实现的读配置项接口;
	// 读配置项的接口; 举例getCfg("deviceId", a, sizeof(a)-1);
	int (*getCfg_callback)(char *item, char *value, int bufsize);

	// 按需实现; 若不实现此接口,sdk使用内部实现的写s配置项接口; 
	// 写配置项的接口;举例SetCfg("deviceId","CMCC-10086-666677778888");
	int (*setCfg_callback)(char *item, char *value);

	// 必须实现;DM信息获取;add by 21.02.19
	int (*get_dmInfo_callback)(char *childDeviceId, cJSON *root);
	
	 
}adl_dev_callback_t;




//必须调用; sdk 启动(sdk中devAttr,devCbs增加字段,不支持向上兼容,故sdk此接口变化导致的升级,应用程序也需要跟着升级)
int andlink_init(adl_dev_attr_t *devAttr, adl_dev_callback_t *devCbs);

// 按需调用,一般无需调用;sdk 销毁
int andlink_destroy(void);

// 按需调用,平台对接的设备不会调用;数据上报;
int devDataReport(char *childDevId, char *eventType, char *seqId, char *data, int dataLen);

// 设备复位,有恢复出厂或强制复位机制时,必须调用;
int devReset(void);


// 按下快联按键,打开快联模式
//int openQlinkMode(void);


// 查询设备相关属性的接口;按需调用;
char *getDeviceInfoStr(EXPORT_DEVICE_ATTRS_e attr);


// 按需调用;子设备注册 token最大长度:MAX_TOKEN_LEN
int childDevBootstrap(char *childDevId, char *childDevType, char *childPdtToken,char *outDeviceToken, char *outAndlinkToken);

// 按需调用;子设备上线
int childDevBoot(char *childDevId, char *childDevType, char *swVersion,char *fmVersion);

// 按需调用;记录日志 
//fid:0表示SDK,1表示设备
//logLevel: 0,不输出日志;1：单次关键日志，2：错误日志，4：告警1级，8：告警2级，16：普通日志，32：周期1级；64:周期2级;
int printLog(int fid, int logLevel, const char * fmt, ...);

// 设置printLog日志接口记录日志的级别;
// logLevel=0-NO output; =1-ONCE; =2-ERROR;  =4-ALARM_1;  =8-ALARM_2; =16-INFO; =32-CYC_1;=64-CYC_2;
// if ONCE+ERROR+ALARM_1+ALARM_2+INFO, then logLevel=31;
// logTo="file" 表示记录到文件;="terminal"表示记录到控制台
int set_printLog_debug_level(int logLevel, char *logTo);


//测试接口,一般无需调用;设置工作环境，默认是线上环境，可以通过此接口设置为测试环境
int setTestEnvFlag(int flag);
int getTestEnvFlag();
int setAdlDgsTestServUrl(char* url);
int setAdlCgwTestServUrl(char* url);



// 按需调用;设置andlink单个日志文件最大阈值,默认0x80000,即500KB 
int setAndlinkLogMaxSize(int size);

//按需调用;设置升级文件存储路径及文件扩展名
int setUpgradeFileStoragedPath(char *path);

//按需调用;设置升级文件扩展名,如"xxx.bin","xxx.upz"
int setUpgradeFilenameExtension (char *ext);


//按需调用;查询andlink SDK版本号
char *getAndlinkVersion();

//按需调用;查询平台对接DM SDK版本号,仅针对cloud access的设备
char *ca_getAndDmVersion();


//按需调用;一般只有扫码绑定的设备对接场景才会使用;
//若设备走扫码绑定,扫码成功后,调用此接口通知Andlink SDK 用户ID和云网关url(gwAddress2).
int setScanCodeBindConfigInfo(char *userKey,char *gwAddress,char *gwAddress2);

//按需调用;禁止andlink的某些功能,funcID可以是ADL_FUNCS_e中元素的组合
int disableAdlFunc(unsigned int funcPos);


#ifdef AHM_DISCOVERY_ENABLE //设备互联,not used

// 能力个数
#define MAX_CAPABILITY_OBJ  8

// 新增回调类型
typedef enum
{
	CB_GET_CS_PORT, 		  // 获取能力端口; 
	CB_GET_CS_VALUE, 		  // 获取能力值; 
	CB_MAX
}ADL_CALLBACK_TYPE_e;

//新增回调接口
typedef struct 
{
	// 获取能力端口
	int (*cs_get_port_callback)(int csIndex,int *csPort);
	
	//获取能力值
	int (*cs_get_value_callback)(int csIndex,char *csValue, int csValueBufSize);
	
}adl_dev_newcallback_t;


// 设置能力模型
int adl_cs_setModel(int csIndex, char *csName, int csID);

//注册回调接口
int set_andlink_callback(ADL_CALLBACK_TYPE_e cbType, void *cbHandler);



#endif

#ifdef __cplusplus
}
#endif

#endif


