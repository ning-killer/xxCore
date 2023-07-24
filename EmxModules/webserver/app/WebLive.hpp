/*
* @Author: xiong
* @Date: 2023/3/13
*/

#ifndef EMX_WebLive_HPP
#define EMX_WebLive_HPP

#include "Rtp.hpp"
#include <queue>

struct mg_connection;
namespace Emx {

    class WebLive {
    public:
        WebLive() : created(false), m_stream(m_loop) {};

        void Create(mg_connection *c, int chn);

        void Destroy();

        void SendRtpFrame();

        void RecvPcm(uint8_t *data, int size);

    private:

        ErrCodeE OnRtpFrame(std::vector<MediaStreamer::Data> &array);

    private:
        bool created;
        EuvLoop m_loop;
        RtpStreamer m_rtp;
        bool m_waitKeyFrame;
        int m_chnId;
        MediaClientStreamAsync m_stream;
        mg_connection *m_connection;
        using TrackPair = std::pair<MediaFrame::TypeE, int>;
        std::map<TrackPair, int> m_trackIdxMap;
        std::queue<MediaFrame> m_frameList;
        std::mutex m_mutex;
    };
}

#endif //EMX_WebLive_HPP
