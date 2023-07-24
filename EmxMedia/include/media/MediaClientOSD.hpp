/*
* @Author: xiong
* @Date: 2022/10/11
*/

#ifndef EMX_MediaClientOSD_HPP
#define EMX_MediaClientOSD_HPP

#include "MediaOSD.hpp"

namespace Emx {

    class MediaClientOSD : public MediaOSD {
    public:
        MediaClientOSD(int chn) : MediaOSD(chn) {}

        /**
        * @brief 设置参数
        * @param param [in] 参数数据数组
        * @return ErrCodeE
        */
        ErrCodeE SetParam(const std::vector<std::shared_ptr<Param>> &param) override;

        /**
         * @brief 获取参数
         * @param param [out] 参数数据数组
         * @return ErrCodeE
         */
        ErrCodeE GetParam(std::vector<std::shared_ptr<Param>> &param) override;

        /**
         * @brief 获取参数取值范围
         * @param range [out] 参数取值范围
         * @return ErrCodeE
         */
        ErrCodeE GetParamRange(Json::Value &range) override;

        /**
         * @brief 获取支持的总通道数量
         * @param num [out] 通道数量
         * @return ErrCode
         */
        static ErrCodeE GetChnNum(int &num);
    };

}

#endif //EMX_MediaClientOSD_HPP
