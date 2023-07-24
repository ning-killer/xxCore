/*
* @Author: xiong
* @Date: 2022/9/23
*/

#ifndef EMX_TimeZone_HPP
#define EMX_TimeZone_HPP

#include "EmxCore.hpp"

namespace Emx {

    class TimeZone {
    public:
        TimeZone() {
            memset(m_zone, 0, sizeof(m_zone));
        }

        static const int MaxZoneSize = 128;//!<zone字符串最大大小
        /**
         * @brief 从flash中加载参数
         * @return ErrCodeE
         */
        ErrCodeE Load();

        /*!
         * @brief 设置系统时区(更改zone.json同时配置时区文件)
         * @param zone [in]时区名("GMT-8")
         * @return ErrcodeE
         */
        ErrCodeE Set(const char *zone);

        /*!
         * @brief 将zone.json中的时区复制到localtime文件
         * @return ErrcodeE
         */
        ErrCodeE CopyTimeZoneToLocalTime();

        /*!
         * @brief 获取当前时区文件名
         * @return 时区文件名
         */
        char *Get() { return m_zone; }

        /*!
         * @brief 获取时区文件列表
         * @param list [out] 获取时区列表
         * @return 时区文件名
         */
        void GetZoneList(std::vector<std::string> &list);

    private:
        void GetZoneListDir(const char *path, const std::string &name, std::vector<std::string> &list);

        char m_zone[MaxZoneSize];
    };

}

#endif //EMX_TimeZone_HPP
