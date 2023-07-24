//
// Created by xiong on 2021/3/25.
//

#ifndef EMX_NetServerEth_HPP
#define EMX_NetServerEth_HPP

#include "NetServerLinkPhyEth.hpp"
#include "NetServerDev.hpp"

namespace Emx {

    class NetServerEth : public NetServerDev {
    public:
        NetServerEth(NetServerResource &res, Json::Value &devJsonParam) :
                NetServerDev(res, devJsonParam, &m_phy), m_phy(m_interface, res) {}

        virtual ~NetServerEth() {}

    private:
        NetServerLinkPhyEth m_phy;
    };
}


#endif //EMX_NetServerEth_HPP
