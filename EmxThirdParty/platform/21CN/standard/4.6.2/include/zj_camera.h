#ifndef _ZJ_CAMERA_H_
#define _ZJ_CAMERA_H_

#ifdef __cplusplus
extern "C" {
#endif

_ZJ_API int ZJ_SetLensMaxCount(int iLensCount);

// focalLenth 镜头的当前焦距
_ZJ_API int ZJ_SetLensInf(int iLenId,char *pucLenName,EN_ZJ_CAMERA_LENS_TYPE enLensType,double focalLenth);

// 变焦镜头: 设置焦距的变化范围
_ZJ_API int ZJ_SetLenFocalLenthScale(int iLenId,double dMinFocalLenth,double dMaxFocalLenth);

_ZJ_API int ZJ_GetCurLenId(int *piOutCurLenId);

/********************************************************************************************************
以下为摄像头能力集设置函数， 不设置默认为不支持；
 ******************************************************************************************************/
// 设置码流数量 若为0则表示没有摄像头，默认为0
_ZJ_API int ZJ_SetStreamCount(int iStreamCount);

//设置语音对讲能力 0.不支持；1.支持单工；2.支持双工
_ZJ_API int ZJ_SetVoicePlayAbility(int iVoicePlayAbility);

//设置云广播能力 0.不支持；1.支持
_ZJ_API int ZJ_SetCloudBroadcastAbility(unsigned int uiCloudBroadcastAbility);

//OSD设置能力支持  0.不支持； 1.支持；
_ZJ_API int ZJ_SetOSDSetAbility(int iOSDSetAbility);

//OSD设置拓展能力支持/自定义水印支持四角同时展示能力  0.不支持； 1.支持；
_ZJ_API int ZJ_SetCustomOSDExpandAbility(int iOSDSetExpandAbility);

//预置点设置能力支持  0.不支持； 1.支持；
_ZJ_API int ZJ_SetPresetPointAbility(int iPresetAbility);

//设置分辨率能力集；支持多种按位与 EN_ZJ_CARERA_RESOLUTION_ABILITY 必须设置
_ZJ_API int ZJ_SetResolutionAbility(int iStreamid,int iResolutionAbility);

//设置编码能力；支持多种按位与 见：EN_ZJ_VIDEOENC_TYPE 必须设置
_ZJ_API int ZJ_SetVideoEncodeAbility(int istreamid,int iEncAbility);

//设置麦克风是否支持 0.不支持； 1.支持；
_ZJ_API int ZJ_SetMicroPhoneAbility(int iMicroPhoneAbility);

//设置IRLed红外灯控制模式  EN_ZJ_IRMODE
_ZJ_API int ZJ_SetIRLedAbility(int iIRLedAbility);
// 已遗弃
_ZJ_API int ZJ_SetCamerCurIRWorkMode(int iCurWorkMode);

//设置图像翻转设置支持能力  0.不支持； 1.支持；
_ZJ_API int ZJ_SetImageInversionAbility(int iInversionAbility);

//设置SD卡支持能力   0.不支持； 1.支持；
_ZJ_API int ZJ_SetSDCardAbility(int iSDCardAbility);

// 设置PTZ支持能力
//设置PTZ能力 0.不支持；0x01: 支持P；0x02: 支持T；0x04: 支持Z；按位与
_ZJ_API int ZJ_SetPTZAbility(int iPTZAbility);

//设置PTZ云台速度设置标志 0.不支持； 1.支持；
_ZJ_API int ZJ_SetPTZSpeedAbility(int iPTZSpeedSetAbility);

//设置外挂云台PTZ能力。
_ZJ_API int ZJ_SetAttachPTZAbility(int iAttachPTZAbility);

//设置wdr宽动态能力
_ZJ_API int ZJ_SetWdrAbility(int iWdrAbility);

//设置Gat1400能力
_ZJ_API int ZJ_SetGat1400Ability(int iGAT1400Ability);

//设置外接音柱能力
_ZJ_API int ZJ_SetExternalSpeakerAbility(unsigned int uiExternalSpeakerAbility);

//设置报警声音自定义能力，0.不支持，1.支持
_ZJ_API int ZJ_SetCustomizeAlarmSoundAbility(unsigned int uiCustimizeAbility);

//设置画面翻转回调接口， 通过该回调接口通知上层做画面翻转
_ZJ_API int ZJ_SetImageInversionCB(ZJ_PFUN_IMAGEINVERSION pfunImageInversion);

//设置红外灯开关控制回调接口， 通过该回调接口通知上层做红外灯开关控制
_ZJ_API int ZJ_SetIRLedSwitchCB(ZJ_PFUN_IRLED_SWITCH pfunIRLedSwitch);

//设置摄像头打开/关闭回调接口， 通过该回调接口通知上层关闭视频采集：停止侦测、停止编码、停止采集、停止采集音频、停止声音侦测；
_ZJ_API int ZJ_SetCameraSwitchCB(ZJ_PFUN_CAMERA_SWITCH pfunCameraSwitch);

//设置摄像头PTZ回调接口，通过该回调接口通知上层完成对应PTZ操作
_ZJ_API int ZJ_SetCameraPTZCB(ZJ_PFUN_CAMERA_ONPTZ pfunOnPTZ, ZJ_PFUN_CAMERA_PTZ_GETPOINT pfunPTZGetPoint,
                ZJ_PFUN_CAMERA_PTZ_GOTOPOINT pfunPTZGotoPoint, ZJ_PFUN_CAMERA_PTZ_AUTOCHECK pfunPTZAutoCheck,
                ZJ_PFUN_CAMERA_CRUISE_START pfunCruiseStart,ZJ_PFUN_CAMERA_PTZ_STOP pfunPtzStop);

_ZJ_API int ZJ_SetGetCameraPTZStatusCB(ZJ_PFUN_CAMERA_PTZ_STATUS pfunPtzStatus);

//摄像机ptz智能巡航回调接口,通过该接口通知上层开始智能巡航
_ZJ_API int ZJ_SetCameraSmartPtzCB(ZJ_PFUN_CAMERA_SMARTCRUISE_START pfunSmartCruiseStart);

//设置录像状态回调接口 通过该接口可以在上层做相应的提示
_ZJ_API int ZJ_SetRecordStatusCB(ZJ_PFUN_CAMERA_RECORD_STATUS pfunRecordStatus);

// 强拆设备触发报警开关
_ZJ_API int ZJ_SetDismantableAlarmSwitchCB(ZJ_PFUN_SET_DISMANTABLE_ALARM pfunSetDismantableAlarm);

// 逗留报警开关
_ZJ_API int ZJ_SetStayAlarmSwitchCB(ZJ_PFUN_SET_STAY_ALARM pfunSetStayAlarm);


//设置摄像机当前状态； 1.打开/0.关闭； 对于摄像机上层有物理开关按钮，设置摄像机关闭、打开状态；
_ZJ_API int ZJ_SetCameraStatus(int iStatus);

//开始摄像机录像； 设备有触发开始录像和停止录像的接口，触发后开始录制，传入录制码流序号，设置序号0为主码流，1为次码流，当多种触发录制时，以主码流录制优先；
_ZJ_API int ZJ_StartRecord(int iStreamID);

//停止当前录像；设备上层关闭录像，SDK收到该消息关闭所有录像。
_ZJ_API int ZJ_StopRecord();

//设置摄像头自定义水印展示模式回调
_ZJ_API int ZJ_SetCustomOSDModeCB(ZJ_PFUN_CAMERA_CUSTOMOSDMODESETTING pfunCustomOSDModeSetting);

//设置摄像机水印设置回调接口；
_ZJ_API int ZJ_SetOSDSettingCB(ZJ_PFUN_CAMERA_OSDSETTING pfunOSDSetting);

//设置是否显示Timeosd
_ZJ_API int ZJ_SetShowTimeOSDCB(ZJ_PFUN_CAMERA_CTRLOSDSHOWFLAG pfunCtrlTimeOSD,ZJ_PFUN_CAMERA_CTRLOSDSHOWFLAG pFunCtrlCustomOsd);


//设置Cache存储路径接口；pcStoragePath：存储路径，路径后不带'/'或'\'符号，为全路径；若设置路径为空表示没有SD卡；
_ZJ_API int ZJ_SetStoragePath(char* pcStoragePath);

//设置卡不可用
_ZJ_API int ZJ_SetSDCardErr();

//设置声音文件路径 
_ZJ_API int ZJ_SetDevSoudFilePath(char* pcStoragePath);

//设置SD卡操作回调接口；
_ZJ_API int ZJ_SetSDCardCB(ZJ_PFUN_CAMERA_FORMATSDCARD pfunFormatSDCard, ZJ_PFUN_CAMERA_GETSDCARDSIZE pfunGetSDCardInfo, ZJ_PFUN_CAMERA_CHECKSDCARD pfunCheckSDCard);

//设置多目摄像头能力
_ZJ_API int ZJ_SetCameraMultiFocalLenthAbility(int iMultiFocalLenthAbility);

//设置最大通道数（NVR、多目摄像头）共用
_ZJ_API int ZJ_SetMaxChnNum(int iMaxChnNum);

/***
 *** 以下为双向语音相关函数
 ****/
//语音对讲接收回调接口；告诉设备有一个媒体流传入,如果当前喇叭没有在播，则返回可播放，设备通过接口获取数据播放。
_ZJ_API int ZJ_SetMediaToPlayCB(ZJ_PFUN_CAMERA_MEDIA_TOPLAY pfunMediaToPlay);

//通过句柄获取音频参数
_ZJ_API int ZJ_GetMediaAudioParam(ZJ_HANDLE hHandle, ST_ZJ_AUDIO_PARAM* pstAudioParam);

//通过句柄，获取音频数据,语音长度BUFFER，数据长度，时间戳
_ZJ_API int ZJ_GtMediaAudioData(ZJ_HANDLE hHandle, unsigned char** ppucDataBuf, int* iDataLen, unsigned int* puiTimestamp);
/***
 *** 以下为音频采集编码相关函数
 ****/
//设置音频编码参数
_ZJ_API int ZJ_SetAudioEncParm(ST_ZJ_AUDIO_PARAM* pstAudioParam);

//设置音频编码参数 回调接口
_ZJ_API int ZJ_SetAudioEncParamCB(ZJ_PFUN_SET_AUDIO_PARM pfunSetAudioParm);

//写音频数据
_ZJ_API int ZJ_Audio_WriteFrame(unsigned char* pucFrame, int iLen, unsigned int uiTimestamp);

//重新设置音频编码参数
_ZJ_API int ZJ_Audio_ResetParam(ST_ZJ_AUDIO_PARAM* pstAudioParam);

//设置音频编码开关回调接口  通过该接口控制音频是否编码
_ZJ_API int ZJ_SetAudioEncSwitchCB(ZJ_PFUN_AUDIO_SWITCH pfunAudioSwitch);

//设置音频采集音量大小接口 通过该接口调节音量
_ZJ_API int ZJ_SetAudioVolumnCB(ZJ_PFUN_AUDIO_VOLUMN_ADJUST pfunAudioVolumnAdjust);

//设置摄像机声音音量，当前值
_ZJ_API int ZJ_SetAudioVolumn(int iVolumn);
// 获取和设置声音开关
_ZJ_API int ZJ_SetCamerMicOpenFlag(int iOpenFlag);

_ZJ_API int ZJ_GetCameraMicOpenFlag();

//声音侦测根据灵敏度设置
/*****************************************************************
以下为视频采集编码相关函数
 ****************************************************************/
//设置码流 广角镜头校正参数；当镜头为广角镜头时用
_ZJ_API int ZJ_SetStreamWideAngleLensInfo(int iStreamID, ST_ZJ_VIDEO_CIRCLE* pstCircleInfo);
_ZJ_API int ZJ_SetStreamWideAngleLensDistortion(int iStreamID, ST_ZJ_VIDEO_DISTORTION* pstDistortionInfo);

//设置码流 视频编码参数；
_ZJ_API int ZJ_SetVideoEncParam(int iStreamID, ST_ZJ_VIDEO_PARAM* pstVideoParam);

//码流写入接口，以Frame一帧一帧数据来写
_ZJ_API int ZJ_Video_WriteFrame(int iStreamID, unsigned char* pucFrame, int iLen, unsigned int uiTimestamp, unsigned int uiFrameType);

//码流写入接口，以NAL格式包的数据来写
_ZJ_API int ZJ_Video_WriteNalFrame(int iSteramID, unsigned char* ptNal[], int iNalLen[], int iNalNum, unsigned int uiTimestamp, unsigned int iFrameType);

//重置编码参数，一般是改变了采集分辨率和编码方法
_ZJ_API int ZJ_Video_ResetParam(int iStreamID, ST_ZJ_VIDEO_PARAM* pstVideoParam);

//设置视频编码开关回调接口 通过该接口控制视频是否编码
_ZJ_API int ZJ_SetVideoEncSwitchCB(ZJ_PFUN_VIDEO_SWITCH pfunVideoSwitch);

//设置视频编码参数
_ZJ_API int ZJ_SetVideoEncParamCB(ZJ_PFUN_SET_VIDEO_PARM pfunSetVideoParm);

//设置需要一个编辑一个I帧回调接口 通过该接口通知编码器编一个I帧写下来
_ZJ_API int ZJ_SetVideoNeedIFrameCB(ZJ_PFUN_VIDEO_NEEDIFRAME pfunVideoNeedIFrame);


//设置获取图片回调接口
_ZJ_API int ZJ_Video_SetGetJpegCB(ZJ_PFUN_VIDEO_GETJPEG pfunVideoGetJpeg);

/*****************************************************************
******************************************************************/
//切换前置摄像头
_ZJ_API int ZJ_SetSwitchLenCB(ZJ_PFUN_SWITCH_LEN pfunSwitchLen);

//摄像机宽动态开关
_ZJ_API int ZJ_SetCamWdrOpenFlagCB(ZJ_PFUN_SET_WIDE_DYNAMIC_CAM pfunSetWideDynamicCam);

/****************************************************************************************
自定义声音报警
*****************************************************************************************/
_ZJ_API int ZJ_SetCamAlarmSoundFileCB(ZJ_PFUN_DELSOUNDFILE  pFunDelSoundFile,ZJ_PFUN_GETSOUNDFILES  pFunGetSoudFiles);

//上报门铃状态； 网关门铃：0 外挂机不可用, 1 外挂机可用
_ZJ_API int ZJ_SetDevCanUseStatus(unsigned int uiCanUseStatus);

/****************************************************************************************
GAT1400
*****************************************************************************************/
//摄像机Gat1400回调
_ZJ_API int ZJ_SetGa1400CB(ZJ_PFUN_SET_GA1400SWITCH pfunSetGa1400SwitchCb, ZJ_PFUN_SET_GA1400INFO pFunSetGa1400InfoCb, ZJ_PFUN_SET_GA1400STATUS pFunSetGa1400StatusCb);

/***************************************************************
AI
***************************************************************/
_ZJ_API int ZJ_SetDevAiFaceAbility(int iAiFaceAbility);

_ZJ_API int ZJ_SetAiMaxPicNum(int iAiMaxPicNum);

_ZJ_API int ZJ_SetAiCurPicNum(int iAiCurPicNum);

// 设置布控人脸图片缓存路径
_ZJ_API int ZJ_SetFaceFileCachePath(unsigned char *pucFaceCachePath);

_ZJ_API int ZJ_SetDevAiMaskDiscernAbility(int iAiMaskDiscernAbility);

_ZJ_API int ZJ_SetDevAiBatteryBikeAbility(int iAiBatteryBikeAbility);

_ZJ_API int ZJ_SetDevAiHighParabolicAbility(int iAiHighParabolicAbility);

_ZJ_API int ZJ_SetDevAiFlameAlarmAbility(int iAiFlameAlarmAbility);

// AI通用能力   Ai名称 / 0.不支持；1.支持
_ZJ_API int ZJ_SetDevAiCommonAbility(char *pcAiName, int iAiAbility);
/************************************************************************
 * 人流量统计
 * *********************************************************************/
//人流量统计能力支持  0.不支持  1.支持；
_ZJ_API int ZJ_SetHumCountAbility(unsigned int uiHumCountAbility);

//回调注册
_ZJ_API int ZJ_SetHumanCountFuncsCB(ZJ_PFUN_SETHUMANCOUNT_PARAM pfunSetHumCountParam,ZJ_PFUN_SETHUMANCOUNT_REGIONS pfunSetHumCountRegions);

//统计人流量数据上报
// uiHumInCntNum 进入人数  uiHumOutCntNum 出来人数 pucStartDate:yyyyMMddHHmm
_ZJ_API int ZJ_SetHumanCountNum(unsigned int uiHumInNum,unsigned int uiHumOutNum,unsigned char *pucStartDate,unsigned char *pucEndDate);

//AI开关返回错误支持能力
_ZJ_API int ZJ_SetAISwitchReturnErrAbility(int iReturnErrAbility);

/************************************************************************
**小喇叭
************************************************************************/
//注册回调
_ZJ_API int ZJ_SetSmallSpeakerCBFuncs(ZJ_PFUN_START_CUSTOM_AUDIO pfun_StartDownData,
                                    ZJ_PFUN_CUSTOM_AUDIOD_TRANS pfun_DataTrans,
                                    ZJ_PFUN_STOP_CUSTOM_AUDIO pfun_StopDownData,
                                    ZJ_PFUN_BROADCAST_CUSTOM_STRING pfun_BroadCastCustomStr);


/************************************************************************
**file download
************************************************************************/
_ZJ_API int ZJ_SetDownLoadFileCBFuncs(ZJ_PFUN_FILEDOWN_NOTICE pfunDownDataNtc,ZJ_PFUN_FILEDOWN_TRANS pfunDownDataTrans,ZJ_PFUN_FILEDOWN_STOP pfunDownDataStop);

#ifdef __cplusplus
}
#endif

#endif
