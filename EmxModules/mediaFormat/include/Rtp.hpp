/*
* @Author: xiong
* @Date: 2023/3/8
*/

#ifndef EMX_Rtp_HPP
#define EMX_Rtp_HPP

#include "MediaFmt.hpp"

struct rtp_payload_t;
namespace Emx {

    class RtpWriter : public MediaWriter {
    public:
        RtpWriter() : MediaWriter(), m_rtp(nullptr), m_trackId(0) {}

        virtual ~RtpWriter() {}

        ErrCodeE Create(const char *path, Json::Value *usr) override;

        int AddTrack(TrackInfo &info) override;

        void Destroy() override;

        ErrCodeE Write(Frame &frame) override;

    private:
        static int OnFlvMuxer(void *param, int type, const void *data, size_t bytes, uint32_t timestamp);

    private:
        void *m_rtp;
        int m_trackId;
        std::map<int, TrackInfo> m_trackMap;
    };

    class RtpStreamer : public MediaStreamer {
    public:
        RtpStreamer() : m_trackIdCount(0) {}

        ErrCodeE Create(Json::Value *usr, OnGetStream cb) override;

        void Destroy() override;

        int AddTrack(TrackInfo &info) override;

        ErrCodeE Write(Frame &frame) override;

    private:

        static void *OnAlloc(void *param, int bytes) { return malloc(bytes); }

        static void OnFree(void *param, void *packet) { if (packet)free(packet); }

        /// @return 0-ok, other-error
        static int OnPacket(void *param, const void *packet, int bytes, uint32_t timestamp, int flags);

    private:
        void *m_rtp;
        int m_trackIdCount;
        struct rtp_payload_t *m_handler;
        struct Track {
            void *encoder;
            TrackInfo info;
        };
        std::map<int, Track> m_trackMap;
        OnGetStream m_cb;
    };


}
#endif //EMX_Rtp_HPP
