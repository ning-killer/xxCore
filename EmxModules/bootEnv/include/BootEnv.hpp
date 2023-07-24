//
// Created by xiong on 2022/2/25.
//

#ifndef EMX_bootEnv_HPP
#define EMX_bootEnv_HPP

#include "EmxCore.hpp"
#include <map>

namespace Emx {

    //!@brief 提供应用层读写uboot环境变量的接口
    class BootEnv {
    public:
        using EnvMap = std::map<std::string, std::string>;

        /*!
         * @brief 获取uboot全部环境变量
         * @param map [out] uboot中的所有环境变量
         * @return ErrCodeE
         */
        static ErrCodeE GetEnv(EnvMap &map);

        /*!
         * @brief 获取uboot某个环境变量
         * @param key [in] 环境变量名
         * @param value [out] 环境变量值
         * @return ErrCodeE
         */
        static ErrCodeE GetEnv(const char *key, std::string &value);

        /*!
         * @brief 配置uboot环境变量
         * @param map [in] 需要配置的环境变量key-value，当value=""时，key会被删除
         * @return ErrCodeE
         */
        static ErrCodeE SetEnv(EnvMap &map);

        /*!
         * @brief 配置uboot环境变量
         * @param key [in] 需要配置的环境变量名
         * @param value [in] 需要配置的环境变量值，当value=""时，key会被删除
         * @return ErrCodeE
         */
        static ErrCodeE SetEnv(const char *key, const char *value);

    private:
        static ErrCodeE Lock(int &fd);

        static void UnLock(int &fd);

        static void CheckCfgDir();
    };
}
#endif //EMX_bootEnv_HPP

