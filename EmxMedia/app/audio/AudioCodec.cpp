//
// Created by xiong on 2020/11/23.
//
#include "core/EmxLog.hpp"
#include "AudioCodec.hpp"
#include "AACCodec.hpp"
#include "G711Codec.hpp"
#include "LPCMCodec.hpp"

using namespace Emx;

AudioEncoder *AudioCodecFactory::MakeEncoder(AudioCodecE codec) {
    switch (codec) {
        case AudioCodecE::AAC:
            return new AACEncoder;
        case AudioCodecE::LPCM:
            return new LPCMEncoder;
        case AudioCodecE::G711A:
            return new AlawEncoder;
        case AudioCodecE::G711U:
            return new UlawEncoder;
        default:
            emxloge("not supported codec %d\n", codec);
            return nullptr;
    }
}

AudioDecoder *AudioCodecFactory::MakeDecoder(AudioCodecE codec) {
    switch (codec) {
        case AudioCodecE::AAC:
            return new AACDecoder;
        case AudioCodecE::LPCM:
            return new LPCMDecoder;
        case AudioCodecE::G711A:
            return new AlawDecoder;
        case AudioCodecE::G711U:
            return new UlawDecoder;
        default:
            emxloge("not supported codec %d\n", codec);
            return nullptr;
    }
}
