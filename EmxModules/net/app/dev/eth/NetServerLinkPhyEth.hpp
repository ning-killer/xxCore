//
// Created by xiong on 2022/6/6.
//

#ifndef EMX_NetServerLinkPhyEth_HPP
#define EMX_NetServerLinkPhyEth_HPP

#include "NetServerLinkPhy.hpp"

namespace Emx {

    class NetServerLinkPhyEth : public NetServerLinkPhy {
    public:
        NetServerLinkPhyEth(const char *interface, NetServerResource &res) :
                NetServerLinkPhy(interface, res) {}

        virtual ~NetServerLinkPhyEth() {}

        void Create(OnPhyChanged cb) override;

        void Destroy() override;

    protected:
        void OnCheck(char *buffer, int size);

    private:
        EuvPoll m_poll;
        int m_sock;
    };

}

#endif //EMX_NetServerLinkPhyEth_HPP
