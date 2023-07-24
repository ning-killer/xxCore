/*
* @Author: xiong
* @Date: 2022/12/7
*/

#ifndef EAPIL_VoipBase_HPP
#define EAPIL_VoipBase_HPP

#include "cmcc_rtc_api.h"
#include "VoipAvPush.hpp"
#include "VoipCallBackResp.hpp"

struct AudioParam {
    int sampleRate;
    int chnNum;
    bool *isPlayVoiceCall;
};

namespace Emx {
    class VoipBase {
    public:
        VoipBase() {}

        virtual ~VoipBase() {}

        ErrCodeE Create(OvdCtx *ctx);

        void Destroy();

        void HangUp();

        bool IsRunning();

    private:
        friend class VoipCallBackResp;

        OvdCtx *m_ctx;
        EuvWork m_workLogin;
        EuvTimer m_timerReLogin;
        bool m_loginSuccess;
        VoipAvPush m_avPush;
        VoipCallBackResp *m_resp;
        struct LoginArgs {
            int ret;
            char app_key[128];
            char app_secret[128];
            char device_id[128];
        };
    };

}

#endif //EAPIL_VoipBase_HPP
