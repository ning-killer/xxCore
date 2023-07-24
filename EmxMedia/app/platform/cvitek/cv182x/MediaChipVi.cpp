/*
* @Author: xiong
* @Date: 2023/4/3
*/

#include "MediaChipVi.hpp"
#include "MediaInner.hpp"
#include "core/EmxLog.hpp"
#include "MediaConfigs.hpp"
#include "cvi_vi.h"

using namespace Emx;


ErrCodeE MediaChipVi::Start() {
    if (m_started) return ErrCodeE::Success;
    if (LoadParam() != ErrCodeE::Success) {
        return ErrCodeE::Failure;
    }
    StartPre();
    Json::Value root;
    m_res.configs.GetCfg("vi", root);
    auto &json = root[m_chn]["attr"];
    m_pipe = (VI_PIPE) json["ViPipe"].asInt();
    m_viChn = (VENC_CHN) json["ViChn"].asInt();
    m_originFlip = json["flip"].asBool();
    m_originMirror = json["mirror"].asBool();
    // 根据原始出厂的翻转状态，来确定最终配置参数
    CVI_S32 ret = CVI_VI_SetChnFlipMirror(m_pipe, m_viChn, m_param.flip != m_originFlip,
                                          m_param.mirror != m_originMirror);
    if (ret != 0) {
        emxloge("chn %d CVI_VI_SetChnFlipMirror failed with %#x!\n", m_chn, ret);
        return ErrCodeE::Failure;
    }
    StartPost();
    m_started = true;
    emxlogi("chn %d done\n", m_chn);
    return ErrCodeE::Success;
}

ErrCodeE MediaChipVi::Stop() {
    if (!m_started) return ErrCodeE::Success;
    m_started = false;
    StopPre();
    StopPost();
    emxlogd("chn %d done\n", m_chn);
    return ErrCodeE::Success;
}

ErrCodeE MediaChipVi::SetParam(const MediaVi::Param &param) {
    if (!ParamChanged(param, m_param)) {
        return ErrCodeE::Success;
    }
    if (!ParamValid(param)) {
        return ErrCodeE::IllegalParam;
    }
    Stop();
    memcpy(&m_param, &param, sizeof(MediaVi::Param));
    SaveParam();
    return Start();
}

ErrCodeE MediaChipVi::GetParam(MediaVi::Param &param) {
    memcpy(&param, &m_param, sizeof(MediaVi::Param));
    return ErrCodeE::Success;
}
