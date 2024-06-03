/*
 * @Author: xiong
 * @Date: 2022/8/17
*/
#include "Mp4.hpp"
#include "Mp4Buffer.h"
#include "mov-reader.h"
#include "mov-format.h"

using namespace Emx;

ErrCodeE Mp4Reader::Create(const char *path, std::vector<TrackInfo> &info, Json::Value *usr) {
    mov_reader_trackinfo_t onTrack = {Mp4Reader::OnVideo,
                                      Mp4Reader::OnAudio,
                                      Mp4Reader::OnSubtitle};
    Arg arg = {&info, usr};

    m_fp = fopen(path, "rb");
    if (!m_fp) {
        emxloge("cannot open %s\n", path);
        goto ERROR;
    }
    m_mov = mov_reader_create(GetBuffer(), m_fp);
    if (!m_mov) {
        emxloge("mov writer create failed\n");
        goto ERROR;
    }
    m_mediaInfo.duration = mov_reader_getduration(m_mov);
    if (mov_reader_getinfo(m_mov, &onTrack, &arg) != 0) {
        emxloge("mov_reader_getinfo failed\n");
        goto ERROR;
    }
    m_ctx.trackMap.clear();
    for (auto &item:info) {
        m_ctx.trackMap[item.trackId] = item;
    }
    return ErrCodeE::Success;
    ERROR:
    if (m_mov) {
        mov_reader_destroy(m_mov);
        m_mov = nullptr;
    }
    if (m_fp) {
        fclose(m_fp);
        m_fp = nullptr;
    }
    return ErrCodeE::Failure;
}

void Mp4Reader::Destroy() {
    if (m_mov) {
        mov_reader_destroy(m_mov);
        m_mov = nullptr;
    }
    if (m_fp) {
        fclose(m_fp);
        m_fp = nullptr;
    }
}

ErrCodeE Mp4Reader::Read(MediaFmt::Frame &frame) {
    int ret = mov_reader_read2(m_mov, OnAlloc, &m_ctx);
    if (ret == 0)
        return ErrCodeE::ReachTheEof;
    if (ret != 1)
        return ErrCodeE::Failure;
    frame.trackId = (int) m_ctx.track;
    frame.pts = m_ctx.pts;
    frame.isKeyFrame = (m_ctx.flags & MOV_AV_FLAG_KEYFREAME) != 0;
    frame.type = m_ctx.trackMap[m_ctx.track].type;
    if (frame.type == MediaFmt::TrackTypeE::Video) {
        //video: 4-byte data length(don't include self length) + H.264 NALU(don't include 0x00000001)
        frame.data = (uint8_t *) m_ctx.data.data() + Ctx::DataReservedSize;
        //replace 4-byte data length with NALU sync header
        frame.data[0] = 0;
        frame.data[1] = 0;
        frame.data[2] = 0;
        frame.data[3] = 1;
        frame.size = (int32_t) m_ctx.bytes;
    } else if (frame.type == MediaFmt::TrackTypeE::Audio) {
        if (m_ctx.trackMap[m_ctx.track].info.audio.codec == AudioCodecE::AAC) {
//            // audio: AAC raw data, don't include ADTS/AudioSpecificConfig
//            const uint8_t ID = 0; // 0-MPEG4/1-MPEG2
//            frame.data = (uint8_t *) m_ctx.data.data() + Ctx::DataReservedSize - 7;
//            frame.size = (int32_t) m_ctx.bytes + 7;
//            frame.data[0] = 0xFF; /* 12-syncword */
//            frame.data[1] = 0xF0 /* 12-syncword */ |
//                            (ID << 3)/*1-ID*/ |
//                            (0x00 << 2) /*2-layer*/ |
//                            0x01 /*1-protection_absent*/;
//            const uint8_t profile = 2;// 0-NULL, 1-AAC Main, 2-AAC LC, 2-AAC SSR, 3-AAC LTP
//            uint8_t samplingFrequencyIndex =
//                    GetSamplingFrequencyIndex(m_ctx.trackMap[m_ctx.track].info.audio.sampleRate);
//            // 0-AOT, 1-1channel,front-center, 2-2channels, front-left/right, 3-3channels: front center/left/right, 4-4channels: front-center/left/right, back-center, 5-5channels: front center/left/right, back-left/right, 6-6channels: front center/left/right, back left/right LFE-channel, 7-8channels
//            uint8_t channelConfiguration = m_ctx.trackMap[m_ctx.track].info.audio.channel;
//            frame.data[2] = ((profile - 1) << 6) | ((samplingFrequencyIndex & 0x0F) << 2) |
//                            ((channelConfiguration >> 2) & 0x01);
//            /*0-original_copy*/ /*0-home*/ /*0-copyright_identification_bit*/ /*0-copyright_identification_start*/
//            frame.data[3] = ((channelConfiguration & 0x03) << 6) | ((frame.size >> 11) & 0x03);
//            frame.data[4] = (uint8_t) (frame.size >> 3);
//            frame.data[5] = ((frame.size & 0x07) << 5) | 0x1F;
//            frame.data[6] = 0xFC /*| ((len / (1024 * aac->channels)) & 0x03)*/;
////            frame.data[4] = 0x41;
////            frame.data[5] = 0x80;
////            frame.data[6] = 0xf0;
////            ff f1 6c 40 5f 3f fc
////            ff f1 6c 40 60 00 00
////            ff f1 6c 40 60 1f fc*
////            ff f1 60 40 41 80 f0
            frame.data = (uint8_t *) m_ctx.data.data() + Ctx::DataReservedSize;
            frame.size = (int32_t) m_ctx.bytes;
        } else {
            frame.data = (uint8_t *) m_ctx.data.data() + Ctx::DataReservedSize;
            frame.size = (int32_t) m_ctx.bytes;
        }
    } else {
        frame.data = (uint8_t *) m_ctx.data.data();
        frame.size = (int32_t) m_ctx.bytes;
    }
    return ErrCodeE::Success;
}

ErrCodeE Mp4Reader::Seek(int64_t &pos) {
    mov_reader_seek(m_mov, &pos);
    return ErrCodeE::Success;
}

void Mp4Reader::OnVideo(void *param, uint32_t track, uint8_t object,
                        int width, int height, const void *extra, size_t bytes) {
    auto arg = (Arg *) param;
    MediaFmt::TrackInfo info = {};
    info.trackId = (int) track;
    info.type = MediaFmt::TrackTypeE::Video;
    auto &video = info.info.video;
    switch (object) {
        case MOV_OBJECT_H264:
            video.codec = VideoCodecE::H264;
            break;
        case MOV_OBJECT_H265:
            video.codec = VideoCodecE::H265;
            break;
        default:
            emxloge("unsupported video object(codec) %d\n", (int) object);
            return;
    }
    video.width = width;
    video.height = height;
    arg->info->push_back(info);
}

void Mp4Reader::OnAudio(void *param, uint32_t track, uint8_t object,
                        int channel_count, int bit_per_sample, int sample_rate, const void *extra, size_t bytes) {
    auto arg = (Arg *) param;
    MediaFmt::TrackInfo info = {};
    info.trackId = (int) track;
    info.type = MediaFmt::TrackTypeE::Audio;
    auto &audio = info.info.audio;
    switch (object) {
        case MOV_OBJECT_AAC:
            audio.codec = AudioCodecE::AAC;
            break;
        case MOV_OBJECT_G711a:
            audio.codec = AudioCodecE::G711A;
            break;
        case MOV_OBJECT_G711u:
            audio.codec = AudioCodecE::G711U;
            break;
        default:
            emxloge("unsupported video object(codec) %d\n", (int) object);
            return;
    }
    audio.sampleRate = sample_rate;
    audio.bitWidth = bit_per_sample;
    audio.channel = channel_count;
    arg->info->push_back(info);
}

void Mp4Reader::OnSubtitle(void *param, uint32_t track, uint8_t object, const void *extra, size_t bytes) {
//    auto arg = (Arg *) param;
}

void *Mp4Reader::OnAlloc(void *param, uint32_t track, size_t bytes, int64_t pts, int64_t dts, int flags) {
    auto buffer = (Ctx *) param;
    if (!buffer->trackMap.count(track)) {
        emxloge("unknown track %d\n", track);
        return nullptr;
    }
    buffer->track = track;
    buffer->pts = pts;
    buffer->flags = flags;
    buffer->bytes = bytes;
    buffer->data.clear();
    if ((buffer->trackMap[track].type == MediaFmt::TrackTypeE::Video && ((flags & MOV_AV_FLAG_KEYFREAME) != 0)) ||
        bytes > buffer->data.capacity() - Ctx::DataReservedSize) {
        // I帧或者内存不够则修改内存大小(内存的大小时刻保持着和上一个I帧相同)
        try {
            if (bytes > buffer->data.capacity() - Ctx::DataReservedSize)
                buffer->data.reserve(bytes + Ctx::DataReservedSize);
        } catch (std::exception &e) {
            emxloge("malloc %d failed\n", bytes + Ctx::DataReservedSize);
            return nullptr;
        }
    }
    // 预留10个字节，为后面填充adts头等
    return (void *) (buffer->data.data() + Ctx::DataReservedSize);
}

uint8_t Mp4Reader::GetSamplingFrequencyIndex(int sampleRate) {
    // 0-96000, 1-88200, 2-64000, 3-48000, 4-44100, 5-32000, 6-24000, 7-22050, 8-16000, 9-12000, 10-11025, 11-8000, 12-7350, 13/14-reserved, 15-frequency is written explictly
    switch (sampleRate) {
        case 96000:
            return 0;
        case 88200:
            return 1;
        case 64000:
            return 2;
        case 48000:
            return 3;
        case 44100:
            return 4;
        case 32000:
            return 5;
        case 24000:
            return 6;
        case 22050:
            return 7;
        case 16000:
            return 8;
        case 12000:
            return 9;
        case 11025:
            return 10;
        case 8000:
            return 11;
        case 7350:
            return 12;
        default :
            emxloge("unsupported sample rate %d\n", sampleRate);
            return -1;
    }
}

