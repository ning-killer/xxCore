#ifndef EMX_MediaChipAdec_HPP
#define EMX_MediaChipAdec_HPP

#include <queue>
#include "AudioCodec.hpp"
#include "MediaServerAdec.hpp"
#include "EmxCore.hpp"

namespace Emx {
    class MediaChipAdec : public MediaServerAdec {
    public:
        MediaChipAdec(int32_t chn, MediaServerResource &res) :
                MediaServerAdec(chn, res),
                m_aoDev(0),
                m_aoChn(0),
                m_adecChn(0),
                m_bind(false),
                m_started(false),
                m_playQuit(false) {}

        virtual ~MediaChipAdec() {};

        ErrCodeE Start() override;

        ErrCodeE Stop() override;

        ErrCodeE SetParam(const Param &param) override;

        ErrCodeE GetParam(Param &param) override;

        ErrCodeE SetVolume(int32_t volume) override;

        ErrCodeE SetVolumeTemp(int32_t volume) override;

        PlayStatusE GetPlayStatus() override;

    protected:
        ErrCodeE Config();

        void PlayFile(void *arg);

        void PlayFileDone(ErrCodeE e, void *arg);

        void SpkEna(bool ena);

        void RunPlayWork();

        void SendToAo(const int16_t *pcm, int32_t pktNum);

        void PadSilentPcm();

        void OnGetMsg(uint32_t topic, const char *data, int32_t size);

    protected:
        EuvLoop m_loopAdec;
        MsgTopicSubscriberAsync m_sub;
        std::shared_ptr<AudioDecoder> m_decoder;
        struct VoiceFile {
            std::string path;
            int volume;
        };
        std::queue<VoiceFile> m_voiceQue;
        EuvWork m_playWork;
        char m_buffer[EMX_AUDIO_MAX_FRAME_SIZE * 2];
        int32_t m_aoDev;
        int32_t m_aoChn;
        int32_t m_adecChn;
        bool m_bind;
        bool m_started;
        bool m_playQuit;
        int m_gainMin;
        int m_gainMax;
        int m_aioSampleRate;
        int m_frameSize;
        int m_silentTimeMs;
        AUDIO_SOUND_MODE_E m_soundMode;
        AUDIO_BIT_WIDTH_E m_bitWidth;
        bool m_autoMute;
        std::list<Gpio *> m_spkGpioList;
        EuvTimer m_muteTimer;
        int m_muteCnt;
        bool m_muting;
    };
}
#endif //EMX_MediaChipAdec_HPP
