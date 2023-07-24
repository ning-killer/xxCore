///*
//* @Author: xiong
//* @Date: 2023/3/6
//*/
//
//#ifndef EMX_Flv_HPP
//#define EMX_Flv_HPP
//
//#include "MediaFmt.hpp"
//
//struct flv_muxer_t;
//struct flv_vec_t;
//
//namespace Emx {
//
//    class FlvWriter : public MediaWriter {
//    public:
//        FlvWriter() : MediaWriter(), m_flv(nullptr), m_muxer(nullptr), m_trackId(0) {}
//
//        virtual ~FlvWriter() {}
//
//        ErrCodeE Create(const char *path, std::vector<TrackInfo> &info, Json::Value *usr) override;
//
//        void Destroy() override;
//
//        ErrCodeE Write(Frame &frame) override;
//
//    private:
//        static int OnFlvMuxer(void *param, int type, const void *data, size_t bytes, uint32_t timestamp);
//
//    private:
//        void *m_flv;
//        flv_muxer_t *m_muxer;
//        int m_trackId;
//        std::map<int, TrackInfo> m_trackMap;
//    };
//
//    class FlvStreamer : public MediaStreamer {
//    public:
//        FlvStreamer() : m_flv(nullptr), m_muxer(nullptr), m_trackId(0) {}
//
//        ErrCodeE Create(std::vector<TrackInfo> &info, Json::Value *usr, OnGetStream cb) override;
//
//        void Destroy() override;
//
//        ErrCodeE Write(Frame &frame) override;
//
//    private:
//        static int OnWrite(void *param, const flv_vec_t *vec, int n);
//
//        static int OnMuxer(void *param, int type, const void *data, size_t bytes, uint32_t timestamp);
//
//    private:
//        void *m_flv;
//        flv_muxer_t *m_muxer;
//        int m_trackId;
//        std::map<int, TrackInfo> m_trackMap;
//        OnGetStream m_cb;
//    };
//}
//#endif //EMX_Flv_HPP
