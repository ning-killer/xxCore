//
// Created by xiong on 2022/6/13.
//

#ifndef EMX_SDCardInner_HPP
#define EMX_SDCardInner_HPP
#define SDCARD_MSG_ID 0x00060000

#define SDCARD_RESP_ID SDCARD_MSG_ID

#define SDCARD_MODULE 1

enum class SDCardMethodE : unsigned char {
    GetChnNum,
    Mount,
    UnMount,
    Format,
    GetInfo,
    Check,
};

#define SDCARD_TOPIC_ID SDCARD_MSG_ID

#endif //EMX_SDCardInner_HPP

