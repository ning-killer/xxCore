//
// Created by xiong on 2020/11/23.
//

#ifndef EMX_MEDIAADEC_HPP
#define EMX_MEDIAADEC_HPP

#include <memory.h>
#include "MediaTypedef.hpp"
#include "json/json.h"

namespace Emx {

    //!@brief 音频解码
    class MediaAdec {
    public:
        //!@brief 音频解码持久化参数
        struct Param {
            AudioCodecE codec; //!<编码类型
            int32_t bitRate; //!<码率
            int32_t sampleRate; //!<采样率
            int32_t volume; //!<音量大小
            uint8_t bitWidth; //!<采样位宽
        };

        //!@brief 音频解码帧结构定义(目前未使用MediaFrame结构)
        struct Frame {
            uint64_t tsInUs; //!<微秒级时间戳
            uint32_t seq; //!<帧序号
            int32_t size; //!<数据大小
            uint8_t *data; //!<数据内存地址
        };

        //!@brief 播放状态
        enum PlayStatusE : uint32_t {
            Unknown = 0, //!<未知
            Playing,     //!<播放中
            Finish,      //!<播放结束
        };

        explicit MediaAdec(int32_t chn) : m_chn(chn) {};

        virtual ~MediaAdec() {};

        /**
         * @brief 设置参数
         * @param param [in] 参数数据
         * @return ErrCodeE
         */
        virtual ErrCodeE SetParam(const Param &param) = 0;

        /**
         * @brief 获取参数
         * @param param [out] 参数数据
         * @return ErrCodeE
         */
        virtual ErrCodeE GetParam(Param &param) = 0;

        /**
         * @brief 获取参数取值范围
         * @param range [out] 参数取值范围
         * @return ErrCodeE
         */
        virtual ErrCodeE GetParamRange(Json::Value &range) = 0;

        /**
         * @brief 设置音量
         * @param volume [in] 音量大小
         * @return ErrCodeE
         */
        virtual ErrCodeE SetVolume(int32_t volume) = 0;

        /**
         * @brief 设置临时音量，调用这个接口配置的音量并不会被存储记录，重启后会失效
         * @param volume [in] 音量大小
         * @return ErrCodeE
         */
        virtual ErrCodeE SetVolumeTemp(int32_t volume) = 0;

        /**
         * @brief 获取媒体音频播放状态
         * @return PlayStatusE
         */
        virtual PlayStatusE GetPlayStatus() = 0;

        /**
         * @brief 判断参数是否改变
         * @param paramA [in] 参数A
         * @param paramB [in] 参数B
         * @retval true:改变
         * @retval false:未改变
         */
        static bool ParamChanged(const Param &paramA, const Param &paramB) {
            return memcmp(&paramA, &paramB, sizeof(Param));
        }

        /**
         * @brief 获取通道号
         * @return 通道号
         */
        int32_t GetChn() { return m_chn; }

    protected:
        int32_t m_chn;
    };

}
#endif //EMX_MEDIAADEC_HPP
