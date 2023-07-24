/*
* @Author: xiong
* @Date: 2023/3/7
*/

#include "ReadMp4.hpp"

using namespace Emx;

void ReadMp4::Create(const char *path) {
    m_mp4.Create(path, m_mediaInfo, nullptr);
    while (true) {
        MediaFmt::Frame f = {};
        if (m_mp4.Read(f) == ErrCodeE::ReachTheEof)
            break;
        printf("read:trackId=%d,type=%s,Key=%d,pts=%llu\n",
               f.trackId,
               f.type==MediaFmt::TrackTypeE::Video ? "Video" : "Audio",
               f.type == MediaFmt::TrackTypeE::Video ? f.isKeyFrame : 0,
               f.pts);
    }
}
