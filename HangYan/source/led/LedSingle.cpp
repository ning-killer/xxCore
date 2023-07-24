//
// Created by xiong on 2022/7/5.
//

#include "LedSingle.hpp"

using namespace Emx;

void LedSingle::OnLedStatusChange() {
    //turn off all of led, and stop timer
    m_led->Set(false, 0);
    m_blinkTimer.Stop();
    //升级过程的指示灯由UpdateServer控制
    if (m_stat[StatE::Updating]){
        return;
    }
    if (m_stat[StatE::StartingUp]) {
        m_led->Set(true, 0);
    } else if (m_stat[StatE::Rebooting]) {
        //reset and reboot
        m_blinkInvert = false;
        m_blinkTimer.Start(0, 200, [this]() {
            m_led->Set(!m_blinkInvert, 0);
            m_blinkInvert = !m_blinkInvert;
        });
    } else if (m_stat[StatE::WifiConfiguring]) {
        //waiting net mode
        m_blinkInvert = false;
        m_blinkTimer.Start(0, 1000, [this]() {
            m_led->Set(!m_blinkInvert, 0);
            m_blinkInvert = !m_blinkInvert;
        });
    } else if (m_stat[StatE::Sleeping]) {
        m_led->Set(false, 0);
    } else if (!m_stat[StatE::ManualLedEna]) {
        m_led->Set(false, 0);
    } else if (!m_stat[StatE::OvdConnected] || !m_stat[StatE::NetConnected]) {
        //net or ovd con mode
        m_blinkInvert = false;
        m_blinkTimer.Start(0, 1000, [this]() {
            m_led->Set(!m_blinkInvert, 0);
            m_blinkInvert = !m_blinkInvert;
        });
    } else {
        //normal running
        m_led->Set(true, 0);
    }
}
