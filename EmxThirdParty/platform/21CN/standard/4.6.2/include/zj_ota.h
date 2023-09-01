#ifndef _ZJ_OTA_H_
#define _ZJ_OTA_H_

#ifdef __cplusplus
extern "C" {
#endif

// 设置设备升级状态回调函数，通过回调返回升级状态；设备上层收到后，启用升级程序下载升级固件；
_ZJ_API int ZJ_SetOtaCBFuncs(ZJ_PFUN_NEWVERSON_NOTICE pFunNewVersionCb,
    ZJ_PFUN_NEWVERSON_DATADOWN pFunVersonDataDownCb, ZJ_PFUN_STOPUPGRADE pFunStopUpgrade,ZJ_PFUN_COVERIMAGE_NOTICE pFunCoverImageNotice);

// 设备开始升级状态，设备上报平台，进入升级状态； 当返回进入升级中状态后，启动升级进程，开始升级；
_ZJ_API int ZJ_StartUpdate();

// 版本下载更新后, 汇报烧录 进度  uiPercentage:0~99 若是升级失败 ZJ_SetBurnningProgress(0);
_ZJ_API int ZJ_SetBurnningProgress(unsigned int uiPercentage);

//设置OTA升级能力，具备多种能力按位与
_ZJ_API int ZJ_SetOTAAbility(unsigned int uiOTAAbility);

#ifdef __cplusplus
}
#endif

#endif
