//
// Created by xiong on 2022/6/10.
//

#ifndef EMX_DevInfo_HPP
#define EMX_DevInfo_HPP

#include "EmxCore.hpp"

namespace Emx {

    //!@brief 提供获取设备类型版本信息等相关的接口
    class DevInfo {
    public:
        DevInfo() {
            memset(&m_inner, 0, sizeof(Info));
            memset(&m_outer, 0, sizeof(Info));
        }

        static const int MaxTypeSize = 64;//!<类型字符串最大大小
        static const int MaxVersionSize = 64;//!<版本字符串最大大小

        //!@brief  类型版本信息
        struct Info {
            char type[MaxTypeSize];//!<类型
            char version[MaxVersionSize];//!<版本
        };

        /**
         * @brief 从flash中加载参数
         * @return ErrCodeE
         */
        ErrCodeE Load();

        /**
         * @brief 获取内部使用信息
         * @return 类型版本信息
         */
        Info *GetInner() { return &m_inner; }

        /**
         * @brief 获取外部使用信息
         * @return 类型版本信息
         */
        Info *GetOuter() { return &m_outer; }

        /**
         * @brief 获取硬件版本号
         * @return 硬件版本号
         */
        Info *GetHardware() { return &m_hardware; }

    private:
        Info m_inner;
        Info m_outer;
        Info m_hardware;
    };
}

#endif //EMX_DevInfo_HPP
