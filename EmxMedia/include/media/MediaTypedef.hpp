//
// Created by xiong on 2020/10/26.
//

#ifndef EMX_MEDIATYPEDEF_HPP
#define EMX_MEDIATYPEDEF_HPP

#include "core/EmxTypeDef.hpp"

//!定义音频帧的最大size
#define EMX_AUDIO_MAX_FRAME_SIZE 2048

namespace Emx {
    //!@brief 视频编码类型
    enum class VideoCodecE : uint8_t {
        H264 = 0, //!<H264编码
        H265, //!<H265编码
        MJPEG, //!<MJPEG编码
        BUTT
    };

    //!@brief 音频编码类型
    enum class AudioCodecE : uint8_t {
        LPCM = 0, //!<LPCM编码
        AAC, //!<AAC编码
        G711A, //!<G711A编码
        G711U, //!<G711U编码
        G726, //!<G726编码
        BUTT
    };

    //!@brief 媒体帧结构体定义，用于stream发送接收
    struct MediaFrame {
        //!@brief 媒体帧类型
        enum class TypeE : uint8_t {
            Venc = 0, //!<编码视频
            Vdec, //!<解码视频
            Aenc, //!<编码音频
            Pcm, //!<原始音频
            Adec, //!<解码音频
            Snap, //!<缩略图
            Data, //!<一般数据
        } type;
        int32_t chn; //!<通道号
        int32_t size; //!<数据大小
        uint8_t *data; //!<数据内存地址
        int32_t off; //!<数据在ringbuffer中的偏移量(内部用户不需要关心)
        uint64_t tsInUs; //!<微秒级时间戳
        uint32_t seq; //!<帧序号
        uint64_t trace; //!<用于记录编码器获取到数据并填充此结构体时刻的时钟，基于Time::GetMonoClockUs();
        bool paramChange; //!<标记编解码参数等是否发生了改变
        bool isKeyFrame; //!<针对于视频编码帧，当前是否为关键帧
    };
}
#endif //EMX_MEDIATYPEDEF_HPP
