/*
* @Author: xiong
* @Date: 2022/10/10
*/

#ifndef EAPIL_SoftProbe_HPP
#define EAPIL_SoftProbe_HPP

#include "EmxCore.hpp"
#include "OvdCtx.hpp"

namespace Emx {

    class SoftProbe {
    public:
        SoftProbe() : m_create(false) {}

        ErrCodeE Create(OvdCtx *ctx);

        void Update();

        void Destroy();

    private:
        void OnTimer();

        void OnDetect(void *arg);

        void OnDetectComplete(ErrCodeE e, void *arg);

    private:
        bool m_create;        
        OvdCtx *m_ctx;
#ifdef OVDSDK_APIVER_1_0
        OVDProbeDetector m_softProbe;
        bool m_isWorking;
        EuvTimer m_timer;
        EuvWork m_work;
#endif
    };

}

#endif //EAPIL_SoftProbe_HPP
