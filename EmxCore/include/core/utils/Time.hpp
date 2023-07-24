//
// Created by xiong on 2022/6/9.
//

#ifndef EMX_Time_HPP
#define EMX_Time_HPP

#include "core/EmxTypeDef.hpp"
#include "json/json.h"
#include <mutex>

namespace Emx {

    //!时间相关函数
    class Time {
    public:
        //!获取64位当前UTC毫秒时间
        static uint64_t GetMs64();

        //!获取32位当前UTC毫秒时间
        static uint32_t GetMs() { return (uint32_t) GetMs64(); }

        //!获取32位当前UTC秒时间
        static uint32_t GetS();

        //!获取64位当前系统启动纳秒时间
        static uint64_t GetMonoClockNs();

        //!获取64位当前系统启动微秒时间
        static uint64_t GetMonoClockUs();

        //!获取64位当前系统启动毫秒时间
        static uint64_t GetMonoClockMS() { return GetMonoClockUs() / 1000; }

        /*!
         * @brief 获取当前系统时间字符串，格式YYYY-MM-DD hh:mm:ss
         * @param timeStr [in,out]时间字符串buffer
         * @param maxSize [in]时间字符串buffer大小
         * @return timeStr
         */
        static char *GetTimeStrNow(char *timeStr, int maxSize);

        /*!
         * @brief 获取指定UTC时间字符串，格式YYYY-MM-DD hh:mm:ss
         * @param utc [in]指定的UTC时间
         * @param timeStr [in,out]时间字符串buffer
         * @param maxSize [in]时间字符串buffer大小
         * @return timeStr
         */
        static char *GetTimeStr(uint32_t utc, char *timeStr, int maxSize);

        /*!
         * @brief 获设置系统UTC时间
         * @param utc [in]UTC时间
         */
        static void SetSysTime(uint32_t utc);

        /*!
         * @brief 设置系统日期
         * @param date [in]日期字符串，格式%Y-%m-%d %H:%M:%S
         */
        static void SetSysDate(const char *date);

        //!获取系统的时间是否已经同步
        static bool GetTimeSync();
    };

    //!记录操作时间
    class TimeRec {
    public:
        TimeRec() { Update(); }

        //!更新初始时间
        void Update() { m_timeStart = Time::GetMonoClockMS(); }

        //!获取当前时间与初始时间的差值
        uint32_t GetInv() const { return (uint32_t) (Time::GetMonoClockMS() - m_timeStart); }

    private:
        uint64_t m_timeStart;
    };

}

#endif //EMX_Time_HPP
