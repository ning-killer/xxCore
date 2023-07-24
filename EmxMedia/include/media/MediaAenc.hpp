//
// Created by xiong on 2020/11/19.
//

#ifndef EMX_MEDIAAENC_HPP
#define EMX_MEDIAAENC_HPP

#include <memory.h>
#include "MediaTypedef.hpp"
#include "json/json.h"

namespace Emx {

    //!@brief 音频编码
    class MediaAenc {
    public:
        //!@brief 音频编码持久化参数
        struct Param {
            AudioCodecE codec; //!<编码类型
            int32_t bitRate; //!<码率
            int32_t sampleRate; //!<采样率
            int32_t volume; //!<音量大小
            uint8_t bitWidth; //!<采样位宽
            bool mute; //!<是否静音
        };

        explicit MediaAenc(int32_t chn) : m_chn(chn) {};

        virtual ~MediaAenc() {};

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
         * @brief 设置静音
         * @param ena [in] 是否静音
         * @return ErrCodeE
         */
        virtual ErrCodeE Mute(bool ena) = 0;

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
#endif //EMX_MEDIAAENC_HPP
