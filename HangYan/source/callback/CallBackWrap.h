//
// Created by xiong on 2021/4/6.
//

#ifndef EAPIL_OVDCallBackWrap_HPP
#define EAPIL_OVDCallBackWrap_HPP

#include <OVD_OpenAPI.h>

#ifdef __cplusplus
extern "C" {
#endif

OVD_int32 GetOVDDeviceInfo(OVDDeviceInfo *deviceInfo);
OVD_int32 GetOVDConfigureInfo(OVD_char **output_ovdconfig, OVD_int32 *output_size);
#ifdef OVDSDK1_38_1
OVD_int32 SetOVDConfigureInfo(OVD_char *in_ovdconfig, OUT OVD_char *errMsg, IN OVD_int32 errMsg_len);
OVD_int32 ReBootDevice(ovd_reboot_reason_e reason);
OVD_int32 ForceIFrame(OVD_int32 channel, OVDCodeStream code_stream);
OVD_int32 SyncMacHash(OVD_char* in_machash);
#else
OVD_int32 SetOVDConfigureInfo(OVD_char *in_ovdconfig);
OVD_int32 ReBootDevice();
OVD_int32 ForceIFrame(OVD_int32 channel);
#endif
OVD_void OVCConnectStatus(OVD_int32 connectStatus);
OVD_int32 ReBootChannel(OVD_int32 channel);

OVD_int32 KeepAwakenUtilExpired(OVD_int32 channel, OVD_int32 notAllowHibernate,
                                OVD_int32 expired, OVDHibernateReason reason);
OVD_int32 ResetConfiguration();
OVD_int32 QueryRecordPage(OVD_uint32 channelmask1, OVD_uint32 channelmask2,
                          OVD_uint32 recordType, OVD_uint64 StartStamp,
                          OVD_uint64 EndStamp, OVD_int32 Page, OVD_int32 numInPage,
                          OVDRecordFileListPerPage *FilePage);
OVD_int32 OpenRecordFile(OVD_int32 channel, OVD_char *recordname,
                         OVDVideoDataFormat *videoInfo, OVDAudioDataFormat *audioInfo,
                         OVD_int32 *fileTotalTime);
OVD_int32 RecordCotrol(OVD_int32 channel, OVDCONTROLTYPE controlType, OVD_int32 value);
OVD_int32 DMEAPI_callback_RecordSearch(OVD_int32 channel, OVD_uint64 starttime, OVD_uint64 endtime, OVD_int32 page,
                                       OVD_int32 numInPage, OVD_DMERecordFileListPerPage *fileinpage);
OVD_void *DMEAPI_callback_RecordOpen(OVD_int32 channel);
OVD_int32 DMEAPI_callback_RecordSeek(OVD_void *ctx, OVD_int64 timestamp);
OVD_int32 DMEAPI_callback_RecordReadFrame(OVD_void *ctx, OVD_FrameInfo *pframe_info);
OVD_int32 DMEAPI_callback_RecordClose(OVD_void *ctx);
OVD_int32 FirmwareUpgrade(OVD_char *firmware_model, OVD_char *upgradeURL);
OVD_int32 QueryFirmwareUpgradeStatus(OVDUpgradeStatus *upgradeStatus, OVD_int32 *upgradeProgress, OVD_char *version,
                                     OVD_int32 version_len, OVD_char *last_upgrade_time, OVD_int32 time_len);
OVD_int32 SyncTime(OVD_char *datetime, OVD_int32 tz, OVD_int32 offset);
OVD_int32 QueryTime(OVD_char *out_datetime, OVD_int32 time_len, OVD_int32 *tz);
OVD_int32 GetSDInfo(OVDSDInfo *sdInfo);
OVD_int32 SetSDCardFormat();
OVD_int32 PTZCmd(OVD_int32 channel, OVCPTZControlCmd ptzcmd, OVD_int32 ptzvalue);
OVD_int32 GetPresetList(OVD_int32 channel, OVD_int32 *presetList, OVD_int32 array_malloc_num, OVD_int32 *count);
OVD_int32 AudioPlayStart(OVD_int32 channel, OVDAudioOutDataFormat andioFormat);
OVD_int32 AudioPlayProGress(OVD_int32 channel, OVD_uchar *buf, OVD_int32 size);
OVD_int32 AudioPlayStop(OVD_int32 channel);
OVD_int32 VedioSwitchQuality(OVD_int32 channel, OVDEncodeQuality quality, OVDVideoDataFormat *vedioInfo);
OVD_int32 Snapshot(OVD_int32 channel, OVDImageInfo *imageInfo, OVD_int32 maxImageSize);

OVD_int32 gettime(OVD_uint64 *out_time);
OVD_int32 settime(OVD_uint64 input_time, OVD_int32 tolerance_value);
OVD_int32 ProbeByval();
OVD_int32 getsimpleovdinfo(SimpleOVDinfo *out_simpleovdinfo);
OVD_int32 GetDiskInfo(OVD_int32 *out_state, OVD_int32 *out_total, OVD_int32 *out_free);
OVD_int32 LogUploadAsync(OVD_char *trans_id, OVD_char *start, OVD_char *end, OVD_char *url);
OVD_int32 extension_callback(char *method, char *in_data, char *out_response, OVD_int32 in_outresponse_len,
                             OVD_int32 *out_outresponse_len);
#ifdef OVDSDK_APIVER_1_0
OVD_int32 AudioOutPlayCtrl(OVD_int32 channel, OVDMp3PlayCtrl ctrl);
OVD_int32 GetAudioOutPlayStatus(OVD_int32 channel, OVD_int32 *status, OVD_char *out_url);
OVD_int32 syncProbeDetector(OVD_int32 avgRTTThreshold, OVD_int32 checkPeriod, OVD_int32 cpuLoadThreshold,
                            OVD_int32 memoryLoadThreshold, OVD_int32 memoryAvaliable, OVD_int32 packetLossRateThreshold,
                            OVD_int32 reportSwitch, OVD_int32 wifiQualityThreshold);
OVD_int32 TriggerSoftProbe();
OVD_int32 SetAudioOutPlay(OVD_int32 channel, OVD_char *url);
#endif

#ifdef OVDSDK_APIVER_3_0
OVD_int32 StopAlarm(int alarmtype);
OVD_int32 SetAudioOutPlay(OVD_int32 channel, OVD_char *url, int repeat, int volume);
#ifdef OVDSDK1_38_1
OVD_int32 GetDevRunningInfo(ovd_probe_devrunning_info_e in_info, void* out_response);
#else
OVD_int32 GetDevRunningInfo(OVD_GetDevRunningInfo_e in_info, void* out_response);
#endif
#endif

void InitCallBackList(OVD_CallBackFunList *list);

#ifdef __cplusplus
}
#endif
#endif //EAPIL_OVDCallBackWrap_HPP
