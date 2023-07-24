/*
 * @Author: xiong
 * @Date: 2022/10/19
*/

#ifndef EAPIL_EnvStrategy_HPP
#define EAPIL_EnvStrategy_HPP

#include "EmxCore.hpp"

namespace Emx {
    class EnvStrategy {
    public:
        ErrCodeE Parse(const Json::Value &json);

        ErrCodeE Serialize(Json::Value &json);

    public:
        struct Speech {
            bool ena;//是否联动语音播报
            int volume;//语音播放音量
            int repeat;//语音播放次数
            char url[EMX_MAX_PATH_SIZE];//语音内容的获取url
            bool playVoiceA;//播放A音频
        } speech;
        struct Light {
            bool ena;//是否联动警灯
            //告警灯亮灯模式
            enum class ModeE : uint8_t {
                AlwaysOn = 0,//常亮
                Blink,//闪烁
            } mode;
            int duration;//告警灯单次亮灯时长，单位：秒
        } light;
        struct Buzz {
            bool ena;//是否联动蜂鸣器
            int duration;//蜂鸣器报警时长，单位：秒
        } buzz;
    };
};

#endif //EAPIL_EnvStrategy_HPP
