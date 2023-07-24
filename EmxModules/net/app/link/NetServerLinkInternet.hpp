//
// Created by xiong on 2022/6/6.
//

#ifndef EMX_NetServerLinkInternet_HPP
#define EMX_NetServerLinkInternet_HPP

#include "Net.hpp"
#include "NetServerResource.hpp"

namespace Emx {

    class NetServerLinkInternet {
    public:
        using OnInternetChanged = std::function<void(bool con4, bool con6)>;

        NetServerLinkInternet(const char *interface, NetServerResource &res) :
                m_interface(interface),
                m_res(res) {}

        virtual ~NetServerLinkInternet() {}

        void Create(OnInternetChanged cb);

        void Destroy();

    protected:
        void Run();

        struct InetCon {
            void Reset() {
                conTmp = false;
                tcpIdx = 0;
                addrArray.clear();
            }

            EuvTcp tcp;
            struct Addr {
                char ip[Net::Ipv6AddrSize];
                uint16_t port;
            };
            std::vector<Addr> addrArray;
            int tcpIdx;
            bool conTmp;
            bool connected;
            using Socket = std::function<int()>;
            Socket socket;
        };

        void ConnectTcp(InetCon &con);

        void OnResolve(ErrCodeE e, struct addrinfo *addr, void *arg);

        void OnConnectResult(InetCon &con, bool connected);

    private:
        const char *m_interface;
        NetServerResource &m_res;
        EuvTimer m_timer;

        EuvDns m_dns;
        struct Domain {
            char name[EMX_MAX_PATH_SIZE];
            uint16_t port;
        };
        std::vector<Domain> m_domainArray;
        int m_dnsIdx;
        InetCon m_con4, m_con6;
        OnInternetChanged m_cb;
    };


}

#endif //EMX_NetServerLinkInternet_HPP
