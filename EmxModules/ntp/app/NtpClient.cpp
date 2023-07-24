//
// Created by xiong on 2021/6/29.
//
#include "NtpClient.hpp"

using namespace Emx;

//when one server's ip parsed fail, the ntpd will be blocked on it , so i have to parse ip by myself
void NtpClient::Create(bool newThread) {
    Json::Value json;
    auto path = new std::string(Misc::GetAppConfigsDir());
    path->append("/ntp.json");
    if (!File::Exist(path->c_str())) {
        printf("%s not exist\n", path->c_str());
        delete path;
        return;
    }
    if (EasyJson::Load(path->c_str(), json) != ErrCodeE::Success) {
        emxloge("load %s failed\n", path->c_str());
        delete path;
        return;
    }
    delete path;
//    int maxInterval = json["maxIntervalS"].asInt();
    m_loop.Init("NtpClient", [this]() {
        m_timer.Destroy();
        for (auto &dns : m_dns) {
            dns->Destroy();
            delete dns;
        }
    }, newThread ? EuvLoop::ModeE::Dynamic : EuvLoop::Default);
    for (auto &item : json["server"]) {
        auto dns = new NtpDns(m_loop, item["name"].asString(),
                              item["isIpv6"].asBool(),
                              std::bind(&NtpClient::OnGetNtpServerIp, this, ph_1));
        dns->Create();
        dns->Run();
        emxlogi("add ntp server %s\n", dns->m_name.data());
        m_dns.push_back(dns);
    }
    m_timer.Create(m_loop);
    json.clear();
    emxlogd("NtpClient newThread(%d)\n",newThread);
    m_loop.Start(newThread);
}

void NtpClient::Destroy() {
    m_loop.StopAndDeInit();
}


void NtpClient::OnGetNtpServerIp(NtpDns *dns) {
    // flush this timer when got new ntp server ip util the timer timeout,
    // then startup the ntpd with collected server ip
    m_timer.Start(1000, 0, [this]() {
        //ntpd -p ntp.aliyun.com -p pool.ntp.org -p time.cloudflare.com -N
        Cmd::Run("killall -q ntpd");
        m_timer.Start(1000, 0, [this]() {
            std::string cmd;
            cmd.reserve(1024);
            cmd.append("ntpd -N");
            for (auto dns:m_dns) {
                if (!dns->m_ip.empty()) {
                    cmd.append(" -p ");
                    cmd.append(dns->m_ip);
                }
            }
            Cmd::Run(cmd.data());
        });
    });
}


void NtpDns::Run() {
    m_dns.Run(m_name.data(),
              std::bind(&NtpDns::OnResolve, this, ph_1, ph_2, ph_3),
              nullptr, m_isIpv6 ? EuvDns::TypeE::Ipv6 : EuvDns::TypeE::Ipv4);
}


void NtpDns::OnResolve(ErrCodeE e, struct addrinfo *addr, void *arg) {
    m_timer.Stop();
    if (e != ErrCodeE::Success) {
        m_timer.Start(6000, 0, std::bind(&NtpDns::Run, this));
        return;
    }
    for (auto p = addr; p; p = p->ai_next) {
        char ip[64] = {};
        if (!m_isIpv6 && p->ai_family == AF_INET) {
            uv_ip4_name(((sockaddr_in *) p->ai_addr), ip, sizeof(ip));
        } else if (m_isIpv6 && p->ai_family == AF_INET6) {
            uv_ip6_name(((sockaddr_in6 *) p->ai_addr), ip, sizeof(ip));
        } else {
            continue;
        }
        m_ip.assign(ip);
        m_cb(this);
        m_timer.Destroy();
        emxlogi("%s->%s\n", m_name.data(), m_ip.data());
        break;
    }
}




