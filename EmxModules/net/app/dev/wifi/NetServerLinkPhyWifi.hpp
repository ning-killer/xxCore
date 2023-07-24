//
// Created by xiong on 2022/6/8.
//

#ifndef EMX_NetServerLinkPhyWifi_HPP
#define EMX_NetServerLinkPhyWifi_HPP

#include "NetServerLinkPhy.hpp"

namespace Emx {

    class NetServerLinkPhyWifi : public NetServerLinkPhy {
    public:
        NetServerLinkPhyWifi(const char *interface, NetServerResource &res) :
                NetServerLinkPhy(interface, res) {}

        virtual ~NetServerLinkPhyWifi() {}

        void Create(OnPhyChanged cb) override;

        void Destroy() override {}

        void SetPhyLinked(bool connected, const char *msg = nullptr);
    };

}

#endif //EMX_NetServerLinkPhyWifi_HPP
