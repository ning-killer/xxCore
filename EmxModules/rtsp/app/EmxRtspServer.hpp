//
// Created by xiong on 2022/4/8.
//

#ifndef EMX_EmxRtspServer_HPP
#define EMX_EmxRtspServer_HPP

#include "Rtsp.hpp"
#include "EmxCore.hpp"
#include "EmxMedia.hpp"
#include "RtspServer.h"
#include "MediaSource.h"

namespace Emx {

    class EmxRtspServer : public Rtsp {
    public:
        EmxRtspServer() : m_stream(m_loop) {}

        virtual ~EmxRtspServer() {}

        void Create(bool newThread);

        void Destroy();

    private:
        ErrCodeE GetParam(Param &param) override;

        ErrCodeE SetParam(const Param &param) override;

    private:
        void CreateRtspServer(Param &param);

        void OnFrameRecv(ErrCodeE e, MediaFrame &frame);

        ErrCodeE OnRecvMsg(uint32_t module, uint32_t method,
                           const char *data, int32_t size,
                           std::string &out);

        ErrCodeE ReStart();

    private:
        EuvLoop m_loop;
        EuvTimer m_restartTimer;
        char m_buffer[4096];
        MsgRpcRep m_resp;
        MediaClientStreamAsync m_stream;
        std::shared_ptr<RtspServer> m_rtsp;
        struct RtspMediaSource {
            std::shared_ptr<MediaSource> media;
            int vencChn;
            int aencChn;
            int sampleRate;
        };
        std::vector<RtspMediaSource> m_mediaArray;
    };

}

#endif //EMX_EmxRtspServer_HPP
