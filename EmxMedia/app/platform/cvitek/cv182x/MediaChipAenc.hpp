//
// Created by xiong on 2020/11/19.
//

#ifndef EMX_MediaChipAenc_HPP
#define EMX_MediaChipAenc_HPP

#include "AudioCodec.hpp"
#include "MediaServerAenc.hpp"
#include "cvi_comm_aenc.h"

namespace Emx {
    class MediaChipAenc : public MediaServerAenc {
    public:
        MediaChipAenc(int32_t chn, MediaServerResource &res) :
                MediaServerAenc(chn, res),
                m_got(false),
                m_started(false),
                m_seq(0),
                m_enSeq(0) {}

        virtual ~MediaChipAenc() {};

        ErrCodeE Start() override;

        ErrCodeE Stop() override;

        ErrCodeE SetParam(const Param &param) override;

        ErrCodeE GetParam(Param &param) override;

        ErrCodeE SetVolume(int32_t volume) override;

        ErrCodeE Mute(bool ena) override;

    protected:
        virtual void StartStream();

        virtual void StopStream();

        virtual void OnStream(void *arg);

        virtual void OnStreamComplete(ErrCodeE e, void *arg);

        virtual void PushAiFrame();

        virtual void PushAencFrame();

        virtual ErrCodeE Config();

        virtual ErrCodeE GetTalkVqeParam(AI_TALKVQE_CONFIG_S &vqe);

        virtual ErrCodeE SetChnParam();

        ErrCodeE SetVolumeTemp(int32_t volume);

        ErrCodeE MuteTemp(bool ena);

    protected:
        int32_t m_aiDev;
        int32_t m_aiChn;
        int32_t m_aencChn;
        bool m_bind;
        int32_t m_vqeType;
        EuvWork m_work;
        AUDIO_FRAME_S m_stFrame;
        AEC_FRAME_S m_stAecFrm;
        bool m_got;
        std::shared_ptr<AudioEncoder> m_encoder;
        bool m_started;
        int m_gainMin;
        int m_gainMax;
        int m_aioSampleRate;
        int m_frameSize;
        uint32_t m_seq;
        uint32_t m_enSeq;
    };
}

#endif //EMX_MediaChipAenc_HPP
