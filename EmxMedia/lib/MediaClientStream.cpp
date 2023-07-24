//
// Created by xiong on 2021/8/6.
//
#include "core/EmxLog.hpp"
#include "media/MediaClientStream.hpp"
#include "media/MediaVenc.hpp"
#include "media/MediaAenc.hpp"
#include "media/MediaAdec.hpp"
#include "media/MediaSnap.hpp"
#include "media/MediaClientVenc.hpp"
#include "MediaInner.hpp"

using namespace Emx;


void MediaClientStream::OnMsgRecv(uint32_t topic, const char *data, int32_t size) {
    MediaFrame frame = {};
    memcpy(&frame, data, size);
    if (m_cbFrameInfo) {
        m_cbFrameInfo(frame);
    }
    if (m_cbFrame) {
        std::string frameData;
        ErrCodeE e = m_ring.Pop(frame.off, frame.size, frame.tsInUs, frameData);
        frame.data = (uint8_t *) frameData.data();
        m_cbFrame(CheckFrame(e, frame), frame);
    }
}

ErrCodeE MediaClientStream::CheckFrame(ErrCodeE errPop, MediaFrame &frame) {
    auto chnItem = m_chn.GetChnItem(frame.type, frame.chn);
    if (!chnItem) {
        emxloge("%p GetChnItem(%d,%d) failed!\n", this, (int) frame.type, frame.chn);
        return ErrCodeE::ResNotExist;
    }
    if (errPop != ErrCodeE::Success) {
        chnItem->inited = false;
        return errPop;
    }
    if (frame.type == MediaFrame::TypeE::Venc) {
        //waiting for key frame
        if (!chnItem->inited && !frame.isKeyFrame) {
            emxlogt("%p wait key frame\n", this);
            return ErrCodeE::FrameError;
        }
        if (!chnItem->inited) {
            emxlogd("stream %p venc chn %d got key frame\n",
                   this, frame.chn);
            chnItem->inited = true;
            chnItem->waitSeq = frame.seq;
        }
        if (chnItem->waitSeq != frame.seq) {
            chnItem->inited = false;
            emxlogw("stream %p type %d chn %d seq is not continuous,wait[%u],actual[%u]\n",
                   this, frame.type, frame.chn, chnItem->waitSeq, frame.seq);
            return ErrCodeE::FrameError;
        }
    }

    chnItem->waitSeq++;
    return ErrCodeE::Success;
}

ErrCodeE MediaClientStream::GetFrameByInfo(MediaFrame &frame) {
    ErrCodeE e = m_ring.Pop(frame.off, frame.size, frame.tsInUs, m_data);
    frame.data = (uint8_t *) m_data.data();
    return CheckFrame(e, frame);
}

ErrCodeE MediaClientStreamAsync::Start(Channel &chn, OnGetFrameCallback cb, OnGetFrameInfoCallback cbInfo) {
    m_chn = chn;
    m_cbFrame = std::move(cb);
    m_cbFrameInfo = std::move(cbInfo);
    MsgTopicSubscriberAsync::Config cfg = {};
    cfg.loop = &m_loop;
    cfg.sendByPack = true;
    cfg.maxSend = 1024;
    for (auto &e: m_chn.array) {
        uint32_t topicPrefix = 0;
        switch (e.type) {
            case MediaFrame::TypeE::Venc:
                topicPrefix = MEDIA_TOPIC_VENC_FRAME;
                break;
            case MediaFrame::TypeE::Aenc:
                topicPrefix = MEDIA_TOPIC_AENC_AENC;
                break;
            case MediaFrame::TypeE::Pcm:
                topicPrefix = MEDIA_TOPIC_AENC_PCM;
                break;
//            case MediaFrame::TypeE::Vdec:
//                break;
            case MediaFrame::TypeE::Adec:
                break;
            case MediaFrame::TypeE::Snap:
                topicPrefix = MEDIA_TOPIC_SNAP;
                break;
//            case MediaFrame::TypeE::Data:
//                break;
            default:
                emxloge("not support type %d\n", e.type);
                return ErrCodeE::OperationNotSupport;
        }
        cfg.topicArray.emplace_back(topicPrefix + e.chn);
    }
    cfg.buffer = m_buffer;
    cfg.size = 256;
    cfg.OnConnected = [this]() {
        for (auto &e: m_chn.array) {
            if (e.type == MediaFrame::TypeE::Venc) {
                MediaClientVenc venc(e.chn);
                venc.FlushIDR();
            }
        }
        emxlogd("MediaClientStreamAsync %s OnConnected\n", m_sub.GetSockName().c_str());
    };
    cfg.OnDisConnected = [this]() {
        emxlogd("MediaClientStreamAsync %s OnDisConnected\n", m_sub.GetSockName().c_str());
    };
    cfg.OnRecvMsg = std::bind(&MediaClientStreamAsync::OnMsgRecv, this, ph_1, ph_2, ph_3);
    m_sub.CreateMsgTopicSubscriberAsync(cfg);
    m_ring.Create(MEDIA_RING_BUFFER_KEY);
    return ErrCodeE::Success;
}

void MediaClientStreamAsync::Stop() {
    m_sub.DestroyMsgTopicSubscriberAsync();
    m_ring.Destroy();
}

ErrCodeE MediaClientStreamSync::Open(Channel &chn, int32_t timeoutMs) {
    MsgTopicSubscriberSync::Config cfg = {};
    cfg.sendByPack = true;
    cfg.maxSend = 1024;
    for (auto &e: chn.array) {
        uint32_t topicPrefix = 0;
        switch (e.type) {
            case MediaFrame::TypeE::Venc:
                topicPrefix = MEDIA_TOPIC_VENC;
                break;
            case MediaFrame::TypeE::Aenc:
                topicPrefix = MEDIA_TOPIC_AENC_AENC;
                break;
            case MediaFrame::TypeE::Pcm:
                topicPrefix = MEDIA_TOPIC_AENC_PCM;
                break;
//            case MediaFrame::TypeE::Vdec:
//                break;
            case MediaFrame::TypeE::Adec:
                break;
            case MediaFrame::TypeE::Snap:
                topicPrefix = MEDIA_TOPIC_SNAP;
                break;
//            case MediaFrame::TypeE::Data:
//                break;
            default:
                emxloge("not support type %d\n", e.type);
                return ErrCodeE::OperationNotSupport;
        }
        cfg.topicArray.emplace_back(topicPrefix + e.chn);
    }

    ErrCodeE e = m_sub.Connect(cfg, timeoutMs);
    if (e != ErrCodeE::Success) {
        return e;
    }
    m_ring.Create(MEDIA_RING_BUFFER_KEY);
    m_chn = chn;
    emxlogd("MediaClientStreamSync %s Opened\n", m_sub.GetSockName());
    return ErrCodeE::Success;
}

ErrCodeE MediaClientStreamSync::GetFrame(MediaFrame &frame, int32_t timeoutMs) {
    uint32_t topic;
    ErrCodeE e = m_sub.GetMsg(topic, m_data, timeoutMs);
    if (e != ErrCodeE::Success) {
        return e;
    }
    memcpy(&frame, m_data.data(), m_data.size());
    e = m_ring.Pop(frame.off, frame.size, frame.tsInUs, m_data);
    if (e != ErrCodeE::Success) {
        return e;
    }
    frame.data = (uint8_t *) m_data.data();
    return CheckFrame(e, frame);
}

ErrCodeE MediaClientStreamSync::GetFrameInfo(MediaFrame &frame, int32_t timeoutMs) {
    uint32_t topic;
    ErrCodeE e = m_sub.GetMsg(topic, m_data, timeoutMs);
    if (e != ErrCodeE::Success) {
        return e;
    }
    memcpy(&frame, m_data.data(), m_data.size());
    return ErrCodeE::Success;
}


void MediaClientStreamSync::Close() {
    m_sub.DisConnect();
    m_ring.Destroy();
    emxlogd("MediaClientStreamSync %s Closed\n", m_sub.GetSockName());
}

