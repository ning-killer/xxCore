#ifndef _ZJ_SWD_H_
#define _ZJ_SWD_H_

#ifdef __cplusplus
extern "C" {
#endif

// 厂商管理第三方线程涉及的状态
typedef enum enum_ZJ_SWD_STATUS_CODE{
      EN_ZJ_SWD_STATUS_CODE_MIN_VALUE = 0,                          // 厂商管理的状态码最小值

      // 一键告警推流第三方线程涉及状态码范围 [1001-1100]; 具体状态码，厂商自行定义

      EN_ZJ_SWD_STATUS_CODE_MAX_VALUE = 9999,                       // 厂商管理的状态码最大值
}EN_ZJ_SWD_STATUS_CODE;

/*
描述: 第三方模块线程备注册SDK看门狗
参数: pucModuleName     注册SDK看门狗模块名
      iTimeOutSecs      喂狗超时时间
      iFeedIntervalSec  喂狗间隔时间
返回值:注册成功的看门狗ID 应用于后续的喂狗和注销
*/
_ZJ_API unsigned int ZJ_ThirdModuleRegistSwd(unsigned char *pucModuleName, int iTimeOutSecs, int iFeedIntervalSec);

/*
描述: 第三方模块线程备向SDK喂狗
参数: uiWatchId  看门狗ID
返回值: 0:成功 -1:失败
*/
_ZJ_API int ZJ_ThirdModuleFeedDog(unsigned int uiWatchId);

/*
描述: 第三方模块线程注销SDK看门狗
参数: uiWatchId  看门狗ID
返回值: 0:成功 -1:失败
*/
_ZJ_API int ZJ_ThirdModuleUnRegistSwd(unsigned int uiWatchId);

/*
描述: 厂商管理第三方线程涉及的状态变更通知SDK
参数: pucModuleName     注册SDK看门狗模块名
      uiStatusCode      厂商管理第三方线程涉及的状态 参考: EN_ZJ_SWD_STATUS_CODE
      pucStrStatusLog   设备运行状态描述字符串
*/
_ZJ_API int ZJ_StatusChangedNotify(unsigned char *pucModuleName, unsigned int uiStatusCode, unsigned char *pucStrStatusLog);


#ifdef __cplusplus
}
#endif

#endif