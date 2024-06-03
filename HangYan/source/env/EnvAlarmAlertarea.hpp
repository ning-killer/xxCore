#ifndef EAPIL_EnvAlarmAlertarea_HPP
#define EAPIL_EnvAlarmAlertarea_HPP

#include "EnvBase.hpp"
#include "EnvSchedule.hpp"
#include "EnvZone.hpp"
#include "EnvStrategy.hpp"

namespace Emx {

    class EnvAlarmAlertarea : public EnvBase {
    public:
    //!@brief 检测目标
    enum TargetType : uint32_t {
        notsupport = 0,                     //!< 不支持
        person = 1,                         //!< 人形
        motorvehicle = 2,                   //!< 机动车
        personAndmotor = 3,                 //!< 人形+机动车
        nonmotorvehicle = 4,                //!< 非机动车
        personAndnonmotor = 5,              //!< 人形+非机动车
        motorAndnonmotor = 6,               //!< 机动车+非机动车
        personAndmotorAndnonmotor = 7,      //!< 人形+机动车+非机动车
        others = 0x08,                      //!< 其他
    };
    public:
        EnvAlarmAlertarea() : EnvBase("alarmAlertarea") {}

        ErrCodeE Load() override;

        ErrCodeE Save() override;

        bool on;
        int sensitivity; // 探测灵敏度, 0 - 100，默认值为60
        TargetType type; // 检测目标, 位运算表示如：0x0F，最低位表示"人形"0x01，第二位表示"机动车"0x02，
                         // 第三位表示"非机动车"0x04，第四位表示"其他"0x08，默认0表示不支持
        int staymodeTime; // 单位为秒，区域被入侵后且目标在区域中停留时长满足要求后再告警，范围0-240秒，默认为0秒表示入侵后立刻报警
        EnvSchedule schedule;
        EnvZone zone;
        EnvStrategy strategy;
    };

}

#endif //EAPIL_EnvAlarmAlertarea_HPP
