/*
 * @Author: xiong
 * @Date: 2022/8/19
*/

#ifndef EAPIL_AvPush_HPP
#define EAPIL_AvPush_HPP

#include "OvdCtx.hpp"
#include "EmxMedia.hpp"

namespace Emx {
    class AvPush {
    public:
        AvPush() : m_created(false), m_stream(m_loop) {}

        struct Config {
            int ovdChn;
            int video_quality;
            bool rsk_encrypt;
        };

        ErrCodeE Create(OvdCtx *ctx);

        void Destroy();

        bool IsCreated() { return m_created; }

    private:
        void OnGetFrame(ErrCodeE e, MediaFrame &frame);

    private:
        bool m_created;
        Config m_cfg;
        EuvLoop m_loop;
        MediaClientStreamAsync m_stream;
        uint64_t m_lastVideoTsInMs;
    };
};

#endif //EAPIL_AvPush_HPP
