//
// Created by xiong on 2022/4/8.
//

#include "EmxRtspServer.hpp"
#include "DigestAuth.h"
#include "MediaSourceManager.h"
#include "RtspInner.hpp"

using namespace Emx;

void EmxRtspServer::Create(bool newThread) {
    m_loop.Init("EmxRtspServer", [this]() {
        m_resp.DestroyMsgRpcRep();
        m_stream.Stop();
        m_restartTimer.Destroy();
        if (m_rtsp) {
            m_rtsp.reset();
        }
        MediaSourceManager::Instance().removeAllMedia();
    },  newThread ? EuvLoop::ModeE::Dynamic : EuvLoop::Default);
    MsgRpcRep::Config cfg = {};
    cfg.loop = &m_loop;
    cfg.clientId = EMX_RTSP_MSG_ID;
    cfg.buffer = m_buffer;
    cfg.size = sizeof(m_buffer);
    cfg.OnConnected = [this]() {
        Param param = {};
        if (GetParam(param) != ErrCodeE::Success) {
            return;
        }
        CreateRtspServer(param);
    };
    cfg.OnDisConnected = []() {};
    cfg.OnRecvReq = std::bind(&EmxRtspServer::OnRecvMsg, this, ph_1, ph_2, ph_3, ph_4, ph_5);
    m_resp.CreateMsgRpcRep(cfg);
    m_restartTimer.Create(m_loop);
    emxlogd("EmxRtspServer newThread(%d)\n",newThread);
    m_loop.Start(newThread);
}

void EmxRtspServer::Destroy() {
    m_loop.StopAndDeInit();
}

ErrCodeE EmxRtspServer::OnRecvMsg(uint32_t module, uint32_t method,
                                    const char *data, int32_t size, std::string &out) {
    switch ((RtspModuleE) module) {
        case RtspModuleE::Rtsp:
            switch ((RtspMethodE) method) {
                case RtspMethodE::GetParam: {
                    out.clear();
                    Param param;
                    if (GetParam(param) != ErrCodeE::Success) {
                        return ErrCodeE::Failure;
                    }
                    param.Serialization(out);
                    return ErrCodeE::Success;
                }
                case RtspMethodE::SetParam: {
                    Param param;
                    param.Parse(data, size);
                    return SetParam(param);
                }
                default:
                    emxloge("unknown method 0x%x\n", method);
                    return ErrCodeE::OperationNotSupport;
            }
        default:
            emxloge("unknown module 0x%x\n", module);
            return ErrCodeE::OperationNotSupport;
    }
}

void EmxRtspServer::CreateRtspServer(Param &param) {
    //todo
    if (!param.userArray.empty())
        DigestAuth::Instance().setUserPassword(param.userArray[0].name, param.userArray[0].password);
    for (auto &media :param.mediaArray) {
        RtspMediaSource source = {};
        std::string vCodec;
        if (media.vencChn != -1) {
            MediaClientVenc venc(media.vencChn);
            MediaVenc::Param paramVenc = {};
            venc.GetParam(paramVenc);
            if (VideoCodecE::H264 == paramVenc.codec) {
                vCodec = "H264";
            } else if (VideoCodecE::H265 == paramVenc.codec) {
                vCodec = "H265";
            }
        }
        std::string aCodec;
        if (media.aencChn != -1) {
            MediaClientAenc aenc(media.aencChn);
            MediaAenc::Param paramAenc = {};
            aenc.GetParam(paramAenc);
            if (AudioCodecE::G711A == paramAenc.codec) { /*3:711a,4:711u,5:aac*/
                aCodec = "PCMA";
            } else if (AudioCodecE::G711U == paramAenc.codec) {
                aCodec = "PCMU";
            } else if (AudioCodecE::AAC == paramAenc.codec) {
                aCodec = "AAC";
            }
            source.sampleRate = paramAenc.sampleRate;
        }
        source.vencChn = media.vencChn;
        source.aencChn = media.aencChn;
        source.media = std::make_shared<MediaSource>(vCodec, aCodec, source.sampleRate);
        MediaSourceManager::Instance().addMedia(media.name, source.media);
        m_mediaArray.push_back(source);
    }
    MediaClientStream::Channel channel;
    for (auto &media :param.mediaArray) {
        if (media.vencChn != -1) {
            if (channel.GetChnItem(MediaFrame::TypeE::Venc, media.vencChn) == nullptr) {
                channel.Add(MediaFrame::TypeE::Venc, media.vencChn);
            }
        }
        if (media.aencChn != -1) {
            if (channel.GetChnItem(MediaFrame::TypeE::Aenc, media.aencChn) == nullptr) {
                channel.Add(MediaFrame::TypeE::Aenc, media.aencChn);
            }
        }
    }
    m_stream.Start(channel, std::bind(&EmxRtspServer::OnFrameRecv, this, ph_1, ph_2));
    m_rtsp = std::make_shared<RtspServer>();
    m_rtsp->start(param.port);
}

void EmxRtspServer::OnFrameRecv(ErrCodeE e, MediaFrame &frame) {
    if (frame.paramChange) {
        m_restartTimer.Start(100, 0, [this]() {
            ReStart();
        });
    }
    if (frame.type == MediaFrame::TypeE::Venc) {
        for (auto &media :m_mediaArray) {
            if (media.vencChn == frame.chn)
                media.media->inputFrame(frame.data, frame.size, frame.tsInUs / 1000 * 90, frame.isKeyFrame, 0);
        }
    } else if (frame.type == MediaFrame::TypeE::Aenc) {
        for (auto &media :m_mediaArray) {
            if (media.aencChn == frame.chn)
                media.media->inputFrame(frame.data, frame.size, frame.tsInUs / 1000 * (media.sampleRate / 1000),
                                        frame.isKeyFrame, 1);
        }
    }
}

ErrCodeE EmxRtspServer::GetParam(Param &param) {
    ::Param rtsp("rtsp");
    Json::Value json;
    if (rtsp.Get(json) != ErrCodeE::Success) {
        emxloge("load param failed\n");
        return ErrCodeE::Failure;
    }
    param.port = json["port"].asInt();
    for (auto &userJson : json["user"]) {
        User user;
        user.name = userJson["name"].asString();
        user.password = userJson["password"].asString();
        param.userArray.emplace_back(user);
    }
    for (auto &mediaJson : json["media"]) {
        Media media = {};
        strncpy(media.name, mediaJson["name"].asCString(), sizeof(media.name));
        media.vencChn = mediaJson["vencChn"].asInt();
        media.aencChn = mediaJson["aencChn"].asInt();
        param.mediaArray.emplace_back(media);
    }
    return ErrCodeE::Success;
}

ErrCodeE EmxRtspServer::SetParam(const Param &param) {
    ::Param rtsp("rtsp");
    Json::Value json;
    json["port"] = param.port;
    auto &userArray = json["user"];
    userArray.resize(0);
    for (auto &user : param.userArray) {
        Json::Value userJson;
        userJson["name"] = user.name;
        userJson["password"] = user.password;
        userArray.append(userJson);
    }
    auto &mediaArray = json["media"];
    mediaArray.resize(0);
    for (auto &media : param.mediaArray) {
        Json::Value mediaJson;
        mediaJson["name"] = media.name;
        mediaJson["vencChn"] = media.vencChn;
        mediaJson["aencChn"] = media.aencChn;
        userArray.append(mediaJson);
    }

    if (rtsp.Set(json) != ErrCodeE::Success) {
        emxloge("save param failed\n");
        return ErrCodeE::Failure;
    }
    return ReStart();
}

ErrCodeE EmxRtspServer::ReStart() {
    m_stream.Stop();
    if (m_rtsp) {
        m_rtsp.reset();
    }
    MediaSourceManager::Instance().removeAllMedia();
    Param param = {};
    if (GetParam(param) != ErrCodeE::Success) {
        return ErrCodeE::Failure;
    }
    CreateRtspServer(param);
    return ErrCodeE::Success;
}
