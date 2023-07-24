///*
//* @Author: xiong
//* @Date: 2023/3/2
//*/
//
//#ifndef EMX_Fmp4_HPP
//#define EMX_Fmp4_HPP
//
//#include "Mp4.hpp"
//
//struct fmp4_writer_t;
//struct mov_reader_t;
//struct mov_buffer_t;
//namespace Emx {
//    class Fmp4Writer : public Mp4Writer {
//    public:
//        Fmp4Writer() : Mp4Writer(), m_fp(nullptr), m_mov(nullptr) {}
//
//        virtual ~Fmp4Writer() {}
//
//        ErrCodeE Create(const char *path, Json::Value *usr) override;
//
//        void Destroy() override;
//
//        ErrCodeE Write(Frame &frame) override;
//
//    private:
//        fmp4_writer_t *m_mov;
//        std::map<int, TrackInfo> m_trackMap;
//    };
//
//
//    class Fmp4Reader : public Mp4Reader {
//    public:
//        Fmp4Reader() : Mp4Reader() {}
//
//        virtual ~Fmp4Reader() {}
//    };
//};
//#endif //EMX_Fmp4_HPP
