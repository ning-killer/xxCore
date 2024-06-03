//
// Created by xiong on 2022/7/5.
//

#include "VoicePlay.hpp"
#include "EmxMedia.hpp"
#include "EmxCore.hpp"

using namespace Emx;

void VoicePlay::Play(VoicePlay::VoiceE voice) {
#ifndef EAPIL_SPARE
    const char *name;
    switch (voice) {
        case VoiceE::CONFIG_OK:
            name = "config_ok_wait_server_online";
            break;
        case VoiceE::NET_ERR:
            name = "net_abnormal_device_cannot_connect_server";
            break;
        case VoiceE::WIFI_FAILED:
            name = "net_connect_failed_check_wifiname_device_near_router";
            break;
        case VoiceE::WIFI_OK:
            name = "connect_wifi_ok_try_connect_server";
            break;
        case VoiceE::WIFI_PSK_ERR:
            name = "wifi_pwderror_reinput";
            break;
        case VoiceE::QR_OK:
            name = "qrscanok_connecting_wifi";
            break;
        case VoiceE::WAIT_CONNECT:
            name = "wait_connect";
            break;
        case VoiceE::DI:
            name = "di";
            break;
        case VoiceE::DIDI:
            name = "didi";
            break;
        case VoiceE::MUSIC:
            name = "music";
            break;
        case VoiceE::ENTER_WOMAN:
            name = "enter_moniter_area_woman";
            break;
        case VoiceE::ENTER_MAN:
            name = "enter_moniter_area_man";
            break;
        case VoiceE::RESET_REBOOT:
            name = "reset_ok_rebooting";
            break;
        case VoiceE::DUDU:
            name = "dudu";
            break;
        case VoiceE::A_CALL:
            name = "a_call";
            break;
        case VoiceE::HANG_UP:
            name = "hang_up";
            break;
        case VoiceE::OFFLINE_NO_CALL:
            name = "offline_no_call";
            break;
        default:
            return;
    }
    char path[EMX_MAX_PATH_SIZE] = {};
    sprintf(path, "%s/audio/%s.aac", Misc::GetAppConfigsDir(), name);
    MediaClientAdecStreamSync::PlayFile(0, path, 1000);
#endif
}
