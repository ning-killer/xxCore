//
// Created by xiong on 2020/11/23.
//
#include "json/json.h"
#include "core/EmxLog.hpp"
#include "core/EmxMsg.hpp"
#include "media/MediaClientAdec.hpp"
#include "media/MediaClientParam.hpp"
#include "MediaInner.hpp"

using namespace Emx;

ErrCodeE MediaClientAdec::SetParam(const MediaAdec::Param &param) {
    MediaClientParam p(m_chn, (uint32_t) MediaModE::Adec);
    return p.SetParam(&param, sizeof(param));
}

ErrCodeE MediaClientAdec::GetParam(MediaAdec::Param &param) {
    MediaClientParam p(m_chn, (uint32_t) MediaModE::Adec);
    return p.GetParam(&param, sizeof(param));
}

ErrCodeE MediaClientAdec::GetParamRange(Json::Value &range) {
    MediaClientParam p(m_chn, (uint32_t) MediaModE::Adec);
    return p.GetParamRange(range);
}

ErrCodeE MediaClientAdec::GetChnNum(int &num) {
    MediaClientParam p(0, (uint32_t) MediaModE::Adec);
    return p.GetChnNum(num);
}

MediaAdec::PlayStatusE MediaClientAdec::GetPlayStatus() {
    PlayStatusE status = PlayStatusE::Unknown;
    std::string rep;
    int32_t chn = m_chn;
    char dataReq[sizeof(chn)];
    memcpy(dataReq, &chn, sizeof(chn));
    ErrCodeE e = MsgRpcReqSync::Request(MEDIA_SERVER_MSG_ID,
                                        (uint32_t) MediaModE::Adec,
                                        (uint32_t) MediaAdecMethodE::GetPlayStatus,
                                        dataReq, (int) sizeof(dataReq),&(rep));
    if (e != ErrCodeE::Success) {
        emxloge("failed\n");
        return status;
    }
    memcpy(&status, rep.data(), sizeof(status));
    return status;
}

ErrCodeE MediaClientAdec::SetVolume(int32_t volume) {
    std::string rep;
    int32_t chn = m_chn;
    char data[sizeof(chn) + sizeof(volume)];
    memcpy(data, &chn, sizeof(chn));
    memcpy(data + sizeof(chn), &volume, sizeof(volume));
    ErrCodeE e = MsgRpcReqSync::Request(MEDIA_SERVER_MSG_ID,
                                        (uint32_t) MediaModE::Adec,
                                        (uint32_t) MediaAdecMethodE::SetVolume,
                                        data, sizeof(data), &(rep));
    if (e != ErrCodeE::Success) {
        emxloge("failed\n");
        return e;
    }
    return e;
}

ErrCodeE MediaClientAdec::SetVolumeTemp(int32_t volume) {
    std::string rep;
    int32_t chn = m_chn;
    char data[sizeof(chn) + sizeof(volume)];
    memcpy(data, &chn, sizeof(chn));
    memcpy(data + sizeof(chn), &volume, sizeof(volume));
    ErrCodeE e = MsgRpcReqSync::Request(MEDIA_SERVER_MSG_ID,
                                        (uint32_t) MediaModE::Adec,
                                        (uint32_t) MediaAdecMethodE::SetVolumeTemp,
                                        data, sizeof(data), &(rep));
    if (e != ErrCodeE::Success) {
        emxloge("failed\n");
        return e;
    }
    return e;
}


ErrCodeE MediaClientAdecStreamSync::PushFrame(int32_t chn, MediaAdec::Frame &frame, int32_t timeoutMs) {
    if (frame.size > EMX_AUDIO_MAX_FRAME_SIZE) {
        emxlogc("adec frame size %d > EMX_AUDIO_MAX_FRAME_SIZE(%d) !!!\n", frame.size, EMX_AUDIO_MAX_FRAME_SIZE);
        return ErrCodeE::FrameError;
    }
    char data[EMX_AUDIO_MAX_FRAME_SIZE + sizeof(MediaAdec::Frame)];
    memcpy(data, &frame, sizeof(frame));
    memcpy(data + sizeof(frame), frame.data, frame.size);
    return MsgTopicPublisherSync::Publish(MEDIA_TOPIC_ADEC_FRAME + chn,
                                          data, (int) sizeof(frame) + frame.size, timeoutMs);
}

ErrCodeE MediaClientAdecStreamSync::PushRawPcm(int32_t chn, MediaAdec::Frame &frame, int32_t timeoutMs) {
    if (frame.size > EMX_AUDIO_MAX_FRAME_SIZE) {
        emxlogc("adec frame size %d > EMX_AUDIO_MAX_FRAME_SIZE(%d) !!!\n", frame.size, EMX_AUDIO_MAX_FRAME_SIZE);
        return ErrCodeE::FrameError;
    }
    char data[EMX_AUDIO_MAX_FRAME_SIZE + sizeof(MediaAdec::Frame)];
    memcpy(data, &frame, sizeof(frame));
    memcpy(data + sizeof(frame), frame.data, frame.size);
    return MsgTopicPublisherSync::Publish(MEDIA_TOPIC_ADEC_RAW_PCM + chn,
                                          data, (int) sizeof(frame) + frame.size, timeoutMs);
}

ErrCodeE MediaClientAdecStreamSync::PlayFile(int32_t chn, const char *path, int32_t timeoutMs) {
    return PlayFileWithVolume(chn, path, -1, timeoutMs);
}

ErrCodeE MediaClientAdecStreamSync::PlayFileWithVolume(int32_t chn, const char *path,
                                                       int32_t volume, int32_t timeoutMs) {
    char data[sizeof(int) + EMX_MAX_PATH_SIZE];
    memcpy(data, &volume, sizeof(volume));
    strncpy(data + sizeof(volume), path, EMX_MAX_PATH_SIZE);
    return MsgTopicPublisherSync::Publish(MEDIA_TOPIC_ADEC_VOICE_PALY + chn,
                                          data, EMX_MAX_PATH_SIZE, timeoutMs);
}

ErrCodeE MediaClientAdecStreamSync::PlayCtrlQuit(int32_t chn, int32_t timeoutMs) {
    return MsgTopicPublisherSync::Publish(MEDIA_TOPIC_ADEC_PALY_CTRL + chn,
                                          "quit", (int) strlen("quit") + 1, timeoutMs);
}