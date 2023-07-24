/*
* @Author: xiong
* @Date: 2022/12/7
*/

#include "VoipCallBackDefine.hpp"
#include "VoipCallBackWrap.h"
#include "ThreadInvoke.hpp"

using namespace Emx;

void InitEventHandlerList(cmcc_rtc_event_handler_t *list) {
    list->on_login_success = OnUserLoginSucceed;
    list->on_login_failed = OnUserLoginFailed;
    list->on_recv_call = OnRecvCall;
    list->on_recv_ring = OnRecvRing;
    list->on_recv_answer = OnRecvAnswer;
    list->on_recv_hangup = OnRecvHangup;
    list->on_recv_notify = OnRecvNotify;
    list->on_recv_keyframe_request = OnRecvKeyframeRequest;
    list->on_recv_dtmf = OnRecvDtmf;
    list->on_recv_audio_packet = OnRecvAudioPacket;
}

static ThreadInvoke::Req gVoipReq(VOIP_CALL_BACK_RPC_ID);

//用户登陆成功通知；
void OnUserLoginSucceed(const char *user) {
    TimeRec timeCost;
    emxlogd("into\n");
    int ret = -1;
    void *buffer[] = {
            &ret,
            &user
    };
    ThreadInvoke::Packet packet((uint32_t) VoipCallBackIdE::OnUserLoginSucceed, buffer);
    gVoipReq.Send(packet);
    emxlogd("leave cost:%ums\n", timeCost.GetInv());
}

//用户登陆失败通知；
void OnUserLoginFailed(const char *user, int errorCode, const char *reason) {
    TimeRec timeCost;
    emxlogd("into\n");
    int ret = -1;
    void *buffer[] = {
            &ret,
            &user
    };
    ThreadInvoke::Packet packet((uint32_t) VoipCallBackIdE::OnUserLoginSucceed, buffer);
    gVoipReq.Send(packet);
    emxlogd("leave cost:%ums\n", timeCost.GetInv());
}

//接收云广播来电通知
void OnRecvCall(int session, const char *from, const char *displayname, const char *to,
                cmcc_call_type_t callType, const char *json_XCallControl) {
    TimeRec timeCost;
    emxlogd("into\n");
    int ret = -1;
    void *buffer[] = {
            &ret,
            &session,
            &from,
            &displayname,
            &to,
            &callType,
            &json_XCallControl,
    };
    ThreadInvoke::Packet packet((uint32_t) VoipCallBackIdE::OnRecvCall, buffer);
    gVoipReq.Send(packet);
    emxlogd("leave cost:%ums\n", timeCost.GetInv());
}

//对方已响铃通知；
void OnRecvRing(int session, const char *from, const char *displayname, const char *to,
                int EarlyMedia) {
    TimeRec timeCost;
    emxlogd("into\n");
    int ret;
    void *buffer[] = {
            &ret,
            &session,
            &from,
            &displayname,
            &to,
            &EarlyMedia,
    };
    ThreadInvoke::Packet packet((uint32_t) VoipCallBackIdE::OnRecvRing, buffer);
    gVoipReq.Send(packet);
    emxlogd("leave cost:%ums\n", timeCost.GetInv());
}

//对方已接听通知；
void OnRecvAnswer(int session, const char *from, const char *displayname, const char *to,
                  cmcc_call_type_t callType) {
    TimeRec timeCost;
    emxlogd("into\n");
    int ret = -1;
    void *buffer[] = {
            &ret,
            &session,
            &from,
            &displayname,
            &to,
            &callType,
    };
    ThreadInvoke::Packet packet((uint32_t) VoipCallBackIdE::OnRecvAnswer, buffer);
    gVoipReq.Send(packet);
    emxlogd("leave cost:%ums\n", timeCost.GetInv());
}

//通话被挂断通知；
void OnRecvHangup(int session, int errorCode, const char *reason, cmcc_call_type_t callType) {
    TimeRec timeCost;
    emxlogd("into\n");
    int ret = -1;
    void *buffer[] = {
            &ret,
            &session,
            &errorCode,
            &reason,
            &callType,
    };
    ThreadInvoke::Packet packet((uint32_t) VoipCallBackIdE::OnRecvHangup, buffer);
    gVoipReq.Send(packet);
    emxlogd("leave cost:%ums\n", timeCost.GetInv());
}

//接受定时广播来电等通知
void OnRecvNotify(cmcc_notify_t notify_type, const char *content) {
    TimeRec timeCost;
    emxlogd("into\n");
    int ret = -1;
    void *buffer[] = {
            &ret,
            &notify_type,
            &content,
    };
    ThreadInvoke::Packet packet((uint32_t) VoipCallBackIdE::OnRecvNotify, buffer);
    gVoipReq.Send(packet);
    emxlogd("leave cost:%ums\n", timeCost.GetInv());
}

//收到此回调事件后，对接厂家需要发送一个关键帧给对端，可解决对端视频出现花屏或者卡顿的问题
void OnRecvKeyframeRequest() {
    TimeRec timeCost;
    emxlogd("into\n");
    int ret = -1;
    void *buffer[] = {
            &ret,
    };
    ThreadInvoke::Packet packet((uint32_t) VoipCallBackIdE::OnRecvKeyframeRequest, buffer);
    gVoipReq.Send(packet);
    emxlogd("leave cost:%ums\n", timeCost.GetInv());
}

//手机按#号键，收到回调
void OnRecvDtmf(int Eventno) {
    TimeRec timeCost;
    emxlogd("into\n");
    int ret = -1;
    void *buffer[] = {
            &ret,
            &Eventno,
    };
    ThreadInvoke::Packet packet((uint32_t) VoipCallBackIdE::OnRecvDtmf, buffer);
    gVoipReq.Send(packet);
    emxlogd("leave cost:%ums\n", timeCost.GetInv());
}

void OnRecvAudioPacket(char *pack, int size) {
    TimeRec timeCost;
    emxlogd("into\n");
    int ret = -1;
    void *buffer[] = {
            &ret,
            &pack,
            &size,
    };
    ThreadInvoke::Packet packet((uint32_t) VoipCallBackIdE::OnRecvAudioPacket, buffer);
    gVoipReq.Send(packet);
    emxlogd("leave cost:%ums\n", timeCost.GetInv());
}
