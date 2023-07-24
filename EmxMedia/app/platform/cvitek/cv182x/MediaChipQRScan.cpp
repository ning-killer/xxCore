//
// Created by xiong on 2021/6/25.
//
#include "core/EmxLog.hpp"
#include "MediaConfigs.hpp"
#include "MediaChipQRScan.hpp"
#include "MediaInner.hpp"
#include "sample_comm.h"

using namespace Emx;

ErrCodeE MediaChipQRScan::Start() {
    if (m_started)
        return ErrCodeE::Success;
    Json::Value root;
    m_res.configs.GetCfg("qrScan", root);
    auto &json = root[m_chn];
    m_vpssGrp = json["vpssGrp"].asInt();
    m_vpssChn = json["vpssChn"].asInt();

    VPSS_CHN_ATTR_S stChnAttr = {};
    int s32Ret = CVI_VPSS_GetChnAttr(m_vpssGrp, m_vpssChn, &stChnAttr);
    if (CVI_SUCCESS != s32Ret) {
        emxloge("chn %d CVI_VPSS_GetChnAttr failed!\n", m_chn);
        return ErrCodeE::Failure;
    }

    if (CreateDecoder((int) stChnAttr.u32Width, (int) stChnAttr.u32Height) != ErrCodeE::Success) {
        emxloge("chn %d create decoder failed!\n", m_chn);
        return ErrCodeE::Failure;
    }

    m_mem = open("/dev/mem", O_CREAT | O_RDWR | O_SYNC);
    if (m_mem < 0) {
        emxloge("chn %d open /dev/mem failed\n", m_chn);
        DestroyDecoder();
        return ErrCodeE::Failure;
    }
    m_work.Create(&m_res.loop, nullptr, [this](void *arg) { OnQrScanRun(arg); },
                  [this](ErrCodeE e, void *arg) { OnQrScanComplete(e, arg); });
    m_got = false;
    m_work.Run();
    m_started = true;
    return ErrCodeE::Success;
}

ErrCodeE MediaChipQRScan::Stop() {
    if (!m_started)
        return ErrCodeE::Success;
    m_work.Destroy();
    close(m_mem);
    m_started = false;
    return ErrCodeE::Success;
}

void MediaChipQRScan::OnQrScanRun(void *arg) {
    VIDEO_FRAME_INFO_S stFrame = {};
    int s32Ret = CVI_VPSS_GetChnFrame(m_vpssGrp, m_vpssChn, &stFrame, 200);
    if (CVI_SUCCESS != s32Ret) {
        emxloge("chn %d CVI_VPSS_GetChnFrame failed!\n", m_chn);
        return;
    }

    CVI_U64 phyAddr = stFrame.stVFrame.u64PhyAddr[0];
    auto dataY = (uint8_t *) CVI_SYS_Mmap(phyAddr, stFrame.stVFrame.u32Length[0]);
    if (!dataY) {
        CVI_VPSS_ReleaseChnFrame(m_vpssGrp, m_vpssChn, &stFrame);
        emxloge("chn %d dataY == nullptr!\n", m_chn);
        return;
    }

    m_got = Decode(dataY, m_content) == ErrCodeE::Success;

    CVI_SYS_Munmap(dataY, stFrame.stVFrame.u32Length[0]);
    /* release frame after using */
    CVI_VPSS_ReleaseChnFrame(m_vpssGrp, m_vpssChn, &stFrame);
}

void MediaChipQRScan::OnQrScanComplete(ErrCodeE e, void *arg) {
    if (e == ErrCodeE::Success) {
        if (m_got) {
            m_got = false;
            Json::Value json;
            json["chn"] = m_chn;
            json["content"] = m_content;
            std::string topic;
            EasyJson::Serialize(json, topic);
            m_res.pub.Publish(MEDIA_TOPIC_QR_SCAN, topic.data(), (int) topic.size() + 1);
        }
        m_work.Run();
    }
}