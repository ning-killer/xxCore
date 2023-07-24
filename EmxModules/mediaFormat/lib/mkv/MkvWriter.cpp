/*
* @Author: xiong
* @Date: 2023/3/8
*/

#include <unistd.h>
#include "Mkv.hpp"
#include "mkv-buffer.h"
#include "mkv-writer.h"

using namespace Emx;

static int mkv_file_read(void *fp, void *data, uint64_t bytes) {
    if (bytes == fread(data, 1, bytes, (FILE *) fp))
        return 0;
    return 0 != ferror((FILE *) fp) ? ferror((FILE *) fp) : -1 /*EOF*/;
}

static int mkv_file_write(void *fp, const void *data, uint64_t bytes) {
    return bytes == fwrite(data, 1, bytes, (FILE *) fp) ? 0 : ferror((FILE *) fp);
}

static int mkv_file_seek(void *fp, int64_t offset) {
    return fseek((FILE *) fp, offset, offset >= 0 ? SEEK_SET : SEEK_END);
}

static int64_t mkv_file_tell(void *fp) {
    return ftell((FILE *) fp);
}

struct mkv_buffer_t buffer = {
        mkv_file_read,
        mkv_file_write,
        mkv_file_seek,
        mkv_file_tell
};


ErrCodeE MkvWriter::Create(const char *path, Json::Value *usr) {
    m_fp = fopen(path, "wb+");
    if (!m_fp) {
        emxloge("cannot open %s\n", path);
        goto ERROR;
    }

    m_mkv = mkv_writer_create(&buffer, this, MKV_OPTION_LIVE);
    if (!m_mkv) {
        emxloge("mkv writer create failed\n");
        goto ERROR;
    }
    if (usr) {
        m_user = *usr;
    }
    return ErrCodeE::Success;
    ERROR:
    if (m_mkv) {
        mkv_writer_destroy(m_mkv);
        m_mkv = nullptr;
    }
    if (m_fp) {
        fclose(m_fp);
        m_fp = nullptr;
        remove(path);
    }
    return ErrCodeE::Failure;
}

int MkvWriter::AddTrack(MediaFmt::TrackInfo &info) {
    int trackId = -1;
    if (info.type == MediaFmt::TrackTypeE::Video) {
        auto &video = info.info.video;
        mkv_codec_t codec;
        if (video.codec == VideoCodecE::H264)
            codec = MKV_CODEC_VIDEO_H264;
        else if (video.codec == VideoCodecE::H265)
            codec = MKV_CODEC_VIDEO_H265;
        else {
            emxloge("unsupported video codec %d\n", (int) video.codec);
            return -1;
        }
        std::string usr;
        if (!m_user.empty()) {
            EasyJson::Serialize(m_user, usr);
            m_user.clear();
        }
        trackId = mkv_writer_add_video(m_mkv, codec,
                                       video.width, video.height,
                                       usr.data(), usr.size());
        if (trackId < 0) {
            emxloge("mov_writer_add_video failed\n");
            return -1;
        }
    } else if (info.type == MediaFmt::TrackTypeE::Audio) {
        auto &audio = info.info.audio;
        mkv_codec_t codec;
        if (audio.codec == AudioCodecE::AAC)
            codec = MKV_CODEC_AUDIO_AAC;
        else if (audio.codec == AudioCodecE::G711A)
            codec = MKV_CODEC_AUDIO_;
        else {
            emxloge("unsupported video codec %d\n", (int) video.codec);
            return -1;
        }
    }

    info.trackId = trackId;
    m_trackMap[trackId] = info;
    return trackId;
}

void MkvWriter::Destroy() {

}

ErrCodeE MkvWriter::Write(MediaFmt::Frame &frame) {
    return ErrCodeE::Timeout;
}
