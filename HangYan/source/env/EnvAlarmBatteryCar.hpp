#ifndef EAPIL_EnvAlarmBatteryCar_HPP
#define EAPIL_EnvAlarmBatteryCar_HPP

#include "EnvBase.hpp"
#include "EnvSchedule.hpp"
#include "EnvZone.hpp"
#include "EnvStrategy.hpp"

namespace Emx {
    class EnvAlarmBatteryCar : public EnvBase {
    public:
        //!@brief 抓拍模式
        enum CaptureMode : uint8_t {
            optimal = 0, //!< 最优推图
            fastest,     //!< 最块推图
            regular,     //!< 定时推图
        };
    public:
        EnvAlarmBatteryCar() : EnvBase("alarmBatteryCar") {}

        ErrCodeE Load() override;

        ErrCodeE Save() override;

        bool on;
        std::string detectModelist; // 可选值有"electromobile","bike",,默认“electromobile”
        int sensitivity; // 探测灵敏度, 0 - 100，默认值为60
        int quality;    // 质量评分（0~100），默认值为60
        CaptureMode mode; // 0：最优推图,1最快推图,2定时推图，默认值为0
        int scheduledCaptureTime; // 定时推图模式下的间隔时间,单位为秒，默认值为10
        EnvZone zone;
        EnvStrategy strategy;
    };

}

#endif //EAPIL_EnvAlarmBatteryCar_HPP
