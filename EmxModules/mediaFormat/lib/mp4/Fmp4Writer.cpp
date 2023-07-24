///*
//* @Author: xiong
//* @Date: 2023/3/2
//*/
//
//#include "Fmp4.hpp"
//#include "Mp4Buffer.h"
//#include "fmp4-writer.h"
//#include "mov-format.h"
//
//using namespace Emx;
//
//ErrCodeE Fmp4Writer::Create(const char *path, Json::Value *usr) {
//    m_fp = fopen(path, "wb+");
//    if (!m_fp) {
//        emxloge("cannot open %s\n", path);
//        goto ERROR;
//    }
//
//    m_mov = fmp4_writer_create(GetBuffer(), m_fp, 0);
//    if (!m_mov) {
//        emxloge("mov writer create failed\n");
//        goto ERROR;
//    }
//    m_trackMap.clear();
//    for (auto &item:info) {
//        if (item.type == MediaFmt::TrackTypeE::Video) {
//            auto &video = item.info.video;
//            uint8_t object;
//            if (video.codec == VideoCodecE::H264)
//                object = MOV_OBJECT_H264;
//            else if (video.codec == VideoCodecE::H265)
//                object = MOV_OBJECT_H265;
//            else {
//                emxloge("unsupported video codec %d\n", (int) video.codec);
//                goto ERROR;
//            }
//            int trackId = fmp4_writer_add_video(m_mov, object,
//                                                video.width, video.height,
//                                                nullptr, 0);
//            if (trackId < 0) {
//                emxloge("mov_writer_add_video failed\n");
//                goto ERROR;
//            }
//            item.trackId = trackId;
//            m_trackMap[trackId] = item;
//        } else if (item.type == MediaFmt::TrackTypeE::Audio) {
//            auto &audio = item.info.audio;
//            uint8_t object;
//            if (audio.codec == AudioCodecE::AAC)
//                object = MOV_OBJECT_AAC;
//            else if (audio.codec == AudioCodecE::G711A)
//                object = MOV_OBJECT_G711a;
//            else if (audio.codec == AudioCodecE::G711U)
//                object = MOV_OBJECT_G711u;
//            else {
//                emxloge("unsupported audio codec %d\n", (int) audio.codec);
//                goto ERROR;
//            }
//            int trackId = fmp4_writer_add_audio(m_mov, object,
//                                                audio.channel, audio.bitWidth, audio.sampleRate,
//                                                nullptr, 0);
//            if (trackId < 0) {
//                emxloge("mov_writer_add_video failed\n");
//                goto ERROR;
//            }
//            item.trackId = trackId;
//            m_trackMap[trackId] = item;
//        }
//    }
//    fmp4_writer_init_segment(m_mov);
//    return ErrCodeE::Success;
//    ERROR:
//    if (m_mov) {
//        fmp4_writer_destroy(m_mov);
//        m_mov = nullptr;
//    }
//    if (m_fp) {
//        fclose(m_fp);
//        m_fp = nullptr;
//        remove(path);
//    }
//    return ErrCodeE::Failure;
//}
//
//void Fmp4Writer::Destroy() {
//    fmp4_writer_destroy(m_mov);
//    fclose(m_fp);
//}
//
//ErrCodeE Fmp4Writer::Write(MediaFmt::Frame &frame) {
//    if (m_trackMap[frame.trackId].type == MediaFmt::TrackTypeE::Audio &&
//        m_trackMap[frame.trackId].info.audio.codec == AudioCodecE::AAC) {
//        //mp4 skip adts
//        frame.data += 7;
//        frame.size -= 7;
//    }
//    int ret = fmp4_writer_write(m_mov, frame.trackId,
//                                frame.data, frame.size,
//                                (int64_t) frame.pts, (int64_t) frame.pts,
//                                frame.isKeyFrame ? MOV_AV_FLAG_KEYFREAME : 0);
////    fmp4_writer_save_segment(m_mov);
//    return ret == 0 ? ErrCodeE::Success : ErrCodeE::Failure;
//}
