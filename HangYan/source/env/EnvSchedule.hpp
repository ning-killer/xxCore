//
// Created by xiong on 2022/7/9.
//

#ifndef EAPIL_EnvSchedule_HPP
#define EAPIL_EnvSchedule_HPP

#include "EmxCore.hpp"

namespace Emx {
    class EnvSchedule {
    public:
        ErrCodeE Parse(const Json::Value &json);

        ErrCodeE Serialize(Json::Value &json);

    public:
        bool onOff;
        bool repeat[7];//周一到周日
        char startTime[12];//19:00:00
        char endTime[12];//19:00:01
        bool once;//是否是一次性执行计划
        char onceStartTime[20]; //一次性执行计划开始时间1920-00-00 19:00:00
        char onceEndTime[20]; //一次性执行计划结束时间1920-00-00 19:00:01
        enum class Status : uint8_t {//状态
            None = 0,
            On,
            Off
        };
        bool manual;
        char manualTime[20]; //手动开关的时间点1920-00-00 19:00:00
    };
}

#endif //EAPIL_EnvSchedule_HPP
