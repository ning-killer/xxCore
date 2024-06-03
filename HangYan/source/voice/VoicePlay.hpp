//
// Created by xiong on 2022/7/5.
//

#ifndef EAPIL_VoicePlay_HPP
#define EAPIL_VoicePlay_HPP

#include "cstdint"

namespace Emx {

    class VoicePlay {
    public:
        enum class VoiceE : uint8_t {
            CONFIG_OK,//配置完成，请等待设备上线
            NET_ERR,//网络异常，设备无法连接服务器
            WIFI_FAILED,//网络连接失败，请检查Wi-Fi名称是否正确，并将设备靠近路由器
            WIFI_OK,//连接Wi-Fi成功，正在尝试连接服务器
            WIFI_PSK_ERR,//Wi-Fi网络密码错误，请重新输入
            QR_OK,//二维码扫描成功，正在连接Wi-Fi
            WAIT_CONNECT,//等待连接
            DI,//滴
            DIDI,//滴滴
            MUSIC,//公司设备声音响应
            ENTER_WOMAN,//您已进入监控区域（女）
            ENTER_MAN,//您已进入监控区域（男）
            RESET_REBOOT,//复位成功，摄像机正在重启
            DUDU,//嘟 嘟
            A_CALL,//发起通话
            HANG_UP,//已挂断
            OFFLINE_NO_CALL,//设备不在线，无法发起通话
        };

        static void Play(VoiceE voice);
    };

}

#endif //EAPIL_VoicePlay_HPP
