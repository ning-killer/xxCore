//
// Created by xiong on 2022/6/11.
//

#ifndef EMX_Mac_HPP
#define EMX_Mac_HPP

#include "EmxCore.hpp"

namespace Emx {

    //!@brief 提供设备MAC地址的获取与配置
    class Mac {
    public:
        Mac() {
            memset(m_mac, 0, sizeof(m_mac));
        }

        static const int MaxMacSize = 32;//!<MAC字符串最大大小

        /**
         * @brief 从flash中加载参数
         * @return ErrCodeE
         */
        ErrCodeE Load();

        /**
         * @brief 获取MAC地址
         * @return MAC地址
         */
        char *Get() { return m_mac; }

        /**
         * @brief 设置MAC地址
         * @param mac [in] MAC地址
         * @return ErrCodeE
         */
        ErrCodeE Set(const char *mac);

    private:
        char m_mac[MaxMacSize];
    };

}

#endif //EMX_Mac_HPP
