/*
* @Author: xiong
* @Date: 2023/3/6
*/

#include "Flv.hpp"
#include "flv-writer.h"
#include "flv-muxer.h"
#include "flv-proto.h"

using namespace Emx;

ErrCodeE FlvWriter::Create(const char *path, std::vector<TrackInfo> &info, Json::Value *usr) {
    m_flv = flv_writer_create(path);
    if (!m_flv) {
        emxloge("flv writer create failed\n");
        goto ERROR;
    }

    m_muxer = flv_muxer_create(OnFlvMuxer, this);
    if (!m_muxer) {
        emxloge("flv writer create failed\n");
        goto ERROR;
    }
//    struct flv_metadata_t metadata = {};
//    metadata.audiocodecid = 4;
//    metadata.audiodatarate = 16.1;
//    metadata.audiosamplerate = 48000;
//    metadata.audiosamplesize = 16;
//    metadata.stereo = true;
//    metadata.videocodecid = 7;
//    metadata.videodatarate = 64.0;
//    metadata.framerate = 25;
//    metadata.width = 1920;
//    metadata.height = 1080;
//    flv_muxer_metadata(e, &metadata);
//    m_trackMap.clear();
    for (auto &item:info) {
        item.trackId = m_trackId++;
        m_trackMap[item.trackId] = item;
    }
    return ErrCodeE::Success;
    ERROR:
    Destroy();
    return ErrCodeE::Failure;
}

void FlvWriter::Destroy() {
    if (m_muxer) {
        flv_muxer_destroy(m_muxer);
        m_muxer = nullptr;
    }
    if (m_flv) {
        flv_writer_destroy(m_flv);
        m_flv = nullptr;
    }
}

ErrCodeE FlvWriter::Write(MediaFmt::Frame &frame) {
    auto &item = m_trackMap[frame.trackId];
    if (item.type == MediaFmt::TrackTypeE::Video) {
        switch (item.info.video.codec) {
            case VideoCodecE::H264:
                flv_muxer_avc(m_muxer, frame.data, frame.size, (uint32_t) frame.pts, (uint32_t) frame.pts);
                break;
            case VideoCodecE::H265:
                flv_muxer_hevc(m_muxer, frame.data, frame.size, (uint32_t) frame.pts, (uint32_t) frame.pts);
                break;
            default:
                break;
        }
    } else if (item.type == MediaFmt::TrackTypeE::Audio) {
        switch (item.info.audio.codec) {
            case AudioCodecE::AAC:
                flv_muxer_aac(m_muxer, frame.data, frame.size, (uint32_t) frame.pts, (uint32_t) frame.pts);
                break;
            case AudioCodecE::G711A:
                flv_muxer_g711a(m_muxer, frame.data, frame.size, (uint32_t) frame.pts, (uint32_t) frame.pts);
                break;
            case AudioCodecE::G711U:
                flv_muxer_g711u(m_muxer, frame.data, frame.size, (uint32_t) frame.pts, (uint32_t) frame.pts);
                break;
            default:
                break;
        }
    }
    return ErrCodeE::Success;
}

int FlvWriter::OnFlvMuxer(void *param, int type, const void *data, size_t bytes, uint32_t timestamp) {
    auto flvWriter = (FlvWriter *) param;
    return flv_writer_input(flvWriter->m_flv, type, data, bytes, timestamp);
}
