//
// Created by xiong on 2021/3/3.
//

#ifndef EAPIL_CallBackResp_HPP
#define EAPIL_CallBackResp_HPP

#include "EmxCore.hpp"
#include "ThreadInvoke.hpp"
#include "CallBackDefine.hpp"
#include "OvdCtx.hpp"
#include "UpdateClient.hpp"

#ifndef EAPIL_SPARE

#include "Record.hpp"

#endif
namespace Emx {
    class Ovd;

    class CallBackResp {
    public:
        CallBackResp(Ovd *ovd);
        ~CallBackResp();

        void Start();

        void Stop();

    private:
        void ProcCallBack(ThreadInvoke::Packet &packet);

    private:
        // ovd call back rpc
        OVD_int32 GetOVDDeviceInfo(OVDDeviceInfo *deviceInfo);

        OVD_int32 GetOVDConfigureInfo(OVD_char **output_ovdconfig, OVD_int32 *output_size);

        OVD_int32 SetOVDConfigureInfo(OVD_char *in_ovdconfig);

        OVD_void OVCConnectStatus(OVD_int32 connectStatus);

        OVD_int32 ReBootChannel(OVD_int32 channel);

        #ifdef OVDSDK1_38_1
        OVD_int32 ReBootDevice(ovd_reboot_reason_e reason);
        #else
        OVD_int32 ReBootDevice();
        #endif

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

        OVD_int32
        DMEAPI_callback_RecordSearch(OVD_int32 channel, OVD_uint64 starttime, OVD_uint64 endtime, OVD_int32 page,
                                     OVD_int32 numInPage, OVD_DMERecordFileListPerPage *fileinpage);

        OVD_void *DMEAPI_callback_RecordOpen(OVD_int32 channel);

        OVD_int32 DMEAPI_callback_RecordSeek(OVD_void *ctx, OVD_int64 timestamp);

        OVD_int32 DMEAPI_callback_RecordReadFrame(OVD_void *ctx, OVD_FrameInfo *pframe_info);

        OVD_int32 DMEAPI_callback_RecordClose(OVD_void *ctx);

        OVD_int32 FirmwareUpgrade(OVD_char *firmware_model, OVD_char *upgradeURL);

        OVD_int32
        QueryFirmwareUpgradeStatus(OVDUpgradeStatus *upgradeStatus, OVD_int32 *upgradeProgress, OVD_char *version,
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

        OVD_int32 ForceIFrame(OVD_int32 channel);

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
                                    OVD_int32 memoryLoadThreshold, OVD_int32 memoryAvaliable,
                                    OVD_int32 packetLossRateThreshold,
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
        void SetVolume();

        void SetLinkageStrategy(const Json::Value &json, EnvStrategy &strategy, std::string alarmVoice);
        void SetAlertTime(const Json::Value &json, EnvSchedule &schedule);
        void GetLinkageStrategy(Json::Value &json, const EnvStrategy &strategy);
        void GetAlertTime(Json::Value &json, const EnvSchedule &schedule);
        OVD_int32 SyncMacHash(OVD_char* in_machash);

        void ShowPassengerFlowOsd();

        void ShowRegionalPeopleOsd();

    private:
        Ovd *m_ovd;
        OvdCtx &m_ctx;
        ThreadInvoke::Resp m_resp;
        bool m_started;
        EuvTimer m_volumeConfig;
    private:
#ifndef EAPIL_SPARE
        //用于录像
        OVD_uint64 m_lastEndStamp = 0;
        OVD_uint64 m_lastStartStamp = 0;
        std::vector<Record::RecFile> m_recordIndex;

        struct RecPlayHandle {
            int InitRecordReader(const char *path);

            Mp4Reader *mp4Reader;
            Record::RecFile curFile;
            int64_t baseUtcMs;
            int64_t duration;
            int64_t lastVideoPts;//sbhy
            std::map<int, MediaFmt::TrackInfo> trackMap;
        };

#endif
    private:
        //用于升级
        struct Update {
            void OnCurlPerform(void *arg);

            static size_t OnDownload(void *ptr, size_t size, size_t number, void *stream);

            EuvWork work;
            std::string url;
            UpdateClient handle;
        } m_update;

    private:
        //用于日志上传
        struct UploadLogArg {
            EuvWork work;
            std::string trans_id;
            std::string start;
            std::string end;
            std::string url;
        };
    private:
        //用于下载警戒音频
        struct InvasionAlarmVoice {
            InvasionAlarmVoice() : urlChanged(false) {}

            struct Arg {
                std::string url;
                std::string alarmVoice;
                bool playVoiceA;
                int ret;
                EnvStrategy *strategy = nullptr;
            };

            void OnDownloadVoice(void *arg);

            void OnDownloadComplete(ErrCodeE e, void *arg);

            std::string url;
            EuvWork work;
            bool urlChanged;
        } m_invasion;

        //用于声音下载测试
        struct AudioOut {
            struct Arg {
                int ret;
                int channel;
                int repeat;
                int volume;
                std::string url;
            };
            std::string url;
            EuvWork work;
        } m_audioOut;
    };
}


#endif //EAPIL_CallBackResp_HPP
