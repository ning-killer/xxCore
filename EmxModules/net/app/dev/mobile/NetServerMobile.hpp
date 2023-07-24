//
// Created by xiong on 2021/3/25.
//

#ifndef EMX_NetServerMobile_HPP
#define EMX_NetServerMobile_HPP

#include "NetServerDev.hpp"

namespace Emx {
    class NetServerMobile : public NetServerDev {
    public:
        NetServerMobile(NetServerResource &res, Json::Value &devJsonParam) :
                NetServerDev(res, devJsonParam, nullptr) {}

        virtual ~NetServerMobile() {}
    };
}


#endif //EMX_NetServerMobile_HPP
