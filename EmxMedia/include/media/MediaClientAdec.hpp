//
// Created by xiong on 2020/11/23.
//

#ifndef EMX_MEDIACLIENTADEC_HPP
#define EMX_MEDIACLIENTADEC_HPP

#include "json/json.h"
#include "MediaAdec.hpp"

namespace Emx {
    class MediaClientAdec : public MediaAdec {
    public:
        MediaClientAdec(int32_t chn) : MediaAdec(chn) {}

        ErrCodeE SetParam(const Param &param) override;

        ErrCodeE GetParam(Param &param) override;

        ErrCodeE GetParamRange(Json::Value &range) override;

        /**
         * @brief 获取支持的总通道数量
         * @param num [out] 通道数量
         * @return ErrCode
         */
        static ErrCodeE GetChnNum(int &num);

        ErrCodeE SetVolume(int32_t volume) override;

        ErrCodeE SetVolumeTemp(int32_t volume) override;

        PlayStatusE GetPlayStatus() override;
    };

    //!@brief 音频流解码接口,均为同步阻塞操作
    class MediaClientAdecStreamSync {
    public:
        /**
         * @brief 发送音频帧到媒体服务进行解码播放，这里发送的音频帧的编码格式需要与MediaClientAenc中配置的解码格式一致
         * @param chn [in] 目的通道号
         * @param frame [in] 与音频帧信息
         * @param timeoutMs [in] 超时时间，默认1s
         * @return ErrCode
         */
        static ErrCodeE PushFrame(int32_t chn, MediaAdec::Frame &frame, int32_t timeoutMs = 1000);

        /**
         * @brief 发送Pcm音频帧到媒体服务直接播放
         * @param chn [in] 目的通道号
         * @param frame [in] 音频帧信息
         * @param timeoutMs [in] 超时时间，默认1s
         * @return ErrCode
         */
        static ErrCodeE PushRawPcm(int32_t chn, MediaAdec::Frame &frame, int32_t timeoutMs = 1000);

        /**
         * @brief 添加音频文件路径至媒体服务的播放列表中，添加后此函数就会返回，列表为空时会立刻播放
         * @param chn [in] 目的通道号
         * @param path [in] 音频文件路径
         * @param timeoutMs [in] 超时时间，默认1s
         * @return ErrCode
         */
        static ErrCodeE PlayFile(int32_t chn, const char *path, int32_t timeoutMs = 1000);

        /**
         * @brief 添加音频文件路径至媒体服务的播放列表中，添加后此函数就会返回，列表为空时会立刻播放
         * @param chn [in] 目的通道号
         * @param path [in] 音频文件路径
         * @param volume [in] 希望临时以此音量播放文件
         * @param timeoutMs [in] 超时时间，默认1s
         * @return ErrCode
         */
        static ErrCodeE PlayFileWithVolume(int32_t chn, const char *path, int32_t volume, int32_t timeoutMs = 1000);

        /**
         * @brief 终止播放并清空媒体服务播放列表
         * @param chn [in] 目的通道号
         * @param timeoutMs [in] 超时时间，默认1s
         * @return ErrCode
         */
        static ErrCodeE PlayCtrlQuit(int32_t chn, int32_t timeoutMs = 1000);
    };
}


#endif //EMX_MEDIACLIENTADEC_HPP
