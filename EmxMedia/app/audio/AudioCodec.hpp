//
// Created by xiong on 2020/6/18.
//

#ifndef EP2DXX_AUDIOCODEC_HPP
#define EP2DXX_AUDIOCODEC_HPP

#include "media/MediaTypedef.hpp"

namespace Emx {
    class AudioEncoder {
    public:
        AudioEncoder() {}

        virtual ~AudioEncoder() {}

        virtual ErrCodeE Create(int32_t bitRate, int32_t sampleRate, int32_t bitWidth) = 0;

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


#endif //EP2DXX_AUDIOCODEC_HPP
