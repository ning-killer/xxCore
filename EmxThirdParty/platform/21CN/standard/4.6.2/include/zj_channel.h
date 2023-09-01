#ifndef _ZJ_CHANNEL_H_
#define _ZJ_CHANNEL_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * 发送自定义消息指令；
 */
_ZJ_API int ZJ_SendCustomData(const unsigned char* pucPeerID, unsigned char* pucData, int iLen);

_ZJ_API int ZJ_SetRecvCustomDataFunc(ZJ_PFUN_DEVICE_RECVCUSTOMDATA pFuncOnRecvCustomData);

/**
 * ptz转动到底了，发送此消息     ，Flag =  1 转到底
 */ 
_ZJ_API int ZJ_SendPTZtoEndFlag(unsigned int uiEndFlag);

#ifdef __cplusplus
}
#endif

#endif