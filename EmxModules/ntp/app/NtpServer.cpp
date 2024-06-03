//
// Created by xiong on 2021/6/29.
//
#include "NtpServer.hpp"

using namespace Emx;

// when one server's ip parsed fail, the ntpd will be blocked on it , so i have
// to parse ip by myself
void NtpServer::Create(OnCreated cb, bool newThread) {
    if (m_created) {
        return;
    }
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
    m_intervalS = json["intervalS"].asInt();
    m_21cnSpecial = json["21cnSpecial"].asBool();
    for (auto &s: json["server"]) {
        auto domain = new Domain;
        domain->name = s["name"].asString();
        domain->isIpv6 = s["isIpv6"].asBool();
        memset(domain->ip, 0, sizeof(domain->ip));
        m_domainArray.push_back(domain);
    }
    json.clear();

    // 21cn需要启动时候设置好随机 NTP 顺序，但需确保 ntp-vcp.21cn.com 放在列表最后
    if (m_21cnSpecial) {
        srand((unsigned int) Time::GetMonoClockNs());
        for (int i = m_domainArray.size() - 2; i >= 1; i--) {
            auto tmp = m_domainArray[i];
            int r = rand() % i;
            m_domainArray[i] = m_domainArray[r];
            m_domainArray[r] = tmp;
        }
        for (auto &domain: m_domainArray) emxlogt("domain=%s\n", domain->name.c_str());
    }

    m_loop.Init(
            "NtpServer",
            [this]() {
                m_created = false;
                for (auto &domain: m_domainArray) { domain->dns.Destroy(); }
                m_timer.Destroy();
            },
            newThread ? EuvLoop::ModeE::Dynamic : EuvLoop::Default);
    for (auto &domain: m_domainArray) {
        domain->dns.Create(&m_loop);
        domain->dns.Run(domain->name.c_str(), std::bind(&NtpServer::OnResolve, this, ph_1, ph_2, ph_3), domain);
        emxlogd("domain name:%s, 21cnSpecial:%d\n", domain->name.c_str(), m_21cnSpecial);
    }
    m_timer.Create(m_loop);
    emxlogd("NtpClient newThread(%d)\n", newThread);
    m_created = true;
    m_loop.Start(newThread);
}

void NtpServer::Destroy() {
    if (m_created) {
        m_loop.StopAndDeInit();
        m_created = false;
    }
}

void NtpServer::OnResolve(ErrCodeE e, struct addrinfo *addr, void *arg) {
    auto domain = (Domain *) arg;
    if (e == ErrCodeE::Success) {
        for (auto p = addr; p; p = p->ai_next) {
            if (p->ai_family == AF_INET6 && domain->isIpv6) {
                uv_ip6_name(((sockaddr_in6 *) p->ai_addr), domain->ip, sizeof(domain->ip));
                m_domainGotCnt++;
                break;
            } else if (p->ai_family == AF_INET) {
                uv_ip4_name(((sockaddr_in *) p->ai_addr), domain->ip, sizeof(domain->ip));
                m_domainGotCnt++;
                break;
            } else {
                continue;
            }
        }
    }

    if (m_domainGotCnt == (int) m_domainArray.size()) {
        // 全部获取到则立刻执行
        RunNtpApp();
    } else {
        // 延时一段时间等一等其他domain，新的domain到来会刷新这个timer
        m_timer.Start(6000, 0, [this]() { RunNtpApp(); });
    }
}

void NtpServer::RunNtpApp() {
    m_timer.Stop();
    Cmd::VforkRun("killall -q ntpd");
    std::string cmd;
    cmd.reserve(1024);
    cmd.append("ntpd -N");
    if (m_21cnSpecial) {
        //中兴要求同步后退出，不能实时跟踪调整
        cmd.append(" -q ");
    }
    for (int i = 0; i < (int) m_domainArray.size(); i++) {
        if (m_domainArray[i]->ip[0]) {
            cmd.append(" -p ");
            cmd.append(m_domainArray[i]->ip);
        }
    }
    Cmd::VforkRun(cmd.data());
    emxlogd("run ntp app, next ntp time:%d second~\n", m_intervalS);
    // m_intervalS后再次执行
    m_timer.Start(m_intervalS * 1000, 0, [this]() { RunNtpApp(); });
}
