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

#ifdef __cplusplus
extern "C" {
#endif

#define APP_USING_ANDLINK_CJSON
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
typedef struct {
    char cmei[32];          // 设备唯一标识，必选
    int authMode;           // 0表示类型认证，1表示设备认证，设备认证时，需使用authId和authKey
    char authId[64];        // 用于生成工作密钥，设备认证必选
    char authKey[256];       // 用于生成工作密钥，设备认证必选
    char mac[32];           // 设备真实MAC，全大写不带冒号
    char sn[32];            // 设备SN，必选
    char reserve[32];       // 标记字段，可选
    char manuDate[32];      // 设备生产日期，格式为年-月
    char OS[32];            // 操作系统
    char chips_type[32];    //  芯片类型，如Main/WiFi/Zigbee/BLE等
    char chips_factory[32]; // 芯片厂商
    char chips_model[32];   // 芯片类型
    char romSize[8];        //flash 储存总容量 16MB
    char ramSize[8];        //内存 总容量    128MB
    char deviceIP[64];      //设备IP
    char powerSupplyMode[8]; //供电类型，电池供电:battery;   POE供电:POE;   市电:220V(110V);   USB供电:USB;   其他方式:other
}andlink_dm_t;

typedef struct {
    CFG_NET_MODE_e  cfgNetMode;
    char deviceVendor[16];      //厂商名称英文或中文拼音拼写，尽量简短
    char deviceBrand[16];      //设备品牌
    char deviceModel[16];      //设备型号
    char deviceType[16];        //设备类型ID,即门户上的 产品ID
    char deviceMac[32];             // 厂商可以填mac或sn,即设备唯一标识
    char andlinkToken[32];          //设备在开发者门户注册的产品类型对应的平台Token
    char productToken[32];          //设备在开发者门户注册的产品类型Token，平台会检查其合法性，非法则不允许注册
    char firmWareVersion[16];       //设备固件版本号
    char softWareVersion[16];       //设备软件版本号
    char cfgPath[64];               //供sdk存储配置文件的系统路径，此路径需可读可写，断电数据不丢失
    andlink_dm_t dm_info;
}andlink_attr_t;

/*
 *andlink 接口初始化并运行
 * */
int andlink_adapt_init(andlink_attr_t *pAttrInfo);

/*
 *在恢复出厂或重新配网时需要调用
 * */
int andlink_adapt_reset();

/*
 *若设备走扫码绑定,扫码成功后,调用此接口通知Andlink SDK 用户ID和云网关url(gwAddress2).
 * */
int andlink_adapt_setScanCode(char *userKey,char *gwAddress,char *gwAddress2);

/*
 *andlink 停止并释放资源
 * */
int andlink_adapt_uninit();

/*
 *andlink ip更改后要重新设置ip
 * */
int andlink_adapt_setIP(char *addr);

#ifdef __cplusplus
}
#endif

#endif// __ANDLINK_ADAPT_H


