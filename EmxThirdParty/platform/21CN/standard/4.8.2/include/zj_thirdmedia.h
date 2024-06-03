#ifndef _ZJ_THIRDMEDIA_H_
#define _ZJ_THIRDMEDIA_H_
#include <time.h>
#ifdef __cplusplus
extern "C" {
#endif
#include "zj_type.h"


/*******************************************************************
在线升级新SDK时使用，三方迁移
*******************************************************************/
typedef ZJ_HANDLE (*ZJ_PFUN_OPENFILE)(unsigned char *pucFilePath,unsigned int *puiMaxFrameSize);

typedef int (*ZJ_PFUN_CLOSEFILE)(ZJ_HANDLE hHandle);

typedef int (*ZJ_PFUN_THIRDREADFRAME)(ZJ_HANDLE hHandle, unsigned char *pucFrame,unsigned int *puiFrameLen, unsigned char *pucAVFlag, int *pIFrame, unsigned int *puiTimeStamp);

typedef int (*ZJ_PFUN_SEEKBYTIMESTAMP)(ZJ_HANDLE hHandle, unsigned int uiSeekTime, unsigned int *puiSeekTimeStamp);

typedef int (*ZJ_PFUN_READFILEDES)(ZJ_HANDLE hHandle,ST_ZJ_VIDEO_PARAM *pstVideoParm,ST_ZJ_VIDEO_CIRCLE *pstCircle,ST_ZJ_AUDIO_PARAM *pstAudioParm);

typedef int (*ZJ_PFUN_DELETEFILE)(unsigned char *pucFilePath);

//第三方厂商打开录像文件
_ZJ_API int ZJ_ThirdOpenFile(ZJ_PFUN_OPENFILE pfunOpenFile);

//第三方厂商关闭录像文件
_ZJ_API int ZJ_ThirdCloseFile(ZJ_PFUN_CLOSEFILE pfunCloseFile);

//读取第三方厂商录像一帧数据
_ZJ_API int ZJ_ThirdReadData(ZJ_PFUN_THIRDREADFRAME pfunReadFrame);

//跳转第三方录像时间
_ZJ_API int ZJ_ThirdSeekTime(ZJ_PFUN_SEEKBYTIMESTAMP pfunSeekTime);

//获取第三方录像音视频参数
_ZJ_API int ZJ_ThirdGetFileDes(ZJ_PFUN_READFILEDES pfunGetFileDes);

//第三方厂商删除文件
_ZJ_API int ZJ_ThirdDeleteFile(ZJ_PFUN_DELETEFILE pfunDeleteFile);

//第三方厂商添加录像日期
_ZJ_API int ZJ_RdAddCompatibleDate(unsigned char *pucDate,int iCamId);

//第三方厂商添加录像文件
_ZJ_API int ZJ_RdAddCompatibleFile(unsigned char *pucDate,int iCamId,unsigned char *pucFilePath,unsigned int uiStartUnixTime,unsigned int uiEndUnixTime,unsigned int uiStartMsTime,unsigned int uiEndMsTime);

//第三方厂商添加事件日期
_ZJ_API int ZJ_EventAddCompatibleDate(unsigned char *pucDate);

//第三方厂商添加事件
_ZJ_API int ZJ_EventAddCompatibleFile(unsigned char *pucDate,time_t cEventTime,unsigned int uiDuration,unsigned int uiIotType,unsigned int uiEventId);


#ifdef __cplusplus
}
#endif


#endif
