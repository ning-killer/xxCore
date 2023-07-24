//
// Created by xiong on 2022/6/23.
//

#ifndef EAPIL_Ovd_HPP
#define EAPIL_Ovd_HPP

#include "CallBackResp.hpp"
#include "OvdCtx.hpp"
#include "OvdNet.hpp"
#include "NetClient.hpp"

#ifndef EAPIL_SPARE

#include "Key.hpp"
#include "AvPush.hpp"
#include "Record.hpp"
#include "Schedule.hpp"
#include "AutoMaintain.hpp"
#include "SoftProbe.hpp"
#include "Alarm.hpp"
#include "NightVisionClient.hpp"
#include "AndLink.hpp"
#include "Voip.hpp"

#endif

#include "OvdSdk.hpp"
#include "AgeingInner.hpp"
#include "FtToolInner.hpp"

namespace Emx {

    class Ovd {
    public:
        Ovd() :
                m_cbResp(this),
#ifndef EAPIL_SPARE
                m_serviceStarted(false),
                m_key(nullptr),
#endif
                m_timeSync(false),
                m_startService(true),
                m_connected(false) {}

        virtual ~Ovd() {}

        void Create();

        void Destroy();

    private:
        void OnQuit();

        void OnTimeSync();

        void OnInitSDKDone();

        void OnNetInitDone();

        void OnNetConnectionChanged(bool connected);

#ifndef EAPIL_SPARE

        void OnResetKeyEvent(const char *name, Key::EventE e);

#endif

        void OnUpdateEvent(UpdateEvent::Event &e);

    private://service
        void ServiceStart();

        void ServiceStop();

        bool GetZnDebugConfig(ZnDebugInfo *info, const char* path);

        FactoryMode GetFactoryMode();

    private:
        OvdCtx m_ctx;
//
        friend class CallBackResp;

        CallBackResp m_cbResp;
        OvdSdk m_sdk;
        EuvTimer m_launcher;
        
        OvdNet m_net;
        bool m_serviceStarted;
#ifndef EAPIL_SPARE
        Key *m_key;
        AvPush m_avPush;
        Record m_record;
        Schedule m_scheduleSleep;
        Schedule m_scheduleInv;
        AutoMaintain m_autoMaintain;
        SoftProbe m_softProbe;
        Alarm m_alarm;
        AndLink m_andLink;
        Voip m_voip;
#endif
        UpdateEvent m_updateEvent;
        bool m_timeSync;
        bool m_startService;
        bool m_connected;
        FactoryMode m_factoryMode;
    };

}

#endif //EAPIL_Ovd_HPP
