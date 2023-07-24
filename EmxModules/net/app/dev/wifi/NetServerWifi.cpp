//
// Created by xiong on 2021/3/9.
#include <unistd.h>
#include "NetServerWifi.hpp"
#include "NetInner.hpp"
#include "wpa_ctrl.h"

using namespace Emx;

#define EMX_WPA_CTRL_DIR "/var/run/wpa_supplicant"

ErrCodeE NetServerWifi::CreateDev() {
    ErrCodeE e;
    if (Misc::ApplicationExist("wpa_supplicant")) {
        Cmd::Run("killall -q wpa_supplicant");
        sleep(1);
    }

    e = Cmd::RunCheck("wpa_supplicant -B -i %s -c %s/wifi.conf", GetInterface(), m_res.netConfigDir);
    if (e != ErrCodeE::Success) {
        emxloge("run wpa_supplicant failed\n");
        return e;
    }

    e = CreateWpa();
    if (e != ErrCodeE::Success) {
        emxloge("create wpa ctrl failed\n");
        return e;
    }

    m_monitor.Start([this]() {
        size_t len = m_res.bufferSize;
        int n = wpa_ctrl_recv(m_monitorWpa, m_res.buffer, &len);
        if (n == 0) {
            m_res.buffer[len] = 0;
            Monitor(m_res.buffer, len);
        } else {
            emxlogw("monitor:n=%d\n", n);
        }
    }, nullptr, []() {
        emxlogd("cmd OnDisconnect\n");
    });

    //add 2 networks for ap and sta
    e = InitNetwork();
    if (e != ErrCodeE::Success) {
        emxloge("init network failed\n");
        m_monitor.Stop();
        DestroyWpa();
        return e;
    }
    emxlogi("wifi init done\n");
    return e;
}

void NetServerWifi::DestroyDev() {
    m_monitor.Stop();
    wpa_ctrl_detach(m_monitorWpa);
    wpa_ctrl_close(m_monitorWpa);
    wpa_ctrl_close(m_cmdWpa);
}

ErrCodeE NetServerWifi::CreateWpa() {
    char wpaCtrlPath[EMX_MAX_PATH_SIZE] = {};
    snprintf(wpaCtrlPath, sizeof(wpaCtrlPath), "%s/%s", EMX_WPA_CTRL_DIR, GetInterface());
    m_cmdWpa = wpa_ctrl_open(wpaCtrlPath);
    if (!m_cmdWpa) {
        emxloge("cannot open %s,for cmd\n", wpaCtrlPath);
        return ErrCodeE::OpenFailed;
    }

    m_monitorWpa = wpa_ctrl_open(wpaCtrlPath);
    if (!m_monitorWpa) {
        emxloge("cannot open %s,for monitor\n", wpaCtrlPath);
        wpa_ctrl_close(m_cmdWpa);
        return ErrCodeE::OpenFailed;
    }

    if (wpa_ctrl_attach(m_monitorWpa) != 0) {
        emxloge("monitor attach to wpa failed\n", wpaCtrlPath);
        wpa_ctrl_close(m_monitorWpa);
        wpa_ctrl_close(m_cmdWpa);
        return ErrCodeE::Failure;
    }

    if (m_monitor.InitSock(m_res.loop, wpa_ctrl_get_fd(m_monitorWpa)) != ErrCodeE::Success) {
        emxloge("init monitor sock failed\n");
        wpa_ctrl_detach(m_monitorWpa);
        wpa_ctrl_close(m_monitorWpa);
        wpa_ctrl_close(m_cmdWpa);
        return ErrCodeE::Failure;
    }
    return ErrCodeE::Success;
}

void NetServerWifi::DestroyWpa() {
    wpa_ctrl_detach(m_monitorWpa);
    wpa_ctrl_close(m_monitorWpa);
    wpa_ctrl_close(m_cmdWpa);
}


ErrCodeE NetServerWifi::ProcDev(uint32_t method, const char *data, int32_t size, std::string &out) {
    switch ((NetClientWifiMethodE) method) {
        case NetClientWifiMethodE::SwitchToMode: {
            Wifi::ModeE mode;
            memcpy(&mode, data, sizeof(mode));
            return SwitchToMode(mode);
        }
        case NetClientWifiMethodE::SetSTA: {
            char ssid[Wifi::MaxSsidSize];
            char psk[MaxPskSize];
            int off = 0;
            memcpy(ssid, data + off, MaxSsidSize);
            off += MaxSsidSize;
            memcpy(psk, data + off, MaxPskSize);
            off += MaxPskSize;
            return SetSTA(ssid, psk);
        }
        case NetClientWifiMethodE::SetAP: {
            char ssid[MaxSsidSize];
            char psk[MaxPskSize];
            int off = 0;
            memcpy(ssid, data + off, MaxSsidSize);
            off += MaxSsidSize;
            memcpy(psk, data + off, MaxPskSize);
            off += MaxPskSize;
            return SetAP(ssid, psk);
        }
        case NetClientWifiMethodE::GetAPList: {
            auto list = std::make_shared<Wifi::ApInfoList>();
            if (GetAPList(list) != ErrCodeE::Success) {
                return ErrCodeE::Failure;
            }
            out.clear();
            for (auto &e : *list) {
                out.append((char *) &e, sizeof(e));
            }
            return ErrCodeE::Success;
        }
        case NetClientWifiMethodE::GetInfo: {
            Wifi::Info info = {};
            if (GetInfo(info) != ErrCodeE::Success) {
                return ErrCodeE::Failure;
            }
            out.assign((char *) &info, sizeof(info));
            return ErrCodeE::Success;
        }
        default:
            emxloge("%s not support method[%u]\n", GetInterface(), method);
            return ErrCodeE::OperationNotSupport;
    }
}

ErrCodeE NetServerWifi::SwitchToMode(Wifi::ModeE mode) {
    if ((Wifi::ModeE) m_data["mode"].asInt() != mode) {
        m_data["mode"] = (int) mode;
        if (m_res.saveParam() != ErrCodeE::Success)
            return ErrCodeE::Failure;
    }
    SelectNetwork();
    return ErrCodeE::Success;
}

ErrCodeE NetServerWifi::SetSTA(const char *ssid, const char *psk) {
    return SetModeParam(Wifi::ModeE::STA, ssid, psk);
}

ErrCodeE NetServerWifi::SetAP(const char *ssid, const char *psk) {
    return SetModeParam(Wifi::ModeE::AP, ssid, psk);
}

ErrCodeE NetServerWifi::SetModeParam(Wifi::ModeE mode, const char *ssid, const char *psk) {
    bool isChanged = false;
    auto &oneMode = m_data[mode == Wifi::ModeE::AP ? "ap" : "sta"];
    if (strcmp(ssid, oneMode["ssid"].asCString()) != 0) {
        oneMode["ssid"] = ssid;
        isChanged = true;
    }
    if (strcmp(psk, oneMode["psk"].asCString()) != 0) {
        oneMode["psk"] = psk;
        isChanged = true;
    }
    if (SetNetwork(mode == Wifi::ModeE::AP ? 0 : 1) != ErrCodeE::Success) {
        emxlogc("set network failed\n");
        return ErrCodeE::Failure;
    }
    if (isChanged)
        return m_res.saveParam();
    else
        return ErrCodeE::Success;
}

ErrCodeE NetServerWifi::GetAPList(ApInfoListPtr &list) {
    if (WpaSendCmd("SCAN") != ErrCodeE::Success) {
        return ErrCodeE::Failure;
    }
    if (WpaSendCmd("SCAN_RESULTS") != ErrCodeE::Success) {
        return ErrCodeE::Failure;
    }
//    bssid / frequency / signal level / flags / ssid
//    0c:4b:54:f0:fa:bb       2437    -32     [WPA-PSK-CCMP][WPA2-PSK-CCMP][ESS]      TP-LINK_HOST93
//    a4:56:02:8f:f4:8f       2472    -44     [WPA-PSK-CCMP][WPA2-PSK-CCMP][ESS]      123 \xe6\xb5\x8b\xe8\xaf\x95@
//    80:ea:07:cb:d2:c6       2442    -47     [ESS]    TP-LINK_D2C6
//    34:ce:00:69:47:73       2472    -49     [WPA-PSK-CCMP+TKIP][WPA2-PSK-CCMP+TKIP][ESS]
//    08:1f:71:36:03:77       2472    -58     [WPA-PSK-CCMP][WPA2-PSK-CCMP][ESS][P2P]      C53_test
//    f8:9a:78:52:71:a8       2437    -60     [WPA-PSK-CCMP+TKIP][WPA2-PSK-CCMP+TKIP][ESS]
//    b8:f8:83:b9:ad:a9       2447    -60     [WPA-PSK-CCMP+TKIP][WPA2-PSK-CCMP+TKIP][ESS]    ZKCX
//    08:1f:71:38:25:3d       2467    -62     [ESS]      DD
//    60:3a:7c:84:d1:81       2442    -31     [WPA-PSK-CCMP+TKIP][WPA2-PSK-CCMP+TKIP][ESS]    EMX_RD_2
//    BSSID/tFREQ/tLEVEL/tFLAGS/tSSID/n

    char *pSave;
    char *pEach = strtok_r(m_res.buffer, "\n", &pSave);
    //skip title 'bssid / frequency / signal level / flags / ssid'
    while ((pEach = strtok_r(nullptr, "\n", &pSave))) {
        if (!strstr(pEach, ":")) {
            break;
        }
        ApInfo info = {};
        //bssid
        memcpy(info.bssid, pEach, 17);
        //skip frequency
        pEach = strstr(pEach, "\t");
        if (!pEach)continue;
        pEach++;
        //signal level
        pEach = strstr(pEach, "\t");
        if (!pEach)continue;
        pEach++;
        long rssi = strtol(pEach, nullptr, 10);
        if (rssi < 0) {
            info.signalLevel = 100 + rssi;
        }
        //flags
        pEach = strstr(pEach, "\t");
        if (!pEach)continue;
        pEach++;
        //todo wep
        bool findWpa = (bool) strstr(pEach, "WPA-PSK");
        bool findWpa2 = (bool) strstr(pEach, "WPA2-PSK");
        if (findWpa && findWpa2)
            info.mgmt = Wifi::KeyMgmtE::WPA_WPA2_PSK;
        else if (findWpa)
            info.mgmt = Wifi::KeyMgmtE::WPA_PSK;
        else if (findWpa2)
            info.mgmt = Wifi::KeyMgmtE::WPA2_PSK;
        else
            info.mgmt = Wifi::KeyMgmtE::NONE;
        //ssid
        pEach = strstr(pEach, "\t");
        if (!pEach)continue;
        pEach++;
        for (int i = 0, j = 0; i < (int) strlen(pEach); i++) {
            if (pEach[j] == '\\' && pEach[j + 1] == 'x') {
                j += 2;
                info.ssid[i] = (char) strtol(pEach + j, nullptr, 16);
                j += 2;
            } else {
                info.ssid[i] = pEach[j++];
            }
        }
        //skip null ssid
        if (info.ssid[0] == 0)
            continue;
        //skip duplicate ssid and bssid
        bool duplicate = false;
        for (auto &e : *list) {
            if (strcmp(e.bssid, info.bssid) == 0) {
                emxlogd("find duolicate bssid %s\n", info.bssid);
                duplicate = true;
                break;
            } else if (strcmp(e.ssid, info.ssid) == 0) {
                emxlogd("find duolicate ssid %s\n", info.ssid);
                duplicate = true;
                break;
            }
        }
        if (duplicate)
            continue;
        list->push_back(info);
    }
    list->empty() ? list = m_apList : m_apList = list;
    return ErrCodeE::Success;
}

ErrCodeE NetServerWifi::GetInfo(Wifi::Info &info) {
    memset(&info, 0, sizeof(info));
//    Wifi::ModeE mode;
//    char ssid[MaxSsidSize];
//    char bssid[EMX_WIFI_BSSID_SIZE];
//    KeyMgmtE mgmt;
//    uint8_t signalLevel;
//    uint8_t linkSpeed;

    //try to get wifi mode
    if (WpaSendCmd("LIST_NETWORKS") != ErrCodeE::Success) {
        return ErrCodeE::Failure;
    }
//    network id / ssid / bssid / flags
//    0               any     [DISABLED]
//    1       TP-LINK_HOST93  any     [CURRENT]
    if (!strstr(m_res.buffer, "[CURRENT]")) {
        info.mode = Wifi::ModeE::None;
        info.mgmt = Wifi::KeyMgmtE::NONE;
        return ErrCodeE::Success;
    }
    int netIdx = 0;
    char *pSave;
    char *pEach = strtok_r(m_res.buffer, "\n", &pSave);
    //skip title 'network id / ssid / bssid / flags'
    if (pEach)
        pEach = strtok_r(nullptr, "\n", &pSave);
    while (pEach) {
        if (strstr(pEach, "[CURRENT]")) {
            break;
        }
        pEach = strtok_r(nullptr, "\n", &pSave);
        netIdx++;
    }
    info.mode = netIdx == 0 ? Wifi::ModeE::AP : Wifi::ModeE::STA;

    //try to get ssid bssid and mgmt
    if (WpaSendCmd("STATUS") != ErrCodeE::Success) {
        return ErrCodeE::Failure;
    }
//    bssid=0c:4b:54:f0:fa:bb
//    freq=2437
//    ssid=TP-LINK_HOST93
//    id=1
//    mode=station
//    wifi_generation=4
//    pairwise_cipher=CCMP
//    group_cipher=CCMP
//    key_mgmt=WPA2-PSK
//    wpa_state=COMPLETED
//    ip_address=192.168.93.103
//    address=40:aa:56:15:56:b9
    pEach = strtok_r(m_res.buffer, "\n", &pSave);
    while (pEach) {
        char *str;
        if (info.bssid[0] == 0 && (str = strstr(pEach, "bssid="))) {
            strncpy(info.bssid, str + strlen("bssid="), MaxBssidSize);
        } else if (info.ssid[0] == 0 && (str = strstr(pEach, "ssid="))) {
            strncpy(info.ssid, str + strlen("ssid="), MaxSsidSize);
        } else if (info.ssid[0] == 0 && (str = strstr(pEach, "key_mgmt="))) {
            str = str + strlen("key_mgmt=");
            //todo wep
            if (strcmp(str, "NONE") == 0) {
                info.mgmt = KeyMgmtE::NONE;
            } else if (strcmp(str, "WPA-PSK") == 0) {
                info.mgmt = KeyMgmtE::WPA_PSK;
            } else if (strcmp(str, "WPA2-PSK+WPA-PSK") == 0) {
                info.mgmt = KeyMgmtE::WPA_WPA2_PSK;
            } else if (strcmp(str, "WPA2-PSK") == 0) {
                info.mgmt = KeyMgmtE::WPA2_PSK;
            } else {
                info.mgmt = KeyMgmtE::OTHER;
            }
        }
        pEach = strtok_r(nullptr, "\n", &pSave);
        netIdx++;
    }
    //try to get signal level
    if (WpaSendCmd("SIGNAL_POLL") != ErrCodeE::Success) {
        return ErrCodeE::Failure;
    }
    pEach = strtok_r(m_res.buffer, "\n", &pSave);
    while (pEach) {
        char *str;
        if ((str = strstr(pEach, "RSSI="))) {
            long rssi = strtol(str + strlen("RSSI="), nullptr, 10);
            if (rssi < 0) {
                info.signalLevel = 100 + rssi;
            }
        } else if ((str = strstr(pEach, "LINKSPEED="))) {
            info.linkSpeed = strtol(str + strlen("LINKSPEED="), nullptr, 10);
        }
        pEach = strtok_r(nullptr, "\n", &pSave);
    }
    return ErrCodeE::Success;
}

void msg_cb(char *msg, size_t len) {
    emxlogd("recv msg:%s,size %d\n", msg, (int) len);
}

ErrCodeE NetServerWifi::WpaSendCmd(const char *cmd) {
    size_t len = m_res.bufferSize;
//    emxlogd("req #%s#\n", cmd);
    int ret = wpa_ctrl_request(m_cmdWpa, cmd, strlen(cmd), m_res.buffer, &len, msg_cb);
    if (0 != ret) {
        emxloge("request cmd #%s# failed\n", cmd);
        exit(-1);
    }
    m_res.buffer[len] = 0;
//    emxlogd("resp #%s#size=%d\n", m_res.buffer, len);
    return ErrCodeE::Success;
}


ErrCodeE NetServerWifi::InitNetwork() {
    //list and remove all networks exists
    if (WpaSendCmd("LIST_NETWORKS") != ErrCodeE::Success) {
        emxloge("list network error\n");
        return ErrCodeE::Failure;
    }

    //remove all networks exists such as below
    //    network id / ssid / bssid / flags
    //    0               any     [DISABLED]
    //    1               any     [DISABLED]
    //    2               any     [DISABLED]
    char tmp[512];
    char *pSave;
    char *pEach = strtok_r(m_res.buffer, "\n", &pSave);
    //skip title 'network id / ssid / bssid / flags'
    if (pEach)
        pEach = strtok_r(nullptr, "\n", &pSave);
    while (pEach) {
        sprintf(tmp, "REMOVE_NETWORK %ld", strtol(pEach, nullptr, 10));
        WpaSendCmd(tmp);
        pEach = strtok_r(nullptr, "\n", &pSave);
    }

    //add 2 network for ap and sta
    for (int i = 0; i < 2; i++) {
        WpaSendCmd("ADD_NETWORK");
    }

    //initialize network 0 as ap mode
    WpaSendCmd("SET_NETWORK 0 mode 2");

    //set ssid and psk of network ap
    SetNetwork(0);

    //set scan_ssid
    WpaSendCmd("SET_NETWORK 1 scan_ssid 1");

    //set ssid and psk of network sta
    SetNetwork(1);

    //select one mode
    SelectNetwork();
    return ErrCodeE::Success;
}

ErrCodeE NetServerWifi::SetNetwork(int id) {
    //todo wifi psk length less than 8
    char tmp[512];
    auto &oneMode = m_data[id == 0 ? "ap" : "sta"];
    snprintf(tmp, sizeof(tmp), "SET_NETWORK %d ssid \"%s\"", id, oneMode["ssid"].asCString());
    WpaSendCmd(tmp);
    if (oneMode["psk"].asString().empty()) {
        snprintf(tmp, sizeof(tmp), "SET_NETWORK %d key_mgmt NONE", id);
        WpaSendCmd(tmp);
    } else {
        snprintf(tmp, sizeof(tmp), "SET_NETWORK %d psk \"%s\"", id, oneMode["psk"].asCString());
        WpaSendCmd(tmp);
    }
    return ErrCodeE::Success;
}

ErrCodeE NetServerWifi::SelectNetwork() {
    Cmd::Run("ps | grep udhcpd_wifi | grep -v grep | awk '{print $1}' | xargs kill ");
    DhcpRelease();
    auto mode = (Wifi::ModeE) m_data["mode"].asInt();
    if (mode == Wifi::ModeE::AP) {
        WpaSendCmd("SELECT_NETWORK 0");
        WpaSendCmd("DISCONNECT");
        WpaSendCmd("RECONNECT");
        auto &ap = m_data["ap"]["ip4"];
        Cmd::Run("ifconfig %s %s netmask %s",
                 GetInterface(),
                 ap.asCString(),
                 "255.255.255.0");
        Cmd::Run("route del default gw 0.0.0.0 dev %s", GetInterface());
        Cmd::Run("route add default gw %s dev %s", ap.asCString(), GetInterface());
        Cmd::Run("udhcpd %s/udhcpd_wifi.conf", m_res.netConfigDir);
        m_phy.SetPhyLinked(true);
    } else if (mode == Wifi::ModeE::STA) {
        WpaSendCmd("SELECT_NETWORK 1");
        WpaSendCmd("DISCONNECT");
        WpaSendCmd("RECONNECT");
        m_phy.SetPhyLinked(false);
    } else {
        WpaSendCmd("DISABLE_NETWORK 0");
        WpaSendCmd("DISABLE_NETWORK 1");
        WpaSendCmd("SCAN");
        m_phy.SetPhyLinked(false);
    }
    return ErrCodeE::Success;
}

void NetServerWifi::Monitor(char *buf, int size) {
    if (!strstr(buf, "CTRL-EVENT-BSS-ADDED") && !strstr(buf, "CTRL-EVENT-BSS-REMOVED")) {
        //        printf("monitor:[%s]\n", buf);
    }
    if ((Wifi::ModeE) m_data["mode"].asInt() != Wifi::ModeE::STA) {
        return;
    }
    if (strstr(buf, "CTRL-EVENT-CONNECTED")) {
        GetAPList(m_apList);
        m_phy.SetPhyLinked(true);
    }
    if (strstr(buf, "reason=WRONG_KEY")) {
        WpaSendCmd("DISCONNECT");
        m_phy.SetPhyLinked(false, "WRONG_KEY");
    }
}

