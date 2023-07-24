/*
* @Author: xiong
* @Date: 2022/10/12
*/

#ifndef EMX_NightVision_HPP
#define EMX_NightVision_HPP

#include "EmxCore.hpp"

namespace Emx {

    class NightVision {
    public:
        enum class AutoModeE : uint8_t {
            IrNightVision = 0,
            ColorNightVision
        };
        enum class ModeE : uint8_t {
            Day = 0,
            IrNightVision,
            ColorNightVision,
        };

        struct Param {
            bool manual;
            ModeE manualMode;
            AutoModeE autoMode;
            struct Sensitivity {
                uint8_t day2ir;
                uint8_t ir2day;
                uint8_t day2color;
                uint8_t color2day;
            } sensitivity;
        };

        struct Status {
            bool switching;
            ModeE mode;
        };

        virtual ErrCodeE SetParam(const Param &param) = 0;

        virtual ErrCodeE GetParam(Param &param) = 0;

        virtual ErrCodeE SetParamTemp(const Param &param) = 0;

        virtual ErrCodeE GetParamTemp(Param &param) = 0;

        virtual ErrCodeE GetStatus(Status &status) = 0;
    };

}

#endif //EMX_NightVision_HPP
