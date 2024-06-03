/*
* @Author: xiong
* @Date: 2022/10/10
*/

#include "SoftProbe.hpp"
#include "NetClient.hpp"
#include "OVD_OpenAPI.h"
#include "OvdUtils.hpp"
using namespace Emx;

ErrCodeE SoftProbe::Create(OvdCtx *ctx) {
    if (m_create)
        return ErrCodeE::Success;
    m_ctx = ctx;
    m_create = false;
    ErrCodeE ret = ErrCodeE::Failure;

#ifdef OVDSDK_APIVER_1_0
    emxlogi("ovdsdk api current version is 1.0\n");
    m_timer.Create(m_ctx->loop);
    m_work.Create(&m_ctx->loop, nullptr,
                  std::bind(&SoftProbe::OnDetect, this, ph_1),
                  std::bind(&SoftProbe::OnDetectComplete, this, ph_1, ph_2));
    if (m_ctx->env.cfg.softprobe_on && m_ctx->softProbe.reportSwitch == 1)
        m_timer.Start(m_ctx->softProbe.checkPeriod, 0, [this]() { OnTimer(); });
    m_create = true;
    ret = ErrCodeE::Success;
#elif defined OVDSDK_API_VER2_0
    emxloge("ovdsdk api version 2.0 no adaptation\n");
#elif defined OVDSDK_APIVER_3_0
    //note: 软探针3.0版本设备端无定时监测业务
    emxlogi("ovdsdk api current version is 3.0\n");
    ret = ErrCodeE::Success;
#else
    emxloge("please check ovdsdk api version defined!\n");
#endif

    return ret;
}

void SoftProbe::Destroy() {
    if (!m_create)
        return;
#ifdef OVDSDK_APIVER_1_0
    m_timer.Destroy();
    m_work.Destroy();
#elif defined OVDSDK_API_VER2_0
    emxloge("ovdsdk api version 2.0 no adaptation\n");
#elif defined OVDSDK_APIVER_3_0
    //note: 软探针3.0版本设备端无定时监测业务
#else
    emxloge("please check ovdsdk api version defined!\n");
#endif 
    m_create = false;
    emxlogi("SoftProbe Destroy\n");
}

void SoftProbe::Update() {
#ifdef OVDSDK_APIVER_1_0
    if (m_create) {
        if (m_ctx->env.cfg.softprobe_on && m_ctx->softProbe.reportSwitch == 1)
            m_timer.Start(m_ctx->softProbe.checkPeriod, 0, [this]() { OnTimer(); });
        else
            m_timer.Stop();
    }
#elif defined OVDSDK_APIVER_3_0
    int ret = -1;
    if (m_ctx->env.cfg.softprobe_on) {
        ret = ovd_probe_start();
    } else {
        ret = ovd_probe_paused();
    }
    if (ret == 0) {
        emxlogi("set (%s) softprobe success\n", 
            m_ctx->env.cfg.softprobe_on ? "start" : "paused");
    } else {
        emxloge("set (%s) softprobe failed, ret(%d)\n", 
            m_ctx->env.cfg.softprobe_on ? "start" : "paused", ret);
    }
#else
#endif
}

void SoftProbe::OnTimer() {
#ifdef OVDSDK_APIVER_1_0
    //如果work还没有complete，则再次延时
    if (m_isWorking)
        m_timer.Start(m_ctx->softProbe.checkPeriod, 0, [this]() { OnTimer(); });
    m_isWorking = true;
    memcpy(&m_softProbe, &m_ctx->softProbe, sizeof(m_softProbe));
    m_work.Run();
#endif
}

void SoftProbe::OnDetect(void *arg) {
#ifdef OVDSDK_APIVER_1_0
    int ret;
    bool need_upload = false;

    OVD_int64 maxRTT = 0;
    OVD_int64 minRTT = 0;
    OVD_int64 avgRTT = 0;
    OVD_int32 count = 0;
    ret = OVD_get_RTTInfo(&maxRTT, &minRTT, &avgRTT, &count);
    if (ret < 0) {
        emxloge("OVD_get_RTTInfo, ret:%d, maxrtt:%lld, minrtt:%lld, avgrtt:%lld, count:%d\n",
               ret, maxRTT, minRTT, avgRTT, count);
    } else {
        need_upload |= avgRTT >= m_softProbe.wifiQualityThreshold;
    }

    //WiFi信号
    int wifiQuality = -100;
    if (NetMainDev::IsSupported(Net::DevE::Wifi)) {
        NetClientWifi wifi;
        NetClientWifi::Info info = {};
        if (wifi.GetInfo(info) == ErrCodeE::Success) {
            wifiQuality = (int) info.signalLevel - 100;
            need_upload |= wifiQuality <= m_softProbe.wifiQualityThreshold;
        }
    }

    //丢包率
    int packetLossRate = 0;
    Net::DevE netDev;
    if (NetMainDev::Get(netDev) == ErrCodeE::Success) {
        NetClient net(netDev);
        packetLossRate = OvdUtils::getPktLossRate(net.GetInterface());
        need_upload |= packetLossRate > m_softProbe.packetLossRateThreshold;
    }

    //cpu负载
//    int cpuLoadThreshold;         //CPU使用率阈值（高于）
//    int memoryLoadThreshold;      //内存使用率阈值（高于）
//    int memoryAvaliable;          //剩余内存 单位kb （低于）
//    int cpu;//!<0-100, cpu占有率
//    int memLoad;//!<占用内存(KB)
//    int memFree;//!<剩余内存(KB)
//    int memTotal;//!<内存总量(KB)
    Misc::SysUsage usage = {};
    Misc::GetSysUsage(usage, 1000);
    need_upload |= usage.cpu >= m_softProbe.cpuLoadThreshold;
    need_upload |= (usage.memLoad * 100 / usage.memTotal) >= m_softProbe.memoryLoadThreshold;
    need_upload |= usage.memFree < m_softProbe.memoryAvaliable;

    if (need_upload) {
        ret = OVD_Probe_StatusReport(packetLossRate,
                                     (OVD_int32) maxRTT, (OVD_int32) minRTT, (OVD_int32) avgRTT,
                                     wifiQuality,
                                     usage.cpu,
                                     (usage.memLoad * 100 / usage.memTotal), usage.memFree, 0);
    }
    emxlogi("need_upload:%d, ret:%d, packetLossRate:%d[%d], maxRTT:%lld, minRTT:%lld, avgRTT:%lld[%lld], "
           "wifiQuality:%d[%d], cpuLoad:%d[%d], memoryLoad:%d[%d], memoryAvailable:%d[%d]\n",
           need_upload, ret,
           packetLossRate, m_softProbe.packetLossRateThreshold,
           maxRTT, minRTT, avgRTT, m_softProbe.avgRTTThreshold,
           wifiQuality, m_softProbe.wifiQualityThreshold,
           usage.cpu, m_softProbe.cpuLoadThreshold,
           (usage.memLoad * 100 / usage.memTotal), m_softProbe.memoryLoadThreshold,
           usage.memFree, m_softProbe.memoryAvaliable);
#endif
}

void SoftProbe::OnDetectComplete(ErrCodeE e, void *arg) {
#ifdef OVDSDK_APIVER_1_0
    m_timer.Start(m_ctx->softProbe.checkPeriod, 0, [this]() { OnTimer(); });
#endif
}
