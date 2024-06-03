#ifndef _ZJ_NETWORK_H_
#define _ZJ_NETWORK_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * 设置设备网络状态信息，主动触发SDK对网络变化的响应，让响应更加及时；
 *************************************************************************/
_ZJ_API int ZJ_SetNetWorkType(EN_ZJ_NETWORK_TYPE enNetType);

// 设置获取WIFI列表和设置WIFI的回调函数；
_ZJ_API int ZJ_SetWifiCB(ZJ_PFUN_DEVICE_SETWIFI pfuncSetWifi, ZJ_PFUN_DEVICE_GETWIFI pfuncGetWifi, ZJ_PFUN_DEVICE_GETNETINFO pfunGetNetInfo);
// 设置网络错误状态，当网络发生错误时，通知错误类型
_ZJ_API int ZJ_SetNetErrStatus(EN_ZJ_NETWORK_ERR_STATUS enNetWorkErrStatus);

// WiFi配置能力设置，根据EN_ZJ_SETWIFI_ABILITY按位与表示支持多种配网模式
_ZJ_API int ZJ_SetWifiSetAbility(unsigned int uiSetWifiAbility); 

/**********************************************************************************
二维码 配网流程 上层识别到二维码，将对应的字符串设置下来
***********************************************************************************/
_ZJ_API int ZJ_SetQRCodeString(unsigned char *pstrQRCode);

//无感配网回调接口
_ZJ_API int ZJ_SetAutoConnSSIDInfoCBFunc(ZJ_PFUN_DEVICE_SETWIFI pFunSetAutoConnSsid);
//启用无感配网
//iConnectFlag 1:成功连接隐藏的wifis后 2:成功连接正常上网的wifi后
_ZJ_API int ZJ_SetAutoConnStatus(int iConnectFlag);
//无感配网结果
_ZJ_API	int ZJ_SetAutoConnResultCBFunc(ZJ_PFUN_DEVICE_AUTOCONN_RESULT pFunAutoConnResult);

// 设置WIFI强度
_ZJ_API int ZJ_SetWifiStrength(int iWifiStrength);

// 设置IPv6能力，设备支持IPv6
_ZJ_API int ZJ_SetIPv6Ability(int iIPv6Ability);

// 设置IPv6开关回调函数，IPv6开关默认打开
_ZJ_API int ZJ_SetIPv6SwitchCB(ZJ_PFUN_IPV6_SWITCH pfunIPv6Switch);

#ifdef __cplusplus
}
#endif

#endif