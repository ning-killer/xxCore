//
// Created by xiong on 2022/7/5.
//

#include "Key.hpp"

using namespace Emx;

void Key::Create(Key::OnKeyEvent e) {
    m_timer.Create(m_loop);
    m_timer.Start(0, DetectInterval, std::bind(&Key::OnTimer, this));
    m_cb = std::move(e);
}

void Key::Destroy() {
    m_timer.Destroy();
}

void Key::OnTimer() {
    if (m_mode == ModeE::Key){
        if (m_key->Get()) {
            //pressed
            if (m_pressTime == -1) {
                //(m_pressCnt == -1) means that we should wait the key to release
                return;
            } else {
                //increase the pressing time
                m_pressTime += DetectInterval;
                if (m_pressTime > (m_longPressTime)) {
                    emxlogi("%s key LongPress\n", m_name.c_str());
                    m_cb(m_name.c_str(), EventE::LongPress);
                    m_pressTime = -1;
                }
            }
        } else {
            //unpressed
            if (m_pressTime == 0) {
                //if the key not be pressed and never be pressed
                return;
            } else if (m_pressTime == -1) {
                //key released
                m_pressTime = 0;
                emxlogi("%s key released\n", m_name.c_str());
                return;
            } else {
                if (m_pressTime >= (DetectInterval * 2)) {
                    //m_pressCnt more than 0 and less than 'long pressed'
                    emxlogi("%s key Click\n", m_name.c_str());
                    m_cb(m_name.c_str(), EventE::Click);
                    m_pressTime = -1;
                }
            }
        }
    }else if (m_mode == ModeE::Check){
        if (m_key->Get() && !m_checkStatus){
            //insert
            m_pressTime++;
            if (m_pressTime>=2){
                m_cb(m_name.c_str(), EventE::Up);
                m_pressTime = 0;
                m_checkStatus = true;
            }
        }else if (!m_key->Get() && m_checkStatus){
            m_pressTime++;
            if (m_pressTime>=2){
                m_cb(m_name.c_str(), EventE::Down);
                m_pressTime = 0;
                m_checkStatus = false;
            }
        }
    }

}
