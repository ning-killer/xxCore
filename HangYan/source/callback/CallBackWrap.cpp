//
// Created by xiong on 2021/4/6.
//
#include "Ovd.hpp"
#include "CallBackWrap.h"
#include "OvdUtils.hpp"
#include "SDCardClient.hpp"

using namespace Emx;

void InitCallBackList(OVD_CallBackFunList *list) {
    list->OVD_GetOVDDeviceInfo = GetOVDDeviceInfo;
    list->OVD_GetOVDConfigureInfo = GetOVDConfigureInfo;
    list->OVC_SetOVDConfigureInfo = SetOVDConfigureInfo;
    list->OVD_OVCConnectStatus = OVCConnectStatus;
    list->OVD_ReBootChannel = ReBootChannel;
    list->OVD_ReBootDevice = ReBootDevice;
    list->OVD_KeepAwakenUtilExpired = KeepAwakenUtilExpired;
    list->OVD_ResetConfiguration = ResetConfiguration;
    list->OVD_QueryRecordPage = QueryRecordPage;
    list->OVD_OpenRecordFile = OpenRecordFile;
    list->OVD_RecordCotrol = RecordCotrol;
    list->OVD_DMEAPI_callback_RecordSearch = DMEAPI_callback_RecordSearch;
    list->OVD_DMEAPI_callback_RecordOpen = DMEAPI_callback_RecordOpen;
    list->OVD_DMEAPI_callback_RecordSeek = DMEAPI_callback_RecordSeek;
    list->OVD_DMEAPI_callback_RecordReadFrame = DMEAPI_callback_RecordReadFrame;
    list->OVD_DMEAPI_callback_RecordClose = DMEAPI_callback_RecordClose;
    list->OVD_FirmwareUpgrade = FirmwareUpgrade;
    list->OVD_QueryFirmwareUpgradeStatus = QueryFirmwareUpgradeStatus;
    list->OVD_SyncTime = SyncTime;
    list->OVD_QueryTime = QueryTime;
    list->OVD_GetSDInfo = GetSDInfo;
    list->OVD_SetSDCardFormat = SetSDCardFormat;
    list->OVD_PTZCmd = PTZCmd;
    list->OVD_GetPresetList = GetPresetList;
    list->OVD_AudioPlayStart = AudioPlayStart;
    list->OVD_AudioPlayProGress = AudioPlayProGress;
    list->OVD_AudioPlayStop = AudioPlayStop;
    list->OVD_VedioSwitchQuality = VedioSwitchQuality;
    list->OVD_ForceIFrame = ForceIFrame;
    list->OVD_Snapshot = Snapshot;
    list->OVD_gettime = gettime;
    list->OVD_settime = settime;
    list->OVD_ProbeByval = ProbeByval;
    list->OVD_getsimpleovdinfo = getsimpleovdinfo;
    list->OVD_GetDiskInfo = GetDiskInfo;
    list->OVD_LogUploadAsync = LogUploadAsync;
    list->OVD_extension_callback = extension_callback;
#ifdef OVDSDK_APIVER_1_0
    list->OVD_AudioOutPlayCtrl = AudioOutPlayCtrl;
    list->OVD_GetAudioOutPlayStatus = GetAudioOutPlayStatus;
    list->OVD_syncProbeDetector = syncProbeDetector;
    list->OVD_TriggerSoftProbe = TriggerSoftProbe;
    list->OVD_SetAudioOutPlay = SetAudioOutPlay;
#elif defined OVDSDK_API_VER2_0
#elif defined OVDSDK_APIVER_3_0
    list->OVD_StopAlarm = StopAlarm;
	list->OVD_GetDevRunningInfo = GetDevRunningInfo;
    list->OVD_SetAudioOutPlay = SetAudioOutPlay;
#else
#endif
#ifdef OVDSDK1_38_1
    list->OVD_syncmachash = SyncMacHash;
#endif // OVDSDK1_38_1
}

static ThreadInvoke::Req gOvdReq(OVD_CALL_BACK_RPC_ID);

OVD_int32 GetOVDDeviceInfo(OVDDeviceInfo *deviceInfo) {
    emxlogd("into\n");
    TimeRec timeCost;
    int ret = -1;
    void *buffer[] = {
            &ret,
            &deviceInfo};
    memset(deviceInfo, 0, sizeof(OVDDeviceInfo));
    if (NetMainDev::IsSupported(Net::DevE::Wifi)) {
        NetClientWifi wifi;
        NetClientWifi::Info info = {};
        wifi.GetInfo(info);
        strncpy(deviceInfo->wifi_ssid, info.ssid, sizeof(deviceInfo->wifi_ssid));
        deviceInfo->wifi_signal = info.signalLevel;
    }
    deviceInfo->upBandwidth = -1;
    deviceInfo->downBandwidth = -1;
    Net::DevE dev;
    NetMainDev::Get(dev);
    Net::Addr addr = {};
    NetClient net(dev);
    net.GetAddr(addr);
    Net::LinkStat link = {};
    net.GetLinkStat(link);
    if (link.internet4)
        strncpy(deviceInfo->ipAddr, addr.ip4, sizeof(deviceInfo->ipAddr));
    else if (link.internet6)
        strncpy(deviceInfo->ipAddr, addr.ip6, sizeof(deviceInfo->ipAddr));
    else if (link.lan4)
        strncpy(deviceInfo->ipAddr, addr.ip4, sizeof(deviceInfo->ipAddr));
    else if (link.lan6)
        strncpy(deviceInfo->ipAddr, addr.ip6, sizeof(deviceInfo->ipAddr));
    strncpy(deviceInfo->macAddr, addr.mac, sizeof(deviceInfo->macAddr));
    Misc::SysUsage usage = {};
    Misc::GetSysUsage(usage, 100);
    deviceInfo->cpuLoad = usage.cpu;
    deviceInfo->memoryTotal = usage.memTotal;
    deviceInfo->memoryAvailable = usage.memFree;
//    printf("%s:%s:%d:cpuLoad=%d,memoryTotal=%d,memoryAvailable=%d\n",
//           __FILE__, __FUNCTION__, __LINE__,
//           deviceInfo->cpuLoad,
//           deviceInfo->memoryTotal,
//           deviceInfo->memoryAvailable);
    ThreadInvoke::Packet packet((uint32_t) OvdCallBackIdE::GetOVDDeviceInfo, buffer);
    gOvdReq.Send(packet);
    emxlogd("leave cost:%ums\n", timeCost.GetInv());
    return ret;
}

OVD_int32 GetOVDConfigureInfo(OVD_char **output_ovdconfig, OVD_int32 *output_size) {
    emxlogd("into\n");
    TimeRec timeCost;
    int ret = -1;
    void *buffer[] = {
            &ret,
            &output_ovdconfig,
            &output_size
    };
    ThreadInvoke::Packet packet((uint32_t) OvdCallBackIdE::GetOVDConfigureInfo, buffer);
    gOvdReq.Send(packet);
    emxlogd("leave cost:%ums\n", timeCost.GetInv());
    return ret;
}

#ifdef OVDSDK1_38_1
OVD_int32 SetOVDConfigureInfo(OVD_char *in_ovdconfig, OUT OVD_char *errMsg, IN OVD_int32 errMsg_len) {
    emxlogd("into\n");
    TimeRec timeCost;
    int ret = -1;
    void *buffer[] = {
            &ret,
            &in_ovdconfig
    };
    ThreadInvoke::Packet packet((uint32_t) OvdCallBackIdE::SetOVDConfigureInfo, buffer);
    gOvdReq.Send(packet);
    emxlogd("leave cost:%ums\n", timeCost.GetInv());
    return ret;
}

OVD_int32 ReBootDevice(ovd_reboot_reason_e reason) {
    emxlogi("into\n");
    TimeRec timeCost;
    int ret = -1;
    void *buffer[] = {
            &ret,
            &reason
    };
    ThreadInvoke::Packet packet((uint32_t) OvdCallBackIdE::ReBootDevice, buffer);
    gOvdReq.Send(packet);
    Reboot::DoReboot(5);
    emxlogd("leave cost:%ums\n", timeCost.GetInv());
    return 0;
}
#else
OVD_int32 SetOVDConfigureInfo(OVD_char *in_ovdconfig) {
    emxlogd("into\n");
    TimeRec timeCost;
    int ret = -1;
    void *buffer[] = {
            &ret,
            &in_ovdconfig
    };
    ThreadInvoke::Packet packet((uint32_t) OvdCallBackIdE::SetOVDConfigureInfo, buffer);
    gOvdReq.Send(packet);
    emxlogd("leave cost:%ums\n", timeCost.GetInv());
    return ret;
}

OVD_int32 ReBootDevice() {
    emxlogi("into\n");
    TimeRec timeCost;
    Reboot::DoReboot(5);
    emxlogd("leave cost:%ums\n", timeCost.GetInv());
    return 0;
}
#endif

OVD_void OVCConnectStatus(OVD_int32 connectStatus) {
    emxlogi("into:connectStatus = %d\n", connectStatus);
    TimeRec timeCost;
    int ret = -1;
    void *buffer[] = {
            &ret,
            &connectStatus};
    ThreadInvoke::Packet packet((uint32_t) OvdCallBackIdE::OVCConnectStatus, buffer);
    gOvdReq.Send(packet);
    emxlogd("leave cost:%ums\n", timeCost.GetInv());
}

OVD_int32 ReBootChannel(OVD_int32 channel) {
    emxlogi("into\n");
    TimeRec timeCost;
    Reboot::DoReboot(5);
    emxlogd("leave cost:%ums\n", timeCost.GetInv());
    return 0;
}

OVD_int32 KeepAwakenUtilExpired(OVD_int32 channel, OVD_int32 notAllowHibernate,
                                OVD_int32 expired, OVDHibernateReason reason) {
    emxlogd("into\n");
    TimeRec timeCost;
    int ret = -1;
    void *buffer[] = {
            &ret,
            &channel,
            &notAllowHibernate,
            &expired,
            &reason,
    };

    ThreadInvoke::Packet packet((uint32_t) OvdCallBackIdE::KeepAwakenUtilExpired, buffer);
    gOvdReq.Send(packet);
    emxlogd("leave cost:%ums\n", timeCost.GetInv());
    return ret;
}

OVD_int32 ResetConfiguration() {
    emxlogd("into\n");
    TimeRec timeCost;
    int ret = -1;
    void *buffer[] = { &ret };
    ThreadInvoke::Packet packet((uint32_t) OvdCallBackIdE::ResetConfiguration, buffer);
    gOvdReq.Send(packet);
    if (Param::ResetAllToFactory() == ErrCodeE::Success)
        ret = 0;
    else
        ret = -1;
    emxlogd("leave cost:%ums\n", timeCost.GetInv());
    return ret;
}

OVD_int32 QueryRecordPage(OVD_uint32 channelmask1, OVD_uint32 channelmask2,
                          OVD_uint32 recordType, OVD_uint64 StartStamp,
                          OVD_uint64 EndStamp, OVD_int32 Page, OVD_int32 numInPage,
                          OVDRecordFileListPerPage *FilePage) {
    emxlogd("into\n");
    TimeRec timeCost;
    int ret = -1;
    void *buffer[] = {
            &ret,
            &channelmask1,
            &channelmask2,
            &recordType,
            &StartStamp,
            &EndStamp,
            &Page,
            &numInPage,
            &FilePage,
    };

    ThreadInvoke::Packet packet((uint32_t) OvdCallBackIdE::QueryRecordPage, buffer);
    gOvdReq.Send(packet);
    emxlogd("leave cost:%ums\n", timeCost.GetInv());
    return ret;
}

OVD_int32 OpenRecordFile(OVD_int32 channel, OVD_char *recordname,
                         OVDVideoDataFormat *videoInfo, OVDAudioDataFormat *audioInfo,
                         OVD_int32 *fileTotalTime) {
    emxlogd("into\n");
    TimeRec timeCost;
    int ret = -1;
    void *buffer[] = {
            &ret,
            &channel,
            &recordname,
            &videoInfo,
            &audioInfo,
            &fileTotalTime,
    };
    ThreadInvoke::Packet packet((uint32_t) OvdCallBackIdE::OpenRecordFile, buffer);
    gOvdReq.Send(packet);
    emxlogd("leave cost:%ums\n", timeCost.GetInv());
    return ret;
}

OVD_int32 RecordCotrol(OVD_int32 channel, OVDCONTROLTYPE controlType, OVD_int32 value) {
    emxlogd("into\n");
    TimeRec timeCost;
    int ret = -1;
    void *buffer[] = {
            &ret,
            &channel,
            &controlType,
            &value,
    };
    ThreadInvoke::Packet packet((uint32_t) OvdCallBackIdE::RecordCotrol, buffer);
    gOvdReq.Send(packet);
    emxlogd("leave cost:%ums\n", timeCost.GetInv());
    return ret;
}

OVD_int32 DMEAPI_callback_RecordSearch(OVD_int32 channel, OVD_uint64 starttime, OVD_uint64 endtime, OVD_int32 page,
                                       OVD_int32 numInPage, OVD_DMERecordFileListPerPage *fileinpage) {
    emxlogd("into\n");
    TimeRec timeCost;
    char buf1[64] = {0};
    char buf2[64] = {0};
    emxlogi("channel:%d, [%llu-%llu]:[%s-%s], page:%d, numInPage:%d\n",
           channel, starttime, endtime,
           Time::GetTimeStr((uint32_t) starttime, buf1, sizeof(buf1)),
           Time::GetTimeStr((uint32_t) endtime, buf2, sizeof(buf2)),
           page, numInPage);
    int ret = -1;
    void *buffer[] = {
            &ret,
            &channel,
            &starttime,
            &endtime,
            &page,
            &numInPage,
            &fileinpage,
    };

    ThreadInvoke::Packet packet((uint32_t) OvdCallBackIdE::DMEAPI_callback_RecordSearch, buffer);
    gOvdReq.Send(packet);
    emxlogd("leave cost:%ums\n", timeCost.GetInv());
    return ret;
}

OVD_void *DMEAPI_callback_RecordOpen(OVD_int32 channel) {
    emxlogd("into\n");
    TimeRec timeCost;
    OVD_void *ret;
    void *buffer[] = {
            &ret,
            &channel,
    };
    ThreadInvoke::Packet packet((uint32_t) OvdCallBackIdE::DMEAPI_callback_RecordOpen, buffer);
    gOvdReq.Send(packet);
    emxlogd("leave cost:%ums\n", timeCost.GetInv());
    return ret;
}

OVD_int32 DMEAPI_callback_RecordSeek(OVD_void *ctx, OVD_int64 timestamp) {
    emxlogd("into\n");
    TimeRec timeCost;
    int ret = -1;
    void *buffer[] = {
            &ret,
            &ctx,
            &timestamp,
    };
    ThreadInvoke::Packet packet((uint32_t) OvdCallBackIdE::DMEAPI_callback_RecordSeek, buffer);
    gOvdReq.Send(packet);
    emxlogd("leave cost:%ums\n", timeCost.GetInv());
    return ret;
}

OVD_int32 DMEAPI_callback_RecordReadFrame(OVD_void *ctx, OVD_FrameInfo *pframe_info) {
    // emxlogd("into\n");
    TimeRec timeCost;
    int ret = -1;
    void *buffer[] = {
            &ret,
            &ctx,
            &pframe_info,
    };
    ThreadInvoke::Packet packet((uint32_t) OvdCallBackIdE::DMEAPI_callback_RecordReadFrame, buffer);
    gOvdReq.Send(packet);
    // emxlogd("leave cost:%ums\n", timeCost.GetInv());
    return ret;
}

OVD_int32 DMEAPI_callback_RecordClose(OVD_void *ctx) {
    emxlogd("into\n");
    TimeRec timeCost;
    int ret = -1;
    void *buffer[] = {
            &ret,
            &ctx,
    };
    ThreadInvoke::Packet packet((uint32_t) OvdCallBackIdE::DMEAPI_callback_RecordClose, buffer);
    gOvdReq.Send(packet);
    emxlogd("leave cost:%ums\n", timeCost.GetInv());
    return ret;
}

OVD_int32 FirmwareUpgrade(OVD_char *firmware_model, OVD_char *upgradeURL) {
    emxlogd("into version[%s],url=[%s]\n", firmware_model, upgradeURL);
    TimeRec timeCost;
    int ret = -1;
    void *buffer[] = {
            &ret,
            &firmware_model,
            &upgradeURL,
    };
    ThreadInvoke::Packet packet((uint32_t) OvdCallBackIdE::FirmwareUpgrade, buffer);
    gOvdReq.Send(packet);
    emxlogd("leave cost:%ums\n", timeCost.GetInv());
    return ret;
}

OVD_int32 QueryFirmwareUpgradeStatus(OVDUpgradeStatus *upgradeStatus, OVD_int32 *upgradeProgress, OVD_char *version,
                                     OVD_int32 version_len, OVD_char *last_upgrade_time, OVD_int32 time_len) {
    emxlogd("into\n");
    TimeRec timeCost;
    int ret = -1;
    void *buffer[] = {
            &ret,
            &upgradeStatus,
            &upgradeProgress,
            &version,
            &version_len,
            &last_upgrade_time,
            &time_len,
    };
    ThreadInvoke::Packet packet((uint32_t) OvdCallBackIdE::QueryFirmwareUpgradeStatus, buffer);
    gOvdReq.Send(packet);
    emxlogd("leave cost:%ums\n", timeCost.GetInv());
    return ret;
}

OVD_int32 SyncTime(OVD_char *datetime, OVD_int32 tz, OVD_int32 offset) {
    emxlogd("into\n");
    TimeRec timeCost;
    int ret = -1;
    void *buffer[] = {
            &ret,
            &datetime,
            &tz,
            &offset,
    };
    ThreadInvoke::Packet packet((uint32_t) OvdCallBackIdE::SyncTime, buffer);
    gOvdReq.Send(packet);
    emxlogd("leave cost:%ums\n", timeCost.GetInv());
    return ret;
}

OVD_int32 QueryTime(OVD_char *out_datetime, OVD_int32 time_len, OVD_int32 *tz) {
    emxlogd("into\n");
    TimeRec timeCost;
    int ret = -1;
    void *buffer[] = {
            &ret,
            &out_datetime,
            &time_len,
            &tz,
    };
    ThreadInvoke::Packet packet((uint32_t) OvdCallBackIdE::QueryTime, buffer);
    gOvdReq.Send(packet);
    emxlogd("leave cost:%ums\n", timeCost.GetInv());
    return ret;
}

OVD_int32 GetSDInfo(OVDSDInfo *sdInfo) {
    emxlogd("into\n");
    TimeRec timeCost;
    int ret = -1;
    SDCard::Info info = {};
    SDCardClient sdcard(0);
    if (sdcard.GetInfo(info) == ErrCodeE::Success) {
        ret = 0;
        sdInfo->SDExist = info.stat == SDCard::StatE::Inserted ||
                          info.stat == SDCard::StatE::Normal;
        sdInfo->SDTotalSize = info.total;
        sdInfo->SDFreeSize = info.free;
        // Todo:get the f**ing file name
        //     OVD_char EarlyFileName[MAX_LEN_24]; //当前SD卡最早一个录像文件
        // sdInfo->EarlyFileName
    } else {
        sdInfo->SDExist = 2;
    }
    emxlogd("leave cost:%ums\n", timeCost.GetInv());
    return ret;
}

OVD_int32 SetSDCardFormat() {
    emxlogd("into\n");
    TimeRec timeCost;
    SDCardClient sdcard(0);
    int ret = sdcard.Format() == ErrCodeE::Success ? 0 : -1;
    emxlogd("leave cost:%ums\n", timeCost.GetInv());
    return ret;
}

OVD_int32 PTZCmd(OVD_int32 channel, OVCPTZControlCmd ptzcmd, OVD_int32 ptzvalue) {
    emxlogd("into\n");
    TimeRec timeCost;
    int ret = -1;
    void *buffer[] = {
            &ret,
            &channel,
            &ptzcmd,
            &ptzvalue,
    };
    ThreadInvoke::Packet packet((uint32_t) OvdCallBackIdE::PTZCmd, buffer);
    gOvdReq.Send(packet);
    emxlogd("leave cost:%ums\n", timeCost.GetInv());
    return ret;
}

OVD_int32 GetPresetList(OVD_int32 channel, OVD_int32 *presetList, OVD_int32 array_malloc_num, OVD_int32 *count) {
    emxlogd("into\n");
    TimeRec timeCost;
    int ret = -1;
    void *buffer[] = {
            &ret,
            &channel,
            &array_malloc_num,
            &count,
    };
    ThreadInvoke::Packet packet((uint32_t) OvdCallBackIdE::GetPresetList, buffer);
    gOvdReq.Send(packet);
    emxlogd("leave cost:%ums\n", timeCost.GetInv());
    return ret;
}

OVD_int32 AudioPlayStart(OVD_int32 channel, OVDAudioOutDataFormat andioFormat) {
    emxloge("not implement\n");
    return -1;
}

OVD_int32 AudioPlayProGress(OVD_int32 channel, OVD_uchar *buf, OVD_int32 size) {
    emxloge("not implement\n");
    return -1;
}

OVD_int32 AudioPlayStop(OVD_int32 channel) {
    emxloge("not implement\n");
    return -1;
}

OVD_int32 VedioSwitchQuality(OVD_int32 channel, OVDEncodeQuality quality, OVDVideoDataFormat *vedioInfo) {
    emxlogd("into\n");
    TimeRec timeCost;
    int ret = -1;
    void *buffer[] = {
            &ret,
            &channel,
            &quality,
            &vedioInfo,
    };
    ThreadInvoke::Packet packet((uint32_t) OvdCallBackIdE::VedioSwitchQuality, buffer);
    gOvdReq.Send(packet);
    emxlogd("leave cost:%ums\n", timeCost.GetInv());
    return ret;
}

#ifdef OVDSDK1_38_1
OVD_int32 ForceIFrame(OVD_int32 channel, OVDCodeStream code_stream)
#else
OVD_int32 ForceIFrame(OVD_int32 channel)
#endif
{
    emxlogd("into\n");
    TimeRec timeCost;
    MediaClientVenc venc(0);
    int ret = venc.FlushIDR() == ErrCodeE::Success ? 0 : -1;
    emxlogd("leave cost:%ums\n", timeCost.GetInv());
    return ret;
}

OVD_int32 Snapshot(OVD_int32 channel, OVDImageInfo *imageInfo, OVD_int32 maxImageSize) {
    emxlogd("into\n");
    TimeRec timeCost;
    int ret = -1;
    MediaClientStream::Channel chn;
    chn.Add(MediaFrame::TypeE::Snap, 0);
    MediaClientStreamSync snap;
    if (snap.Open(chn, 3000) != ErrCodeE::Success) {
        emxloge("open snap chn 0 failed\n");
        return -1;
    }
    MediaClientSnap::Trigger(0, 3000);
    MediaFrame frame = {};
    ErrCodeE e = snap.GetFrame(frame, 3000);
    if (e != ErrCodeE::Success) {
        emxloge("get snap frame failed\n");
        ret = -1;
    } else {
        //maxImageSize空间不足，并在OVDImageInfo.size中带回所需要图片大小
        if (frame.size > maxImageSize) {
            emxloge("GetFrameSize(%d); maxImageSize(%d)\n", frame.size, maxImageSize);
            ret = -2;
        } else {
            ret = 0;
            memcpy(imageInfo->buf, frame.data, frame.size);
        }
        imageInfo->size = frame.size;
    }
    snap.Close();
    emxlogd("leave cost:%ums\n", timeCost.GetInv());
    return ret;
}

#ifdef OVDSDK_APIVER_1_0
OVD_int32 SetAudioOutPlay(OVD_int32 channel, OVD_char *url) {
    emxloge("not implement\n");
    return 0;
}
#elif defined OVDSDK_APIVER_3_0
OVD_int32 SetAudioOutPlay(OVD_int32 channel, OVD_char *url, int repeat, int volume) {
    emxlogd("into\n");
    TimeRec timeCost;
    int ret = -1;
    void *buffer[] = {
            &ret,
            &channel,
            &url,
            &repeat,
            &volume
    };
    ThreadInvoke::Packet packet((uint32_t) OvdCallBackIdE::SetAudioOutPlay, buffer);
    gOvdReq.Send(packet);
    emxlogd("leave cost:%ums\n", timeCost.GetInv());
    return ret;
}
#endif 


#ifdef OVDSDK_APIVER_1_0
OVD_int32 AudioOutPlayCtrl(OVD_int32 channel, OVDMp3PlayCtrl ctrl) {
    emxloge("not implement\n");
    return 0;
}
#endif 

#ifdef OVDSDK_APIVER_1_0
OVD_int32 GetAudioOutPlayStatus(OVD_int32 channel, OVD_int32 *status, OVD_char *out_url) {
    emxloge("not implement\n");
    return 0;
}
#endif

OVD_int32 gettime(OVD_uint64 *out_time) {
    emxlogd("into\n");
    TimeRec timeCost;
    *out_time = Time::GetMs64();
    emxlogi("leave cost:%ums,out_time: %lld\n", timeCost.GetInv(), *out_time);
    return 0;
}

OVD_int32 settime(OVD_uint64 input_time, OVD_int32 tolerance_value) {
    emxlogi("into input_time=%llu,tolerance_value=%d\n", input_time, tolerance_value);
    TimeRec timeCost;
    int ret = -1;
    void *buffer[] = {
            &ret,
            &input_time,
            &tolerance_value,
    };
    ThreadInvoke::Packet packet((uint32_t) OvdCallBackIdE::settime, buffer);
    gOvdReq.Send(packet);
    emxlogd("leave cost:%ums\n", timeCost.GetInv());
    return 0;
}

#ifdef OVDSDK_APIVER_1_0
OVD_int32 syncProbeDetector(OVD_int32 avgRTTThreshold, OVD_int32 checkPeriod, OVD_int32 cpuLoadThreshold,
                            OVD_int32 memoryLoadThreshold, OVD_int32 memoryAvaliable, OVD_int32 packetLossRateThreshold,
                            OVD_int32 reportSwitch, OVD_int32 wifiQualityThreshold) {
    emxlogd("into\n");
    TimeRec timeCost;
    int ret = -1;
    void *buffer[] = {
            &ret,
            &avgRTTThreshold,
            &checkPeriod,
            &cpuLoadThreshold,
            &memoryLoadThreshold,
            &memoryAvaliable,
            &packetLossRateThreshold,
            &reportSwitch,
            &wifiQualityThreshold,
    };
    ThreadInvoke::Packet packet((uint32_t) OvdCallBackIdE::syncProbeDetector, buffer);
    gOvdReq.Send(packet);
    emxlogd("leave cost:%ums\n", timeCost.GetInv());
    return ret;
}
#endif

OVD_int32 ProbeByval() {
    emxlogd("into\n");
    TimeRec timeCost;
    int ret = -1;
    void *buffer[] = {
            &ret,
    };
    ThreadInvoke::Packet packet((uint32_t) OvdCallBackIdE::ProbeByval, buffer);
    gOvdReq.Send(packet);
    emxlogd("leave cost:%ums\n", timeCost.GetInv());
    return ret;
}

OVD_int32 getsimpleovdinfo(SimpleOVDinfo *out_simpleovdinfo) {
    emxlogd("into\n");
    TimeRec timeCost;
    int ret = -1;
    void *buffer[] = {
            &ret,
            &out_simpleovdinfo,
    };
    ThreadInvoke::Packet packet((uint32_t) OvdCallBackIdE::getsimpleovdinfo, buffer);
    gOvdReq.Send(packet);
    emxlogd("leave cost:%ums\n", timeCost.GetInv());
    return ret;
}

OVD_int32 GetDiskInfo(OVD_int32 *out_state, OVD_int32 *out_total, OVD_int32 *out_free) {
    emxlogd("into\n");
    TimeRec timeCost;
    int ret = -1;
    SDCard::Info info = {};
    SDCardClient sdcard(0);
    if (sdcard.GetInfo(info) == ErrCodeE::Success) {
        ret = 0;
        if (info.stat == SDCard::StatE::Normal)
            *out_state = 0;
        else if (info.stat == SDCard::StatE::Rejected)
            *out_state = 1;
        else if (info.stat == SDCard::StatE::Inserted)
            *out_state = 2;
        else if (info.stat == SDCard::StatE::Formatting)
            *out_state = 3;
        else
            *out_state = 2;
        *out_total = info.total;
        *out_free = info.free;
    } else {
        *out_state = 1;
    }
    emxlogd("out_state: %d\n", *out_state);
    emxlogd("leave cost:%ums\n", timeCost.GetInv());
    return ret;
}

#ifdef OVDSDK_APIVER_1_0
OVD_int32 TriggerSoftProbe() {
    emxlogd("into\n");
    TimeRec timeCost;
    int ret = -1;
//    void *buffer[] = {
//            &ret,
//    };
//    ThreadInvoke::Packet packet((uint32_t) OvdCallBackIdE::TriggerSoftProbe, buffer);
//    gOvdReq.Send(packet);
    OVD_int64 maxRTT = 0;
    OVD_int64 minRTT = 0;
    OVD_int64 avgRTT = 0;
    OVD_int32 count = 0;
    ret = OVD_get_RTTInfo(&maxRTT, &minRTT, &avgRTT, &count);
    if (ret < 0) {
        emxloge("OVD_get_RTTInfo, ret:%d, maxrtt:%lld, minrtt:%lld, avgrtt:%lld, count:%d\n",
               ret, maxRTT, minRTT, avgRTT, count);
    }
    //WiFi信号
    int wifiQuality = -100;
    if (NetMainDev::IsSupported(Net::DevE::Wifi)) {
        NetClientWifi wifi;
        NetClientWifi::Info info = {};
        if (wifi.GetInfo(info) == ErrCodeE::Success) {
            wifiQuality = (int) info.signalLevel - 100;
        }
    }

    //丢包率
    int packetLossRate = 0;
    Net::DevE netDev;
    if (NetMainDev::Get(netDev) == ErrCodeE::Success) {
        NetClient net(netDev);
        packetLossRate = OvdUtils::getPktLossRate(net.GetInterface());
    }

    //cpu负载
    Misc::SysUsage usage = {};
    Misc::GetSysUsage(usage, 1000);
    ret = OVD_Probe_StatusReport(packetLossRate,
                                 (OVD_int32) maxRTT, (OVD_int32) minRTT, (OVD_int32) avgRTT,
                                 wifiQuality,
                                 usage.cpu,
                                 (usage.memLoad * 100 / usage.memTotal), usage.memFree, 0);

    emxlogi("ret:%d, packetLossRate:%d, maxRTT:%lld, minRTT:%lld, avgRTT:%lld, "
           "wifiQuality:%d, cpuLoad:%d, memoryLoad:%d, memoryAvailable:%d\n",
           ret,
           packetLossRate,
           maxRTT, minRTT,
           wifiQuality,
           usage.cpu,
           (usage.memLoad * 100 / usage.memTotal),
           usage.memFree);
    emxlogd("leave cost:%ums\n", timeCost.GetInv());
    return ret;
}
#endif

OVD_int32 LogUploadAsync(OVD_char *trans_id, OVD_char *start, OVD_char *end, OVD_char *url) {
    emxlogd("into\n");
    TimeRec timeCost;
    int ret = -1;
    void *buffer[] = {
            &ret,
            &trans_id,
            &start,
            &end,
            &url,
    };
    ThreadInvoke::Packet packet((uint32_t) OvdCallBackIdE::LogUploadAsync, buffer);
    gOvdReq.Send(packet);
    emxlogd("leave cost:%ums\n", timeCost.GetInv());
    return ret;
}

OVD_int32 extension_callback(char *method, char *in_data, char *out_response, OVD_int32 in_outresponse_len,
                             OVD_int32 *out_outresponse_len) {
    emxlogd("into\n");
    TimeRec timeCost;
    int ret = -1;
    void *buffer[] = {
            &ret,
            &method,
            &in_data,
            &out_response,
            &in_outresponse_len,
            &out_outresponse_len,
    };
    ThreadInvoke::Packet packet((uint32_t) OvdCallBackIdE::extension_callback, buffer);
    gOvdReq.Send(packet);
    emxlogd("leave cost:%ums\n", timeCost.GetInv());
    return ret;
}

#ifdef OVDSDK_APIVER_3_0
OVD_int32 StopAlarm(int alarmtype) {
    emxloge("not implement\n");
    return -1;
}
#endif

#ifdef OVDSDK_APIVER_3_0
#ifdef OVDSDK1_38_1
OVD_int32 GetDevRunningInfo(ovd_probe_devrunning_info_e in_info, void* out_response) 
#else
OVD_int32 GetDevRunningInfo(OVD_GetDevRunningInfo_e in_info, void* out_response)
#endif
{
    emxlogd("into\n");
    TimeRec timeCost;
    int ret = -1;
    bool is_time_consuming = false;
    switch(in_info) {
        case OVD_CMD_GET_PACKET_LOSSRATE: /*丢包率 单位：%*/ {
            is_time_consuming = true;
            Net::DevE netDev;
            if (NetMainDev::Get(netDev) == ErrCodeE::Success) {
                NetClient net(netDev);
                *(int*)out_response = OvdUtils::getPktLossRate(net.GetInterface());
                ret = 0;
            }
            break;
        }
        case OVD_CMD_GET_WIFI_QUALITY: /*WIFI强度 单位：dB*/ {
            is_time_consuming = true;
            if (NetMainDev::IsSupported(Net::DevE::Wifi)) {
                NetClientWifi wifi;
                NetClientWifi::Info info = {};
                if (wifi.GetInfo(info) == ErrCodeE::Success) {
                    *(int*)out_response = (int) info.signalLevel - 100;
                    ret = 0;
                }
            }
            break;
        }
        case OVD_CMD_GET_CPU_LOAD: /*CPU占用率 单位：%*/ {
            is_time_consuming = true;
            Misc::SysUsage usage = { 0 };
            if (Misc::GetSysUsage(usage, 1000) == ErrCodeE::Success) {
                *(int*)out_response = usage.cpu;
                ret = 0;
            }
            break;
        }
        case OVD_CMD_GET_MEMORY_LOAD: /*内存占用率 单位：%*/ {
            is_time_consuming = true;
            Misc::SysUsage usage = { 0 };
            if (Misc::GetSysUsage(usage, 1000) == ErrCodeE::Success) {
                *(int*)out_response = (int)(100 * ((double)usage.memLoad / (double)usage.memTotal));
                ret = 0;
            }
            break;
        }
        case OVD_CMD_GET_RUNTIME: /*设备运行时长 单位：s*/ {
            is_time_consuming = true;
            *(int*)out_response = (int)OvdUtils::GetUptime();
            ret = 0;
            break;
        }
        case OVD_CMD_GET_MEMORY_AVAILABLE: /*剩余内存 单位：KB*/ {
            is_time_consuming = true;
            Misc::SysUsage usage = { 0 };
            if (Misc::GetSysUsage(usage, 1000) == ErrCodeE::Success) {
                *(int*)out_response = usage.memFree;
                ret = 0;
            }
            break;
        }
        case OVD_CMD_GET_MEMORY_TOTAL: /*运行总内存 单位：KB*/ {
            is_time_consuming = true;
            Misc::SysUsage usage = { 0 };
            if (Misc::GetSysUsage(usage, 1000) == ErrCodeE::Success) {
                *(int*)out_response = usage.memTotal;
                ret = 0;
            }
            break;
        }
        case OVD_CMD_GET_FLASH_TOTAL: /*运行总闪存 单位：KB*/ {
            break;
        }
        case OVD_CMD_GET_CHIP_SUPPLIER: /*芯片厂家 char[64]*/ {
            break;
        }
        case OVD_CMD_GET_CHIP_MODEL:  /*芯片型号 char[64]*/ {
            break;
        }
        case OVD_CMD_GET_HJZH_SDKVER: /*和家智话SDK版本号 char[64]*/ {
            break;
        }
        case OVD_CMD_GET_ANDLINK_SDKVER: /*Andlink SDK版本号 char[64]*/ {
            break;
        }
        case OVD_CMD_GET_PING:
        case OVD_CMD_GET_TRACEROUTE:
        case OVD_CMD_GET_RSSI_4G:
        #ifdef OVDSDK1_38_1
        case OVD_CMD_GET_POWER_MODE:
        #endif
            ret = 101;
        break;
    }
    if (is_time_consuming) {
        emxlogd("leave cost:%ums\n", timeCost.GetInv());
        return ret;
    }
    
    void *buffer[] = {
        &ret,
        &in_info,
        &out_response,
    };
    ThreadInvoke::Packet packet((uint32_t) OvdCallBackIdE::GetDevRunningInfo, buffer);
    gOvdReq.Send(packet);
    emxlogd("leave cost:%ums\n", timeCost.GetInv());
    return ret;
}
#endif

#ifdef OVDSDK1_38_1
OVD_int32 SyncMacHash(OVD_char* in_machash) {
    emxlogd("into\n");
    TimeRec timeCost;
    int ret = -1;
    void *buffer[] = {
        &ret,
        &in_machash
    };
    ThreadInvoke::Packet packet((uint32_t) OvdCallBackIdE::SyncMacHash, buffer);
    gOvdReq.Send(packet);
    emxlogd("leave cost:%ums\n", timeCost.GetInv());
    return ret;
}
#endif
