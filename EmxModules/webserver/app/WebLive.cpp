/*
* @Author: xiong
* @Date: 2023/3/13
*/

#include "WebLive.hpp"
#include "mongoose.h"

using namespace Emx;

void WebLive::Create(mg_connection *c, int chn) {
    Destroy();
    m_connection = c;
    m_waitKeyFrame = true;
    m_loop.Init("web-stream", [this]() {
        m_stream.Stop();
    });
    m_rtp.Create(nullptr, std::bind(&WebLive::OnRtpFrame, this, ph_1));
    Json::Value reply;
    {
        MediaVenc::Param param = {};
        MediaClientVenc venc(chn);
        if (venc.GetParam(param) == ErrCodeE::Success) {
            MediaFmt::TrackInfo info = {};
            info.type = MediaFmt::TrackTypeE::Video;
            auto &video = info.info.video;
            video.codec = param.codec;
            video.width = param.width;
            video.height = param.height;
            m_trackIdxMap[TrackPair(MediaFrame::TypeE::Venc, chn)] = m_rtp.AddTrack(info);
            if (param.codec == VideoCodecE::H265)
                reply["vcodec"] = "h265";
            else
                reply["vcodec"] = "h264";
        } else {
            emxloge("venc %d get param failed\n", 0);
        }
    }
    {
        MediaAenc::Param param = {};
        MediaClientAenc aenc(0);
        if (aenc.GetParam(param) == ErrCodeE::Success) {
            MediaFmt::TrackInfo info = {};
            info.type = MediaFmt::TrackTypeE::Audio;
            auto &audio = info.info.audio;
            audio.codec = param.codec;
            audio.sampleRate = param.sampleRate;
            audio.bitWidth = param.bitWidth;
            audio.channel = 1;
            m_trackIdxMap[TrackPair(MediaFrame::TypeE::Aenc, 0)] = m_rtp.AddTrack(info);
            if (param.codec == AudioCodecE::G711A)
                reply["acodec"] = "alaw";
            else if (param.codec == AudioCodecE::G711U)
                reply["acodec"] = "ulaw";
            else if (param.codec == AudioCodecE::LPCM)
                reply["acodec"] = "pcm";
            else
                reply["acodec"] = "aac";
        } else {
            emxloge("aenc %d get param failed\n", 0);
        }
    }
    {
        MediaAenc::Param param = {};
        MediaClientAenc adec(0);
        if (adec.GetParam(param) == ErrCodeE::Success) {
            reply["talkbackSampleRate"] = param.sampleRate;
        } else {
            emxloge("aenc %d get param failed\n", 0);
        }
    }
    std::string replyStr;
    EasyJson::Serialize(reply, replyStr);
    mg_ws_send(m_connection, replyStr.data(), replyStr.size(), WEBSOCKET_OP_TEXT);
    MediaClientStream::Channel channel;
    channel.Add(MediaFrame::TypeE::Venc, chn);
    channel.Add(MediaFrame::TypeE::Aenc, 0);
    m_stream.Start(channel, nullptr, [this](MediaFrame &frame) {
        m_mutex.lock();
        m_frameList.push(frame);
        m_mutex.unlock();
    });
    m_loop.Start();
    created = true;
}

void WebLive::Destroy() {
    if (created) {
        m_loop.StopAndDeInit();
        m_rtp.Destroy();
        while (!m_frameList.empty()) {
            m_frameList.pop();
        }
        created = false;
    }
}

void WebLive::SendRtpFrame() {
    m_mutex.lock();
    if (m_frameList.empty()) {
        m_mutex.unlock();
        return;
    }
    MediaFrame frame = m_frameList.front();
    m_frameList.pop();
    m_mutex.unlock();
    auto e = m_stream.GetFrameByInfo(frame);
    if (e != ErrCodeE::Success)
        return;
    if (m_waitKeyFrame) {
        if (!frame.isKeyFrame)
            return;
        m_waitKeyFrame = false;
    }

    MediaFmt::Frame f = {};
    if (frame.type == MediaFrame::TypeE::Venc) {
        f.type = MediaFmt::TrackTypeE::Video;
    } else if (frame.type == MediaFrame::TypeE::Aenc) {
        f.type = MediaFmt::TrackTypeE::Audio;
    } else {
        emxloge("not support frame type %d\n", (int) frame.type);
        return;
    }
    TrackPair pair(frame.type, frame.chn);
    if (!m_trackIdxMap.count(pair)) {
        emxloge("not register frame type %d with chn %d\n", (int) frame.type, frame.chn);
        return;
    }
    f.trackId = m_trackIdxMap[pair];
    f.isKeyFrame = frame.isKeyFrame;
    f.data = frame.data;
    f.size = frame.size;
    f.pts = frame.tsInUs / 1000;
    m_chnId = f.type == MediaFmt::TrackTypeE::Video ? 0x00 : 0x02;
    m_rtp.Write(f);
}

ErrCodeE WebLive::OnRtpFrame(std::vector<MediaStreamer::Data> &array) {
    for (auto &data:array) {
        char header[4] = {0x24, 0, 0, 0};
        header[1] = m_chnId;
        *((uint16_t *) (header + 2)) = htons((uint16_t) data.size);
        mg_ws_send(m_connection, header, 4, WEBSOCKET_OP_BINARY);
        mg_ws_send(m_connection, data.data, data.size, WEBSOCKET_OP_BINARY);
//        if (m_chnId == 2) {
//            auto d = (uint8_t *) data.data;
//            for (int i = 0; i < 32; i++)
//                printf("%02x ", d[i]);
//            printf("\n");
//        }
    }
    return ErrCodeE::Success;
}

void WebLive::RecvPcm(uint8_t *data, int size) {
    int remain = size;
    int off = 0;
    MediaAdec::Frame frame = {};
    while (remain > 0) {
        auto sendSize = remain > 2048 ? 2048 : remain;
        frame.data = data + off;
        frame.size = sendSize;
        MediaClientAdecStreamSync::PushRawPcm(0, frame, 100);
        remain -= sendSize;
        off += sendSize;
    }

}
