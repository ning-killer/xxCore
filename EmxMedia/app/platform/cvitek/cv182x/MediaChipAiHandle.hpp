//
// Created by xiong on 2022/5/11.
//

#ifndef EMX_MediaChipAiHandle_HPP
#define EMX_MediaChipAiHandle_HPP
#include "cviai.h"

namespace Emx {

    class MediaChipAiHandle {
    public:
        virtual ~MediaChipAiHandle();

    private:
        MediaChipAiHandle();

        MediaChipAiHandle(const MediaChipAiHandle &) {};

        MediaChipAiHandle &operator=(const MediaChipAiHandle &) { return *this; }

    public:
        static MediaChipAiHandle *GetInst() {
            if (!m_inst)
                m_inst = new MediaChipAiHandle;
            return m_inst;
        }

    public:
        cviai_handle_t m_aiHandle;
        cviai_service_handle_t m_aiServiceHandle;
    private:
        static MediaChipAiHandle *m_inst;
    };
}

#endif //EMX_MediaChipAiHandle_HPP
