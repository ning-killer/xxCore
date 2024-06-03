/*
* @Author: xiong
* @Date: 2022/11/8
*/

#ifndef EAPIL_OvdNet_HPP
#define EAPIL_OvdNet_HPP

#include "OvdCtx.hpp"
#include "NetClient.hpp"
#include "EmxMedia.hpp"

namespace Emx {

    class OvdNet {
    public:
        OvdNet() : m_linkMain(&m_linkEth), m_qrScan(0), m_wifiConfiguring(false) {}

        using OnOvdNetInitDone = std::function<void()>;

        using OnNetConnectionChanged = std::function<void(bool connected)>;

        void Create(OvdCtx *ctx, OnOvdNetInitDone onOvdNetInitDone, OnNetConnectionChanged onNetConnectionChanged);

        void Destroy();

        void WifiConfig();

        Net::DevE GetMainDev() { return m_mainDev; }

    private:
        void InitNetStat();

        void OnNetEventMsg(Net::LinkMsg &msg);

        void InitNetDone();

        void OnGetQrContent(int chn, const char *content);

        bool NetConnected(Net::LinkStat &link) {
            return link.phy && (link.lan4 || link.lan6);
        }

    private:
        OvdCtx *m_ctx;
        OnOvdNetInitDone m_onOvdNetInitDone;
        OnNetConnectionChanged m_onNetConnectionChanged;
        EuvTimer m_timer;
        Net::DevE m_mainDev;
        bool m_supportEth;
        bool m_supportWifi;
        Net::LinkStat m_linkWifi;
        Net::LinkStat m_linkEth;
        Net::LinkStat *m_linkMain;
        NetClientEvent m_netEvent;
        MediaClientQRScan m_qrScan;
        bool m_wifiConfiguring;
    };

}

#endif //EAPIL_OvdNet_HPP
