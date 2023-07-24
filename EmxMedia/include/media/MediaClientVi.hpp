/*
* @Author: xiong
* @Date: 2023/4/3
*/

#ifndef EMX_MediaClientVi_HPP
#define EMX_MediaClientVi_HPP

#include "MediaVi.hpp"

namespace Emx {

    class MediaClientVi : public MediaVi {
    public:
        MediaClientVi(int32_t chn) : MediaVi(chn) {}

        ErrCodeE SetParam(const Param &param) override;

        ErrCodeE GetParam(Param &param) override;

        ErrCodeE GetParamRange(Json::Value &range) override;

        /**
         * @brief 获取支持的总通道数量
         * @param num [out] 通道数量
         * @return ErrCode
         */
        static ErrCodeE GetChnNum(int &num);
    };

}

#endif //EMX_MediaClientVi_HPP
