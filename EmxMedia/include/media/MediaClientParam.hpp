//
// Created by xiong on 2020/11/20.
//

#ifndef EMX_MEDIACLIENTPARAM_HPP
#define EMX_MEDIACLIENTPARAM_HPP

#include "MediaTypedef.hpp"

namespace Emx {
    //!@brief 客户端参数基类，方便通用操作
    class MediaClientParam {
    public:
        /**
         * @brief 构造函数
         * @param chn [in] 通道号
         * @param moduleId [in] 模块号
         */
        MediaClientParam(int32_t chn, uint32_t moduleId) :
                m_chn(chn), m_moduleId(moduleId) {}

        /**
         * @brief 设置参数
         * @param param [in] 参数数据
         * @param size [in] 参数数据大小
         * @param timeoutMs [in] 超时时间，默认2s
         * @return ErrCodeE
         */
        ErrCodeE SetParam(const void *param, int32_t size, int32_t timeoutMs = 2000);

        /**
         * @brief 获取参数
         * @param param [out] 参数数据
         * @param size [in] 参数数据大小
         * @param timeoutMs [in] 超时时间，默认2s
         * @return ErrCodeE
         */
        ErrCodeE GetParam(void *param, int32_t size, int32_t timeoutMs = 2000);

        /**
         * @brief 获取参数取值范围
         * @param range [out] 参数取值范围
         * @param timeoutMs [in] 超时时间，默认2s
         * @return ErrCodeE
         */
        ErrCodeE GetParamRange(Json::Value &range, int32_t timeoutMs = 2000);

        /**
         * @brief 获取支持的总通道数量
         * @param num [out] 通道数量
         * @param timeoutMs [in] 超时时间，默认2s
         * @return ErrCode
         */
        ErrCodeE GetChnNum(int &num, int32_t timeoutMs = 2000);

    private:
        int32_t m_chn;
        uint32_t m_moduleId;
    };
}


#endif //EMX_MEDIACLIENTPARAM_HPP
