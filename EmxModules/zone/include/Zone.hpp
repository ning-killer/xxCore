//
// Created by xiong on 2022/7/8.
//

#ifndef EMX_Zone_HPP
#define EMX_Zone_HPP

#include "EmxCore.hpp"

namespace Emx {

    class Zone {
    public:
        static const int Width = 40;
        static const int Height = 30;

        Zone() : map() {}

        ErrCodeE Parse(const Json::Value &json);

        ErrCodeE Serialize(Json::Value &json);

        bool map[Width * Height];
    };

}

#endif //EMX_Zone_HPP
