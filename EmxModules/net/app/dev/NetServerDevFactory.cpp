//
// Created by xiong on 2021/4/13.
//

#include "NetServerDevFactory.hpp"
#include "NetServerWifi.hpp"
#include "NetServerEth.hpp"
#include "NetServerMobile.hpp"

using namespace Emx;

NetServerDev *NetServerDevFactory::Make(NetServerResource &res, Json::Value &devJsonParam) {
    auto type = Net::GetDevType(devJsonParam["name"].asCString());
    switch (type) {
        case Net::DevE::Eth:
            return new(std::nothrow)NetServerEth(res, devJsonParam);
        case Net::DevE::Wifi:
            return new(std::nothrow)NetServerWifi(res, devJsonParam);
        case Net::DevE::Mobile:
            return new(std::nothrow)NetServerMobile(res, devJsonParam);
        default:
            return nullptr;
    }
}
