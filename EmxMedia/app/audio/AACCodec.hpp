//
// Created by xiong on 2020/6/18.
//

#ifndef EP2DXX_AACCODEC_HPP
#define EP2DXX_AACCODEC_HPP

#include "aacenc_lib.h"
#include "aacdecoder_lib.h"
#include "AudioCodec.hpp"

namespace Emx {

    class AACEncoder : public AudioEncoder {
    public:
        AACEncoder() {}

        virtual ~AACEncoder() {}

        ErrCodeE Create(int32_t bitRate, int32_t sampleRate, int32_t bitWidth) override;

        void Destroy() override;

        ErrCodeE Encode(int16_t *inData, int32_t inNum, uint8_t *outData, int32_t &outNum) override;

    private:
        HANDLE_AACENCODER m_handle;
    };

    class AACDecoder : public AudioDecoder {
    public:
        AACDecoder() {}

        virtual ~AACDecoder() {}

        ErrCodeE Create(int32_t sampleRate, int32_t bitWidth) override;

        void Destroy() override;

        ErrCodeE Decode(uint8_t *inData, int32_t inNum, int16_t *outData, int32_t &outNum) override;

    private:
        HANDLE_AACDECODER m_handle;
    };
}
#endif
