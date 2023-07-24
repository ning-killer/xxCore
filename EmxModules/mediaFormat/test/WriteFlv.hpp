/*
* @Author: xiong
* @Date: 2023/3/8
*/

#ifndef EMX_WriteFlv_HPP
#define EMX_WriteFlv_HPP
#include "Flv.hpp"

namespace Emx {

    class WriteFlv {
    public:
        WriteFlv() : m_stream(m_loop) {}

        void Create(const char *path);

        void Destroy();

    private:
        void OnGetFrame(ErrCodeE e, MediaFrame &frame);

    private:
        EuvLoop m_loop;
        MediaClientStreamAsync m_stream;
        FlvWriter m_flv;
        using TrackPair = std::pair<MediaFrame::TypeE, int>;
        std::map<TrackPair, int> m_trackIdxMap;
        std::vector<MediaFmt::TrackInfo> m_mediaInfo;
    };

}

#endif //EMX_WriteFlv_HPP
