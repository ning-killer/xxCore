//
// Created by xiong on 2021/4/13.
//

#ifndef EMX_NetServerDevFactory_HPP
#define EMX_NetServerDevFactory_HPP

#include "NetServerResource.hpp"
#include "NetServerDev.hpp"

namespace Emx {

    class NetServerDevFactory {
    public:
        static NetServerDev *Make(NetServerResource &res, Json::Value &devJsonParam);
    };
}

#endif //EMX_NetServerDevFactory_HPP
