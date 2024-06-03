/*
* @Author: xiong
* @Date: 2022/11/8
*/

#include "OvdNet.hpp"
#include "VoicePlay.hpp"

using namespace Emx;

void OvdNet::Create(OvdCtx *ctx, OnOvdNetInitDone onOvdNetInitDone, OnNetConnectionChanged onNetConnectionChanged) {
    m_ctx = ctx;
    m_onOvdNetInitDone = std::move(onOvdNetInitDone);
    m_onNetConnectionChanged = std::move(onNetConnectionChanged);
    m_netEvent.Create(m_ctx->loop, m_ctx->buffer, m_ctx->bufferSize,
                      std::bind(&OvdNet::OnNetEventMsg, this, ph_1),
                      std::bind(&OvdNet::InitNetStat, this));
    m_timer.Create(m_ctx->loop);
}

void OvdNet::Destroy() {
    m_timer.Destroy();
    m_netEvent.Destroy();
}

void OvdNet::WifiConfig() {
    m_ctx->led->StatChange(Led::StatE::WifiConfiguring, true);
    VoicePlay::Play(VoicePlay::VoiceE::WAIT_CONNECT);
    NetClientWifi wifi;
    wifi.SwitchToMode(Wifi::ModeE::None);
    m_qrScan.StartQR(m_ctx->loop, m_ctx->buffer, m_ctx->bufferSize,
                     std::bind(&OvdNet::OnGetQrContent, this, ph_1, ph_2));
}

void OvdNet::InitNetStat() {
    m_mainDev = Net::DevE::None;//初始化为空设备

    m_supportWifi = NetMainDev::IsSupported(Net::DevE::Wifi);
    if (m_supportWifi) {
        NetClient net(Net::DevE::Wifi);
        net.GetLinkStat(m_linkWifi);
        emxlogi("[Wifi phy:%d,lan4:%d,lan6:%d,inet4:%d,inet6:%d]\n",
               m_linkWifi.phy, m_linkWifi.lan4, m_linkWifi.lan6,
               m_linkWifi.internet4, m_linkWifi.internet6);
    }

    m_supportEth = NetMainDev::IsSupported(Net::DevE::Eth);
    if (m_supportEth) {
        NetClient net(Net::DevE::Eth);
        net.GetLinkStat(m_linkEth);
        emxlogi("[Eth phy:%d,lan4:%d,lan6:%d,inet4:%d,inet6:%d]\n",
               m_linkEth.phy, m_linkEth.lan4, m_linkEth.lan6,
               m_linkEth.internet4, m_linkEth.internet6);
    }

    if (m_supportWifi && !m_supportEth) {
        //仅支持wifi的情况下,wifi作为主网络设备
        m_mainDev = Net::DevE::Wifi;
    } else if (m_supportEth && !m_supportWifi) {
        //仅支持eth的情况下,eth作为主网络设备
        m_mainDev = Net::DevE::Eth;
    } else if (m_supportWifi && m_supportEth) {
        //都支持的情况下
        if (!m_linkEth.phy) {
            //如果网线没有插上，则wifi作为主网络设备
            m_mainDev = Net::DevE::Wifi;
        } else if (NetConnected(m_linkEth)) {
            //如果有线网已经连接上路由器，则eth作为网络主设备
            m_mainDev = Net::DevE::Eth;
        } else {
            //主网络设备目前无法决定,需要使用m_ethCheckTimer等待10s，看看期间eth能否正常连接，
            //若不能正常连接则配置主网络设备为wifi
            //若eth期间连接上了路由器并获取到ip，则eth配置为主网络设备，
            //当eth获取到ip后需要停止这个计时器
        }
    } else {
        emxlogc("supportWifi=%d,supportEth=%d\n", m_supportWifi, m_supportEth);
        return;
    }

    if (m_mainDev != Net::DevE::None) {
        InitNetDone();
    } else {
        m_timer.Start(10000, 0, [this]() {
            //超时，期间并没有获取到eth的ip
            m_mainDev = Net::DevE::Wifi;
            InitNetDone();
        });
    }
}

void OvdNet::OnNetEventMsg(Net::LinkMsg &msg) {
    emxlogi("dev %s stat changed [phy:%d,lan4:%d,lan6:%d,inet4:%d,inet6:%d]\n",
           Net::GetDevName(msg.dev),
           msg.link.phy, msg.link.lan4, msg.link.lan6,
           msg.link.internet4, msg.link.internet6);

    if (msg.dev == Net::DevE::Eth)
        memcpy(&m_linkEth, &msg.link, sizeof(m_linkEth));
    else if (msg.dev == Net::DevE::Wifi)
        memcpy(&m_linkWifi, &msg.link, sizeof(m_linkWifi));

    if (m_mainDev == Net::DevE::None) {
        //主网络设备尚未确定
        if (msg.dev == Net::DevE::Eth && NetConnected(m_linkEth)) {
            //eth成功连接
            m_timer.Stop();
            m_mainDev = Net::DevE::Eth;
            InitNetDone();
        }
    } else {
        //主网络设备已确定
        m_ctx->led->StatChange(Led::StatE::NetConnected, NetConnected(*m_linkMain));
        if (msg.dev == Net::DevE::Wifi && m_wifiConfiguring) {
            //wifi处于二维码配网中
            if (strcmp(msg.link.msg, "WRONG_KEY") == 0) {
                VoicePlay::Play(VoicePlay::VoiceE::WIFI_PSK_ERR);
                m_wifiConfiguring = false;
                m_timer.Stop();
            }
        }
        if (msg.dev == m_mainDev) {
            m_onNetConnectionChanged(NetConnected(*m_linkMain));
        }
    }
}

//主网络设备已经确定好
void OvdNet::InitNetDone() {
    //更新m_linkMain的指向
    if (m_mainDev == Net::DevE::Wifi)
        m_linkMain = &m_linkWifi;
    else if (m_mainDev == Net::DevE::Eth)
        m_linkMain = &m_linkEth;
    else {
        emxlogc("unexpected dev %d\n", m_mainDev);
        return;
    }

    //更新主网络设备配置
    Net::DevE main;
    NetMainDev::Get(main);
    if (m_mainDev != main) {
        NetMainDev::Set(m_mainDev);
        emxlogi("net main dev from %s to %s\n", Net::GetDevName(main), Net::GetDevName(m_mainDev));
    } else {
        emxlogi("net main dev %s\n", Net::GetDevName(m_mainDev));
    }

    m_ctx->led->StatChange(Led::StatE::NetConnected, NetConnected(*m_linkMain));
    m_onOvdNetInitDone();
    m_onNetConnectionChanged(NetConnected(*m_linkMain));
}

void OvdNet::OnGetQrContent(int chn, const char *content) {
    OVDXXSSIDWiFiInfo info = {};
    int ret = OVD_QRString(&info, (OVD_uchar *) content, strlen(content));
    if (ret == 7) {
        m_qrScan.StopQR();
        VoicePlay::Play(VoicePlay::VoiceE::DIDI);
        m_ctx->led->StatChange(Led::StatE::WifiConfiguring, false);
        emxlogi("ret:%d, ssid: %s ssid_len:%d pwd:%s pwd_len:%d phone:%s\n",
               ret, info.ssid, info.ssidLen, info.pwd, info.pwdLen, info.phone);
        NetClientWifi wifi;
        wifi.SetSTA(info.ssid, info.pwd);
        wifi.SwitchToMode(Wifi::ModeE::STA);
        m_wifiConfiguring = true;
//        m_timer.Start(30000, 0, [this]() {
//            m_wifiConfiguring = false;
//
//            //callback wifi connected timeout
//        });
        //上报绑定信息(王建推荐回调时上报此ID)
        if (OVD_DeviceBindInfo(info.phone) < 0) {
            emxloge("ovd upload dev bind id failed\n");
        }
//#ifdef ANDLINK_SUPPORT
//        andlink_adapt_setScanCode(appCtx->bindId,NULL,NULL);
//#endif
    } else {
        emxloge("OVD_QRString failed, ret:%d\n", ret);
    }
}
