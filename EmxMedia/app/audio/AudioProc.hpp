//
// Created by xiong on 2022/4/15.
//

#ifndef EMX_AudioProc_HPP
#define EMX_AudioProc_HPP

#include "EmxCore.hpp"

namespace Emx {

    class AudioProc {
    public:
        static uint64_t ResampleS16(const int16_t *input, int16_t *output,
                                    int inSampleRate, int outSampleRate,
                                    uint64_t inputSize, uint32_t channels);

    };

}

#endif //EMX_AudioProc_HPP
