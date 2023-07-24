//
// Created by xiong on 2020/11/2.
//

#ifndef EMX_MEDIACLIENTVENC_HPP
#define EMX_MEDIACLIENTVENC_HPP

#include "MediaVenc.hpp"

namespace Emx {
    class MediaClientVenc : public MediaVenc {
    public:
        MediaClientVenc(int32_t chn) : MediaVenc(chn) {}

        ErrCodeE FlushIDR() override;

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

#endif //EMX_MEDIACLIENTVENC_HPP
