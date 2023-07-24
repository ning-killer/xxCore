//
// Created by xiong on 2022/6/6.
//

#ifndef EMX_NetServerLinkLan_HPP
#define EMX_NetServerLinkLan_HPP

#include "Net.hpp"
#include "NetServerResource.hpp"

namespace Emx {

    class NetServerLinkLan {
    public:
        NetServerLinkLan(const char *interface, NetServerResource &res) :
                m_interface(interface),
                m_res(res),
                m_con4(false),
                m_con6(false) {}

        virtual ~NetServerLinkLan() {}

        using OnLanChanged = std::function<void(bool con4, bool con6)>;

        void Create(OnLanChanged cb);

        void Destroy();

    protected:
        void OnCheck();

    private:
        const char *m_interface;
        NetServerResource &m_res;
        EuvTimer m_timer;
        bool m_con4, m_con6;
        OnLanChanged m_cb;
    };

}

#endif //EMX_NetServerLinkLan_HPP
