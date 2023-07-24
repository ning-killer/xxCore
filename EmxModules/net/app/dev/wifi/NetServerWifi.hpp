//
// Created by xiong on 2021/3/9.
//

#ifndef EMX_NetServerWifi_HPP
#define EMX_NetServerWifi_HPP

#include "NetServerDev.hpp"
#include "NetServerLinkPhyWifi.hpp"

struct wpa_ctrl;
namespace Emx {

    class NetServerWifi : public Wifi, public NetServerDev {
    public:
        NetServerWifi(NetServerResource &res, Json::Value &devJsonParam) :
                NetServerDev(res, devJsonParam, &m_phy),
                m_phy(m_interface, res),
                m_data(devJsonParam["data"]),
                m_cmdWpa(
                        nullptr),
                m_monitorWpa(nullptr) {
            m_apList = std::make_shared<ApInfoList>();
        }

        virtual ~NetServerWifi() {};

    private:
        ErrCodeE CreateDev() override;

        void DestroyDev() override;

        ErrCodeE ProcDev(uint32_t method, const char *data, int32_t size, std::string &out) override;

    private:
        ErrCodeE SwitchToMode(ModeE mode) override;

        ErrCodeE SetSTA(const char *ssid, const char *psk) override;

        ErrCodeE SetAP(const char *ssid, const char *psk) override;

        ErrCodeE GetAPList(ApInfoListPtr &list) override;

        ErrCodeE GetInfo(Info &info) override;

    private:
        ErrCodeE CreateWpa();

        void DestroyWpa();

        ErrCodeE SetModeParam(ModeE mode, const char *ssid, const char *psk);

        ErrCodeE WpaSendCmd(const char *cmd);

        ErrCodeE InitNetwork();

        ErrCodeE SetNetwork(int id);

        ErrCodeE SelectNetwork();

        void Monitor(char *buf, int size);

    private:
        NetServerLinkPhyWifi m_phy;
        Json::Value &m_data;
        wpa_ctrl *m_cmdWpa;
        wpa_ctrl *m_monitorWpa;
        EuvPoll m_monitor;
        ApInfoListPtr m_apList;
    };
}

#endif //EMX_NetServerWifi_HPP
