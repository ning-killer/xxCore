//
// Created by xiong on 2022/6/9.
//

#ifndef EMX_Cmd_HPP
#define EMX_Cmd_HPP

#include "core/EmxTypeDef.hpp"

namespace Emx {

    //!@brief 执行外部命令
    class Cmd {
    public:

        /*!
         * @brief 使用popen的方式执行命令，支持格式化字符串输入
         * @param cmd [in] 格式化字符串
         * @param ... [in] 可变参数
         * @return ErrCodeE
         */
        static ErrCodeE Run(const char *cmd, ...);

        /*!
         * @brief 使用popen的方式执行命令，支持格式化字符串输入,
         * 并且会检测命令的执行结果是否返回0
         * @param cmd [in] 格式化字符串
         * @param ... [in] 可变参数
         * @return ErrCodeE
         */
        static ErrCodeE RunCheck(const char *cmd, ...);

        /*!
         * @brief 使用psystem的方式执行命令，支持格式化字符串输入
         * @param cmd [in] 格式化字符串
         * @param ... [in] 可变参数
         * @return ErrCodeE
         */
        static ErrCodeE System(const char *cmd, ...);

    };
}

#endif //EMX_Cmd_HPP
