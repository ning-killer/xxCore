//
// Created by xiong on 2020/11/19.
//

#ifndef EMX_MEDIACLIENTAENC_HPP
#define EMX_MEDIACLIENTAENC_HPP

#include "MediaAenc.hpp"

namespace Emx {
    class MediaClientAenc : public MediaAenc {
    public:
        MediaClientAenc(int32_t chn) : MediaAenc(chn) {}

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

        ErrCodeE Mute(bool ena) override;
    };
}


#endif //EMX_MEDIACLIENTAENC_HPP
