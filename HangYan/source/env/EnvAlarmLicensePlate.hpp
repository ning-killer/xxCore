#ifndef EAPIL_EnvAlarmLicensePlate_HPP
#define EAPIL_EnvAlarmLicensePlate_HPP

#include "EnvBase.hpp"
#include "EnvSchedule.hpp"
#include "EnvZone.hpp"
#include "EnvStrategy.hpp"

namespace Emx {
    class EnvAlarmLicensePlate : public EnvBase {
    public:
        //!@brief 抓拍模式
        enum CaptureMode : uint8_t {
            optimal = 0, //!< 最优推图
            fastest,     //!< 最块推图
            regular,     //!< 定时推图
        };
    public:
        EnvAlarmLicensePlate() : EnvBase("alarmLicensePlate") {}

        ErrCodeE Load() override;

        ErrCodeE Save() override;

        bool on;
        int sensitivity; // 探测灵敏度, 0 - 100，默认值为60
        int quality;    // 质量评分（0~100），默认值为60
        CaptureMode mode; // 0：最优推图,1最快推图,2定时推图，默认值为0
        int scheduledCaptureTime; // 定时推图模式下的间隔时间,单位为秒，默认值为10
        int detectSite; // 检测地点,1：道路,2：岗亭（默认），默认值为2
        int exitMode; // 检测地点只为岗亭时有效,1：出口,车头离开,车尾进入2：入口,车头进入,车尾离开，默认值为1
        EnvZone zone;
        EnvStrategy strategy;
    };

}

#endif //EAPIL_EnvAlarmLicensePlate_HPP
