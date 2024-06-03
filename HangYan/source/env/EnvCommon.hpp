#ifndef EAPIL_Env_Common_HPP
#define EAPIL_Env_Common_HPP

#include <map>
#include "EmxCore.hpp"
#include "EmxMedia.hpp"

namespace Emx { 
    class SoundAlarmControl {
        public:
            enum Type {
                VEHICLE_GEN = 0,
                VEHICLE_MOTOR,
                TRANSGRESSION,
                ALERTAREA,
                FACE_MASK,
                RegionalPeople,
            };
            enum Status {
                Off = 0,
                On,
            };

        public:
            SoundAlarmControl() {};
            ~SoundAlarmControl() {};

        public:
            void Update(Type type, Status status) {
                auto it = m_alarmStatus.find(type);
                if (it != m_alarmStatus.end()) {
                    it->second = status;
                    return;
                }
                m_alarmStatus[type] = status;
            }
            
            void NotifyControlOff(Type type) {
                auto it = m_alarmStatus.find(type);
                if (it == m_alarmStatus.end()) {
                    // 不存在事件则不控制
                    return;
                }
                if (m_alarmStatus[type] == On) {
                    // 中断音频
                    MediaClientAdecStreamSync::PlayCtrlQuit(0);
                }
            }
        private:
            std::map<Type, Status> m_alarmStatus;
    };
}

#endif //EAPIL_Env_Common_HPP
