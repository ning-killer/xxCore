//
// Created by xiong on 2022/6/24.
//

#ifndef EAPIL_CallBackDefine_HPP
#define EAPIL_CallBackDefine_HPP


#include "OVD_OpenAPI.h"

namespace Emx {
#define OVD_CALL_BACK_RPC_ID "/tmp/ovdRpc"
    enum class OvdCallBackIdE : uint32_t {
        GetOVDDeviceInfo,
        GetOVDConfigureInfo,
        SetOVDConfigureInfo,
        OVCConnectStatus,
        ReBootChannel,
        ReBootDevice,
        KeepAwakenUtilExpired,
        ResetConfiguration,
        QueryRecordPage,
        OpenRecordFile,
        RecordCotrol,
        DMEAPI_callback_RecordSearch,
        DMEAPI_callback_RecordOpen,
        DMEAPI_callback_RecordSeek,
        DMEAPI_callback_RecordReadFrame,
        DMEAPI_callback_RecordClose,
        FirmwareUpgrade,
        QueryFirmwareUpgradeStatus,
        SyncTime,
        QueryTime,
        GetSDInfo,
        SetSDCardFormat,
        PTZCmd,
        GetPresetList,
        AudioPlayStart,
        AudioPlayProGress,
        AudioPlayStop,
        VedioSwitchQuality,
        ForceIFrame,
        Snapshot,
        SetAudioOutPlay,
        AudioOutPlayCtrl,
        GetAudioOutPlayStatus,
        gettime,
        settime,
        syncProbeDetector,
        ProbeByval,
        getsimpleovdinfo,
        GetDiskInfo,
        TriggerSoftProbe,
        LogUploadAsync,
        extension_callback,
        StopAlarm,
        GetDevRunningInfo,
        SyncMacHash,
    };
}
#endif //EAPIL_CallBackDefine_HPP
