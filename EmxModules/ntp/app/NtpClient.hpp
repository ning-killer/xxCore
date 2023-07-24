//
// Created by xiong on 2021/6/29.
//

#ifndef EMX_NtpClient_HPP
#define EMX_NtpClient_HPP

#include <string>
#include <utility>
#include <vector>
#include "EmxCore.hpp"

namespace Emx {
    class NtpDns {
    public:
        using OnGetIp = std::function<void(NtpDns *dns)>;

        NtpDns(EuvLoop &loop, std::string &&name, bool isIpv6, OnGetIp cb) :
                m_name(name), m_isIpv6(isIpv6), m_loop(loop), m_cb(std::move(cb)) {
            m_timer.Create(m_loop);
        }

        void Create() { m_dns.Create(&m_loop); }

        void Destroy() { m_dns.Destroy(); }

        void Run();

    private:
        void OnResolve(ErrCodeE e, struct addrinfo *addr, void *arg);

    public:
        std::string m_name;
        std::string m_ip;
        bool m_isIpv6;
    private:
        EuvLoop &m_loop;
        EuvDns m_dns;
        EuvTimer m_timer;
        OnGetIp m_cb;
    };

    class NtpClient {
    public:
        NtpClient() {}

        virtual ~NtpClient() {}

        void Create(bool newThread);

        void Destroy();

    private:
        void OnGetNtpServerIp(NtpDns *dns);

        void OnTimer();

    private:

        void RunNtpApp();

    private:
        EuvLoop m_loop;
        EuvTimer m_timer;
        std::vector<NtpDns *> m_dns;
    };
}

#endif //EMX_NtpClient_HPP
