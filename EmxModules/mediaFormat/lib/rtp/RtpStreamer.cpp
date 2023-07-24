/*
* @Author: xiong
* @Date: 2023/3/8
*/

#include "Rtp.hpp"
#include "rtp-payload.h"
#include "rtp-profile.h"

using namespace Emx;


ErrCodeE RtpStreamer::Create(Json::Value *usr, MediaStreamer::OnGetStream cb) {
    m_handler = new rtp_payload_t;
    m_handler->alloc = RtpStreamer::OnAlloc;
    m_handler->free = RtpStreamer::OnFree;
    m_handler->packet = RtpStreamer::OnPacket;
    m_cb = std::move(cb);
    return ErrCodeE::Success;
}

void RtpStreamer::Destroy() {
    for (auto track:m_trackMap) {
        rtp_payload_encode_destroy(track.second.encoder);
    }
    m_trackMap.clear();
    m_trackIdCount = 0;
    delete m_handler;
}

int RtpStreamer::AddTrack(MediaFmt::TrackInfo &info) {
    int trackId = -1;
    void *encoder = nullptr;
    if (info.type == MediaFmt::TrackTypeE::Video) {
        auto &video = info.info.video;
        if (video.codec == VideoCodecE::H264)
            encoder = rtp_payload_encode_create(RTP_PAYLOAD_H264, "H264", 0, 66600, m_handler, this);
        else if (video.codec == VideoCodecE::H265)
            encoder = rtp_payload_encode_create(RTP_PAYLOAD_H265, "H265", 0, 66601, m_handler, this);
        else {
            emxloge("unsupported video codec %d\n", (int) video.codec);
            return -1;
        }
    } else if (info.type == MediaFmt::TrackTypeE::Audio) {
        auto &audio = info.info.audio;
        if (audio.codec == AudioCodecE::AAC)
            encoder = rtp_payload_encode_create(RTP_PAYLOAD_MP4A, "mpeg4-generic", 0, 77700, m_handler, this);
        else if (audio.codec == AudioCodecE::G711A)
            encoder = rtp_payload_encode_create(RTP_PAYLOAD_PCMA, "G711A", 0, 77701, m_handler, this);
        else if (audio.codec == AudioCodecE::G711U)
            encoder = rtp_payload_encode_create(RTP_PAYLOAD_PCMU, "G711U", 0, 77702, m_handler, this);
        else {
            emxloge("unsupported audio codec %d\n", (int) audio.codec);
            return -1;
        }
    } else {
        emxloge("unsupported type %d\n", (int) info.type);
        return -1;
    }
    if (!encoder) {
        emxloge("rtp_payload_encode_create failed\n");
        return -1;
    }
    trackId = m_trackIdCount++;
    info.trackId = trackId;
    Track track = {};
    track.info = info;
    track.encoder = encoder;
    m_trackMap[trackId] = track;
    return trackId;
}

ErrCodeE RtpStreamer::Write(MediaFmt::Frame &frame) {
    if (m_trackMap.count(frame.trackId) == 0)
        return ErrCodeE::ResNotExist;
//    if (m_trackMap[frame.trackId].info.type == MediaFmt::TrackTypeE::Audio &&
//        m_trackMap[frame.trackId].info.info.audio.codec == AudioCodecE::AAC) {
//        //mp4 skip adts
//        frame.data += 7;
//        frame.size -= 7;
//    }
    int ret = rtp_payload_encode_input(m_trackMap[frame.trackId].encoder,
                                       frame.data, frame.size,
                                       (uint32_t) frame.pts);
    return ret == 0 ? ErrCodeE::Success : ErrCodeE::Failure;
}

int RtpStreamer::OnPacket(void *param, const void *packet, int bytes, uint32_t timestamp, int flags) {
    auto streamer = (RtpStreamer *) param;
    std::vector<Data> array(1);
    array[0].data = packet;
    array[0].size = bytes;
    return streamer->m_cb(array) == ErrCodeE::Success ? 0 : -1;
}
