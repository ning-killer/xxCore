/*
*
* 文件名称：andlink_adapt.h
* 说 明:厂商集成andlink SDK需要适配的接口信息,由厂商补充实现.
*
*
*/

#ifndef __ANDLINK_ADAPT_H
#define __ANDLINK_ADAPT_H

#include "stdio.h"
#include "stdlib.h"
#include "errno.h"
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

//mkdir
#include <sys/stat.h>
#include <sys/types.h>


#include "andlink_pub.h"
//#include "lwm2msdk.h"


#ifdef __cplusplus
extern "C" {
#endif

// 如果厂商需要使用andlink封装的json能力
// 可在demo目录下的Makefile中开启APP_USING_ANDLINK_CJSON=y

#ifdef APP_USING_ANDLINK_CJSON



// json编码
/* 创建一个json对象  ,形似{%s}*/
cJSON *adl_cJSON_CreateObject();	

/* 创建一个对象类型的数组,形似[%s]  */
cJSON *adl_cJSON_CreateArray();

/* 创建一个int类型的数组, 形似numbers[count] */
cJSON *adl_cJSON_CreateIntArray(int *numbers, int count);

/* root对象中添加一个名为name的新的json对象 ,形似name:{}   */
void adl_cJSON_AddItemToObject(cJSON *root, char *name, cJSON *child);

/* root数组中添加一个名为name的新的json对象 ,形似name:{}   */
void adl_cJSON_AddItemToArray(cJSON *array, cJSON *item);

/* 根节点添加一个数字 ,形似name:123*/
cJSON *adl_cJSON_AddNumberToObject(cJSON *root, char *name, double number);

/* 根节点添加一个字符串, 形似name:"catcat" */
cJSON *adl_cJSON_AddStringToObject(cJSON *root, char *name, char *string);

/* 将json对象转换为普通字符串 */
char *adl_cJSON_Print(cJSON *root);

/* 将json对象转换为紧凑型字符串 */
char *adl_cJSON_PrintUnformatted(cJSON *root);

/* 删除json对象 */
void adl_cJSON_Delete(cJSON *root);





// json解码
const char *adl_cJSON_GetErrorPtr(void);

/*  fun1：解析json数据包，按照CJSON结构体序列化这个数据包，最终获得一个句柄 */
cJSON *adl_cJSON_Parse(char *string);

/* fun2：从json结构体中获取名为name的一个新的json对象；即获取json指定对象的成员；入参1是fun1的返回值 */
cJSON *adl_cJSON_GetObjectItem(cJSON *root, char *name);

/* fun3：一旦fun2中获取的对象成员是一个数组对象，需要使用这个函数，获取这个数组指定的下标对象；入参1是fun2的返回值；入参2是数组下标 */
cJSON *adl_cmhiJSON_GetArrayItem(cJSON *array,int item);

int adl_cmhiJSON_GetArraySize(cJSON *array);
/* 从json结构体中获取名为name的int值 */
int adl_cmhiJSON_GetValueInt(cJSON *root, char *name, int *poutValue);

/* 从json结构体中获取名为name的string值 */
int adl_cmhiJSON_GetValueString(cJSON *root, char *name,char *poutString, int outBufSize);

/* 从json结构体中获取名为name的int数组对象的值 */
int adl_cmhiJSON_GetValueIntArray(cJSON *root, char *name,int *poutArray, int maxArrayLen);

/* 从json结构体中获取名为name的string数组对象的值 */
int adl_cmhiJSON_GetValueStringArray(cJSON *root, char *name,char *poutArray, int maxArrayLen, int maxElementLen);

/* 从json结构体中获取名为name的新对象并将其转化为json字符串数组 */
int adl_cmhiJSON_GetObjItemString(cJSON *root, char *name,char *poutString,int outBufLen);

/* 从json结构体中获取名为name的新对象并将其转化为json字符串数组,调用者需对返回的地址进行free */
char *adl_cmhiJSON_GetObjItemAndRetString(cJSON *root, char *name);

#endif

#define DEMO_DBG_PRINT(format, arg...)  printf("[demo][INFO ]:""[%-20s:%4d]"format, __FILE__, __LINE__,##arg)
#define DEMO_DBG_PRINT_ERROR(format, arg...)  printf("[demo][ERROR]:""[%-20s:%4d]"format, __FILE__, __LINE__,##arg)


/* 厂商必要实现的函数声明         start */
int demo_ctrl_wifi_callback(WIFI_CTRL_OPT_e opt, wifi_cfg_info_t *wificfg, char *outMsg, int msgBufSize);
int demo_set_led_callback(ADL_DEV_STATE_e state);
int demo_dn_send_cmd_callback(RESP_MODE_e mode, dn_dev_ctrl_frame_t *ctrlFrame, char *eventType, char *respData, int respBufSize);
int demo_get_device_ipaddr(char *ip, char *broadAddr);
int demo_reset_device_Ipaddr();
int demo_download_upgrade_version_callback(char *childDeviceID, char *downloadrul, char *filetype, int chkfilesize);
int demo_dev_paramsSync_callback();
char *getDevExtInfo();


// 新增回调接口
int demo_getCfg(char *item, char *value, int bufsize);
int demo_setCfg(char *item, char *value);
int demo_childdev_download_upgrade_version_callback(char *childDevId, char *downloadurl, char *filetype, int chkfilesize);
int demo_upgrade_version_callback(char *childDevId, char *filename, char *filetype);
int demo_get_dmInfo_callback(char *childDeviceId, cJSON *root);
int demo_get_led_state();





/* 厂商需要实现的函数声明         end */



// main函数所在文件引用的函数
int demo_setDeviceIpAddr(char *ip, char *broadAddr);

int demo_setCfgNetMode(int mode);

int demo_getCfgNetMode();

int recoverFacDevinfoCfg(adl_dev_attr_t *outDevAttr);

int buildFacDevinfoCfgFile(adl_dev_attr_t *outDevAttr);

int demo_cs_get_port(int csIndex,int *csPort);

int demo_cs_get_value(int csIndex,char *csValue, int csValueBufSize);

char* adlDemoVersionGet ();





#define DBG_PRINT_DM(format, arg...) do { \
		printLog(1,16,"[DM:ADAPT][INFO]:[%s:%d]"format, __FILE__, __LINE__, ##arg);\
	} while(0)

#define DBG_PRINT_ADL(format, arg...) do { \
			printLog(1,16,"[ADL:ADAPT][INFO]:[%s:%d]"format, __FILE__, __LINE__, ##arg);\
		} while(0)
		

#ifdef DEMO_DEBUG_ENABLE
// andlink SDK启动后,随机选择时间点,验证扫码绑定
void test_randInputUserkey(int msBegin, int msEnd);
void test_devReset(int msBegin, int msEnd);

#endif

#ifdef __cplusplus
}
#endif

#endif// __ANDLINK_ADAPT_H


