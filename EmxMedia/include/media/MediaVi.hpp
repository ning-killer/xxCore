/*
* @Author: xiong
* @Date: 2023/4/3
*/

#ifndef EMX_MediaVi_HPP
#define EMX_MediaVi_HPP

#include <memory.h>
#include "MediaTypedef.hpp"
#include "json/json.h"

namespace Emx {

    //!@brief 视频输入
    class MediaVi {
    public:
        //!@brief 视频输入持久化参数
        struct Param {
            bool flip; //!<是否上下翻转
            bool mirror; //!<是否左右镜像
        };

        explicit MediaVi(int32_t chn) : m_chn(chn) {};

        virtual ~MediaVi() {};

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
#endif //EMX_MediaVi_HPP
