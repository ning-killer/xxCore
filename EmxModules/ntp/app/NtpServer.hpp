//
// Created by xiong on 2021/6/29.
//

#ifndef EMX_NtpServer_HPP
#define EMX_NtpServer_HPP

#include "EmxCore.hpp"
#include "Net.hpp"
#include <string>
#include <utility>
#include <vector>

namespace Emx {

    class NtpServer {
    public:
        NtpServer() : m_created(false) {}

        virtual ~NtpServer() {}

        using OnCreated = std::function<void()>;

        void Create(OnCreated cb, bool newThread);

        void Destroy();

    private:
        void OnResolve(ErrCodeE e, struct addrinfo *addr, void *arg);

        void RunNtpApp();


    private:
        EuvLoop m_loop;
        EuvTimer m_timer;
        // MsgRpcRep m_resp;
        OnCreated m_cb;
        struct Domain {
            EuvDns dns;
            std::string name;
            bool isIpv6;
            char ip[Net::MaxAddrSize];
        };
        int m_domainGotCnt;
        std::vector<Domain *> m_domainArray;
        int m_intervalS;
        bool m_21cnSpecial;
        bool m_created;
    };
}// namespace Emx

#endif//EMX_NtpServer_HPP
