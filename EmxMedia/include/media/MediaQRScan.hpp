//
// Created by xiong on 2021/6/25.
//

#ifndef EMX_MediaQRScan_HPP
#define EMX_MediaQRScan_HPP

#include "core/EmxTypeDef.hpp"

namespace Emx {
    //!@brief 二维码扫描
    class MediaQRScan {
    public:
        explicit MediaQRScan(int32_t chn) : m_chn(chn) {};

        virtual ~MediaQRScan() {};

        /**
         * @brief 启动扫码
         * @return ErrCodeE
         */
        virtual ErrCodeE Start() = 0;

        /**
         * @brief 结束扫码
         * @return ErrCodeE
         */
        virtual ErrCodeE Stop() = 0;

        /**
         * @brief 获取通道号
         * @return 通道号
         */
        int32_t GetChn() { return m_chn; }

    protected:
        int32_t m_chn;
    };
}
#endif //EMX_MediaQRScan_HPP

