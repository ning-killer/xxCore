//
// Created by xiong on 2021/3/12.
//

#ifndef EMX_NetServerAddr_HPP
#define EMX_NetServerAddr_HPP

#include "Net.hpp"
#include "NetServerResource.hpp"
#include "NetServerLink.hpp"

namespace Emx {
    class NetServerDev : public Net {
    public:

        NetServerDev(NetServerResource &res, Json::Value &devJsonParam, NetServerLinkPhy *phy);

        virtual ~NetServerDev();

        ErrCodeE Create();

        void Destroy();

        ErrCodeE Proc(uint32_t method, const char *data, int32_t size, std::string &out);

        void DoDns();

        //override
    protected:
        const char *GetInterface() override { return m_interface; }

        ErrCodeE SetAddr(Addr &addr) override;

        ErrCodeE SetTempAddr(Addr &addr) override;

        ErrCodeE GetAddr(Addr &addr) override;

        ErrCodeE SetEna(bool ena) override;

        ErrCodeE GetEna(bool &ena) override;

        ErrCodeE GetLinkStat(LinkStat &link) override {
            link = m_linkMsg.link;
            return ErrCodeE::Success;
        }

        ErrCodeE ReLink() override {
            ConfigEna(false);
            ConfigEna(true);
            return ErrCodeE::Success;
        }
        //base implement
    protected:
        void DhcpRelease();

        void DhcpReNew();

        ErrCodeE ConfigAddr();

        ErrCodeE ConfigMac();

        ErrCodeE ConfigEna(bool ena);

        void OnLinkChanged(LinkStat &link);

        std::string GetMac();

        void Ipv6Delete();
    protected:

        virtual ErrCodeE CreateDev() { return ErrCodeE::Success; }

        virtual void DestroyDev() {}

        virtual ErrCodeE ProcDev(uint32_t method, const char *data,
                                 int32_t size, std::string &out);

    protected:
        NetServerResource &m_res;
        Json::Value &m_devJsonParam;
        char m_interface[Net::InterfaceSize];
        NetServerLink *m_link;
        LinkMsg m_linkMsg;
    };
}
#endif //EMX_NetServerAddr_HPP
