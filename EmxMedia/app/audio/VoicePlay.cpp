//
// Created by xiong on 2021/4/8.
//
#include "core/EmxLog.hpp"
#include "core/EmxUtils.hpp"
#include "VoicePlay.hpp"
#include "AudioCodec.hpp"
#include "wav.h"
#include <cmath>

using namespace Emx;

//static uint64_t Resample_s16(const int16_t *input, uint64_t inputSize, int16_t *output,
//                             int inSampleRate, int outSampleRate, uint32_t channels) {
//    if (input == NULL)
//        return 0;
//    uint64_t outputSize = (uint64_t) (inputSize * (double) outSampleRate / (double) inSampleRate);
//    outputSize -= outputSize % channels;
//    if (output == NULL)
//        return outputSize;
//    double stepDist = ((double) inSampleRate / (double) outSampleRate);
//    const uint64_t fixedFraction = (1LL << 32);
//    const double normFixed = (1.0 / (1LL << 32));
//    uint64_t step = ((uint64_t) lround(stepDist * fixedFraction + 0.5));
//    uint64_t curOffset = 0;
//    for (uint32_t i = 0; i < outputSize; i += 1) {
//        for (uint32_t c = 0; c < channels; c += 1) {
//            *output++ = (int16_t) (input[c] + (input[c + channels] - input[c]) * (
//                    (double) (curOffset >> 32) + (double) ((curOffset & (fixedFraction - 1)) * normFixed)
//            )
//            );
//        }
//        curOffset += step;
//        input += (curOffset >> 32) * channels;
//        curOffset &= (fixedFraction - 1);
//    }
//    return outputSize;
//}

//static void Resample(const int16_t *in, int inNum, int16_t *out, int &outNum,
//                     int inSampleRate, int outSampleRate, uint32_t channels) {
//    int off = 0;
//    auto output = (int8_t *) out;
//    for (int i = 0; i < inNum; i++) {
//        auto n = Resample_s16(in + i,
//                              2,
//                              (int16_t *) (output + off),
//                              inSampleRate,
//                              outSampleRate,
//                              channels);
//        off += (int) n;
//    }
//    outNum = off / 2;
//}

/*音频数据重采样，目前仅支持 8-16 16-8*/
static void ResampleSimple(const uint8_t *in, int inSize, uint8_t *out, int &outSize,
                           int inSampleRate, int outSampleRate) {

    if ((inSampleRate == 8000) && (outSampleRate == 16000)) {
        if (outSize < inSize * 2) {
            emxloge("outSize(%d) < inSize*2(%d)\n", outSize, inSize * 2);
            return;
        }
        outSize = 0;
        for (int i = 0; i < inSize;) {
            out[outSize++] = in[i];
            out[outSize++] = in[i + 1];
            out[outSize++] = in[i];
            out[outSize++] = in[i + 1];
            i += 2;
        }
    } else if ((inSampleRate == 16000) && (outSampleRate == 8000)) {
        if (outSize < inSize / 2) {
            emxloge("outSize(%d) < inSize/2(%d)\n", outSize, inSize / 2);
            return;
        }
        outSize = 0;
        for (int i = 0; i < inSize;) {
            out[outSize++] = (in[i] + in[i + 1]) / 2;
            i += 2;
        }
    } else {
        if (outSize < inSize) {
            emxloge("outSize(%d) < inSize(%d)\n", outSize, inSize);
            return;
        }
        memcpy(out, in, inSize);
    }
}
///*音频数据重采样，目前仅支持 8-16 16-8*/
//static void ResampleSimple(const int16_t *in, int inNum, int16_t *out, int &outNum,
//                           int inSampleRate, int outSampleRate) {
//
//    if ((inSampleRate == 8000) && (outSampleRate == 16000)) {
//        if (outNum < inNum * 2) {
//            emxloge("outNum(%d) < inNum*2(%d)\n", outNum, inNum * 2);
//            return;
//        }
//        outNum = 0;
//        for (int i = 0; i < inNum - 1; i++) {
//            out[outNum++] = in[i];
//            out[outNum++] = (int16_t) (((int) in[i] + (int) in[i + 1]) / 2);
//        }
//    } else if ((inSampleRate == 16000) && (outSampleRate == 8000)) {
//        if (outNum < inNum / 2) {
//            emxloge("outNum(%d) < inNum/2(%d)\n", outNum, inNum / 2);
//            return;
//        }
//        outNum = 0;
//        for (int i = 0; i < inNum;) {
//            out[outNum++] = (int16_t) (((int) in[i] + (int) in[i + 1]) / 2);
//            i += 2;
//        }
//    }
//}

//
//static void PlayFileAmrNb(const char *path, AudioOutCallBack cb, bool &quit) {
//    uint8_t buffer[500] = {};
//    int16_t outbuffer[160] = {};
//    int32_t outsize = 160;
//    FILE *fp = fopen(path, "rb");
//    if (!fp) {
//        emxloge("cannot open %s\n", path);
//        return;
//    }
//    int n = fread(buffer, 1, 6, fp);
//    if (n != 6 || memcmp(buffer, "#!AMR\n", 6) != 0) {
//        emxloge("Bad header\n");
//        fclose(fp);
//        return;
//    }
//    auto dec = AudioCodecFactory::MakeDecoder(AudioCodecE::AMRNB);
//    if (!dec) {
//        fclose(fp);
//        emxloge("create audio decoder failed\n");
//        return;
//    }
//    dec->Create(0, 0);
//
//    /* From WmfDecBytesPerFrame in dec_input_format_tab.cpp */
//    const int sizes[] = {12, 13, 15, 17, 19, 20, 26, 31, 5, 6, 5, 5, 0, 0, 0, 0};
//    while (!quit) {
//        int size;
//        /* Read the mode byte */
//        n = fread(buffer, 1, 1, fp);
//        if (n <= 0)
//            break;
//        /* Find the packet size */
//        size = sizes[(buffer[0] >> 3) & 0x0f];
//        n = fread(buffer + 1, 1, size, fp);
//        if (n != size)
//            break;
//        dec->Decode(buffer, size, (uint16_t *) outbuffer, outsize);
//        cb((uint16_t *) outbuffer, outsize);
//    }
//    dec->Destroy();
//    delete dec;
//    fclose(fp);
//}
//
//static void PlayFileAmrWb(const char *path, AudioOutCallBack cb, bool &quit) {
//    uint8_t buffer[500] = {};
//    int16_t outbuffer[320] = {};
//    int32_t outsize = 320;
//    FILE *fp = fopen(path, "rb");
//    if (!fp) {
//        emxloge("cannot open %s\n", path);
//        return;
//    }
//    int n = fread(buffer, 1, 9, fp);
//    if (n != 9 || memcmp(buffer, "#!AMR-WB\n", 9) != 0) {
//        emxloge("Bad header\n");
//        fclose(fp);
//        return;
//    }
//    auto dec = AudioCodecFactory::MakeDecoder(AudioCodecE::AMRWB);
//    if (!dec) {
//        fclose(fp);
//        emxloge("create audio decoder failed\n");
//        return;
//    }
//    dec->Create(0, 0);
//
//    /* From WmfDecBytesPerFrame in dec_input_format_tab.cpp */
//    const int sizes[] = {17, 23, 32, 36, 40, 46, 50, 58, 60, 5, -1, -1, -1, -1, -1, 0};
//    while (!quit) {
//        int size;
//        /* Read the mode byte */
//        n = fread(buffer, 1, 1, fp);
//        if (n <= 0)
//            break;
//        /* Find the packet size */
//        size = sizes[(buffer[0] >> 3) & 0x0f];
//        n = fread(buffer + 1, 1, size, fp);
//        if (n != size)
//            break;
//        dec->Decode(buffer, size, (uint16_t *) outbuffer, outsize);
//        cb((uint16_t *) outbuffer, outsize);
//    }
//    dec->Destroy();
//    delete dec;
//    fclose(fp);
//}
//
//static void PlayFileAmr(const char *path, AudioOutCallBack cb, bool &quit) {
//    FILE *fp = fopen(path, "rb");
//    if (!fp) {
//        emxloge("cannot open %s\n", path);
//        return;
//    }
//    char buf[10] = {};
//    fread(buf, 1, 9, fp);
//    fclose(fp);
//    if (memcmp(buf, "#!AMR-WB\n", 9) == 0) {
//        PlayFileAmrWb(path, cb, quit);
//    } else if (memcmp(buf, "#!AMR\n", 6) == 0) {
//        PlayFileAmrNb(path, cb, quit);
//    } else {
//        emxloge("unknown amr format %s\n", path);
//    }
//}
static const int aacSampleIndexTable[16] = {96000, 88200, 64000, 48000,
                                            44100, 32000, 24000, 22050,
                                            16000, 12000, 11025, 8000,
                                            7350, 0, 0, 0};

static void PlayFileAAC(const char *path, int32_t devSampleRate, AudioOutCallBack cb, bool &quit) {
//    FILE *fp_aac = fopen("/mnt/nfs/data/audio.aac", "wb+");
//    FILE *fp_pcm_org = fopen("/mnt/nfs/data/audio.pcm_org", "wb+");
//    FILE *fp_pcm_resample = fopen("/mnt/nfs/data/audio.pcm_resample", "wb+");
    FILE *fp = fopen(path, "rb");
    if (!fp) {
        emxloge("cannot open %s\n", path);
        return;
    }
    auto dec = AudioCodecFactory::MakeDecoder(AudioCodecE::AAC);
    if (!dec) {
        fclose(fp);
        emxloge("create audio decoder failed\n");
        return;
    }
    dec->Create(0, 0);
    char data0[4096];
    char data1[2048];
    int data0Size;
    int data1Size;
    while (!quit) {
        auto n = fread(data0, 1, 7, fp);
        if (n <= 0) {
            break;
        }
        data0Size = ((data0[3] & 0x03) << 11) | (data0[4] << 3) | (data0[5] >> 5);
        n = fread(data0 + 7, 1, data0Size - 7, fp);
        if ((int) n != data0Size - 7) {
            printf("Not an ADTS packet\n");
            break;
        }
//        fwrite(data0, data0Size, 1, fp_aac);
//        printf("%s:%s:%d aac size = %d\n", __FILE__, __FUNCTION__, __LINE__, data0Size);
        data1Size = sizeof(data1) / 2;
        dec->Decode((uint8_t *) data0, data0Size, (int16_t *) data1, data1Size);
//        fwrite(data1, data1Size * 2, 1, fp_pcm_org);
//        printf("%s:%s:%d org size = %d\n", __FILE__, __FUNCTION__, __LINE__, data1Size * 2);
        int orgSampleRate = aacSampleIndexTable[(data0[2] >> 2) & 0xf];
//        if (orgSampleRate != devSampleRate) {
//            data0Size = sizeof(data0) / 2;
//            ResampleSimple((int16_t *) data1, data1Size, (int16_t *) data0, data0Size, orgSampleRate,
//                           devSampleRate);
//            fwrite(data0, data0Size * 2, 1, fp_pcm_resample);
//            printf("%s:%s:%d resample size = %d\n", __FILE__, __FUNCTION__, __LINE__, data0Size);
////            cb((int16_t *) data0, data0Size);
//        } else {
////            cb((int16_t *) data1, data1Size);
//        }
        if (orgSampleRate != devSampleRate) {
            data0Size = sizeof(data0);
            ResampleSimple((uint8_t *) data1, data1Size * 2, (uint8_t *) data0, data0Size, orgSampleRate,
                           devSampleRate);
//            fwrite(data0, data0Size, 1, fp_pcm_resample);
//            printf("%s:%s:%d resample size = %d\n", __FILE__, __FUNCTION__, __LINE__, data0Size);
            cb((int16_t *) data0, data0Size / 2);
//            cb((int16_t *) data1, data1Size);
        } else {
            cb((int16_t *) data1, data1Size);
        }
    }
    dec->Destroy();
    delete dec;
    fclose(fp);
//    fclose(fp_aac);
//    fclose(fp_pcm_org);
//    fclose(fp_pcm_resample);
}


static void PlayFileWav(const char *path, int32_t devSampleRate, AudioOutCallBack cb, bool &quit) {
//    FILE *fp_alaw = fopen("/mnt/nfs/data/audio.alaw", "wb+");
//    FILE *fp_pcm_org = fopen("/mnt/nfs/data/audio.pcm_org", "wb+");
//    FILE *fp_pcm_resample = fopen("/mnt/nfs/data/audio.pcm_resample", "wb+");
    WavFile *wav = wav_open(path, WAV_OPEN_READ);
    if (!wav) {
        emxloge("cannot open %s\n", path);
        return;
    }
    AudioDecoder *dec;
    switch (wav_get_format(wav)) {
        case WAV_FORMAT_PCM:
            dec = AudioCodecFactory::MakeDecoder(Emx::AudioCodecE::LPCM);
            break;
        case WAV_FORMAT_ALAW:
            dec = AudioCodecFactory::MakeDecoder(Emx::AudioCodecE::G711A);
            break;
        case WAV_FORMAT_MULAW:
            dec = AudioCodecFactory::MakeDecoder(Emx::AudioCodecE::G711U);
            break;
        default:
            wav_close(wav);
            emxloge("wav format %d is unsupported\n", wav_get_format(wav));
            return;
    }
    if (!dec) {
        wav_close(wav);
        emxloge("create audio decoder failed\n");
        return;
    }
    dec->Create(0, 0);
    char data0[4096];
    char data1[2048];
    int data0Size;
    int data1Size;
    while (!quit) {
        auto n = wav_read(wav, data0, 320);
        if (n <= 0) {
            break;
        }
        data0Size = (int) wav_get_sample_size(wav) * (int) n;
//        fwrite(data0, inSize, 1, fp_alaw);
        data1Size = sizeof(data1) / 2;
        dec->Decode((uint8_t *) data0, data0Size, (int16_t *) data1, data1Size);
//        fwrite(data1, outSize * 2, 1, fp_pcm_org);
        int orgSampleRate = (int) wav_get_sample_rate(wav);
        if (orgSampleRate != devSampleRate) {
            data0Size = sizeof(data0);
            ResampleSimple((uint8_t *) data1, data1Size * 2, (uint8_t *) data0, data0Size, orgSampleRate,
                           devSampleRate);
//            fwrite(data0, data0Size, 1, fp_pcm_resample);
//            printf("%s:%s:%d resample size = %d\n", __FILE__, __FUNCTION__, __LINE__, data0Size);
            cb((int16_t *) data0, data0Size / 2);
//            cb((int16_t *) data1, data1Size);
        } else {
            cb((int16_t *) data1, data1Size);
        }
    }
    dec->Destroy();
    delete dec;
    wav_close(wav);
//    fclose(fp_alaw);
//    fclose(fp_pcm_org);
//    fclose(fp_pcm_resample);
}

static void PlayFilePcm(const char *path, AudioOutCallBack cb, bool &quit) {
    FILE *fp = fopen(path, "rb");
    if (!fp) {
        emxloge("cannot open %s\n", path);
        return;
    }
    unsigned char packet[2048];
    while (!quit) {
        auto n = fread(packet, 2, 1024, fp);
        if (n <= 0)
            break;
        cb((int16_t *) packet, (int) n);
    }
    fclose(fp);
}

void VoicePlay::Play(const char *path, int32_t devSampleRate, AudioOutCallBack cb, bool &quit) {
    if (!File::Exist(path)) {
        emxloge("cannot open %s\n", path);
        return;
    }
    if (strstr(path, ".aac")) {
        PlayFileAAC(path, devSampleRate, cb, quit);
//    } else if (strstr(path, ".amr")) {
//        PlayFileAmr(path, cb, quit);
    } else if (strstr(path, ".wav")) {
        PlayFileWav(path, devSampleRate, cb, quit);
    } else if (strstr(path, ".pcm")) {
        PlayFilePcm(path, cb, quit);
    } else {
        emxloge("nor supported voice format\n");
    }
}
