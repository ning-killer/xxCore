/*
* @Author: xiong
* @Date: 2022/9/22
*/

#ifndef EAPIL_EnvBurn_HPP
#define EAPIL_EnvBurn_HPP

#include "EmxCore.hpp"

namespace Emx {

    class EnvBurn : public Param {
    public:
        EnvBurn() : Param("burnInfo") {}

        virtual ~EnvBurn() {}

        ErrCodeE Load();

        ErrCodeE Save();

    public:
        std::string OVDLoginPassword;
        std::string OVDMediaEncPassword;
        std::string cmei;
        std::string andlinkID;
        std::string andlinkKey;
    };

}
#endif //EAPIL_EnvBurn_HPP
