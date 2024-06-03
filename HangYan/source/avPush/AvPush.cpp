/*
 * @Author: xiong
 * @Date: 2022/8/19
*/

#include "AvPush.hpp"
#include "OVD_OpenAPI.h"

using namespace Emx;

ErrCodeE AvPush::Create(OvdCtx *ctx) {
    if (m_created)
        return ErrCodeE::Success;
    m_cfg.ovdChn = 0;
    m_cfg.video_quality = ctx->env.cfg.video_quality;
    m_cfg.rsk_encrypt = ctx->env.cfg.rsk_encrypt;
    //构造video data format
    OVDVideoDataFormat videoFormat = {};
    OVDVideoDataFormat *video = nullptr;
    do {
        MediaVenc::Param param = {};
        MediaClientVenc venc(0);
        if (venc.GetParam(param) != ErrCodeE::Success) {
            emxloge("get venc param failed\n");
            break;
        }
        if (param.codec == VideoCodecE::H264)
            videoFormat.codec = source_STREAM_CODEC_H264;
        else if (param.codec == VideoCodecE::H265)
            videoFormat.codec = source_STREAM_CODEC_H265;
        else {
            emxloge("ovd not support venc codec %d\n", param.codec);
            break;
        }
        videoFormat.quality = m_cfg.video_quality;
        videoFormat.bitrate = param.bitRate;
        videoFormat.width = param.width;
        videoFormat.height = param.height;
        videoFormat.framerate = param.fps;
        videoFormat.frameInterval = param.iFrameInv * param.fps;
#ifdef OVDSDK_APIVER_1_0
        videoFormat.rsk_encrypt = m_cfg.rsk_encrypt;
#endif  
        videoFormat.colorDepth = 8;
        videoFormat.reserve = 0;
        video = &videoFormat;
    } while (false);

    //构造audio data format
    OVDAudioDataFormat audioFormat = {};
    OVDAudioDataFormat *audio = nullptr;
    do {
        MediaAenc::Param param = {};
        MediaClientAenc aenc(0);
        if (aenc.GetParam(param) != ErrCodeE::Success) {
            emxloge("get aenc param failed\n");
            break;
        }
        if (param.codec != AudioCodecE::AAC) {
            emxloge("ovd not support aenc codec %d\n", param.codec);
            break;
        }
        emxlogd("param.codec: %d\n", param.codec);
        audioFormat.codec = source_STREAM_CODEC_AAC_WITH_ADTS;
        audioFormat.samplesRate = param.sampleRate;
        audioFormat.bitrate = param.bitRate;
        audioFormat.bitsPerSample = param.bitWidth;
        audioFormat.sampleperframe = 1024;
//        audioFormat.waveFormat = xxx;
//        audioFormat.channelNumber = 1;
//        audioFormat.blockAlign = 0;
//        audioFormat.frameInterval = 0;
//        audioFormat.reserve = 0;
        audio = &audioFormat;
    } while (false);
#ifdef OVDSDK1_38_1
    if (OVD_AVPushStart(m_cfg.ovdChn, OVD_HIGH_STREAM, video, audio) < 0) {
        emxloge("ovd av push start failed\n");
        return ErrCodeE::Failure;
    }
#else
    if (OVD_AVPushStart(m_cfg.ovdChn, video, audio) < 0) {
        emxloge("ovd av push start failed\n");
        return ErrCodeE::Failure;
    }
#endif 
    
    m_loop.Init("AvPush", [this]() { m_stream.Stop(); });
    MediaClientStream::Channel chn;
    chn.Add(MediaFrame::TypeE::Venc, 0);
    chn.Add(MediaFrame::TypeE::Aenc, 0);
    m_stream.Start(chn, std::bind(&AvPush::OnGetFrame, this, ph_1, ph_2));
    m_loop.Start();
    m_created = true;
    emxlogi("AvPush created\n");
    return ErrCodeE::Success;
}

void AvPush::Destroy() {
    if (!m_created)
        return;
    m_loop.StopAndDeInit();
#ifdef OVDSDK1_38_1
    OVD_AVPushEnd(m_cfg.ovdChn, OVD_HIGH_STREAM);
#else
    OVD_AVPushEnd(m_cfg.ovdChn);
#endif 
    
    m_created = false;
    emxlogi("AvPush destroyed\n");
}

void AvPush::OnGetFrame(ErrCodeE e, MediaFrame &frame) {
    if (m_loop.isQuiting())
        return;
    if (e != ErrCodeE::Success)
        return;
    OVDContentType type;
    int ret;
    uint64_t ts;
    if (frame.type == MediaFrame::TypeE::Aenc) {
        //杭研要求音频时间戳等于最后一个视频时间戳+1
        type = OVD_Audio;
        ts = m_lastVideoTsInMs + 1;
#ifdef OVDSDK1_38_1
        ret = OVD_AVPushData(m_cfg.ovdChn, OVD_HIGH_STREAM, type, frame.isKeyFrame, frame.data, frame.size, ts);
#else
        ret = OVD_AVPushData(m_cfg.ovdChn, type, frame.isKeyFrame, frame.data, frame.size, ts);
#endif        
    } else if (frame.type == MediaFrame::TypeE::Venc) {
        type = OVD_Video;
        m_lastVideoTsInMs = frame.tsInUs / 1000;
        ts = m_lastVideoTsInMs;
#ifdef OVDSDK1_38_1
        ret = OVD_AVPushData(m_cfg.ovdChn, OVD_HIGH_STREAM, type, frame.isKeyFrame, frame.data, frame.size, ts);
#else
        ret = OVD_AVPushData(m_cfg.ovdChn, type, frame.isKeyFrame, frame.data, frame.size, ts);
#endif 
    } else {
        emxloge("unexpected type %d\n", frame.type);
        return;
    }
    if (ret < 0) {
        emxloge("push av data type[%d],key[%d],size[%d],ts[%llu] failed, return ret[%d]\n",
               type, frame.isKeyFrame, frame.size, ts, ret);
    }
}
