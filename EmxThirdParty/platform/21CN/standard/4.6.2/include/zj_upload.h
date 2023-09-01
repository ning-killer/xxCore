#ifndef _ZJ_UPLOAD_H_
#define _ZJ_UPLOAD_H_

#ifdef __cplusplus
extern "C" {
#endif

//获取默认时间
typedef int (*ZJ_PFUN_DEVICE_GETUTCRSP)(int uiReqId,int iCode,long long iGMTSecond);

_ZJ_API int ZJ_SetGetUtcRspCbFun(ZJ_PFUN_DEVICE_GETUTCRSP pFunGetUtcRsp);

_ZJ_API int ZJ_GetUtcTimeFromServer(int uiReqId);

// 请求超时后, 由 使用者 cancle 掉
_ZJ_API int ZJ_CancleOneRequset(int iReqId);

#ifdef __cplusplus
}
#endif

#endif