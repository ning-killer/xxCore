//
// Created by xiong on 2021/7/30.
//

#ifndef EAPILCOREMEDIA_MEDIAINNER_HPP
#define EAPILCOREMEDIA_MEDIAINNER_HPP

#define MEDIA_RING_BUFFER_KEY  0x12345000

//MediaServer Msg Client ID
#define MEDIA_SERVER_MSG_ID 0x00020000

#define MEDIA_TOPIC_SERVER (MEDIA_SERVER_MSG_ID+0x100)

#define MEDIA_TOPIC_SYSTEM (MEDIA_SERVER_MSG_ID+0x200)

//Isp
#define MEDIA_TOPIC_ISP (MEDIA_SERVER_MSG_ID+0x300)
#define MEDIA_TOPIC_ISP_INFO (MEDIA_TOPIC_ISP+0x01)
#define MEDIA_TOPIC_ISP_FPS_CHANGE (MEDIA_TOPIC_ISP+0x02)

//Venc
#define MEDIA_TOPIC_VENC (MEDIA_SERVER_MSG_ID+0x400)
#define MEDIA_TOPIC_VENC_DRAW_RECT (MEDIA_TOPIC_VENC+0x20)
#define MEDIA_TOPIC_VENC_FRAME (MEDIA_TOPIC_VENC+0x40)

//Aenc
#define MEDIA_TOPIC_AENC (MEDIA_SERVER_MSG_ID+0x500)
#define MEDIA_TOPIC_AENC_AENC (MEDIA_TOPIC_AENC+0x20)
#define MEDIA_TOPIC_AENC_PCM (MEDIA_TOPIC_AENC+0x40)

//Adec
#define MEDIA_TOPIC_ADEC (MEDIA_SERVER_MSG_ID+0x600)
#define MEDIA_TOPIC_ADEC_FRAME (MEDIA_TOPIC_ADEC+0x20)
#define MEDIA_TOPIC_ADEC_RAW_PCM (MEDIA_TOPIC_ADEC+0x40)
#define MEDIA_TOPIC_ADEC_VOICE_PALY (MEDIA_TOPIC_ADEC+0x60)
#define MEDIA_TOPIC_ADEC_PALY_CTRL (MEDIA_TOPIC_ADEC+0x80)

#define MEDIA_TOPIC_SNAP (MEDIA_SERVER_MSG_ID+0x700)

#define MEDIA_TOPIC_QR_SCAN (MEDIA_SERVER_MSG_ID+0x900)

#define MEDIA_TOPIC_AI (MEDIA_SERVER_MSG_ID+0xa00)

namespace Emx {
    ////////////////RPC///////////////////
    //Module
    enum class MediaModE : uint32_t {
        Server = 1,
        System = 2,
        ISP = 3,
        Venc = 4,
        Aenc = 5,
        Adec = 6,
        Snap = 7,
        QrScan = 8,
        Osd = 9,
        Ai = 10,
        Vi = 11,
    };

    //MediaServerMethod
    enum class MediaServerMethodE : uint32_t {
        Create = 1,
        Destroy,
        SwitchScene,
        Butt,
    };

    //MediaParamMethod
    enum class MediaParamMethodE : uint32_t {
        SetParam = 1,
        GetParam,
        GetParamRange,
        GetChnNum,
        Butt,
    };

    enum class MediaIspMethodE : uint32_t {
        SetRunMode = (uint32_t) MediaParamMethodE::Butt,
        GetRunMode,
        SetGrey,
    };

    enum class MediaQrScanMethodE : uint32_t {
        Start = 1,
        Stop,
    };

    enum class MediaAencMethodE : uint32_t {
        SetVolume = (uint32_t) MediaParamMethodE::Butt,
        Mute,
    };

    enum class MediaAdecMethodE : uint32_t {
        SetVolume = (uint32_t) MediaParamMethodE::Butt,
        SetVolumeTemp,
        GetPlayStatus,
    };

    enum class MediaSnapMethodE : uint32_t {
        Trigger = 1,
    };

    enum class MediaVencMethodE : uint32_t {
        FlushIdr = (uint32_t) MediaParamMethodE::Butt,
    };

    enum class MediaViMethodE : uint32_t {
        SetFps = (uint32_t) MediaParamMethodE::Butt,
        SetRotate,
    };

    enum class MediaDoubleLensMethodE : uint32_t {
        SwitchMode =(uint32_t)MediaServerMethodE::Butt,
        SetOutMode,
        GetOutMode,
        DeleteMode,
    };
}

#endif //EAPILCOREMEDIA_MEDIAINNER_HPP
