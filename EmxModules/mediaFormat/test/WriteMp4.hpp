///*
//* @Author: xiong
//* @Date: 2023/3/7
//*/
//
//#ifndef EMX_WriteMp4_HPP
//#define EMX_WriteMp4_HPP
//
//#include "Fmp4.hpp"
//
//namespace Emx {
//
//    class WriteMp4 {
//    public:
//        WriteMp4() : m_stream(m_loop) {}
//
//        void Create(const char *path);
//
//        void Destroy();
//
//    private:
//        void OnGetFrame(ErrCodeE e, MediaFrame &frame);
//
//    private:
//        EuvLoop m_loop;
//        MediaClientStreamAsync m_stream;
//        Fmp4Writer m_mp4;
//        using TrackPair = std::pair<MediaFrame::TypeE, int>;
//        std::map<TrackPair, int> m_trackIdxMap;
//        std::vector<MediaFmt::TrackInfo> m_mediaInfo;
//    };
//
//}
//
//#endif //EMX_WriteMp4_HPP
