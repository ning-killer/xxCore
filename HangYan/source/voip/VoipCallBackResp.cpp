/*
* @Author: xiong
* @Date: 2022/12/7
*/
#include "VoipCallBackDefine.hpp"
#include "VoipCallBackResp.hpp"
#include "EmxMedia.hpp"
#include "VoipBase.hpp"
#include "OvdCtx.hpp"

using namespace Emx;

VoipCallBackResp::VoipCallBackResp(VoipBase *voip) :
        m_voip(voip),
        m_resp(voip->m_ctx->loop) {}


void VoipCallBackResp::Start() {
    m_resp.Start(VOIP_CALL_BACK_RPC_ID, std::bind(&VoipCallBackResp::ProcCallBack, this, std::placeholders::_1));
}

void VoipCallBackResp::Stop() {
    m_resp.Stop();
}

void VoipCallBackResp::ProcCallBack(ThreadInvoke::Packet &packet) {
    emxlogt("start callback id:%d\n", packet.id);
    switch ((VoipCallBackIdE) packet.id) {
        case VoipCallBackIdE::OnUserLoginSucceed: {
            auto e = (void **) packet.data;
            //@_@!!!
            OnUserLoginSucceed(*(const char **) e[1]);
            break;
        }
        case VoipCallBackIdE::OnUserLoginFailed: {
            auto e = (void **) packet.data;
            OnUserLoginFailed(*(const char **) e[1],
                              *(int *) e[2],
                              *(const char **) e[3]);
            break;
        }
        case VoipCallBackIdE::OnRecvCall: {
            auto e = (void **) packet.data;
            OnRecvCall(*(int *) e[1],
                       *(const char **) e[2],
                       *(const char **) e[3],
                       *(const char **) e[4],
                       *(cmcc_call_type_t *) e[5],
                       *(const char **) e[6]);
            break;
        }
        case VoipCallBackIdE::OnRecvRing: {
            auto e = (void **) packet.data;
            OnRecvRing(*(int *) e[1],
                       *(const char **) e[2],
                       *(const char **) e[3],
                       *(const char **) e[4],
                       *(int *) e[5]);
            break;
        }
        case VoipCallBackIdE::OnRecvAnswer: {
            auto e = (void **) packet.data;
            OnRecvAnswer(*(int *) e[1],
                         *(const char **) e[2],
                         *(const char **) e[3],
                         *(const char **) e[4],
                         *(cmcc_call_type_t *) e[5]);
            break;
        }
        case VoipCallBackIdE::OnRecvHangup: {
            auto e = (void **) packet.data;
            OnRecvHangup(*(int *) e[1],
                         *(int *) e[2],
                         *(const char **) e[3],
                         *(cmcc_call_type_t *) e[4]);
            break;
        }
        case VoipCallBackIdE::OnRecvNotify: {
            auto e = (void **) packet.data;
            OnRecvNotify(*(cmcc_notify_t *) e[1],
                         *(const char **) e[2]);
            break;
        }
        case VoipCallBackIdE::OnRecvKeyframeRequest: {
//            auto e = (void **) packet.data;
            OnRecvKeyframeRequest();
            break;
        }
        case VoipCallBackIdE::OnRecvDtmf: {
            auto e = (void **) packet.data;
            OnRecvDtmf(*(int *) e[1]);
            break;
        }
        case VoipCallBackIdE::OnRecvAudioPacket: {
            auto e = (void **) packet.data;
            OnRecvAudioPacket(*(char **) e[1],
                              *(int *) e[2]);
            break;
        }
        default:
            emxloge("unknown OVD Call back ID : %d\n", packet.id);
            break;
    }
    emxlogt("end callback id:%d\n", packet.id);

}


void VoipCallBackResp::OnUserLoginSucceed(const char *user) {
    emxlogi("user:%s\n", user);
    m_voip->m_loginSuccess = true;
}

void VoipCallBackResp::OnUserLoginFailed(const char *user, int errorCode, const char *reason) {
    emxloge("user:%s, errorCode:%d, reason:%s\n", user, errorCode, reason);
    m_voip->m_loginSuccess = false;
}

static void PlayDuDu() {
    char path[EMX_MAX_PATH_SIZE] = {};
    sprintf(path, "%s/audio/dudu.aac", Misc::GetAppConfigsDir());
    MediaClientAdecStreamSync::PlayFile(0, path, 1000);
}

void VoipCallBackResp::OnRecvCall(int session,
                                  const char *from, const char *displayname, const char *to,
                                  cmcc_call_type_t callType, const char *json_XCallControl) {
    /*
     * 说明：
     * [1] json_call_control中有"answer_auto"字段，表示需要自动接听的来电，走自动接听逻辑；
     * [2] json_call_control中没有"answer_auto"字段，表示非自动接听的来电，走手动接听逻辑；
     * [3] json_call_control中有"app_broadcast"字段，表示云广播来电
     * [4] json_call_control中有"app_talk"字段，表示安防对讲来电
     *
     * 备注：
     * [1] 对于需要自动接听的来电，先调用cmcc_rtc_setopt接口，将自动接听的flag设置到sdk层，
     *     然后再调用cmcc_rtc_pickup接口，接听来电；
     * [2] 对于未带answer_auto的来电，需要手动接听；
     *
     */

    emxlogi("OnRecvCall, session:%d, from:%s, displayname:%s, to:%s, callType:%d, json_XCallControl:%s\n",
           session, from ? from : "null", displayname ? displayname : "null", to ? to : "null", callType,
           json_XCallControl ? json_XCallControl : "null");

    if (json_XCallControl == nullptr) {
        emxloge("json_XCallControl is null\n");
        return;
    }

    //const char *nick_name = cmcc_rtc_get_nickname_by_number(from);
    if (m_voip->m_ctx->running.sleep || !m_voip->m_ctx->running.ovdSdkConnected) {
        emxloge("sleep=%d,connected=%d,Hangup\n", m_voip->m_ctx->running.sleep, m_voip->m_ctx->running.ovdSdkConnected);
        Hangup(session);
        return;
    }
    //云广播（app_broadcast） 监控（app_monitor）
    auto autoPtr = strstr(json_XCallControl, "answer_auto");
    auto broadcastPtr = strstr(json_XCallControl, "app_broadcast");
    if (autoPtr || broadcastPtr) {
        if (autoPtr) {
            if (!broadcastPtr) {
                //实时对讲
                m_voip->m_ctx->running.voip.isRunVideoCall = true;
                emxlogi("talkback\n");
                if (m_voip->m_ctx->deviceJsonCfg["voip"]["spkLoop"].asBool()) {
//                    media_aenc_set_Aec(0, 1);//对讲开启回声消除
                }
            } else {
                m_voip->m_ctx->running.voip.isRunVoiceCall = true;
                //智能广播
                emxlogi("broadcast\n");
            }
            cmcc_rtc_setopt(CMCC_OPT_AUTO_ANSWER_FLAG, "1");
            Pickup(session, callType);
        } else {
            emxlogi("manual\n");
            Pickup(session, callType);
        }
    } else {
        if (strlen(json_XCallControl) == 0) {
            cmcc_rtc_setopt(CMCC_OPT_AUTO_ANSWER_FLAG, "1");
            Pickup(session, callType);
        } else if (m_voip->m_ctx->deviceJsonCfg["voip"]["aCall"].asBool()) {
            emxlogi("wait_answer...\n");
            PlayDuDu();
//            g_voip.wait_answer = 1;
            if (CMCC_CALL_TYPE_IMS_1V1_VIDEO == callType) {//video call
                m_voip->m_avPush.Create(session, m_vencChn);
            }
        }
    }
}

void VoipCallBackResp::OnRecvRing(int session,
                                  const char *from, const char *displayname, const char *to, int EarlyMedia) {
    emxlogi("OnRecvRing,session=%d,from=%s,displayname=%s,to=%s,EarlyMedia=%d\n",
           session, from, displayname, to, EarlyMedia);
    m_voip->m_ctx->running.voip.isRunVideoCall = true;
    if (EarlyMedia == 0) {
        PlayDuDu();
    } else {
        MediaClientAdecStreamSync::PlayCtrlQuit(0);
    }
}

void VoipCallBackResp::OnRecvAnswer(int session, const char *from, const char *displayname, const char *to,
                                    cmcc_call_type_t callType) {
    emxlogi("OnRecvAnswer, session:%d, from:%s, displayname:%s, to:%s, callType:%d\n",
           session, from, displayname, to, callType);
    MediaClientAdecStreamSync::PlayCtrlQuit(0);
    m_voip->m_ctx->running.voip.isRunVideoCall = true;
    emxlogi("callType=%d, av push running=%d\n", callType, m_voip->m_avPush.IsRunning());
    if (callType == CMCC_CALL_TYPE_IMS_1V1_VIDEO) {
        m_voip->m_avPush.Create(session, m_vencChn);
    }
}

void VoipCallBackResp::OnRecvHangup(int session, int errorCode, const char *reason, cmcc_call_type_t callType) {
    emxlogi("OnRecvHangup, session:%d, errorCode:%d, reason:%s, callType:%d\n", session, errorCode,
           reason ? reason : "null", callType);
    m_keyFrameCnt = 0;
    m_voip->m_avPush.Destroy();
    Hangup(session);
}

void VoipCallBackResp::OnRecvNotify(cmcc_notify_t notify_type, const char *content) {
    emxlogi("type:%d, content:%s\n", notify_type, content);
    if (CMCC_NOTIFY_VIDEO_CODEC == notify_type) {
        MediaClientVenc venc[2] = {0, 1};
        MediaVenc::Param param[2] = {};
        venc[0].GetParam(param[0]);
        venc[1].GetParam(param[1]);
        VideoCodecE codec;
        if (strstr(content, "H265")) {
            codec = VideoCodecE::H265;
        } else {
            codec = VideoCodecE::H264;
        }
        emxlogd("venc chn0 width(%d);m_height(%d);codec(%d)\n", param[0].width, param[0].height, param[0].codec);
        if (m_width == param[0].width && m_height == param[0].height && codec == param[0].codec) {
            m_vencChn = 0;
            emxlogi("use live video to send\n");
        } else {
            m_vencChn = 1;
            emxlogi("use sub video to send\n");
            param[m_vencChn].codec = codec;
            m_width = param[m_vencChn].width;
            m_height = param[m_vencChn].height;
            venc[m_vencChn].SetParam(param[m_vencChn]);
            emxlogd("use chn=%d,codec=%d,width=%d,height=%d\n",m_vencChn, (int) codec, m_width, m_height);
            return;
            // param[m_vencChn].codec = codec;
            // param[m_vencChn].width = m_width;
            // param[m_vencChn].height = m_height;
            // if (venc[m_vencChn].SetParam(param[m_vencChn]) == ErrCodeE::Success) {
            //     return;
            // }
            //note: 下发分辨率不支持，切换ovdDeviceCfg.json配置的voip对讲分辨率
            // m_width = m_voip->m_ctx->deviceJsonCfg["voip"]["resolution"]["width"].asInt();
            // m_height = m_voip->m_ctx->deviceJsonCfg["voip"]["resolution"]["height"].asInt();
            // emxlogd("resolution issued by the platform is unsupported, switch voip default config\n");
        }
        emxlogd("use chn=%d,codec=%d,width=%d,height=%d\n",
               m_vencChn, (int) param[m_vencChn].codec, param[m_vencChn].width, param[m_vencChn].height);
    } else if (CMCC_NOTIFY_VIDEO_RESOLUTION == notify_type) {
        /*
        * @说明：
        * 收到此回调事件后，厂家要根据此分辨率来发送视频数据
        *
        * [1]若content中存在quality字段，说明当前通话为对讲通话，
        *    厂家需要判断当前对讲通话的quality值是否与安防直播的quality
        *    值一致，若一致且编码方式（H264/H265）也和安防一致，则复用安防视频流，
        *    否则按照当前quality字段和编码方式送流。
        * [2]特殊情况下，如果安防和固话的视频流不一致（即编码方式或分辨率不一致），且设备不能同时出两路高清（或超高清）视频流时，
        *    设备出的第二路流（固话流）的分辨率可以降到设备能够出的最高分辨率。
        *    如：安防直播流是H265、1080P的流，对讲是H264、1080P的流，但此时设备能出的第二路对讲流的最高分辨率是H264、720P，
        *    那么厂家可以推H264、720P的流给固话SDK。
        * [3]若content中不存在quality字段，则说明当前通话为实时通话，
        *    厂家按照content中实际的宽高和编码送流
        * [4]quality字段取值和安防一致，如sd表示标清，hd表示高清，fhd表示超清
        *
        * 回调示例：
        * [1]content:width=640,height=360,quality=sd，
        *    表明当前通话为对讲通话，发送视频流的分辨率根据quality字段，忽略宽高
        * [2]content:width=640,height=360，
        *    表明当前通话为实时通话，发送视频流的分辨率根据width和height
        */

        /*
             实测时，SDK先给到CMCC_NOTIFY_VIDEO_RESOLUTION回调再给到CMCC_NOTIFY_VIDEO_CODEC
             需要等两个回调都收到再处理，并且回调的顺序是固定的（已经向智话开发进行确定）
        */

        m_width = 0;
        m_height = 0;
        /* 存在quality字段 */
        if (strstr(content, "quality")) {
            char quality[8] = {};
            /* 实际下发：width=640,height=360,quality=sd */
            int ret = sscanf(content, "width=%d,height=%d,quality=%s", &m_width, &m_height, quality);
            if (ret != 3) {
                emxloge("sscanf ret:%d != 3\n", ret);
            } else {
                if (m_voip->m_ctx->deviceJsonCfg["videoQuality"].isMember(quality)) {
                    m_width = m_voip->m_ctx->deviceJsonCfg["videoQuality"][quality]["venc"]["width"].asInt();
                    m_height = m_voip->m_ctx->deviceJsonCfg["videoQuality"][quality]["venc"]["height"].asInt();
                }
            }
        } else {
            int tmp[2] = {};
            int ret = sscanf(content, "width=%d,height=%d", &tmp[0], &tmp[1]);
            if (ret != 2) {
                emxloge("sscanf ret:%d != 2\n", ret);
            } else {
                m_width = tmp[0];
                m_height = tmp[1];
            }
        }
        emxlogd("width=%d,height=%d\n",m_width, m_height);
    } else if (CMCC_NOTIFY_UNBIND == notify_type) {
        m_voip->m_ctx->env.voip.bind = false;
        m_voip->m_ctx->env.voip.Save();
        emxlogi("unbind\n");
    }
}

void VoipCallBackResp::OnRecvKeyframeRequest() {
    if (m_keyFrameCnt % 5 == 0) {
        MediaClientVenc venc(m_vencChn);
        venc.FlushIDR();
    }
    m_keyFrameCnt++;
}

void VoipCallBackResp::OnRecvDtmf(int Eventno) {
    emxlogi("OnRecvDtmf:%d\n", Eventno);
}

void VoipCallBackResp::OnRecvAudioPacket(char *packet, int size) {
    printf("%s:%d\n", __FUNCTION__, __LINE__);
}

void VoipCallBackResp::Clear() {
    m_keyFrameCnt = 0;
    m_voip->m_avPush.Destroy();
}

void VoipCallBackResp::Hangup(int session) {
    struct HangupArg {
        int session = -1;
        EuvWork work;
    };
    auto hangupArg = new HangupArg;
    hangupArg->session = session;
    m_voip->m_ctx->running.voip.isRunVideoCall = false;
    m_voip->m_ctx->running.voip.isRunVoiceCall = false;
    hangupArg->work.Create(&m_voip->m_ctx->loop, hangupArg,
                           [this](void *arg) {
                               auto hangupArg = (HangupArg *) arg;
                               cmcc_rtc_hangup(hangupArg->session);
                               emxlogi("cmcc_rtc_hangup done\n");
                           },
                           [this](ErrCodeE e, void *arg) {
                               auto hangupArg = (HangupArg *) arg;
                               if (m_voip->m_ctx->deviceJsonCfg["voip"]["spkLoop"].asBool()) {
                                   //media_aenc_set_Aec(0, 0);//默认关闭回声消除
                               }
                               hangupArg->work.Destroy();
                               delete hangupArg;
                           });
    hangupArg->work.Run();
}

void VoipCallBackResp::Pickup(int session, cmcc_call_type_t callType) {
    struct PickupArg {
        int session = -1;
        cmcc_call_type_t callType = CMCC_CALL_TYPE_IMS_1V1_VIDEO;
        int ret = 0;
        EuvWork work;
    };
    auto pickupArg = new PickupArg;
    pickupArg->session = session;
    pickupArg->callType = callType;
    pickupArg->work.Create(&m_voip->m_ctx->loop, pickupArg,
                           [this](void *arg) {
                                auto pickupArg = (PickupArg *) arg;
                                pickupArg->ret = cmcc_rtc_pickup(pickupArg->session);
                                emxlogd("pickup done, return %d\n", pickupArg->ret);
                           },
                           [this](ErrCodeE e, void *arg) {
                               auto pickupArg = (PickupArg *) arg;
                               pickupArg->work.Destroy();
                               if (CMCC_CALL_TYPE_IMS_1V1_VIDEO == pickupArg->callType 
                                    && 0 == pickupArg->ret) {//video call
                                    m_voip->m_avPush.Create(pickupArg->session, m_vencChn);
                                    // m_voip->m_ctx->running.voip.isRunVideoCall = true;
                                } else if (CMCC_CALL_TYPE_IMS_1V1_VIDEO != pickupArg->callType 
                                    && 0 == pickupArg->ret) {//voice call
                                    // m_voip->m_ctx->running.voip.isRunVoiceCall = true;
                                }
                               delete pickupArg;
                           });
    pickupArg->work.Run();
}