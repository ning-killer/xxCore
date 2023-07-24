//
// Created by xiong on 2022/6/9.
//

#include "core/utils/Time.hpp"
#include "core/utils/File.hpp"
#include <sys/time.h>

using namespace Emx;


uint64_t Time::GetMs64() {
    timeval tv{};
    gettimeofday(&tv, (struct timezone *) nullptr);
    return (uint64_t) tv.tv_sec * 1000 + (uint64_t) tv.tv_usec / 1000;
}

uint32_t Time::GetS() {
    time_t time_now;
    time(&time_now);
    return (uint32_t) time_now;
}

uint64_t Time::GetMonoClockNs() {
    timespec tp = {};
    clock_gettime(CLOCK_MONOTONIC, &tp);
    return (uint64_t) tp.tv_sec * 1000000000 + tp.tv_nsec;
}

uint64_t Time::GetMonoClockUs() {
    timespec tp = {};
    clock_gettime(CLOCK_MONOTONIC, &tp);
    return tp.tv_sec * 1000000 + tp.tv_nsec / 1000;
}

char *Time::GetTimeStr(uint32_t utc, char *timeStr, int maxSize) {
    auto timep = (time_t) utc;
    struct tm *p = localtime(&timep);
    snprintf(timeStr, maxSize, "%04d-%02d-%02d %02d:%02d:%02d",
             (1900 + p->tm_year), (1 + p->tm_mon), p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
    return timeStr;
}

char *Time::GetTimeStrNow(char *timeStr, int maxSize) {
    time_t timep;
    time(&timep);
    return GetTimeStr(timep, timeStr, maxSize);
}

void Time::SetSysTime(uint32_t utc) {
    timeval tv = {(time_t) utc, 0};
    settimeofday(&tv, (struct timezone *) 0);
}

void Time::SetSysDate(const char *date) {
    tm tmp_time = {};
    time_t t;
    //2018-09-12 11:05:57
    strptime(date, "%Y-%m-%d %H:%M:%S", &tmp_time);
    t = mktime(&tmp_time);
    SetSysTime(t);
}

bool Time::GetTimeSync() {
    time_t timenow;
    tm tt = {};
    time(&timenow);
    localtime_r(&timenow, &tt);
    return tt.tm_year + 1900 > 2000;
}

