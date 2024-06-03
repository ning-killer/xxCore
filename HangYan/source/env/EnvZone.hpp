//
// Created by xiong on 2022/7/9.
//

#ifndef EAPIL_EnvZone_HPP
#define EAPIL_EnvZone_HPP

#include "Zone.hpp"

namespace Emx {
    class EnvZone : public Zone {
      public:  
        EnvZone() : Zone() {}

        /**
         * @brief 矩形区域填充
         * @param json [in] 区域json数据
         * @return 结果
         */
        ErrCodeE SetOvdZone(const Json::Value &json);
        
        /**
         * @brief 多边形区域填充
         * @param json [in] 区域json数据
         * @return 结果
         */
        ErrCodeE SetOvdZoneEx(const Json::Value &json);

        Json::Value ovdZone;
    };
}

#endif //EAPIL_EnvZone_HPP
