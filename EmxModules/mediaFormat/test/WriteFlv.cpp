/*
* @Author: xiong
* @Date: 2023/3/8
*/

#include "WriteFlv.hpp"

using namespace Emx;


void WriteFlv::Create(const char *path) {
    m_loop.Init("WriteFlv", [this]() {
        m_stream.Stop();
        m_flv.Destroy();
    });

    MediaFmt::TrackInfo info = {};
    info.type = MediaFmt::TrackTypeE::Video;
    info.info.video.codec = VideoCodecE::H265;
    info.info.video.width = 2304;
    info.info.video.height = 1296;
    m_mediaInfo.push_back(info);
    info.type = MediaFmt::TrackTypeE::Audio;
    info.info.audio.codec = AudioCodecE::AAC;
    info.info.audio.channel = 1;
    info.info.audio.sampleRate = 16000;
    info.info.audio.bitWidth = 16;
    m_mediaInfo.push_back(info);
    m_flv.Create(path, m_mediaInfo, nullptr);

    MediaClientStream::Channel channel;
    channel.Add(MediaFrame::TypeE::Venc, 0);
    channel.Add(MediaFrame::TypeE::Aenc, 0);
    m_stream.Start(channel, std::bind(&WriteFlv::OnGetFrame, this, ph_1, ph_2));

    m_trackIdxMap[std::make_pair(MediaFrame::TypeE::Venc, 0)] = m_mediaInfo[0].trackId;
    m_trackIdxMap[std::make_pair(MediaFrame::TypeE::Aenc, 0)] = m_mediaInfo[1].trackId;
    m_loop.Start();
}

void WriteFlv::Destroy() {
    m_loop.StopAndDeInit();
}

void WriteFlv::OnGetFrame(ErrCodeE e, MediaFrame &frame) {
    if (e != ErrCodeE::Success)
        return;

    //设想中venc的frame.paramChange和frame.isKeyFrame应该同时出现，这里加打印看看会不会有异常情况
    if (frame.type == MediaFrame::TypeE::Venc && frame.paramChange && !frame.isKeyFrame)
        emxlogc("chn %d venc param changed but is not key frame\n", frame.chn);

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
    f.trackId = m_mediaInfo[m_trackIdxMap[pair]].trackId;
    f.isKeyFrame = frame.isKeyFrame;
    f.data = frame.data;
    f.size = frame.size;
    f.pts = frame.tsInUs / 1000;
//        printf("%s:%d write[%d,%d,%d,%llu,%d]\n",
//                __FUNCTION__, __LINE__,
//               (int) f.type, f.isKeyFrame, f.size, f.pts, f.trackId);
//        for (int i = 0; i < 10; i++) {
//            printf("%02x ", f.data[i]);
//        }
//        printf("\n");
    m_flv.Write(f);
    printf("write:trackId=%d,type=%s,Key=%d,pts=%llu\n",
           f.trackId,
           f.type == MediaFmt::TrackTypeE::Video ? "Video" : "Audio",
           f.type == MediaFmt::TrackTypeE::Video ? f.isKeyFrame : 0,
           f.pts);
}