//
// Created by xiong on 2022/3/2.
//

#ifndef EMX_EmxParam_HPP
#define EMX_EmxParam_HPP

#include <json/json.h>
#include "core/EmxTypeDef.hpp"

namespace Emx {
    //!@brief 关于系统需要持久化保存的参数的相关操作
    class Param {
    public:
        /**
         * @brief 构造函数
         * @param [in] name 指定参数的名称(参数实际存储的文件名去掉后缀.json)
         */
        Param(const char *name);

        /**
         * @brief 构造函数
         * @param [in] name 指定参数的名称(参数实际存储的文件名去掉后缀.json)
         */
        Param(const std::string &name);

        /**
         * @brief 保存参数
         * @param [in] json 参数的json对象
         * @return ErrCode
         */
        ErrCodeE Set(Json::Value &json);

        /**
         * @brief 获取参数
         * @param [out] json 参数的json对象
         * @return ErrCode
         */
        ErrCodeE Get(Json::Value &json);

        /**
         * @brief 获取默认参数
         * @param [out] json 参数的json对象
         * @return ErrCode
         */
        ErrCodeE GetDefault(Json::Value &json);

        /**
         * @brief 将存储的参数恢复为默认值
         * @return ErrCode
         */
        ErrCodeE Reset();

        /**
         * @brief 将设备的所有参数恢复为默认值(除resetReserved.json中声明保留的文件)
         * @return ErrCode
         */
        static ErrCodeE ResetAllToFactory();

    private:
        char m_name[EMX_MAX_PATH_SIZE];
    };
}
#endif //EMX_EmxParam_HPP

