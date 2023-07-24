/*
* @Author: xiong
* @Date: 2023/3/6
*/
#include <Rtp.hpp>

#include "Flv.hpp"
#include "flv-writer.h"
#include "flv-muxer.h"
#include "flv-proto.h"

using namespace Emx;

ErrCodeE FlvStreamer::Create(std::vector<TrackInfo> &info, Json::Value *usr, MediaStreamer::OnGetStream cb) {
    m_cb = std::move(cb);
    int hasVideo = 0;
    int hasAudio = 0;
    struct flv_metadata_t metadata = {};
    std::string s;
    m_trackMap.clear();
    for (auto &track:info) {
        if (track.type == MediaFmt::TrackTypeE::Video) {
            hasVideo = 1;
            metadata.video.codec = (int) track.info.video.codec;
            metadata.video.width = track.info.video.width;
            metadata.video.height = track.info.video.height;
        } else if (track.type == MediaFmt::TrackTypeE::Audio) {
            hasAudio = 1;
            metadata.audio.codec = (int) track.info.audio.codec;
            metadata.audio.samplerate = track.info.audio.sampleRate;
            metadata.audio.bitwidth = track.info.audio.bitWidth;
            metadata.audio.channels = track.info.audio.channel;
        }
        //填充trackId并做好映射记录
        track.trackId = m_trackId++;
        m_trackMap[track.trackId] = track;
    }

    if (usr) {
        EasyJson::Serialize(*usr, s);
        metadata.user.data = s.data();
        metadata.user.size = (int) s.size();
    }

    m_flv = flv_writer_create2(hasAudio, hasVideo, OnWrite, this);
    if (!m_flv) {
        emxloge("flv writer create failed\n");
        goto ERROR;
    }
    m_muxer = flv_muxer_create(OnMuxer, this);
    if (!m_muxer) {
        emxloge("flv writer create failed\n");
        goto ERROR;
    }
    flv_muxer_metadata(m_muxer, &metadata);
    return ErrCodeE::Success;
    ERROR:
    Destroy();
    return ErrCodeE::Failure;
}

void FlvStreamer::Destroy() {
    if (m_muxer) {
        flv_muxer_destroy(m_muxer);
        m_muxer = nullptr;
    }
    if (m_flv) {
        flv_writer_destroy(m_flv);
        m_flv = nullptr;
    }
}

ErrCodeE FlvStreamer::Write(MediaFmt::Frame &frame) {
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


int FlvStreamer::OnWrite(void *param, const struct flv_vec_t *vec, int n) {
    auto flvStreamer = (FlvStreamer *) param;
    std::vector<Data> array;
    for (int i = 0; i < n; i++) {
        Data data = {};
        data.data = (vec + i)->ptr;
        data.size = (vec + i)->len;
        array.emplace_back(data);
    }
    return (int) flvStreamer->m_cb(array);
}

int FlvStreamer::OnMuxer(void *param, int type, const void *data, size_t bytes, uint32_t timestamp) {
    auto flvStreamer = (FlvStreamer *) param;
    return flv_writer_input(flvStreamer->m_flv, type, data, bytes, timestamp);
}
