#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <ovd_env.h>
#include <xtime.h>


#include "cmcc_rtc_api.h"

#include "xthread.h"
#include "xlog.h"
#include "cmcc_voip.h"
#include "media.h"
#include "third_party_device.h"

#define XLOGMOD (char *)"cmcc_voip"
//#define VIDEO_DATA_SIZE 1024 * 50

//static int g_call_session 					= 0;
//static int g_calling 						= 0;
//static int g_login_success 					= 0;
//static int g_invoke_login_interface_counts_ = 20;
//static int g_enable_video   				= 0;
//static int g_call_type                      = CMCC_CALL_TYPE_IMS_1V1_AUDIO;
//static int g_h265                       	= 0;

//enum BUSINESS_TYPE{
//    CMCC_HJGH_TYPE = 0,//家业模式
//    CMCC_PBX_TYPE,
//    CMCC_GE_TYPE,//政企模式
//};

using namespace std;

static xthread_t g_voice_thread;
static void cmcc_voip_dudu_play();

typedef struct
{
    int voipLoginSuccess;
    int call_session;
    int recvHangup;
    int user_login;//用户登录标志位
    int video_codec;//0,264;1,265
    int video_width;   //视频宽
    int video_height;   //视频高
    int quality;        //分辨率
    xthread_t sendVdThd;
    int isDevOn;    //设备是否正常运行
    int is_voipPlay; //和家智话是否正在执行中（正在下发语音对讲）
    int is_voipBroadCast;//智能喇叭广播
    int wait_answer;    //等待接听
}voip_t;

static voip_t g_voip;
static cmcc_voip_info_t g_voip_info;
static xthread_t g_voip_thd;//和家固话线程
static xthread_t g_hangUp_thd;//挂断通话
static bool isVoipInitStart = false;

static void SleepMs(int msecs) {
  struct timespec short_wait;
  struct timespec remainder;
  short_wait.tv_sec = msecs / 1000;
  short_wait.tv_nsec = (msecs % 1000) * 1000 * 1000;
  nanosleep(&short_wait, &remainder);
}


/*
 * 需要说明的是：摄像头设备视频通话期间，摄像头设备须同时上传H264和H265两种编码方式的视频流，
 * 其中H264编码方式的视频流上传和家固话平台，H265编码方式的视频流上传家庭安防平台。
 * H264编码方式的视频流清晰度360P，视频比例16:9.
 */
static void* readVideoRTPPacket()
{
	xthread_set_name(__FUNCTION__ );
    xlog(XLOG_INFO, XLOGMOD, "readVideoRTPPacket proc start\n");
    xlog(XLOG_DBG, XLOGMOD, "g_voip.video_codec:%d, g_voip.video_width:%d, g_voip.video_height:%d\n", g_voip.video_codec, g_voip.video_width, g_voip.video_height);
    g_voip_info.voip_video_change(g_voip.video_codec, g_voip.video_width, g_voip.video_height);
    int res = 0;
    int vd_type = g_voip_info.video_chn;//子码流上传
    int ifrm_need = 50;
    media_buf_t mbuf;
    int multi_chn = MEDIA_VIDEO_CHN(vd_type);
    int stream_id = media_stream_open((unsigned int)multi_chn);
    xlog(XLOG_DBG, XLOGMOD, "[%s:%d] stream_id:%d\n", __FUNCTION__ , __LINE__, stream_id);
    if(stream_id == X_FAILURE) {
        xlog(XLOG_ERR, XLOGMOD, "readVideoRTPPacket media open failed\n");
        memset(&g_voip.sendVdThd, 0, sizeof(xthread_t));
        return NULL;
    }
    media_venc_flush_IDR(vd_type);
    while(g_voip.sendVdThd.run) {
        if(!g_voip_info.get_voip_audio_play_feasible()){
            char *reason = (char *)"device is sleep or not online";
            hangUpByThread(reason, strlen(reason));
            xlog(XLOG_ERR, XLOGMOD, "get_voip_audio_play_feasible:%d, break\n", g_voip_info.get_voip_audio_play_feasible());
            break;
        }
        if(ifrm_need > 0){
            if(ifrm_need%10 == 0){
                media_venc_flush_IDR(vd_type);
            }
            ifrm_need--;
        }
        res = media_stream_get(stream_id, 2000, &mbuf);
        if (res != X_SUCCESS) {
            xlog(XLOG_ERR, XLOGMOD, "readVideoRTPPacket media_stream_get failed\n");
            continue;
        }
        cmcc_rtc_send_video(g_voip.call_session, (char *)(mbuf.data), (int )mbuf.data_len);
        media_stream_release(&mbuf);
    }
    media_stream_close(stream_id);
    xlog(XLOG_INFO, XLOGMOD, "readVideoRTPPacket proc end\n");
    return NULL;
}



static void readVideoRTPPacket_inThread()
{
	xthread_start(&g_voip.sendVdThd, (void *)readVideoRTPPacket, NULL);
	xlog(XLOG_INFO, XLOGMOD, "readVideoRTPPacket_inThread start ok\n");
}

//网络连接服务器成功；
void OnNetConnected(){
    xlog(XLOG_INFO, XLOGMOD, "OnNetConnected\n");
}
//网络连接服务器失败；
void OnNetConnectionFailed(int errorCode){
    xlog(XLOG_INFO, XLOGMOD,"OnNetConnectionFailed,errorCode:%d\n", errorCode);
}
//用户登陆成功通知；
void OnUserLoginSucceed(const char*user){
    xlog(XLOG_INFO, XLOGMOD,"OnUserLoginSucceed, user:%s\n", user);
    g_voip.voipLoginSuccess=1;
}
//用户登陆失败通知；
void OnUserLoginFailed(const char *user, int errorCode, const char *reason) {
    xlog(XLOG_INFO, XLOGMOD,"OnUserLoginFailed, user:%s, errorCode:%d, reason:%s\n", user, errorCode, reason);
    g_voip.voipLoginSuccess=0;
}
//用户注销通知；
void OnUserLogout(const char *user) {
    xlog(XLOG_INFO, XLOGMOD,"OnUserLogout, user:%s\n", user);
}
//接收云广播来电通知
void OnRecvCall(int session, const char *from, const char *displayname, const char *to,
                cmcc_call_type_t callType, const char* json_XCallControl) {
    /**
     * [1]json_call_control为”answer_auto,app_broadcast”，表示云广播来电
     * [2]json_call_control为"answer_auto,app_monitor"，表示监控和安防对讲来电
     * [3]普通来电，json_call_control为空
     *
     * 说明：
     * [1][2]种类型的来电，对接厂商需要实现自动接听的逻辑，即在此回调中先调用cmcc_rtc_setopt接口，
     * 将自动接听的flag设置到sdk层，然后再调用cmcc_rtc_pickup接口，接听来电；
     * 如果收到[3]类型的普通来电也需要实现自动接听功能，则直接在此回调中调用cmcc_rtc_pickup接口，接听来电
     *
     */

    int ret = 0;
    xlog(XLOG_INFO, XLOGMOD,"OnRecvCall, session:%d, from:%s, displayname:%s, to:%s, callType:%d, json_XCallControl:%s\n",
         session, from?from:"null", displayname?displayname:"null", to?to:"null", callType, json_XCallControl?json_XCallControl:"null");
    xlog(XLOG_INFO, XLOGMOD,"json_XCallControl:%s\n", json_XCallControl);
    if(json_XCallControl==NULL){
        return;
    }
    g_voip.call_session = session;
    //const char *nick_name = cmcc_rtc_get_nickname_by_number(from);
    if(!g_voip.isDevOn){
        cmcc_rtc_hangup(session);
        xlog(XLOG_ERR, XLOGMOD,"g_voip.isDevOn == %d,cmcc_rtc_hangup \n",g_voip.isDevOn);
        return;
    }
    //云广播（app_broadcast） 监控（app_monitor）
	if(NULL != strstr(json_XCallControl, "app_broadcast") || 
	   NULL != strstr(json_XCallControl, "app_monitor") ||
       NULL != strstr(json_XCallControl, "answer_auto"))
	{
	    if (NULL != strstr(json_XCallControl, "answer_auto"))
        {

	        if (NULL == strstr(json_XCallControl, "app_broadcast")) {
                g_voip.is_voipPlay = true;//实时对讲
#ifdef SPEAKER_LOOP_SUPPORT
                media_aenc_set_Aec(0, 1);//对讲开启回声消除
#endif
            }
	        else {
	            g_voip.is_voipBroadCast = true;//智能广播
            }

            xlog(XLOG_ERR, XLOGMOD,"g_voip.is_voipPlay:%d, g_voip.is_voipBroadCast:%d\n", g_voip.is_voipPlay, g_voip.is_voipBroadCast);
            //收到来电自动接听的，调用CPickup接听接口前，需要先设置自动接听的flag到底层
            cmcc_rtc_setopt(CMCC_OPT_AUTO_ANSWER_FLAG,"1");
            ret = cmcc_rtc_pickup(session);
        }else {
            //收到不需要自动接听的普通来电，实现自动接听
            ret = cmcc_rtc_pickup(session);
        }
    }
#ifdef CMCC_VOIP_A_CALL_SUPPORT
	else{
        xlog(XLOG_INFO, XLOGMOD,"wait_answer...\n");
        cmcc_voip_dudu_play();
        g_voip.wait_answer = 1;
	}
#endif

    if (CMCC_CALL_TYPE_IMS_1V1_VIDEO == callType && 0 == ret) //video call
    {
        readVideoRTPPacket_inThread();//发送视频数据
    }
}

//static xthread_t g_voice_thread;
static void *cmcc_voip_dudu_play_thread(void *arg) {
    xthread_set_name(__FUNCTION__ );
    xlog(XLOG_DBG, XLOGMOD, "cmcc_voip_dudu_play_thread start\n");
    unsigned char packet[2048];
    char *dudu_path = "/root/firmware/audio/dudu.pcm";
    FILE *fp = fopen(dudu_path, "rb");
    if (!fp) {
        xlog(XLOG_ERR, XLOGMOD, "cannot open %s\n", dudu_path);
        return NULL;
    }
    cmcc_voip_info_t *voip_info_t = get_cmcc_voip_info();
    int tmpPlayoutBufferSize = voip_info_t->audio_samplerate / 100 * voip_info_t->audio_chn_num * 2;
    while (g_voice_thread.run) {
        int n = fread(packet, 1, tmpPlayoutBufferSize, fp);
        if (n <= 0) {
            fseek(fp,0,SEEK_SET);
            continue;
        }
        media_pcm_frame(0, packet, n);
      //  usleep(10000);
    }
    fclose(fp);
    xlog(XLOG_DBG, XLOGMOD, "voice alarm proc end\n");
    return NULL;
}
static void cmcc_voip_dudu_play() {
    xlog(XLOG_DBG, XLOGMOD, "cmcc_voip_dudu_play\n");
    xthread_stop(&g_voice_thread);
    xthread_start(&g_voice_thread, cmcc_voip_dudu_play_thread, NULL);
}

void cmcc_voip_dudu_stop() {
    xlog(XLOG_DBG, XLOGMOD, "cmcc_voip_dudu_stop\n");
    xthread_stop(&g_voice_thread);
}

//对方已响铃通知；
void OnRecvRing(int session, const char *from, const char *displayname, const char *to,
                int EarlyMedia) { // 0 no media, 1 server media
    xlog(XLOG_INFO, XLOGMOD,"OnRecvRing,session=%d,from=%s,displayname=%s,to=%s,EarlyMedia=%d\n",session,from,displayname,to,EarlyMedia);
    g_voip.call_session = session;
    g_voip.is_voipPlay = true;
    if(EarlyMedia == 0) {
        cmcc_voip_dudu_play();
    }else{
        xthread_stop(&g_voice_thread);
    }
}

//对方已接听通知；
void OnRecvAnswer(int session, const char *from, const char *displayname, const char *to,
                  cmcc_call_type_t callType) {
    xlog(XLOG_INFO, XLOGMOD,"OnRecvAnswer, session:%d, from:%s, displayname:%s, to:%s, callType:%d\n",
         session, from, displayname, to, callType);
    xlog(XLOG_INFO, XLOGMOD,"callType:%d, g_sendVideo:%d\n",callType, g_voip.sendVdThd.run);
    xthread_stop(&g_voice_thread);
    g_voip.is_voipPlay = true;
    g_voip.call_session = session;
    if((callType == CMCC_CALL_TYPE_IMS_1V1_VIDEO) && (g_voip.sendVdThd.run == 0)){
        readVideoRTPPacket_inThread();//发送视频数据
    }
}
//通话被挂断通知；
void OnRecvHangup(int session, int errorCode, const char *reason, cmcc_call_type_t callType) {
    xlog(XLOG_INFO, XLOGMOD,"OnRecvHangup, session:%d, errorCode:%d, reason:%s, callType:%d\n", session, errorCode, reason?reason:"null", callType);
    g_voip.is_voipPlay = false;
    g_voip.is_voipBroadCast = false;
    cmcc_rtc_hangup(g_voip.call_session);
	g_voip.call_session = -1;
	g_voip.recvHangup = 1;
    xthread_stop(&g_voice_thread);
    xthread_stop(&g_voip.sendVdThd);
}

void OnRecvReinvite(int session, const char *from, const char *displayname, const char *to,
                    int callType) {
    xlog(XLOG_INFO, XLOGMOD,"OnRecvReinvite\n");
}

void OnNotifyVideoRtpPort(int port)
{
    xlog(XLOG_INFO, XLOGMOD,"OnNotifyVideoRtpPort:%d\n", port);

}

//收到此回调事件后，对接厂家需要发送一个关键帧给对端，可解决对端视频出现花屏或者卡顿的问题
void OnRecvKeyframeRequest() {
    xlog(XLOG_INFO, XLOGMOD,"__on_recv_keyframe_request \n");
    media_venc_flush_IDR(1);
}

//手机按#号键，收到回调
void OnRecvDtmf(int Eventno){
    xlog(XLOG_INFO, XLOGMOD,"OnRecvDtmf:%d\n", Eventno);
}

//接受定时广播来电等通知
void OnRecvNotify(cmcc_notify_t notify_type, const char* content){
    xlog(XLOG_INFO, XLOGMOD,"CMNotifyType, type:%d, Context:%s\n", notify_type,content);
//    int height, width;
    if(CMCC_NOTIFY_VIDEO_CODEC == notify_type){
        //CMNotifyType, type:50, Context:H264
        if(NULL != strstr(content,"H265")){
            g_voip.video_codec = 1;
        }else{
            g_voip.video_codec = 0;
        }
    }else if(CMCC_NOTIFY_VIDEO_RESOLUTION == notify_type){
        //收到此回调事件后，厂家要根据此分辨率来发送视频数据
        //CMNotifyType, type:51, Context:width=640,height=360
//        sscanf(content, "width=%d,height=%d", &g_voip.video_width,&g_voip.video_height);
        sscanf(content, "width=%d,height=%d,quality=%d", &g_voip.video_width,&g_voip.video_height,&g_voip.quality);
        xlog(XLOG_DBG, XLOGMOD,"width=%d,height=%d,quality=%d\n", g_voip.video_width,g_voip.video_height,g_voip.quality);
        //todo g_voip.quality
        /** @说明： * 收到此回调事件后，厂家要根据此分辨率来发送视频数据 ** [1]若 content 中存在 quality 字段，说明当前通话为对讲通话， *
         * 厂家需要判断当前对讲通话的 quality 值是否与安防直播的 quulity * 值一致，若一致则复用安防视频流，否则按照当前 quality 字段送流 *
         * [2]若 content 中不存在 quality 字段，则说明当前通话为实时通话， * 厂家按照 content 中实际的宽高送流 *
         * [3] quality 字段取值和安防一致，如 sd 表示标清，hd 表示高清，fhd 表示超清 ** 回调示例： *
         * [1]content:width=640,height=360,quality=sd， * 表明当前通话为对讲通话，发送视频流的分辨率根据 quality 字段，忽略宽高 *
         * [2]content:width=640,height=360， * 表明当前通话为实时通话，发送视频流的分辨率根据 width 和 height **/

    }else if(CMCC_NOTIFY_UNBIND == notify_type){
        ovd_env_t *ovd_env = ovd_env_get();
        ovd_env->cfg.binded = 0;//解绑
        ovd_env_cfg_save(&ovd_env->cfg);
        xlog(XLOG_INFO, XLOGMOD,"[%s:%d], ovd_env->cfg.binded:%d\n", __FUNCTION__ , __LINE__, ovd_env->cfg.binded);
    }
}

#ifndef ENABLE_AUDIO_CODEC
static void OnRecvAudioPacket(char *packet ,int size){

}
#endif
/**
 *  设备绑定成功后，app 层调用 GetIMSAccountNumber 接口进行获取账号登录
    时：如果返回失败的话，需循环每隔 2min 调用一次此接口，返回成功了就退出
    循环；循环期间，如果收到 OnUserLoginSucceed 注册成功的回调后，也退出循
    环。如果循环 10 次调用接口还是失败，则退出循环；
 */
int user_login()
{
    char deviceId[64] = {0};//格式为cmcc-AndlinkID-sn
    char imsNumber[20]={0};
    int account_return;
    int sleep_time = 2*60;
    int left_time = 0;
    int try_times = 0;

    sprintf(deviceId, "cmcc-%s-%s", g_voip_info.deviceid, g_voip_info.sn);
    xlog(XLOG_INFO, XLOGMOD, "user login start, appkey:%s, appsecret:%s, deviceId:%s\n", g_voip_info.appkey, g_voip_info.appsecret, deviceId);

    cmcc_rtc_login_params_t voip_login_params;
    voip_login_params.app_key 		= g_voip_info.appkey; 			// 固话平台产品创建后自动生成的
    voip_login_params.app_secret 	= g_voip_info.appsecret; 				// 固话平台产品创建后自动生成的
    voip_login_params.device_id 	= deviceId; 	// 格式为cmcc-AndlinkID-序列号

    g_voip.user_login = 1;
    do {
        if (try_times >= 10) {
            xlog(XLOG_INFO, XLOGMOD, "try_times:%d, quit loop\n", try_times);
            account_return = -1;
            break;
        }
        account_return = cmcc_rtc_login(&voip_login_params);
        if (account_return == 0 || g_voip.voipLoginSuccess) {
            if (account_return == 0)
                xlog(XLOG_INFO, XLOGMOD, "account_return:%d, imsNumber:%s, quit loop\n",account_return, imsNumber);
            if (g_voip.voipLoginSuccess == 1)
                xlog(XLOG_INFO, XLOGMOD, "user login success, quit loop\n");
            account_return = 0;
            break;
        } else {
            xlog(XLOG_ERR, XLOGMOD, "account_return error:%d\n", account_return);
        }

        left_time = sleep_time;
        while (left_time >= 0) {
            left_time--;
            if (!g_voip.user_login) {
                xlog(XLOG_INFO, XLOGMOD, "user login quit, g_voip.user_login:%d\n", g_voip.user_login);
                account_return = -1;
                break;
            }
            sleep(1);
        }
        try_times++;
    } while (g_voip.user_login);

    xlog(XLOG_INFO, XLOGMOD, "user login quit, account_return:%d\n", account_return);
    return account_return;
}

void *cmcc_voip_proc()
{
    xlog(XLOG_INFO, XLOGMOD, "cmcc_voip_proc start\n");
    xthread_set_name(__FUNCTION__ );
    int ret;
    int cnt = 0;
    memset(&g_voip, 0, sizeof(voip_t));
/*
	[1]device_model 命名规则：
	终端设备类型|厂家名称|系统类型
	说明：
	终端设备类型：SpeakerBox（音箱），Camera（摄像头），CameraCall（可主动发起呼叫的摄像头），
				ML（门铃拼英缩写），DoorLock（门锁），TV（电视），APP（手机应用）、ACS（门禁），
				Watch（手表），Robot（机器人），Lamp（灯）
	厂家名称：厂家全拼，如Baidu，Xiaomi，Tianmao，Xunfei，Huawei等
	系统类型：Linux，LiteOS，RTOS，Android等

	其中第三方厂家填写的字符串中不能有'|'

	举例：
	SpeakerBox|Baidu|RTOS
	Camera|Haikang|Linux

	[2]芯片厂家接入时，chip_model、chip_uuid,chip_brand必须填写，设备厂家接入时，直接传入空字符串即可
	[3]所填参数必须是对应芯片或设备的真实参数
*/

    cmcc_rtc_device_info_t voip_device_info;
    char device_model[64] = {0};
#ifndef CMCC_VOIP_A_CALL_SUPPORT
    sprintf(device_model,"Camera|%s|Linux", g_voip_info.manufacturers);
#else
    sprintf(device_model,"CameraCall|%s|Linux", g_voip_info.manufacturers);
#endif
    xlog(XLOG_DBG, XLOGMOD, "device_model:%s\n", device_model);

    voip_device_info.os_version         = g_voip_info.os_version;
    voip_device_info.image_version      = g_voip_info.image_version;
    voip_device_info.hardware_version   = g_voip_info.hardware_version;
    voip_device_info.device_model       = device_model;
    voip_device_info.sn 		        = g_voip_info.sn;
    voip_device_info.cmei 		        = g_voip_info.cmei;
    voip_device_info.mac 		        = g_voip_info.mac;
    voip_device_info.chip_model         = g_voip_info.chip_model;
    voip_device_info.chip_uuid 	        = g_voip_info.chip_uuid;
    voip_device_info.chip_brand	        = g_voip_info.chip_brand;

    cmcc_rtc_log_config_t voip_log_config;
    voip_log_config.log_size 			= 1024 * 100;
    voip_log_config.console_log_enable  = g_voip_info.open_debug;
    voip_log_config.log_path 			= "/tmp/cmcc_voip.txt";

    cmcc_rtc_event_handler_t voip_event_handler;

    voip_event_handler.on_login_success 		= OnUserLoginSucceed;
    voip_event_handler.on_login_failed 			= OnUserLoginFailed;
    voip_event_handler.on_recv_call 			= OnRecvCall;
    voip_event_handler.on_recv_ring 			= OnRecvRing;
    voip_event_handler.on_recv_answer 			= OnRecvAnswer;
    voip_event_handler.on_recv_hangup 			= OnRecvHangup;
    voip_event_handler.on_recv_notify 			= OnRecvNotify;
    voip_event_handler.on_recv_keyframe_request = OnRecvKeyframeRequest;
    voip_event_handler.on_recv_dtmf          	= OnRecvDtmf;
#ifndef ENABLE_AUDIO_CODEC
    voip_event_handler.on_recv_audio_packet		= OnRecvAudioPacket;
#endif

    char *sdk_version = (char *)cmcc_rtc_get_version();
    xlog(XLOG_DBG, XLOGMOD, "cmcc_rtc_get_version:%s\n", sdk_version);

    /**********************SDK初始化过程**************************/
    ret = cmcc_rtc_init(&voip_device_info, &voip_log_config, &voip_event_handler);
    xlog(XLOG_DBG, XLOGMOD, "cmcc_rtc_init ret:%d\n", ret);

    // 【SDK默认开启OPUS编码，调用如下接口可关闭OPUS编解码】
    // 【由于OPUS音频编码质量优于PCMA，故设备配置允许的情况下，建议不要关闭OPUS编码协商功能】
     cmcc_rtc_setopt(CMCC_OPT_ENABLE_OPUS_NEGOTIATE,"0");//hisi开启后，CPU占用高

    // 【SDK默认关闭H265编码协商，调用如下接口可开启H265编码协商】
    // 【协商结果会通过__on_recv_notify回调上来】
//     cmcc_rtc_setopt(CMCC_OPT_ENABLE_H265,"0");

    // 【设置分辨率, 目前支持的分辨率为640x360, 1280x720, 1920x1080, 底层默认使用640x360】
    // 【厂家最终发送视频流的分辨率要以协商结果为准，即以__on_recv_notify接口回调上来的分辨率为准】
//    cmcc_rtc_setopt(CMCC_OPT_VIDEO_RESOLUTION,"width=1920,height=1080");

    //【设置SDK 信令发送线程调度模型为轮询方式】
    // cmcc_rtc_setopt(CMCC_OPT_THREAD_MODE_LOOP, "1");


    // 打开fec
    // 0:关闭, 1: 使能xorfec, 2：使能rsfec, 99:同时使能xorfec和rsfec(计划仅调测用)
    // 目前fec功能仅支持xorfec，测试时支持xorfec和rsfec两种fec的协商
    // cmcc_rtc_setopt(CMCC_OPT_VIDEO_FEC,"0");

    // 设置竖屏模式，底层默认都是用横屏模式，特殊厂家设备可开启竖屏模式
    // cmcc_rtc_setopt(CMCC_OPT_PORTRAIT,"1");

    /**********************拉取固话账号及登录过程**************************/
    while (user_login() != X_SUCCESS && g_voip_thd.run)
    {
        cnt += 1;
        xlog(XLOG_INFO, XLOGMOD, "user_login failed, loop cnt:%d\n", cnt);
//        memset(&g_voip_thd, 0, sizeof(xthread_t));
//        return NULL;
    }
    xlog(XLOG_INFO, XLOGMOD, "user login success\n");



    while (g_voip_thd.run)
    {
        sleep(1);
    }
    cmcc_rtc_fini();
    xlog(XLOG_INFO, XLOGMOD, "cmcc_voip_proc end\n");
    return NULL;
}

static void *hangUpProc(void *arg)
{
    xlog(XLOG_INFO, XLOGMOD,"hangUp proc start\n");
    xthread_set_name(__FUNCTION__ );
    pthread_detach(pthread_self());
    char *ptr = (char *)arg;
    if (!ptr) {
        xlog(XLOG_ERR, XLOGMOD, "ptr is null\n");
        memset(&g_hangUp_thd, 0, sizeof(xthread_t));
        return NULL;
    }
    xthread_stop(&g_voice_thread);
    xlog(XLOG_INFO, XLOGMOD,"hangUp proc, CHangupWithReason:%s\n", ptr);
    int ret = cmcc_rtc_hangup(g_voip.call_session);
    g_voip.is_voipPlay = false;
    g_voip.is_voipBroadCast = false;
    g_voip.call_session = -1;
    g_voip.recvHangup = 1;
    g_voip.wait_answer = false;
    xthread_stop(&g_voip.sendVdThd);
    if (ptr) {
        free(ptr);
        ptr = NULL;
    }
    memset(&g_hangUp_thd, 0, sizeof(xthread_t));
    xlog(XLOG_INFO, XLOGMOD,"hangUp excute ok, ret:%d, session:%d\n", ret, g_voip.call_session);
    return NULL;
}

/**
 * 开启和家固话功能
 * @return
 */
int cmcc_voip_start(cmcc_voip_info_t *voip_info)
{
    //只初始化一次
    if (!isVoipInitStart)
    {
        memset(&g_voip_info,0,sizeof(cmcc_voip_info_t));
        memcpy(&g_voip_info,voip_info,sizeof(cmcc_voip_info_t));
        xthread_start(&g_voip_thd, (void *)cmcc_voip_proc, NULL);
        isVoipInitStart = true;
        xlog(XLOG_INFO, XLOGMOD, "cmcc_voip_start ok\n");
    }else {
        xlog(XLOG_INFO, XLOGMOD, "cmcc_voip_start is already start\n");
    }
    return 0;
}

/**
 * 关闭和家固话功能
 * @return
 */
int cmcc_voip_stop()
{
    isVoipInitStart = false;
    g_voip.user_login = 0;
    xthread_stop(&g_voip_thd);
    xlog(XLOG_INFO, XLOGMOD, "cmcc_voip_stop ok\n");
    return 0;
}

/**
 * 主动挂断通话
 * @return
 */
int hangUpByThread(char *reason, int strSize)
{
    if (g_voip.is_voipBroadCast || g_voip.is_voipPlay) {
        if (g_hangUp_thd.run == 0) {
            char *ptr = (char *)malloc(strSize+1);
            if (!ptr) {
                xlog(XLOG_ERR, XLOGMOD, "malloc size:%d failed\n", strSize);
                return -1;
            }
            strncpy(ptr, reason, strSize);
            xthread_start(&g_hangUp_thd, (void *)hangUpProc, ptr);
            xlog(XLOG_INFO, XLOGMOD, "hangUpByThread start ok, reason:%s\n", reason);
            return 0;
        }else {
            xlog(XLOG_INFO, XLOGMOD, "1 hangUpByThread already ok, reason:%s\n", reason);
        }
    }else {
        xlog(XLOG_INFO, XLOGMOD, "2  already ok, reason:%s\n", reason);
    }
    return 0;
}

/**
 * 设置设备当前状态，主要设备休眠或运行
 * @return
 */
int cmcc_voip_set_status(int isRun)
{
    xlog(XLOG_INFO, XLOGMOD, "cmcc_voip_set_status:%d\n", isRun);
    g_voip.isDevOn = isRun;
    return isRun;
}

/**
 * 获取和家智话是否正在执行中（正在下发语音对讲）
 * @return
 */
int get_cmcc_voip_play_status()
{
    return g_voip.is_voipPlay;
}

/**
 * 获取和家智话是否正在执行中（正在下发broadcast）
 * @return
 */
int get_cmcc_voip_broadcast_status()
{
    return g_voip.is_voipBroadCast;
}

int get_cmcc_wait_answer(){
    return g_voip.wait_answer;
}

void reset_cmcc_wait_answer(){
    g_voip.wait_answer = false;
}

int get_cmcc_call_session(){
    return g_voip.call_session;
}

void set_cmcc_voip_play_status(int status)
{
    g_voip.is_voipPlay = status;
}

cmcc_voip_info_t* get_cmcc_voip_info()
{
    return &g_voip_info;
}

/**
 * 设备主动呼叫
 * @return
 */
int cmcc_voip_callout(char *to_number )
{
    int ret = 0;
    if(to_number) {
        ret = cmcc_rtc_callout(to_number, CMCC_CALL_TYPE_IMS_1V1_VIDEO);
    }else{
        ret = cmcc_rtc_callout("88886668888", CMCC_CALL_TYPE_IMS_1V1_VIDEO);
    }
    return ret;
}

/**
 * @brief 固话是否实际已经开始语音播放
 * 
 * @return int 
 */
int get_cmcc_voip_isactual_play()
{
    return g_voip_info.audio_play;
}