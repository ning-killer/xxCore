//
// Created by xiong on 2022/6/9.
//

#ifndef EMX_EasyJson_HPP
#define EMX_EasyJson_HPP

#include "core/EmxTypeDef.hpp"
#include "json/json.h"

namespace Emx {

    //!@brief Json相关操作
    class EasyJson {
    public:
        /*!
         * @brief 从文件中加载json数据
         * @param path [in] json文件路径
         * @param json [out] 存储json数据
         * @return ErrCodeE
         */
        static ErrCodeE Load(const char *path, Json::Value &json);

        /*!
         * @brief 解析json数据
         * @param data [in] json字符串
         * @param json [out] 存储json数据
         * @return ErrCodeE
         */
        static ErrCodeE Parse(const char *data, Json::Value &json);

        /*!
         * @brief 解析json数据
         * @param data [in] json字符串
         * @param json [out] 存储json数据
         * @return ErrCodeE
         */
        static ErrCodeE Parse(const char *data, int size, Json::Value &json);

        /*!
         * @brief 解析json数据
         * @param data [in] json字符串
         * @param json [out] 存储json数据
         * @return ErrCodeE
         */
        static ErrCodeE Parse(const std::string &data, Json::Value &json);

        /*!
         * @brief 序列化json，转换为字符串
         * @param json
         * @param jsonStr
         * @return ErrCodeE
         */
        static ErrCodeE Serialize(const Json::Value &json, std::string &jsonStr);
    };
}

#endif //EMX_EasyJson_HPP
