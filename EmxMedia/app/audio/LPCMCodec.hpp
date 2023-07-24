//
// Created by xiong on 2020/11/23.
//

#ifndef EMX_LPCMCODEC_HPP
#define EMX_LPCMCODEC_HPP

#include <cstring>
#include "AudioCodec.hpp"

namespace Emx {

    class LPCMEncoder : public AudioEncoder {
    public:
        LPCMEncoder() {}

        virtual ~LPCMEncoder() {}

        ErrCodeE Create(int32_t bitRate, int32_t sampleRate, int32_t bitWidth) override { return ErrCodeE::Success; }

        void Destroy() override {}

        ErrCodeE Encode(int16_t *inData, int32_t inNum, uint8_t *outData, int32_t &outNum) override {
            outNum = inNum << 1;
            memcpy(outData, inData, outNum);
            return ErrCodeE::Success;
        }
    };

    class LPCMDecoder : public AudioDecoder {
    public:
        LPCMDecoder() {}

        virtual ~LPCMDecoder() {}

        ErrCodeE Create(int32_t sampleRate, int32_t bitWidth) override { return ErrCodeE::Success; }

        void Destroy() override {}

        ErrCodeE Decode(uint8_t *inData, int32_t inNum, int16_t *outData, int32_t &outNum) override {
            outNum = inNum >> 1;
            memcpy(outData, inData, inNum);
            return ErrCodeE::Success;
        }
    };
}
#endif //EMX_LPCMCODEC_HPP
