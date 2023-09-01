//
// Created by xiong on 2020/11/19.
//
#include <sys/ioctl.h>
#include <memory>
#include "acodec.h"
#include "sample_comm.h"
#include "core/EmxLog.hpp"
#include "MediaChipAenc.hpp"
#include "MediaConfigs.hpp"
#include "MediaInner.hpp"

#define ACODEC_FILE "/dev/cv182xadc"

using namespace Emx;

ErrCodeE MediaChipAenc::Start() {
    if (m_started)
        return ErrCodeE::Success;
    if (LoadParam() != ErrCodeE::Success) {
        return ErrCodeE::Failure;
    }
    StartPre();
    if (Config() != ErrCodeE::Success) {
        return ErrCodeE::Failure;
    }
    StartPost();
    StartStream();
    m_started = true;
    emxlogi("chn %d done\n", m_chn);
    return ErrCodeE::Success;
}

ErrCodeE MediaChipAenc::Stop() {
    if (!m_started)
        return ErrCodeE::Success;
    m_started = false;
    StopStream();
    StopPre();
    m_encoder->Destroy();
    SAMPLE_COMM_AUDIO_StopAi(m_aiDev, m_aiChn, (CVI_BOOL) (m_aioSampleRate != m_param.sampleRate), m_vqeType);
    StopPost();
    emxlogd("chn %d done\n", m_chn);
    return ErrCodeE::Success;
}

ErrCodeE MediaChipAenc::Config() {
    Json::Value root;
    m_res.configs.GetCfg("aenc", root);
    auto &json = root[m_chn]["attr"];
    m_aiDev = json["AiDev"].asInt();
    m_aiChn = json["AiChn"].asInt();
    m_aencChn = json["AencChn"].asInt();
    m_bind = json["bind"].asBool();
    m_gainMin = json["gain"]["min"].asInt();
    m_gainMax = json["gain"]["max"].asInt();
    m_aioSampleRate = json["aioSampleRate"].asInt();
    m_frameSize = json["frameSize"].asInt();
    m_vqeType = json["vqeType"].asInt();
    AIO_ATTR_S stAioAttr = {};
    stAioAttr.enSamplerate = (AUDIO_SAMPLE_RATE_E) m_aioSampleRate;
    stAioAttr.enBitwidth = (AUDIO_BIT_WIDTH_E) (m_param.bitWidth / 8 - 1);
    stAioAttr.enWorkmode = (AIO_MODE_E) json["enWorkmode"].asInt();
    stAioAttr.enSoundmode = (AUDIO_SOUND_MODE_E) json["enSoundmode"].asInt();
    stAioAttr.u32EXFlag = json["u32EXFlag"].asInt();
    stAioAttr.u32FrmNum = json["u32FrmNum"].asInt();
    stAioAttr.u32PtNumPerFrm = m_frameSize;
    stAioAttr.u32ChnCnt = json["u32ChnCnt"].asInt();
    stAioAttr.u32ClkSel = json["u32ClkSel"].asInt();
    stAioAttr.enI2sType = (AIO_I2STYPE_E) json["enI2sType"].asInt();

    CVI_S32 s32Ret = CVI_SUCCESS;
    s32Ret = SAMPLE_COMM_AUDIO_CfgAcodec(&stAioAttr);
    if (s32Ret != CVI_SUCCESS) {
        emxloge("chn %d SAMPLE_COMM_AUDIO_CfgAcodec failed!ret=%#x\n", m_chn, s32Ret);
        return ErrCodeE::Failure;
    }

    CVI_VOID *pAiVqeAttr = nullptr;
    AI_TALKVQE_CONFIG_S stAiVqeAttr = {};
    if (m_vqeType == 2) {
        GetTalkVqeParam(stAiVqeAttr);
        pAiVqeAttr = &stAiVqeAttr;
    }
    s32Ret = SAMPLE_COMM_AUDIO_StartAi(m_aiDev, m_aiChn, &stAioAttr,
                                       (AUDIO_SAMPLE_RATE_E) m_param.sampleRate,
                                       (CVI_BOOL) (m_aioSampleRate != m_param.sampleRate),
                                       pAiVqeAttr, m_vqeType);
    if (s32Ret != CVI_SUCCESS) {
        emxloge("chn %d SAMPLE_COMM_AUDIO_StartAi failed!ret=%#x\n", m_chn, s32Ret);
        return ErrCodeE::Failure;
    }
    SetChnParam();
    SetVolumeTemp(m_param.volume);
    MuteTemp(m_param.mute);
    m_encoder.reset(AudioCodecFactory::MakeEncoder(m_param.codec));
    //note: 开启vqe后，输入音频保持为单声道。
    int channelNum = 1; 
    m_encoder->Create(m_param.bitRate, m_param.sampleRate, m_param.bitWidth, channelNum);
    return ErrCodeE::Success;
}

ErrCodeE MediaChipAenc::GetTalkVqeParam(AI_TALKVQE_CONFIG_S &vqe) {
    Json::Value root;
    m_res.configs.GetCfg("aiVqe", root);
    auto &json = root[m_chn];

    memset(&vqe, 0, sizeof(vqe));
    vqe.enWorkstate = (VQE_WORKSTATE_E) json["enWorkstate"].asInt();
    vqe.s32FrameSample = m_frameSize;
    vqe.s32WorkSampleRate = m_aioSampleRate;
    vqe.s32BytesPerSample = (AUDIO_BIT_WIDTH_16 + 1);//???
    vqe.para_notch_freq = 0;
    auto &aec = json["aec"];
    if (aec["ena"].asBool()) {
        vqe.u32OpenMask |= AI_TALKVQE_MASK_AEC;
        auto &cfg = vqe.stAecCfg;
        cfg.para_aec_filter_len = aec["para_aec_filter_len"].asInt();
        cfg.para_aes_std_thrd = aec["para_aes_std_thrd"].asInt();
        cfg.para_aes_supp_coeff = aec["para_aes_supp_coeff"].asInt();
    }

    auto &agc = json["agc"];
    if (agc["ena"].asBool()) {
        vqe.u32OpenMask |= AI_TALKVQE_MASK_AGC;
        auto &cfg = vqe.stAgcCfg;
        cfg.para_agc_max_gain = (CVI_S8) agc["para_agc_max_gain"].asInt();
        cfg.para_agc_target_high = (CVI_S8) agc["para_agc_target_high"].asInt();
        cfg.para_agc_target_low = (CVI_S8) agc["para_agc_target_low"].asInt();
        cfg.para_agc_vad_enable = (CVI_BOOL) agc["para_agc_vad_enable"].asBool();
        cfg.para_agc_vad_cnt = (CVI_S8) agc["para_agc_vad_cnt"].asInt();
        cfg.para_agc_cut6_enable = (CVI_BOOL) agc["para_agc_cut6_enable"].asInt();
    }

    auto &anr = json["anr"];
    if (anr["ena"].asBool()) {
        vqe.u32OpenMask |= AI_TALKVQE_MASK_ANR;
        auto &cfg = vqe.stAnrCfg;
        cfg.para_nr_snr_coeff = (CVI_S8) anr["para_nr_snr_coeff"].asInt();
        cfg.para_nr_noise_coeff = (CVI_S8) anr["para_nr_noise_coeff"].asInt();
    }
    return ErrCodeE::Success;
}

ErrCodeE MediaChipAenc::SetChnParam() {
    AI_CHN_PARAM_S stAiChnPara = {};
    CVI_S32 s32Ret;
    s32Ret = CVI_AI_GetChnParam(m_aiDev, m_aiChn, &stAiChnPara);
    if (s32Ret != CVI_SUCCESS) {
        emxloge("chn %d CVI_AI_GetChnParam failed with %#x\n", m_chn, s32Ret);
        return ErrCodeE::Failure;
    }
    stAiChnPara.u32UsrFrmDepth = 10;
    s32Ret = CVI_AI_SetChnParam(m_aiDev, m_aiChn, &stAiChnPara);
    if (s32Ret != CVI_SUCCESS) {
        emxloge("chn %d CVI_AI_SetChnParam failed with %#x\n", m_chn, s32Ret);
        return ErrCodeE::Failure;
    }
    return ErrCodeE::Success;
}

ErrCodeE MediaChipAenc::SetParam(const MediaAenc::Param &param) {
    if (!ParamChanged(param, m_param)) {
        return ErrCodeE::Success;
    }
    if (!ParamValid(param)) {
        return ErrCodeE::IllegalParam;
    }
    Stop();
    memcpy(&m_param, &param, sizeof(MediaAenc::Param));
    SaveParam();
    return Start();
}

ErrCodeE MediaChipAenc::GetParam(MediaAenc::Param &param) {
    memcpy(&param, &m_param, sizeof(MediaAenc::Param));
    return ErrCodeE::Success;
}

static int InitMicGain() {
    CVI_S32 s32Ret = CVI_SUCCESS;
    CVI_S32 fdAcodec_adc = open(ACODEC_FILE, O_RDWR);
    if (fdAcodec_adc < 0) {
        emxloge("%s: can't open Acodec,%s\n", ACODEC_FILE);
        return CVI_FAILURE;
    }
    ACODEC_VOL_CTRL vol_ctrl = {};
    vol_ctrl.vol_ctrl_mute = 0;
    vol_ctrl.vol_ctrl = 1;
    s32Ret = ioctl(fdAcodec_adc, ACODEC_SET_ADCR_VOL, &vol_ctrl);
    close(fdAcodec_adc);
    return s32Ret;
}

ErrCodeE MediaChipAenc::SetVolumeTemp(int32_t volume) {
    Json::Value range;
    if (GetParamRange(range) != ErrCodeE::Success) {
        emxloge("GetParamRange failed!\n");
        return ErrCodeE::Failure;
    }
    if (!MediaConfigs::CheckRangeLegal("volume", (int) volume, range)) {
        emxloge("CheckRangeLegal failed!\n");
        return ErrCodeE::IllegalParam;
    }

    int gain = m_gainMin + (m_gainMax - m_gainMin) * volume / 100;
    emxlogd("set volume[%d] = gain[%d]\n", volume, gain);
    CVI_AI_SetVolume(m_aiDev, gain);
    InitMicGain();
    return ErrCodeE::Success;
}

ErrCodeE MediaChipAenc::SetVolume(int32_t volume) {
    if (volume == m_param.volume) {
        emxlogd("SetVolume no changed!\n");
        return ErrCodeE::Success;
    } 
    if (SetVolumeTemp(volume) != ErrCodeE::Success) {
        return ErrCodeE::Failure;
    }
    m_param.volume = volume;
    m_param.mute = false;
    SaveParam();
    return ErrCodeE::Success;
}

ErrCodeE MediaChipAenc::MuteTemp(bool ena) {
    if (ena) {
        CVI_AI_SetVolume(m_aiDev, 0);
        emxlogd("ai mute\n");
    } else {
        int gain = m_gainMin + (m_gainMax - m_gainMin) * m_param.volume / 100;
        CVI_AI_SetVolume(m_aiDev, gain);
        InitMicGain();
        emxlogd("set volume[%d] = gain[%d]\n", m_param.volume, gain);
    }
    return ErrCodeE::Success;
}

ErrCodeE MediaChipAenc::Mute(bool ena) {
    if (ena == m_param.mute) {
        emxlogd("SetMute no changed!\n");
        return ErrCodeE::Success;
    }
    if (MuteTemp(ena) != ErrCodeE::Success) {
        return ErrCodeE::Failure;
    }
    m_param.mute = ena;
    SaveParam();
    return ErrCodeE::Success;
}

void MediaChipAenc::StartStream() {
//    m_streamQuit = false;
    m_got = false;
    m_work.Create(&m_res.loop, nullptr,
                [this](void *arg) { OnStream(arg); },
                [this](ErrCodeE e, void *arg) { OnStreamComplete(e, arg); });
    m_work.Run();
}

void MediaChipAenc::StopStream() {
    m_work.Destroy();
}

void MediaChipAenc::OnStream(void *arg) {
    CVI_S32 s32Ret;
    s32Ret = CVI_AI_GetFrame(m_aiDev, m_aiChn, &m_stFrame, &m_stAecFrm, CVI_FALSE);
//    static uint64_t ts = 0;
//    printf("%u,%llu\n", m_stFrame.u32Len * 2, m_stFrame.u64TimeStamp - ts);
//    ts = m_stFrame.u64TimeStamp;
    if (s32Ret != CVI_SUCCESS)
        emxloge("chn %d CVI_AI_GetFrame failed with %#x!!\n", m_chn, s32Ret);
    else {
        m_got = true;
    }
}

void MediaChipAenc::OnStreamComplete(ErrCodeE e, void *arg) {
    if (m_got) {
        PushAiFrame();
        PushAencFrame();
        m_seq++;
        CVI_AI_ReleaseFrame(m_aiDev, m_aiChn, &m_stFrame, &m_stAecFrm);
    }
    if (e == ErrCodeE::Success) {
        m_got = false;
        m_work.Run();
    }
}

void MediaChipAenc::PushAiFrame() {
    MediaFrame frame = {};
    frame.type = MediaFrame::TypeE::Pcm;
    frame.chn = m_chn;
    frame.tsInUs = m_stFrame.u64TimeStamp;
    frame.seq = m_seq;
    frame.size = (int) m_stFrame.u32Len << 1;
    frame.off = m_res.ring.Push((char *) m_stFrame.u64VirAddr[0], frame.size, frame.tsInUs);
    if (frame.off >= 0) {
        m_res.pub.Publish(MEDIA_TOPIC_AENC_PCM + m_chn, (char *) &frame, sizeof(frame));
    }
}

void MediaChipAenc::PushAencFrame() {
    uint8_t data[EMX_AUDIO_MAX_FRAME_SIZE] = {};
    if (m_stFrame.u32Len > EMX_AUDIO_MAX_FRAME_SIZE) {
        emxlogc("chn %d aenc frame size %d > EMX_AUDIO_MAX_FRAME_SIZE(%d) !!!\n",
               m_chn, m_stFrame.u32Len, EMX_AUDIO_MAX_FRAME_SIZE);
        return;
    }
    int32_t outSize = EMX_AUDIO_MAX_FRAME_SIZE;
    m_encoder->Encode((int16_t *) m_stFrame.u64VirAddr[0],
                      (int) m_stFrame.u32Len, data, outSize);
    //aac会控制包长，可能多次输入对应一次输出
    if (outSize <= 0) {
        return;
    }
    MediaFrame frame = {};
    frame.type = MediaFrame::TypeE::Aenc;
    frame.chn = m_chn;
    frame.tsInUs = m_stFrame.u64TimeStamp;
    frame.trace = Time::GetMonoClockUs();
    frame.seq = m_enSeq;
    frame.size = outSize;
    frame.off = m_res.ring.Push((char *) data, frame.size, frame.tsInUs);
    if (frame.off >= 0) {
        m_res.pub.Publish(MEDIA_TOPIC_AENC_AENC + m_chn, (char *) &frame, sizeof(frame));
    }
    m_enSeq++;
}
