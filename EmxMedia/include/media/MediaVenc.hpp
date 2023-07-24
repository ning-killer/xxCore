//
// Created by xiong on 2020/6/15.
//

#ifndef EMX_MEDIAVENC_HPP
#define EMX_MEDIAVENC_HPP

#include <memory.h>
#include "MediaTypedef.hpp"
#include "json/json.h"

namespace Emx {

    //!@brief 视频编码
    class MediaVenc {
    public:
        //!@brief 视频编码码率控制类型
        enum class RcModeE : uint8_t {
            CBR = 0, //!<固定码率
            VBR, //!<可变码率
            AVBR, //!<平均可变码率
            BUTT,
        };

        //!@brief 视频编码持久化参数
        struct Param {
            VideoCodecE codec; //!<编码类型
            uint16_t width; //!<图像宽
            uint16_t height; //!<图像高
            uint16_t fps; //!<帧率
            uint16_t iFrameInv; //!<I帧间隔单位s
            uint32_t bitRate; //!<码率
            RcModeE rcMode; //!<码率控制模式
            bool drawRect; //!<是否画框
        };

        explicit MediaVenc(int32_t chn) : m_chn(chn) {};

        virtual ~MediaVenc() {};

        /**
         * @brief 刷新I帧
         * @return ErrCodeE
         */
        virtual ErrCodeE FlushIDR() = 0;

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


#endif //EMX_MEDIAVENC_HPP
