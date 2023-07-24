//
// Created by xiong on 2022/5/25.
//

#ifndef EMX_NetInner_HPP
#define EMX_NetInner_HPP

#define NIGHT_VISION_MSG_ID 0x00080000

#define NIGHT_VISION_RESP_ID NIGHT_VISION_MSG_ID

#define NIGHT_VISION_MODULE 1

enum class NightVisionMethodE : unsigned char {
    GetParam,
    SetParam,
    GetParamTemp,
    SetParamTemp,
    SetModeTemp,
    GetStatus,
};

#define NightVisionTopicId NIGHT_VISION_MSG_ID

#endif //EMX_NetInner_HPP

