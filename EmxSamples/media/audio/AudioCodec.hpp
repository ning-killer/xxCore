#ifndef DEMO_EP2DXX_AUDIOCODEC_HPP
#define DEMO_EP2DXX_AUDIOCODEC_HPP

#include "EmxCore.hpp"

namespace Emx {
    //!@brief 音频编码类型
    enum AudioCodecE : uint8_t {
        LPCM = 0, //!<LPCM编码
        AAC, //!<AAC编码
        G711A, //!<G711A编码
        G711U, //!<G711U编码
        G726, //!<G726编码
        BUTT
    };
    class AudioEncoder {
    public:
        AudioEncoder() {}

        virtual ~AudioEncoder() {}

        virtual ErrCodeE Create(int32_t bitRate, int32_t sampleRate, int32_t bitWidth, int32_t channelNum) = 0;

        virtual ErrCodeE Encode(int16_t *inData, int32_t inNum, uint8_t *outData, int32_t &outNum) = 0;

        virtual void Destroy() = 0;
    };

    class AudioDecoder {
    public:
        AudioDecoder() {}

        virtual ~AudioDecoder() {}

        virtual ErrCodeE Create(int32_t sampleRate, int32_t bitWidth) = 0;

        virtual ErrCodeE Decode(uint8_t *inData, int32_t inNum, int16_t *outData, int32_t &outNum) = 0;

        virtual void Destroy() = 0;

    };

    class AudioCodecFactory {
    public:
        static AudioEncoder *MakeEncoder(AudioCodecE codec);

        static AudioDecoder *MakeDecoder(AudioCodecE codec);
    };
}


#endif //DEMO_EP2DXX_AUDIOCODEC_HPP
