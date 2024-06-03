#ifndef _ZJ_TIME_H_
#define _ZJ_TIME_H_
#include "zj_type.h"
#ifdef __cplusplus
extern "C" {
#endif

// 设置UTC时间回调接口，告诉设备UTC时间(0时区时间，非北京时间)
typedef int (*ZJ_PFUN_DEVICE_SETUTC)(long long illSecond);

// 设置配置UTC时间回调函数, 设备上线时会回调，上线之后3-4小时会定时回调
_ZJ_API int ZJ_SetUtcTimeCB(ZJ_PFUN_DEVICE_SETUTC pfunSetUtcTime);

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
// piSyncFlag:查询是否同步过互联网时间 0.不同步；1.同步
typedef int (*ZJ_PFUN_DEVICE_GETTIMEANDZONE)(int *piZone,unsigned char *pucTime,int *piSyncFlag);

//设置系统时间获取与设置回调函数
_ZJ_API int ZJ_SetZoneAndTimeCB(ZJ_PFUN_DEVICE_SETTIMEZONE pfunSetTimeZone, ZJ_PFUN_DEVICE_GETTIMEANDZONE pfunGetTimeZone);

// 设置默认时间和时区 iSetZoneFlag 时区是否有效, 1 有效 时区为iZone,iGMTSecond: utc标准时单位s, 0 无效。
typedef int (*ZJ_PFUN_DEVICE_SETDEFAULTZONETIME)(int iSetZoneFlag,int iZone,long long iGMTSecond);

// 设置默认的时间和时区,当固件层没有合入ntp的时候使用
_ZJ_API int ZJ_SetDefaultZoneAndTimeCB(ZJ_PFUN_DEVICE_SETDEFAULTZONETIME pFunSetDefaultTimeAndZone);

#ifdef __cplusplus
}
#endif

#endif