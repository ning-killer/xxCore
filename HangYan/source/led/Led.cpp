/*
* @Author: xiong
* @Date: 2022/9/28
*/

#include "Led.hpp"

using namespace Emx;

void Led::Create() {
    ResetStat();
    m_blinkTimer.Create(m_loop);
    OnLedStatusChange();
}

void Led::Destroy() {
    ResetStat();
    OnLedStatusChange();
    m_blinkTimer.Destroy();
}

void Led::StatChange(Led::StatE stat, bool value) {
    if (m_stat.count(stat) == 0) {
        emxloge("unsupported stat %d\n", stat);
        return;
    }
    auto org = m_stat[stat];
    m_stat[stat] = value;
    char tmp[32] = {};
    int off = 0;
    for (auto &e:m_stat) {
        off += snprintf(tmp + off, sizeof(tmp) - off, "%d,", (int) e.second);
    }
    tmp[off - 1] = 0;
    emxlogi("[%s]%s:%d->%d\n", tmp, GetNameByStat(stat), org, value);
    OnLedStatusChange();
}

void Led::ResetStat() {
    m_stat[StatE::StartingUp] = true;
    m_stat[StatE::Sleeping] = false;
    m_stat[StatE::ManualLedEna] = true;
    m_stat[StatE::WifiConfiguring] = false;
    m_stat[StatE::OvdConnected] = false;
    m_stat[StatE::NetConnected] = false;
    m_stat[StatE::Updating] = false;
    m_stat[StatE::Rebooting] = false;
}

const char *Led::GetNameByStat(Led::StatE stat) {
    switch (stat) {
        case StatE::StartingUp:
            return "StartingUp";
        case StatE::ManualLedEna:
            return "ManualLedEna";
        case StatE::Sleeping:
            return "Sleeping";
        case StatE::WifiConfiguring:
            return "WifiConfiguring";
        case StatE::OvdConnected:
            return "OvdConnected";
        case StatE::NetConnected:
            return "NetConnected";
        case StatE::Updating:
            return "Updating";
        case StatE::Rebooting:
            return "Rebooting";
        default:
            break;
    }
    return "nullptr";
}
