//
// Created by xiong on 2020/6/18.
//

#ifndef EP2DXX_G711CODEC_HPP
#define EP2DXX_G711CODEC_HPP

#include "AudioCodec.hpp"

namespace Emx {

    class AlawEncoder : public AudioEncoder {
    public:
        AlawEncoder();

        virtual ~AlawEncoder() {}

        ErrCodeE Create(int32_t bitRate, int32_t sampleRate, int32_t bitWidth, int32_t channelNum) override { return ErrCodeE::Success; }

        void Destroy() override {};

        ErrCodeE Encode(int16_t *inData, int32_t inNum, uint8_t *outData, int32_t &outNum) override;
    };

    class AlawDecoder : public AudioDecoder {
    public:
        AlawDecoder();

        virtual ~AlawDecoder() {}

        ErrCodeE Create(int32_t sampleRate, int32_t bitWidth) override { return ErrCodeE::Success; }

        void Destroy() override {};

        ErrCodeE Decode(uint8_t *inData, int32_t inSize, int16_t *outData, int32_t &outSize) override;
    };

    class UlawEncoder : public AudioEncoder {
    public:
        UlawEncoder();

        virtual ~UlawEncoder() {}

        ErrCodeE Create(int32_t bitRate, int32_t sampleRate, int32_t bitWidth, int32_t channelNum) override { return ErrCodeE::Success; }

        void Destroy() override {};

        ErrCodeE Encode(int16_t *inData, int32_t inNum, uint8_t *outData, int32_t &outNum) override;

    };

    class UlawDecoder : public AudioDecoder {
    public:
        UlawDecoder();

        virtual ~UlawDecoder() {}

        ErrCodeE Create(int32_t sampleRate, int32_t bitWidth) override { return ErrCodeE::Success; }

        void Destroy() override {};

        ErrCodeE Decode(uint8_t *inData, int32_t inSize, int16_t *outData, int32_t &outSize) override;

    };
}


#endif //EP2DXX_G711CODEC_HPP
