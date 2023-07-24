//
// Created by xiong on 2022/6/10.
//

#ifndef EMX_Sn_HPP
#define EMX_Sn_HPP

#include "EmxCore.hpp"

namespace Emx {

    //!@brief 提供设备序列号的获取与配置
    class Sn {
    public:
        Sn() {
            memset(m_sn, 0, sizeof(m_sn));
        }

        static const int MaxSnSize = 64;//!<SN字符串最大大小

        /**
         * @brief 从flash中加载参数
         * @return ErrCodeE
         */
        ErrCodeE Load();

        /**
         * @brief 获取SN
         * @return SN
         */
        char *Get() { return m_sn; }

        /**
         * @brief 设置SN
         * @param sn [in] SN
         * @return ErrCodeE
         */
        ErrCodeE Set(const char *sn);

    private:
        char m_sn[MaxSnSize];
    };

}

#endif //EMX_Sn_HPP
