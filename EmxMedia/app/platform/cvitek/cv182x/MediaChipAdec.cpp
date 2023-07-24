//
// Created by xiong on 2020/11/23.
//
#include <sys/ioctl.h>
#include <memory>
#include "acodec.h"
#include "sample_comm.h"
#include "core/EmxLog.hpp"
#include "MediaChipAdec.hpp"
#include "MediaConfigs.hpp"
#include "VoicePlay.hpp"
#include "MediaInner.hpp"

#define ACODEC_FILE "/dev/cv182xadc"

using namespace Emx;

ErrCodeE MediaChipAdec::Start() {
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
    m_started = true;
    emxlogi("chn %d done\n", m_chn);
    return ErrCodeE::Success;
}

ErrCodeE MediaChipAdec::Stop() {
    if (!m_started)
        return ErrCodeE::Success;
    m_started = false;
    StopPre();
    m_loopAdec.StopAndDeInit();
    m_decoder->Destroy();
    if (m_autoMute) {
        SpkEna(false);
        for (auto &e:m_spkGpioList)
            delete e;
        m_spkGpioList.clear();

    }
    SAMPLE_COMM_AUDIO_StopAo(m_aoDev, m_aoChn, (CVI_BOOL) (m_aioSampleRate != m_param.sampleRate));
    StopPost();
    emxlogd("chn %d done\n", m_chn);
    return ErrCodeE::Success;
}

ErrCodeE MediaChipAdec::SetParam(const MediaAdec::Param &param) {
    if (!ParamChanged(param, m_param)) {
        return ErrCodeE::Success;
    }
    if (!ParamValid(param)) {
        return ErrCodeE::IllegalParam;
    }
    Stop();
    memcpy(&m_param, &param, sizeof(MediaAdec::Param));
    SaveParam();
    return Start();
}

ErrCodeE MediaChipAdec::GetParam(MediaAdec::Param &param) {
    memcpy(&param, &m_param, sizeof(MediaAdec::Param));
    return ErrCodeE::Success;
}

ErrCodeE MediaChipAdec::SetVolume(int32_t volume) {
    auto e = SetVolumeTemp(volume);
    if (e == ErrCodeE::Success) {
        if (m_param.volume != volume) {
            m_param.volume = volume;
            e = SaveParam();
        }
    }
    return e;
}

ErrCodeE MediaChipAdec::SetVolumeTemp(int32_t volume) {
    Json::Value range;
    if (GetParamRange(range) != ErrCodeE::Success)
        return ErrCodeE::Failure;
    if (!MediaConfigs::CheckRangeLegal("volume", (int) volume, range))
        return ErrCodeE::IllegalParam;
    int gainTarget = m_gainMin + (m_gainMax - m_gainMin) * volume / 100;
    int gainOrg = -1;
    CVI_AO_GetVolume(m_aoDev, &gainOrg);
    if (gainOrg != gainTarget) {
        emxlogd("set volume[%d] / gain[%d]\n", volume, gainTarget);
        CVI_AO_SetVolume(m_aoDev, gainTarget);
    }
    return ErrCodeE::Success;
}

MediaAdec::PlayStatusE MediaChipAdec::GetPlayStatus() {
    return m_playQuit ? PlayStatusE::Finish : PlayStatusE::Playing; 
}

ErrCodeE MediaChipAdec::Config() {
    Json::Value root;
    m_res.configs.GetCfg("adec", root);
    auto &json = root[m_chn]["attr"];
    m_aoDev = json["AoDev"].asInt();
    m_aoChn = json["AoChn"].asInt();
    m_adecChn = json["AdecChn"].asInt();
    m_bind = json["bind"].asBool();
    m_gainMin = json["gain"]["min"].asInt();
    m_gainMax = json["gain"]["max"].asInt();
    m_aioSampleRate = json["aioSampleRate"].asInt();
    m_frameSize = json["frameSize"].asInt();
    m_silentTimeMs = json["silentTimeMs"].asInt();
    m_soundMode = (AUDIO_SOUND_MODE_E) json["enSoundmode"].asInt();
    m_bitWidth = (AUDIO_BIT_WIDTH_E) (m_param.bitWidth / 8 - 1);
    m_autoMute = json["spkAmpCtrl"]["autoMute"].asBool();

    char name[EMX_MAX_PATH_SIZE];
    snprintf(name, EMX_MAX_PATH_SIZE, "AdecLoop%d\n", m_chn);
    m_loopAdec.Init(name, [this]() {
        m_sub.DestroyMsgTopicSubscriberAsync();
        m_playWork.Destroy();
        if (m_autoMute) {
            m_muteTimer.Destroy();
        }
    });
    m_playWork.Create(&m_loopAdec, nullptr,
                      std::bind(&MediaChipAdec::PlayFile, this, ph_1),
                      std::bind(&MediaChipAdec::PlayFileDone, this, ph_1, ph_2));
    if (m_autoMute) {
        auto &spkAmpCtrl = json["spkAmpCtrl"];
        for (auto &e : spkAmpCtrl["gpioList"]) {
            auto gpio = new Gpio(e["num"].asInt(),
                                 Gpio::DirE::Out,
                                 (Gpio::ActiveE) e["active"].asInt(),
                                 (Gpio::ValueE) e["default"].asInt());
            m_spkGpioList.push_back(gpio);
        }
        m_muteTimer.Create(m_loopAdec);
        m_muteCnt = 0;
        m_muteTimer.Start(1000, 100, [this]() {
            if (m_muting || !m_voiceQue.empty()) {
                m_muteCnt = 0;
                return;
            }
            AO_CHN_STATE_S status = {};
            CVI_S32 ret = CVI_AO_QueryChnStat(m_aoDev, m_aoChn, &status);
            if (ret != CVI_SUCCESS) {
                emxloge("chn %d CVI_AO_QueryChnStat failed with %#x\n", m_chn, ret);
            } else {
                if (status.u32ChnBusyNum < (uint32_t) m_frameSize * 2) {
                    m_muteCnt++;
                    if (m_muteCnt > 5) {
                        SpkEna(false);
                        emxlogd("chn %d spk disable\n", m_chn);
                    }
                } else
                    m_muteCnt = 0;
            }
        });
    }

    AIO_ATTR_S stAioAttr = {};
    stAioAttr.enSamplerate = (AUDIO_SAMPLE_RATE_E) m_aioSampleRate;
    stAioAttr.enBitwidth = m_bitWidth;
    stAioAttr.enWorkmode = (AIO_MODE_E) json["enWorkmode"].asInt();
    stAioAttr.enSoundmode = m_soundMode;
    stAioAttr.u32EXFlag = json["u32EXFlag"].asInt();
    stAioAttr.u32FrmNum = json["u32FrmNum"].asInt();
    stAioAttr.u32PtNumPerFrm = m_frameSize;
    stAioAttr.u32ChnCnt = json["u32ChnCnt"].asInt();
    stAioAttr.u32ClkSel = json["u32ClkSel"].asInt();
    stAioAttr.enI2sType = (AIO_I2STYPE_E) json["enI2sType"].asInt();

    CVI_S32 s32Ret;
    s32Ret = SAMPLE_COMM_AUDIO_StartAo(m_aoDev, m_aoChn, &stAioAttr,
                                       (AUDIO_SAMPLE_RATE_E) m_param.sampleRate,
                                       (CVI_BOOL) (m_aioSampleRate != m_param.sampleRate));
    if (s32Ret != CVI_SUCCESS) {
        emxloge("chn %d SAMPLE_COMM_AUDIO_StartAo failed 0x%x\n", m_chn, s32Ret);
        return ErrCodeE::Failure;
    }
    SpkEna(false);
    SetVolume(m_param.volume);
    CVI_AO_ClearChnBuf(m_aoDev, m_aoChn);
    m_decoder.reset(AudioCodecFactory::MakeDecoder(m_param.codec));
    m_decoder->Create(m_param.sampleRate, m_param.bitWidth);
    PadSilentPcm();
    MsgTopicSubscriberAsync::Config cfg = {};
    cfg.loop = &m_loopAdec;
    cfg.sendByPack = true;
    cfg.maxSend = 64;
    cfg.topicArray.emplace_back(MEDIA_TOPIC_ADEC_FRAME + m_chn);
    cfg.topicArray.emplace_back(MEDIA_TOPIC_ADEC_RAW_PCM + m_chn);
    cfg.topicArray.emplace_back(MEDIA_TOPIC_ADEC_VOICE_PALY + m_chn);
    cfg.topicArray.emplace_back(MEDIA_TOPIC_ADEC_PALY_CTRL + m_chn);
    cfg.buffer = m_buffer;
    cfg.size = sizeof(m_buffer);
    cfg.OnConnected = []() {};
    cfg.OnDisConnected = []() {};
    cfg.OnRecvMsg = std::bind(&MediaChipAdec::OnGetMsg, this, ph_1, ph_2, ph_3);
    m_sub.CreateMsgTopicSubscriberAsync(cfg);
    m_loopAdec.Start();
    return ErrCodeE::Success;
}

ErrCodeE MediaChipAdec::GetVqeParam(AO_VQE_CONFIG_S &vqe) {
    return ErrCodeE::OperationNotSupport;
}

void MediaChipAdec::OnGetMsg(uint32_t topic, const char *data, int32_t size) {
    if (topic == MEDIA_TOPIC_ADEC_FRAME + (uint32_t) m_chn) {
        if (!m_voiceQue.empty())
            return;
        SpkEna(true);
        Frame frame = {};
        memcpy(&frame, data, sizeof(frame));
        int32_t outNum;
        int16_t outData[EMX_AUDIO_MAX_FRAME_SIZE] = {};
        m_decoder->Decode((uint8_t *) data + sizeof(frame), frame.size, outData, outNum);
        SendToAo(outData, outNum);
    } else if (topic == MEDIA_TOPIC_ADEC_RAW_PCM + (uint32_t) m_chn) {
        if (!m_voiceQue.empty())
            return;
        SpkEna(true);
        Frame frame = {};
        memcpy(&frame, data, sizeof(frame));
        SendToAo((int16_t *) (data + sizeof(frame)), frame.size >> 1);
    } else if (topic == MEDIA_TOPIC_ADEC_VOICE_PALY + (uint32_t) m_chn) {
        VoiceFile file;
        memcpy(&file.volume, data, sizeof(file.volume));
        file.path = data + sizeof(file.volume);
        m_voiceQue.push(file);
        if (!m_playWork.IsWorking()) {
            RunPlayWork();
        }
    } else if (topic == MEDIA_TOPIC_ADEC_PALY_CTRL + (uint32_t) m_chn) {
        if (strcmp(data, "quit") == 0) {
            m_playQuit = true;
            while (!m_voiceQue.empty()) {
                emxlogd("cancel %s\n", m_voiceQue.front().path.data());
                m_voiceQue.pop();
            }
        }
    }
}


void MediaChipAdec::SendToAo(const int16_t *pcm, int32_t pktNum) {
    int off = 0;
    while (pktNum > 0) {
        AUDIO_FRAME_S audioFrame = {};
        int sendNum = pktNum > m_frameSize ? m_frameSize : pktNum;
        audioFrame.u64VirAddr[0] = (CVI_U8 *) (pcm + off);
        audioFrame.u32Len = sendNum;//samples size for each channel
        audioFrame.u64TimeStamp = 0;
        audioFrame.enSoundmode = m_soundMode;
        audioFrame.enBitwidth = m_bitWidth;
        CVI_S32 ret = CVI_AO_SendFrame(m_aoDev, m_aoChn, &audioFrame, 1000);
        if (ret != 0) {
            emxloge("chn %d send Frame Data error with %#x,sendNum=%d\n", m_chn, ret, sendNum);
        }
        off += sendNum;
        pktNum -= sendNum;
    }
}

void MediaChipAdec::RunPlayWork() {
    m_playQuit = false;
    SpkEna(true);
    auto &file = m_voiceQue.front();
    if (file.volume == -1) {
        SetVolumeTemp(m_param.volume);
    } else if (file.volume == -2) {
    } else {
        SetVolumeTemp(file.volume);
    }
    ErrCodeE e = m_playWork.Run(new std::string(file.path));
    m_voiceQue.pop();
    if (e != ErrCodeE::Success) {
        while (!m_voiceQue.empty()) m_voiceQue.pop();
        SetVolumeTemp(m_param.volume);
        SpkEna(false);
    }
}

void MediaChipAdec::PlayFile(void *arg) {
    auto path = (std::string *) arg;
    emxlogd("play file %s\n", path->c_str());
    VoicePlay::Play(path->c_str(), m_param.sampleRate, [this](int16_t *pcm, int num) {
        SendToAo(pcm, num);
    }, m_playQuit);
    //填充静音数据
    PadSilentPcm();
    //自动静音
    if (m_autoMute) {
        AO_CHN_STATE_S status = {};
        while (CVI_AO_QueryChnStat(m_aoDev, m_aoChn, &status) == CVI_SUCCESS &&
               status.u32ChnBusyNum >= (uint32_t) m_frameSize * 2) {
            usleep(100000);
        }
        SpkEna(false);
        emxlogd("chn %d spk disable\n", m_chn);
    }
}

void MediaChipAdec::PlayFileDone(ErrCodeE e, void *arg) {
    auto path = (std::string *) arg;
    emxlogd("play file %s done\n", path->c_str());
    delete path;
    if (!m_voiceQue.empty()) {
        RunPlayWork();
    } else {
        m_playQuit = true;
        SetVolumeTemp(m_param.volume);
    }
}

void MediaChipAdec::SpkEna(bool ena) {
    if (!m_autoMute)
        return;
    for (auto &e:m_spkGpioList)
        e->SetValue(ena ? Gpio::ValueE::Enable : Gpio::ValueE::Disable);
    m_muting = !ena;
}

void MediaChipAdec::PadSilentPcm() {
    auto pcm = malloc(m_frameSize * 2);
    if (!pcm) {
        emxloge("malloc pcm %d size failed\n", m_frameSize * 2);
        return;
    }
    memset(pcm, 0, m_frameSize * 2);
    //pad silent pcm
    int cnt = m_silentTimeMs * m_aioSampleRate / 1000 / m_frameSize;
    emxlogd("chn %d pad %d ms (cnt=%d) silent pcm\n", m_chn, m_silentTimeMs, cnt);
    while (cnt--) {
        SendToAo((const int16_t *) pcm, m_frameSize);
    }
    free(pcm);
}
