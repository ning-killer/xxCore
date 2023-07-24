/*
* @Author: xiong
* @Date: 2023/3/3
*/

#ifndef EMX_MediaFmt_HPP
#define EMX_MediaFmt_HPP

#include "EmxMedia.hpp"
#include "EmxCore.hpp"

namespace Emx {

    class MediaFmt {
    public:
        enum class TrackTypeE : uint8_t {
            Video,
            Audio,
            Data,
        };
        struct Frame {
            TrackTypeE type;
            int trackId;//same as trackId in TrackInfo
            bool isKeyFrame;
            uint8_t *data;
            int32_t size;
            uint64_t pts;//timestamp in millisecond
        };
        struct TrackInfo {
            TrackTypeE type;
            int trackId;
            union {
                struct Video {
                    VideoCodecE codec;
                    int width;
                    int height;
                } video;
                struct Audio {
                    AudioCodecE codec;
                    int sampleRate;
                    int channel;
                    int bitWidth;
                } audio;
                struct Data {
                    enum class TypeE : uint8_t {
                        Gyro,
                    } type;
                };
            } info;
        };
    };

    class MediaReader : public MediaFmt {
    public:
        struct SpecInfo {
            uint64_t duration;
        };

        virtual ErrCodeE Create(const char *path, std::vector<TrackInfo> &info, Json::Value *usr) = 0;

        virtual void Destroy() = 0;

        virtual ErrCodeE GetSpecInfo(SpecInfo &info) { return ErrCodeE::OperationNotSupport; }

        virtual ErrCodeE Read(Frame &frame) = 0;

        virtual ErrCodeE Seek(int64_t &pos) = 0;
    };

    class MediaWriter : public MediaFmt {
    public:
        virtual ErrCodeE Create(const char *path, Json::Value *usr) = 0;

        virtual void Destroy() = 0;

        virtual int AddTrack(TrackInfo &info) = 0;

        virtual ErrCodeE Write(Frame &frame) = 0;
    };

    class MediaStreamer : public MediaFmt {
    public:
        struct Data {
            const void *data;
            int size;
        };
        using OnGetStream = std::function<Emx::ErrCodeE(std::vector<Data> &array)>;

        virtual ErrCodeE Create(Json::Value *usr, OnGetStream cb) = 0;

        virtual void Destroy() = 0;

        virtual int AddTrack(TrackInfo &info) = 0;

        virtual ErrCodeE Write(Frame &frame) = 0;
    };

}
#endif //EMX_MediaFmt_HPP
