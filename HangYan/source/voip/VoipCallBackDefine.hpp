/*
* @Author: xiong
* @Date: 2022/12/7
*/

#ifndef EAPIL_VoipCallBackDefine_HPP
#define EAPIL_VoipCallBackDefine_HPP

#include <cstdint>

namespace Emx {

#define VOIP_CALL_BACK_RPC_ID "/tmp/voipRpc"
    enum class VoipCallBackIdE : uint32_t {
        OnUserLoginSucceed,
        OnUserLoginFailed,
        OnRecvCall,
        OnRecvRing,
        OnRecvAnswer,
        OnRecvHangup,
        OnRecvNotify,
        OnRecvKeyframeRequest,
        OnRecvDtmf,
        OnRecvAudioPacket,
    };
    struct VoipOnRecvCallExtArg {
        bool sleep;
        bool ovdSdkConnected;
        bool spkLoop;
        bool aCall;
    };
}
#endif //EAPIL_VoipCallBackDefine_HPP
