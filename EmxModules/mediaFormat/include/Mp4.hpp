/*
 * @Author: xiong
 * @Date: 2022/8/18
*/

#ifndef EMX_Mp4_HPP
#define EMX_Mp4_HPP

#include "MediaFmt.hpp"

struct mov_writer_t;
struct mov_reader_t;
struct mov_buffer_t;
namespace Emx {
    class Mp4Writer : public MediaWriter {
    public:
        Mp4Writer() : MediaWriter(), m_fp(nullptr), m_mov(nullptr) {}

        virtual ~Mp4Writer() {}

        ErrCodeE Create(const char *path, Json::Value *usr) override;

        void Destroy() override;

        int AddTrack(TrackInfo &info) override;

        ErrCodeE Write(Frame &frame) override;

    private:
        FILE *m_fp;
        mov_writer_t *m_mov;
        std::string m_usr;
        std::map<int, TrackInfo> m_trackMap;
    };

    class Mp4Reader : public MediaReader {
    public:
        Mp4Reader() : MediaReader(), m_fp(nullptr), m_mov(nullptr) {}

        virtual ~Mp4Reader() {}

        ErrCodeE Create(const char *path, std::vector<TrackInfo> &info, Json::Value *usr) override;

        void Destroy() override;

        ErrCodeE GetSpecInfo(SpecInfo &info) override {
            info = m_mediaInfo;
            return ErrCodeE::Success;
        }

        ErrCodeE Read(Frame &frame) override;

        ErrCodeE Seek(int64_t &pos) override;

    private:
        struct Arg {
            std::vector<TrackInfo> *info;
            Json::Value *usr;
        };

        static void OnVideo(void *param, uint32_t track, uint8_t object,
                            int width, int height, const void *extra, size_t bytes);

        static void OnAudio(void *param, uint32_t track, uint8_t object,
                            int channel_count, int bit_per_sample, int sample_rate, const void *extra, size_t bytes);

        static void OnSubtitle(void *param, uint32_t track, uint8_t object, const void *extra, size_t bytes);


        static void *OnAlloc(void *param, uint32_t track, size_t bytes, int64_t pts, int64_t dts, int flags);

        static uint8_t GetSamplingFrequencyIndex(int sampleRate);

    private:
        FILE *m_fp;
        mov_reader_t *m_mov;
        SpecInfo m_mediaInfo;
        struct Ctx {
            const static int DataReservedSize = 10;
            uint32_t track;
            std::string data;
            size_t bytes;
            int64_t pts;
            int flags;
            std::map<uint32_t, TrackInfo> trackMap;
        } m_ctx;
    };
};

#endif //EMX_Mp4_HPP