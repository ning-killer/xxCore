//
// Created by xiong on 2021/4/8.
//

#ifndef EMX_VoicePlay_HPP
#define EMX_VoicePlay_HPP

#include <functional>

namespace Emx {

    using AudioOutCallBack = std::function<void(int16_t *pcm, int num)>;

    class VoicePlay {
    public:
        static void Play(const char *path, int32_t devSampleRate, AudioOutCallBack cb, bool &quit);
    };

}

#endif //EMX_VoicePlay_HPP
