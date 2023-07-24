//
// Created by xiong on 2022/7/5.
//

#include "LedDouble.hpp"

using namespace Emx;

void LedDouble::OnLedStatusChange() {
    //turn off all of led, and stop timer
    SetOff();
    m_blinkTimer.Stop();
    //升级过程的指示灯由UpdateServer控制
    if (m_stat[StatE::Updating])
        return;
    if (m_stat[StatE::StartingUp]) {
        SetNo1();
    } else if (m_stat[StatE::Rebooting]) {
        //reset and reboot
        m_blinkInvert = false;
        m_blinkTimer.Start(0, 200, [this]() {
            if (m_blinkInvert) {
                SetOff();
            } else {
                SetNo1();
            }
            m_blinkInvert = !m_blinkInvert;
        });
    } else if (m_stat[StatE::Sleeping]) {
        SetOff();
    } else if (m_stat[StatE::WifiConfiguring]) {
        //waiting net mode
        m_blinkInvert = false;
        m_blinkTimer.Start(0, 1000, [this]() {
            if (m_blinkInvert) {
                SetOff();
            } else {
                SetNo1();
            }
            m_blinkInvert = !m_blinkInvert;
        });
    } else if (!m_stat[StatE::OvdConnected] || !m_stat[StatE::NetConnected]) {
        //net or ovd con mode
        m_blinkInvert = false;
        m_blinkTimer.Start(0, 1000, [this]() {
            if (m_blinkInvert) {
                SetOff();
            } else {
                SetNo2();
            }
            m_blinkInvert = !m_blinkInvert;
        });
    } else {
        //normal running
        SetNo2();
    }
}

void LedDouble::SetOff() {
    m_led->Set(false, 0);
    m_led->Set(false, 1);
}

void LedDouble::SetNo1() {
    m_led->Set(true, 0);
}

void LedDouble::SetNo2() {
    m_led->Set(true, 1);
}
