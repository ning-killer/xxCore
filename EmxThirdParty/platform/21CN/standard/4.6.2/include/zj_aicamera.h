#ifndef _ZJ_AICAMERA_H_
#define _ZJ_AICAMERA_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct str_ZJ_POLICY_MODIFY_INFO
{
    unsigned int uiAIIotType;            //算法iot类型
    unsigned long long lluAIIoTID;       //AIiot ID
    unsigned int uiEventId;              //事件id,0 1 2 3 ...
    unsigned int uiOutIotType;           //联动iot类型
    unsigned long long lluOutIoTID;      //Outiot ID
    unsigned char *pucOutput;            //json
}ST_ZJ_POLICY_MODIFY_INFO;

/***************************************************************
云化AI算法 (云化sdk调用，厂商无需关注)
ctrlDev入参:
1、iotType==1009
"{\"CtrlType\":\"0\",\"AlarmType\":\"1000\",\"SoundType\":\"4\",\"SoundName\":\"\",\"LoopCnt\":\"2\",\"SpeakerId\":\"0\"}"
2、iotType==1007
"{\"CtrlType\":\"1\",\"Duration\":\"\"}"
***************************************************************/
// 添加云化摄像头IoT
//uiAIIoTType:	算法iot类型  
//lluAIIoTID：	camid默认0
_ZJ_API int ZJ_AddCloudCamIoTDevice(unsigned int uiAIIoTType, unsigned long long lluAIIoTID);
 
 // 主动调用添加Iot设备
_ZJ_API int ZJ_CloudCamAddIoTDevice(unsigned int uiAIIoTType, unsigned long long lluAIIoTID,
                        ZJ_PFUN_AIIOT_START pfunAIIoTStart, ZJ_PFUN_AIIOT_STOP pfunAIIoTStop, 
                        ZJ_PFUN_AIIOT_GETINPUT pfunAIIoTGetInput, ZJ_PFUN_AIIOT_OUTPUT pfunAIIoTOutPut,
                        ZJ_PFUN_AIIOT_SETPROP pfunAIIoTSetProp, ZJ_PFUN_AIIOT_CHECKEVENT pfunAIIotCheckEvent);

// 设置IoT设备的属性，IoT设备属性以一个JSON字符串传入
_ZJ_API int ZJ_CloudCamSetIoTDefaultProp(unsigned int uiAIIoTType, unsigned long long lluAIIoTID, char* pcProp);

// set default policy,add policy_prop,output_cloud
_ZJ_API int ZJ_CloudCamAddIoTDefaultPolicy(ST_ZJ_IOT_POLICY_INFO *pstIoTPolicyInfo);

// 获取IOT是否注册标志
_ZJ_API int ZJ_CloudCamGetIotRegistFlag(unsigned int uiAIIoTType, unsigned long long lluAIIoTID,unsigned int *puiRegistFlag);

// json能力值字符串输入
_ZJ_API int ZJ_SetCloudCameraExtAbility(unsigned char *pucJson);

// 设备基础功能接口
//uiAIIoTType:	iot类型 
//pcInput：		json字符串，控制硬件动作
_ZJ_API int ZJ_CloudCamCtrlDev(unsigned int uiAIIoTType, char * pcInput);

// 获取流水号 注册回调
_ZJ_API int ZJ_CloudCamSetEventNumGetCB(ZJ_PFUN_CLOUDCAM_GETEVENTNUM pfunGetEventNum);

// 套餐变化通知 注册回调
_ZJ_API int ZJ_SetNoticeCloudCameraCB(ZJ_PFUN_CLOUDCAMERA_NOTICE pfunNtcCloudCamera);

// 设置获取云摄像头SDK能力 1支持云化  0不支持云化
_ZJ_API int ZJ_SetCloudCameraAbility(unsigned int uiCloudCameraAbility);


// iot属性变化通知 注册回调
_ZJ_API int ZJ_SetIotPropChangeNtcCB(ZJ_PFUN_CLOUDCAM_SETPROP pfunSetAIIotProp);

// 云化摄像头设置和上报告警策略
_ZJ_API int ZJ_CloudCamera_SetAndReportAlarmPolicy();

// 云化摄像头设置人型侦测告警策略
_ZJ_API int ZJ_CloudCamera_CloseFenceAlarmPolicy();

// 修改Iot策略
_ZJ_API int ZJ_ModifyPolicyParam(ST_ZJ_POLICY_MODIFY_INFO *pstPolicyModifyInfo);

// 获取Iot策略 使用者自己释放得到的结果指针
_ZJ_API unsigned char* ZJ_GetPolicyParam(unsigned int uiAIIotType,long long lluAIIoTID,unsigned int uiEventId);

// 云化摄像头设置告警策略
_ZJ_API int ZJ_CloudCamera_SetAlarmPolicy(unsigned int uiKjIoTType,long long lluKjIoTId,unsigned int uiPolicyId, unsigned char* pucStrTmp, unsigned char ucAlarmPolicyUpdate);

// 云化摄像头获取告警策略 使用者自己释放得到的结果指针
_ZJ_API unsigned char* ZJ_CloudCamera_GetAlarmPolicy(unsigned int uiKjIoTType,long long lluKjIoTId,unsigned int uiPolicyId);

// 云化摄像头设置AI字段
_ZJ_API int ZJ_CloudCamera_SetAIJson(unsigned char* pucStrTmp, unsigned char ucCfgAiUpdate);

// 云化摄像头获取AI字段 使用者自己释放得到的结果指针
_ZJ_API unsigned char* ZJ_CloudCamera_GetAIJson();

// 线程监控模块注册 
// ucAppName  监控模块的名称  iTimeOutSecs  超时的秒数 > 0
_ZJ_API ZJ_HANDLE ZJ_AppThreadMonitorRegist(unsigned char *ucAppName, int iTimeOutSecs);

// 线程监控喂狗  hWdWriter  线程监控模块注册得到的地址  喂狗不能太频繁  建议1~3秒一次
_ZJ_API int ZJ_AppThreadMonitorFeedDog(ZJ_HANDLE hWdWriter);

// 线程监控模块注销  hWdWriter  线程监控模块注册得到的地址
_ZJ_API int ZJ_AppThreadMonitorUnRegist(ZJ_HANDLE hWdWriter);

#ifdef __cplusplus
}
#endif

#endif