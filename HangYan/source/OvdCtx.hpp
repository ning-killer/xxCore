//
// Created by xiong on 2022/7/5.
//

#ifndef EAPIL_OvdCtx_HPP
#define EAPIL_OvdCtx_HPP

#include "EmxBsp.hpp"
#include "Env.hpp"
#include "LedFactory.hpp"
#include "UpdateClient.hpp"
#include "OVD_OpenAPI.h"
#include "VoipBase.hpp"
namespace Emx {
    class VoipBase;
    class OvdCtx {
    public:
        //!@brief 通话模块对外暴露信息结构定义
        struct VoipCtx {
            VoipBase *handle = nullptr; //这里引入VoipBase, 会存在互相依赖
            bool isRunVideoCall = false; //视频通话状态：一般指对讲(和家智话)
            bool isRunVoiceCall = false; //语音通话状态：一般指云广播
        };

        //!@brief 告警模块对外暴露信息结构定义
        struct AlarmCtx {
            bool isLighting = false; //Invasion告警是否在闪灯中。
            bool isPlayVoiceCall = true; //语音通话下是否应该playVoice。
            bool isAudioAlarm = false; //是否处于警觉告警中。
            int alarmVolume = 2; //警戒告警音量
        };

    public:
        Json::Value deviceJsonCfg;
        EuvLoop loop;
        Env env;
        Bsp bsp;
        Led *led;
        char *buffer;
        int bufferSize;
        EuvAsync timeSyncInfo;

    public:
        static const int UpdateSession = 1230001;//Update模块需要提供一个session号作为触发升级主体的标识

        struct Running {
//            bool wifiConfiguring = false;//正在配置wifi，用于判断响应wifi密码错误等
            bool ovdSdkConnected = false;//ovd sdk回调返回的 sdk 联网状态
            UpdateEvent::Event updateEvent = {};//存储当前的升级状态，升级程序写入，供杭研回调读取
            bool nightVisionProcing = false;//当前是否处于夜视状态切换中，用于MD告警屏蔽
            bool sleep;//记录设备的休眠状态
            VoipCtx voip;
            AlarmCtx alarm;
        } running;

        int adecVolume = 2; //spkVolume

#ifdef OVDSDK_APIVER_1_0
        OVDProbeDetector softProbe = {};//杭研下发的软探针的临时配置信息
#endif
    };
}
#endif //EAPIL_OvdCtx_HPP
