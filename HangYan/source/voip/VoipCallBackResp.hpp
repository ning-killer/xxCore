/*
* @Author: xiong
* @Date: 2022/12/7
*/

#ifndef EAPIL_VoipCallBackResp_HPP
#define EAPIL_VoipCallBackResp_HPP

#include "ThreadInvoke.hpp"
#include "VoipCallBackDefine.hpp"
#include "cmcc_rtc_api.h"

namespace Emx {
    class OvdCtx;
    class VoipBase;
    class VoipCallBackResp {
    public:
        VoipCallBackResp(VoipBase *voip);

        void Start();

        void Stop();

        void PauseHandle(bool isPause);

    private:
        void ProcCallBack(ThreadInvoke::Packet &packet);

    private:
        //用户登陆成功通知；
        void OnUserLoginSucceed(const char *user);

        //用户登陆失败通知；
        void OnUserLoginFailed(const char *user, int errorCode, const char *reason);

        //接收云广播来电通知
        struct OnRecvCallExtArg {
            bool sleep;//记录设备的休眠状态
            bool ovdSdkConnected;//ovd sdk回调返回的 sdk 联网状态
            bool spkLoop;//mic是否临时取消aec
            bool aCall;//是否支持一键喊话
        };

        void OnRecvCall(int session, const char *from, const char *displayname, const char *to,
                        cmcc_call_type_t callType, const char *json_XCallControl);

        //对方已响铃通知；
        void OnRecvRing(int session, const char *from, const char *displayname, const char *to,
                        int EarlyMedia);

        //对方已接听通知；
        void OnRecvAnswer(int session, const char *from, const char *displayname, const char *to,
                          cmcc_call_type_t callType);

        //通话被挂断通知；
        void OnRecvHangup(int session, int errorCode, const char *reason, cmcc_call_type_t callType);

        //接受定时广播来电等通知
        void OnRecvNotify(cmcc_notify_t notify_type, const char *content);

        //收到此回调事件后，对接厂家需要发送一个关键帧给对端，可解决对端视频出现花屏或者卡顿的问题
        void OnRecvKeyframeRequest();

        //手机按#号键，收到回调
        void OnRecvDtmf(int Eventno);

        void OnRecvAudioPacket(char *packet, int size);

    private:
        VoipBase *m_voip;
        int m_session;
        ThreadInvoke::Resp m_resp;
        int m_vencChn;
        int m_width;
        int m_height;
        int m_keyFrameCnt;
        bool m_pauseHandle;

    public:
        void Clear();
        void Hangup(int session);

    private:
        void Pickup(int session, cmcc_call_type_t callType);
    };
}

#endif //EAPIL_VoipCallBackResp_HPP
