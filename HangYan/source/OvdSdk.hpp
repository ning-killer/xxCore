/*
* @Author: xiong
* @Date: 2022/11/9
*/

#ifndef EAPIL_OvdSdk_HPP
#define EAPIL_OvdSdk_HPP

#include "OvdCtx.hpp"

namespace Emx {

    class OvdSdk {
    public:
        OvdSdk() : m_inited(false) {}

        using OnInitDone = std::function<void()>;

        void Init(OvdCtx *ctx, OnInitDone onInitDone);

        void DeInit();

        void makeSdkPath(OvdCtx *ctx);

    private:
        struct InitSDKArg {
            OVDCapInfo capInfo;
            OVDClientParam clientParam;
            OVDLogParam logParam;
            OVD_CallBackFunList callBackFunList;
            std::string jsonParam;
            int logContrl;
            int ret;
#ifdef OVDSDK1_38_1
            ovd_reboot_param_t rebootParam;
#endif
        };
        bool m_inited;
        OnInitDone m_onInitDone;
        EuvWork m_initSDKWork;
        EuvTimer m_timer;
    };

}

#endif //EAPIL_OvdSdk_HPP
