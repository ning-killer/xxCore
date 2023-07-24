/*
* @Author: xiong
* @Date: 2023/1/12
*/

#ifndef EMX_OnvifClient_HPP
#define EMX_OnvifClient_HPP

#include "Onvif.hpp"

namespace Emx {

    class OnvifClient : public Onvif {
    public:
        ErrCodeE GetParam(Param &param) override;

        ErrCodeE SetParam(const Param &param) override;
    };

}

#endif //EMX_OnvifClient_HPP
