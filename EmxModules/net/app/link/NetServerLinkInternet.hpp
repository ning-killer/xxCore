//
// Created by xiong on 2022/6/6.
//

#ifndef EMX_NetServerLinkInternet_HPP
#define EMX_NetServerLinkInternet_HPP

#include "Net.hpp"
#include "NetServerResource.hpp"

namespace Emx {

class NetServerLinkInternetConnect {
   public:
    using OnConnectionChanged = std::function<void(bool connected)>;
    using Socket = std::function<int()>;
    NetServerLinkInternetConnect(const char *interface, NetServerResource &res,
                                 int family)
        : m_interface(interface), m_res(res), m_family(family), m_sock(-1), m_id(0), m_pingSend(false) {}
    virtual ~NetServerLinkInternetConnect() {}

    void Create(OnConnectionChanged cb);
    void Destroy();

   private:
    void OnResolve(ErrCodeE e, struct addrinfo *addr, void *arg);
    void Start();
    void ReStart();
    void RunDns();
    void ConnectTcp();
    void OnConnectResult(bool connected);
    const char *GetProtoName() { return m_family == AF_INET ? "ipv4" : "ipv6"; }

    void RecvPong();
    int SocketCreate();
    void SendPingPack(int sock, const char *ip, uint16_t id, uint16_t sequence, uint32_t timeMs);
    bool RecvPongPack(int sock, uint16_t &id, uint16_t &sequence, uint32_t &timeMs, char *ip);
    
    protected:
    static uint16_t CheckSum(uint16_t *buffer, int length);

   private:
    const char *m_interface;
    NetServerResource m_res;
    int m_family;
    struct Domain {
        char name[EMX_MAX_PATH_SIZE];
        uint16_t port;
        struct Addr {
            char ip[Net::MaxAddrSize];
        };
        std::vector<Addr> addrArray;
    };
    int m_domainIdx;
    std::vector<Domain> m_domainArray;
    EuvDns m_dns;
    EuvTcp m_tcp;
    int m_tcpIdx;
    EuvTimer m_timer;
    OnConnectionChanged m_cb;
    char m_gateway[Net::MaxAddrSize];

    EuvPoll m_poll;
    int m_sock;
    uint16_t m_id;
    bool m_pingSend;
    // bool m_created;
};

class NetServerLinkInternet {
   public:
    using OnInternetChanged = std::function<void(bool con4, bool con6)>;

    NetServerLinkInternet(const char *interface, NetServerResource &res)
        : m_interface(interface),
          m_res(res),
          m_connect4(interface, res, AF_INET),
          m_connect6(interface, res, AF_INET6) {}

    virtual ~NetServerLinkInternet() {}

    void Create(OnInternetChanged cb);

    void Destroy();

   private:
    void OnConnect4Done(bool connected);
    void OnConnect6Done(bool connected);

   private:
    const char *m_interface;
    NetServerResource &m_res;
    OnInternetChanged m_cb;
    NetServerLinkInternetConnect m_connect4;
    bool m_connect4last;
    NetServerLinkInternetConnect m_connect6;
    bool m_connect6last;
};
}  // namespace Emx

#endif  // EMX_NetServerLinkInternet_HPP
