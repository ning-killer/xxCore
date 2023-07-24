//
// Created by xiong on 2020/11/6.
//
#include "EmxCore.hpp"
#include "StreamService.hpp"
#include "EmxMedia.hpp"

using namespace Emx;

void StreamService::CreateService(Json::Value &json) {
    auto &stream = json["stream"];
    m_maxSendBufSize = stream["maxSendBufferKB"].asInt() * 1024;
    m_maxAVBufferNumber = stream["maxAVBufferNumber"].asInt();
    m_loop.Init("StreamService", [this]() { Destroy(); });
    Create(m_loop, "0.0.0.0",
           EMX_STREAM_SERVICE_PORT,
           stream["maxClientNum"].asInt(), m_buffer, sizeof(m_buffer));
    m_loop.Start();
}

void StreamService::DestroyService() {
    m_loop.StopAndDeInit();
}

EuvStmServerClientBase *StreamService::OnAllocClient() {
    return new StreamClient(this, m_loop);
}

void StreamService::OnClientDisConnect(EuvStmServerClientBase *client) {
    auto c = (StreamClient *) client;
    m_clientList.remove(c);
    if (c->m_avStream) {
        c->m_avStream->Stop();
        delete c;
    }
    emxlogd("client[%s] disconnected\n", c->ip);
}

void StreamService::OnClientConnect(EuvStmServerClientBase *client) {
    sockaddr_in addr = {};
    size_t len = sizeof(addr);
    client->GetStmInf()->GetPeerName((sockaddr *) &addr, len);
    auto c = (StreamClient *) client;
    uv_ip4_name(&addr, c->ip, sizeof(c->ip));
    int32_t sndBuf = m_maxSendBufSize;
    if (setsockopt(c->GetStmInf()->GetFd(), SOL_SOCKET, SO_SNDBUF, &sndBuf, sizeof(int)) == -1)
        printf("SO_SNDBUF error %d:%s\n", errno, strerror(errno));
    emxlogd("client[%s] connected\n", c->ip);
    m_clientList.push_back((StreamClient *) client);
}

void StreamService::OnRecv(EuvStmServerClientBase *client, char *data, int32_t size) {
    auto c = (StreamClient *) client;
    if (c->running) {
        emxlogw("client %s is running\n", c->ip);
        return;
    }
    c->data.append(data, size);
    while (c->data.size() >= c->waitSize) {
        if (c->waitHeader) {
            memcpy(&c->header, c->data.data(), sizeof(Stream::Header));
            if (c->header.sync[0] != 'A' || c->header.sync[1] != 'V' || c->header.sync[2] != 'S') {
                emxloge("client[%s] miss sync\n", c->ip);
                DisConnectClient(c);
                return;
            }
            c->waitSize = sizeof(Stream::AVReqInfo) + sizeof(Stream::Header);
            c->waitHeader = false;
            continue;
        }
        Stream::AVReqInfo info = {};
        memcpy(&info, c->data.data() + sizeof(Stream::Header), sizeof(Stream::AVReqInfo));
        if ((info.chn >= 2) ||
            (info.type != Stream::StreamTypeE::MAJOR &&
             info.type != Stream::StreamTypeE::MINOR)) {
            c->header.type = Stream::CmdTypeE::RESP;
            c->header.size = htonl(sizeof(Stream::VideoInfo));
            Stream::VideoInfo videoInfo = {};
            videoInfo.errorCode = htonl((uint32_t) Stream::ErrCodeE::UNSUPPORT_STREAM_TYPE);
            auto s = std::make_shared<std::string>((const char *) &c->header, sizeof(Stream::Header));
            s->append((const char *) &videoInfo, sizeof(Stream::VideoInfo));
            c->Send(s);
            DisConnectClient(c);
        } else {
            int chn = info.type == Stream::StreamTypeE::MAJOR ? 0 : 1;
            c->SetupStream(chn);
            MediaClientStream::Channel channel;
            channel.Add(MediaFrame::TypeE::Venc, chn);
            channel.Add(MediaFrame::TypeE::Aenc, 0);
            c->m_avStream = std::make_shared<MediaClientStreamAsync>(m_loop);
            c->m_avStream->Start(channel, [c](ErrCodeE e, MediaFrame &frame) { c->OnGetFrame(e, frame); });
        }
        c->data.clear();
        c->running = true;
    }
}


void StreamClient::OnSendDone(ErrCodeE e, const char *buffer, int32_t size) {
    //keep frame data blocking on media buffer, rather than blocking on socket
    //start receiving frame
    buffered--;
    if (buffered <= 0) {
        if (!m_avStream->IsReceivingFrame()) {
            m_avStream->RecvFrameResume();
        }
    }
}


void StreamClient::OnGetFrame(ErrCodeE e, MediaFrame &frame) {
    //keep frame data blocking on media buffer, rather than blocking on socket
    //stop receiving frame
    if (buffered > 4 && m_avStream->IsReceivingFrame()) {
        m_avStream->RecvFramePause();
    }
    if (e != ErrCodeE::Success) {
        emxlogd("%p get frame type %d chn %d seq %d failed\n", this, frame.type, frame.chn, frame.seq);
        return;
    }
    auto s = std::make_shared<std::string>();
    if (frame.type == MediaFrame::TypeE::Venc) {
        header.size = htonl(sizeof(video) + frame.size);
        s->assign((const char *) &header, sizeof(header));
        video.frameType = frame.isKeyFrame ? Stream::FrameTypeE::I_FRAME : Stream::FrameTypeE::P_FRAME;
        video.frameSize = htonl(frame.size);
        video.frameId = htonl(frameId++);
        video.timeStampH = 0;
        video.timeStampL = htonl((uint32_t) (frame.tsInUs / 1000));
        s->append((const char *) &video, sizeof(video));
    } else if (frame.type == MediaFrame::TypeE::Aenc) {
        header.size = htonl(sizeof(audio) + frame.size);
        s->assign((const char *) &header, sizeof(header));
        audio.frameSize = htonl(frame.size);
        audio.frameId = htonl(frameId++);
        audio.timeStampH = 0;
        audio.timeStampL = htonl((uint32_t) (frame.tsInUs / 1000));
        s->append((const char *) &audio, sizeof(audio));
    } else {
        emxloge("unregister type %d\n", frame.type);
        return;
    }
    s->append((const char *) frame.data, frame.size);
    Send(s);
    buffered++;
}

void StreamClient::SetupStream(int chn) {
    MediaClientVenc venc(chn);
    MediaVenc::Param vParam = {};
    venc.GetParam(vParam);
    memset(&video, 0, sizeof(video));
    if (chn == 0) {
        if (vParam.codec == VideoCodecE::H264) {
            video.encoderType = Stream::VideoCodecChnTypeE::H264_MAJOR;
        } else if (vParam.codec == VideoCodecE::H265) {
            video.encoderType = Stream::VideoCodecChnTypeE::H265_MAJOR;
        } else if (vParam.codec == VideoCodecE::MJPEG) {
            video.encoderType = Stream::VideoCodecChnTypeE::MJPEG_MAJOR;
        }
    } else {
        if (vParam.codec == VideoCodecE::H264) {
            video.encoderType = Stream::VideoCodecChnTypeE::H264_MINOR;
        } else if (vParam.codec == VideoCodecE::H265) {
            video.encoderType = Stream::VideoCodecChnTypeE::H265_MINOR;
        } else if (vParam.codec == VideoCodecE::MJPEG) {
            video.encoderType = Stream::VideoCodecChnTypeE::MJPEG_MINOR;
        }
    }
    video.frameRate = vParam.fps;
    video.width = htons(vParam.width);
    video.height = htons(vParam.height);

    memset(&audio, 0, sizeof(audio));
    MediaClientAenc aenc(0);
    MediaAenc::Param aParam = {};
    if (aenc.GetParam(aParam) == ErrCodeE::Success) {
        audio.frameType = Stream::FrameTypeE::AUDIO;
        if (aParam.codec == AudioCodecE::LPCM)
            audio.encoderType = Stream::AudioCodecChnTypeE::PCM;
        else if (aParam.codec == AudioCodecE::G711A)
            audio.encoderType = Stream::AudioCodecChnTypeE::G711A;
        else if (aParam.codec == AudioCodecE::G711U)
            audio.encoderType = Stream::AudioCodecChnTypeE::G711U;
        else
            audio.encoderType = Stream::AudioCodecChnTypeE::AAC;

        audio.bitCount = htons((unsigned short) aParam.bitWidth);
        audio.samples = htons((unsigned short) aParam.sampleRate);
    }

    header.type = Stream::CmdTypeE::RESP;
    frameId = 0;
}
