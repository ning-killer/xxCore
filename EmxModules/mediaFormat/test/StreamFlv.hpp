/*
* @Author: xiong
* @Date: 2023/3/8
*/

#ifndef EMX_StreamFlv_HPP
#define EMX_StreamFlv_HPP

#include "Flv.hpp"

namespace Emx {

    class StreamFlv {
    public:
        StreamFlv() : m_stream(m_loop) {}

        void Create();

        void Destroy();

    private:
        void OnGetFrame(ErrCodeE e, MediaFrame &frame);

        ErrCodeE OnGetStream(std::vector<MediaStreamer::Data> &array);

    private:
        EuvLoop m_loop;
        MediaClientStreamAsync m_stream;
        FlvStreamer m_flv;
        using TrackPair = std::pair<MediaFrame::TypeE, int>;
        std::map<TrackPair, int> m_trackIdxMap;
        std::vector<MediaFmt::TrackInfo> m_mediaInfo;
    };

}

#endif //EMX_StreamFlv_HPP
