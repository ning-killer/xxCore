//
// Created by xiong on 2020/6/18.
//

#include <mutex>
#include "G711Codec.hpp"

using namespace Emx;
static uint8_t *linear2alawTable = nullptr;//[16384];
static uint8_t *linear2ulawTable = nullptr;//[16384];
static int16_t *alaw2linearTable = nullptr;//[256];
static int16_t *ulaw2linearTable = nullptr;//[256];
static std::mutex initMutex;

#define         SIGN_BIT        (0x80)      /* Sign bit for a A-law byte. */
#define         QUANT_MASK      (0xf)       /* Quantization field mask. */
#define         NSEGS           (8)         /* Number of A-law segments. */
#define         SEG_SHIFT       (4)         /* Left shift for segment number. */
#define         SEG_MASK        (0x70)      /* Segment field mask. */

#define         BIAS            (0x84)      /* Bias for linear code. */

/* alaw2linear() - Convert an A-law value to 16-bit linear PCM */
static int alaw2linear(unsigned char a_val) {
    int t;
    int seg;

    a_val ^= 0x55;

    t = a_val & QUANT_MASK;
    seg = ((unsigned) a_val & SEG_MASK) >> SEG_SHIFT;
    if (seg) t = (t + t + 1 + 32) << (seg + 2);
    else t = (t + t + 1) << 3;

    return (a_val & SIGN_BIT) ? t : -t;
}

static int ulaw2linear(unsigned char u_val) {
    int t;

    /* Complement to obtain normal u-law value. */
    u_val = ~u_val;

    /*
     * Extract and bias the quantization bits. Then
     * shift up by the segment number and subtract out the bias.
     */
    t = ((u_val & QUANT_MASK) << 3) + BIAS;
    t <<= ((unsigned) u_val & SEG_MASK) >> SEG_SHIFT;

    return (u_val & SIGN_BIT) ? (BIAS - t) : (t - BIAS);
}

static void build_xlaw_table(uint8_t *&linear_to_xlaw,
                             int (*xlaw2linear)(unsigned char),
                             int mask) {
    int i, j, v, v1, v2;
    linear_to_xlaw = (uint8_t *) malloc(16384);
    j = 1;
    linear_to_xlaw[8192] = mask;
    for (i = 0; i < 127; i++) {
        v1 = xlaw2linear(i ^ mask);
        v2 = xlaw2linear((i + 1) ^ mask);
        v = (v1 + v2 + 4) >> 3;
        for (; j < v; j += 1) {
            linear_to_xlaw[8192 - j] = (i ^ (mask ^ 0x80));
            linear_to_xlaw[8192 + j] = (i ^ mask);
        }
    }
    for (; j < 8192; j++) {
        linear_to_xlaw[8192 - j] = (127 ^ (mask ^ 0x80));
        linear_to_xlaw[8192 + j] = (127 ^ mask);
    }
    linear_to_xlaw[0] = linear_to_xlaw[1];
}

static void build_linear_table(int16_t *&xlaw2linearTable, int (*xlaw2linear)(unsigned char)) {
    xlaw2linearTable = (int16_t *) malloc(256 * 2);
    for (int i = 0; i < 256; i++)
        xlaw2linearTable[i] = xlaw2linear(i);
}

AlawEncoder::AlawEncoder() {
    initMutex.lock();
    if (!linear2alawTable) {
        build_xlaw_table(linear2alawTable, alaw2linear, 0xd5);
    }
    initMutex.unlock();
}

ErrCodeE AlawEncoder::Encode(int16_t *inData, int32_t inNum, uint8_t *outData, int32_t &outNum) {
    int v;
    auto samples = inData;
    outNum = 0;
    while (inNum--) {
        v = *samples++;
        *outData++ = linear2alawTable[(v + 32768) >> 2];
        outNum++;
    }
    return ErrCodeE::Success;
}

AlawDecoder::AlawDecoder() {
    initMutex.lock();
    if (!alaw2linearTable) {
        build_linear_table(alaw2linearTable, alaw2linear);
    }
    initMutex.unlock();
}

ErrCodeE AlawDecoder::Decode(uint8_t *inData, int32_t inNum, int16_t *outData, int32_t &outNum) {
    outNum = 0;
    while (inNum--) {
        *outData++ = alaw2linearTable[*inData++];
        outNum++;
    }
    return ErrCodeE::Success;
}


UlawEncoder::UlawEncoder() {
    initMutex.lock();
    if (!linear2ulawTable) {
        build_xlaw_table(linear2ulawTable, ulaw2linear, 0xff);
    }
    initMutex.unlock();
}

ErrCodeE UlawEncoder::Encode(int16_t *inData, int32_t inNum, uint8_t *outData, int32_t &outNum) {
    int v;
    auto samples = inData;
    outNum = 0;
    while (inNum--) {
        v = *samples++;
        *outData++ = linear2ulawTable[(v + 32768) >> 2];
        outNum++;
    }
    return ErrCodeE::Success;
}

UlawDecoder::UlawDecoder() {
    initMutex.lock();
    if (!ulaw2linearTable) {
        build_linear_table(ulaw2linearTable, ulaw2linear);
    }
    initMutex.unlock();
}

ErrCodeE UlawDecoder::Decode(uint8_t *inData, int32_t inNum, int16_t *outData, int32_t &outNum) {
    outNum = 0;
    while (inNum--) {
        *outData++ = ulaw2linearTable[*inData++];
        outNum++;
    }
    return ErrCodeE::Success;
}
