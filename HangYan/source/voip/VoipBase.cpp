/*
* @Author: xiong
* @Date: 2022/12/7
*/

#include "VoipBase.hpp"
#include "VoipCallBackWrap.h"
#include "OvdCtx.hpp"
#include "VoipDefine.hpp"

using namespace Emx;
AudioParam gAudioParam;

void VoipBase::HangUp() {
    if (m_resp == nullptr) {
        return;
    }
    if (!m_ctx->running.voip.isRunVideoCall) {
        //note: 对讲未开启不触发主动挂断
        return;
    }
    if (m_ctx->running.voip.isRunVoiceCall) {
        //note: 云广播开启不触发主动挂断
        return;
    }
    m_resp->Clear();
    m_resp->Hangup(m_avPush.GetSession());
}

ErrCodeE VoipBase::Create(OvdCtx *ctx) {
    if (m_pauseHandle) {
        m_resp->PauseHandle(false);
        m_pauseHandle = false;
        return ErrCodeE::Success;;
    }
    m_ctx = ctx;
    if (m_resp == nullptr) {
        m_resp = new VoipCallBackResp(this);
    }
    m_resp->Start();
    m_ctx->running.voip.handle = this;
    gAudioParam.isPlayVoiceCall = &(m_ctx->running.alarm.isPlayVoiceCall);
/*
 *
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
    MediaClientAdec adec(0);
    MediaAdec::Param param = {};
    adec.GetParam(param);
    gAudioParam.sampleRate = param.sampleRate;
    gAudioParam.chnNum = 1;
    static cmcc_rtc_device_info_t voip_device_info = {};
    char device_model[64] = {0};
    sprintf(device_model, "%s|%s|Linux",
            m_ctx->deviceJsonCfg["voip"]["aCall"].asBool() ? "CameraCall" : "Camera",
            m_ctx->env.manufacturers.deviceBrand);
    emxlogd("device_model:%s\n", device_model);

    voip_device_info.os_version = m_ctx->env.manufacturers.os;
    voip_device_info.image_version = m_ctx->env.devInfo.GetOuter()->version;
    voip_device_info.hardware_version = m_ctx->env.devInfo.GetHardware()->version;
    voip_device_info.device_model = device_model;
    voip_device_info.sn = m_ctx->env.sn.Get();
    voip_device_info.cmei = m_ctx->env.burn.cmei.data();
    voip_device_info.mac = m_ctx->env.mac.Get();
    voip_device_info.chip_model = m_ctx->env.manufacturers.chipModel;
    // voip_device_info.chip_uuid = m_ctx->env.manufacturers.chipUuid;
    voip_device_info.chip_uuid = (m_ctx->env.burn.cmei).c_str();
    voip_device_info.chip_brand = m_ctx->env.manufacturers.chipFactory;

    static cmcc_rtc_log_config_t voip_log_config = {};
    voip_log_config.log_size = 1024 * 100;
    voip_log_config.console_log_enable = 0;
    voip_log_config.log_path = "/tmp/syslog/cmcc_voip.txt";

    static cmcc_rtc_event_handler_t voip_event_handler = {};
    InitEventHandlerList(&voip_event_handler);

    /**********************SDK初始化过程**************************/
    int ret = cmcc_rtc_init(&voip_device_info, &voip_log_config, &voip_event_handler);
    emxlogd("cmcc_rtc_init ret:%d\n", ret);

    char *sdk_version = (char *) cmcc_rtc_get_version();
    emxlogd("cmcc_rtc_get_version:%s\n", sdk_version);

    // 【SDK默认开启OPUS编码，调用如下接口可关闭OPUS编解码】
    // 【由于OPUS音频编码质量优于PCMA，故设备配置允许的情况下，建议不要关闭OPUS编码协商功能】
    cmcc_rtc_setopt(CMCC_OPT_ENABLE_OPUS_NEGOTIATE, "0");//hisi开启后，CPU占用高

    // 【SDK默认关闭H265编码协商，调用如下接口可开启H265编码协商】
    // 【协商结果会通过__on_recv_notify回调上来】
//     cmcc_rtc_setopt(CMCC_OPT_ENABLE_H265,"0");

    // 【设置设备支持的最高分辨率, 目前SDK支持设置的分辨率为低清(640*360)，高清(1280*720)，超高清(1920*1080)】
    // 【厂家发送视频流的分辨率原则上以协商结果为准，即以__on_recv_notify接口回调上来的分辨率为准】
    // 【特殊情况下，如果不能发送协商结果对应分辨率的流，则可以降一档分辨率发送。如协商结果为1080P，厂家不能发送1080P的流，则发送720P的流】
    // 【厂家需调用如下接口，设置支持的最高分辨率（如1080P）】
    char resolution[23] = {};
    snprintf(resolution, sizeof(resolution), "width=%d,height=%d",
             m_ctx->deviceJsonCfg["voip"]["resolution"]["width"].asInt(),
             m_ctx->deviceJsonCfg["voip"]["resolution"]["height"].asInt());
    cmcc_rtc_setopt(CMCC_OPT_VIDEO_RESOLUTION, resolution);
    //emxlogd("CMCC_OPT_VIDEO_RESOLUTION:%s\n",resolution);
    //【设置SDK 信令发送线程调度模型为轮询方式】
    // cmcc_rtc_setopt(CMCC_OPT_THREAD_MODE_LOOP, "1");

    // 打开fec
    // 0:关闭, 1: 使能xorfec, 2：使能rsfec, 99:同时使能xorfec和rsfec(计划仅调测用)
    // 目前fec功能仅支持xorfec，测试时支持xorfec和rsfec两种fec的协商
    // cmcc_rtc_setopt(CMCC_OPT_VIDEO_FEC,"0");

    // 设置竖屏模式，底层默认都是用横屏模式，特殊厂家设备可开启竖屏模式
    // cmcc_rtc_setopt(CMCC_OPT_PORTRAIT,"1");
    m_timerReLogin.Create(m_ctx->loop);
    auto loginArg = new LoginArgs;
    loginArg->ret = -1;
    strncpy(loginArg->app_key, m_ctx->env.voip.appKey, sizeof(loginArg->app_key));
    strncpy(loginArg->app_secret, m_ctx->env.voip.appSecret, sizeof(loginArg->app_secret));
    sprintf(loginArg->device_id, "cmcc-%s-%s", m_ctx->env.manufacturers.deviceType, m_ctx->env.sn.Get());
    emxlogi("user login start, appkey:%s, appsecret:%s, deviceId:%s\n",
           loginArg->app_key,
           loginArg->app_secret,
           loginArg->device_id);
    m_workLogin.Create(&m_ctx->loop, loginArg, [](void *arg) {
        auto loginArg = (LoginArgs *) arg;
        cmcc_rtc_login_params_t voip_login_params;
        voip_login_params.app_key = loginArg->app_key;            // 固话平台产品创建后自动生成的
        voip_login_params.app_secret = loginArg->app_secret;                // 固话平台产品创建后自动生成的
        voip_login_params.device_id = loginArg->device_id;
        loginArg->ret = cmcc_rtc_login(&voip_login_params);
    }, [this](ErrCodeE e, void *arg) {
        auto loginArg = (LoginArgs *) arg;
        if (loginArg->ret == 0 || m_loginSuccess) {
            if (loginArg->ret == 0)
                emxlogi("login by return\n");
            if (m_loginSuccess)
                emxlogi("login by callback\n");
            m_workLogin.Destroy();
            delete loginArg;
        } else {
            emxloge("login error:%d\n", loginArg->ret);
            m_timerReLogin.Start(2 * 60 * 1000, 0, [this]() {
                m_workLogin.Run();
            });
        }
    });
    m_workLogin.Run();
    return ErrCodeE::Success;
}

void VoipBase::Destroy() {
    // 调整停止业务不释放资源，只检验启停标志位
    if (m_resp != nullptr) {
        m_resp->PauseHandle(true);
    }
    m_pauseHandle = true;
}

VoipBase::~VoipBase() {
    cmcc_rtc_fini();
    m_workLogin.Destroy();
    m_timerReLogin.Destroy();
    if (m_resp != nullptr) {
        m_resp->Stop();
    }
}