//
// Created by xiong on 2022/7/9.
//

#ifndef EAPIL_EnvAlarmFace_HPP
#define EAPIL_EnvAlarmFace_HPP

#include "EnvBase.hpp"
#include "Zone.hpp"
#include "EnvZone.hpp"

namespace Emx {
    class EnvAlarmFace : public EnvBase {
      public:
        //!@brief 抓拍模式
        enum SnapMode : uint8_t {
            Quality = 0, //!< 质量优先
            Quick,       //!< 速度优先
        }; 

      public:
        EnvAlarmFace() : EnvBase("alarmFace") {}

        ErrCodeE Load() override;

        ErrCodeE Save() override;

        bool ena;
        int sensitivity;
        bool exposure_on;
        int exposure_adjust;
        SnapMode mode;
        int quality;
        EnvZone zone;
        float sensitivity_map[5];
        int quality_map[5];
    };

}

#endif //EAPIL_EnvAlarmFace_HPP
