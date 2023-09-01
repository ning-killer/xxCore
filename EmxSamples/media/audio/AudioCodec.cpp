#include "core/EmxLog.hpp"
#include "AudioCodec.hpp"
#include "AACCodec.hpp"

using namespace Emx;

AudioEncoder *AudioCodecFactory::MakeEncoder(AudioCodecE codec) {
    switch (codec) {
        case AudioCodecE::AAC:
            return new AACEncoder;
        default:
            emxloge("not supported codec %d\n", codec);
            return nullptr;
    }
}

AudioDecoder *AudioCodecFactory::MakeDecoder(AudioCodecE codec) {
    switch (codec) {
        case AudioCodecE::AAC:
            return new AACDecoder;
        default:
            emxloge("not supported codec %d\n", codec);
            return nullptr;
    }
}
