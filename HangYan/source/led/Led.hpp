//
// Created by xiong on 2022/7/4.
//

#ifndef EAPIL_Led_HPP
#define EAPIL_Led_HPP

#include "EmxCore.hpp"
#include "EmxBsp.hpp"

namespace Emx {

    class Led {
    public:
        Led(EuvLoop &loop) :
                m_loop(loop),
                m_blinkInvert(false) {}

        virtual ~Led() {}

        virtual void Create();

        virtual void Destroy();

        enum class StatE : uint8_t {
            StartingUp, //设备是否处于启动中
            ManualLedEna, //LED手动使能状态
            Sleeping, //设备是否处于休眠中
            WifiConfiguring, //设备是否处于Wifi配网中
            OvdConnected, //ovd sdk 是否连接成功，来自OVCConnectStatus回调
            NetConnected, //网络是否连接成功，来自NetServer
            Updating, //是否处于升级中
            Rebooting, //是否正在准备重启中
        };

        void StatChange(StatE stat, bool value);

    protected:
        void ResetStat();

        virtual void OnLedStatusChange() = 0;

        const char *GetNameByStat(StatE stat);

    protected:
        std::map<StatE, bool> m_stat;
        EuvLoop &m_loop;
        EuvTimer m_blinkTimer;
        bool m_blinkInvert;
    };
}

#endif //EAPIL_Led_HPP
