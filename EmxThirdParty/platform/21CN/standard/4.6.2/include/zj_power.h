#ifndef _ZJ_POWER_H_
#define _ZJ_POWER_H_

#ifdef __cplusplus
extern "C" {
#endif

// 设置设备电源类型，是否带电池。 0.不带电池；1.带电池； 2 太阳能电池
_ZJ_API int ZJ_SetEnergySavingType(unsigned int iEngeryType);

// 设置唤醒能力， 见：EN_ZJ_AWAKE_ABILITY
_ZJ_API int ZJ_SetAwakeAbility(unsigned int uiAwakeAbility);

// 设置设备当前是否充电；（仅对电池类设备）
_ZJ_API int ZJ_SetPowerSupply(unsigned int uiSupplyFlag);

// 设置进入休眠回调函数
_ZJ_API int ZJ_SetStartSleepCb(ZJ_PFUN_STARTSLEEP pfuncStartSleep);

// 设置设备当前电池余量。
_ZJ_API int ZJ_SetPowerLevel(unsigned int uiPowerLevel);

// 获取平台接入地址，用于设备休眠后的保活连接
_ZJ_API int ZJ_GetLinkAddr(unsigned char **pucAddrIpv4, unsigned char **pucAddrIpv6, unsigned int *puiPort, unsigned char **pucToken);

#ifdef __cplusplus
}
#endif

#endif