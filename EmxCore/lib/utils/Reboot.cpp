//
// Created by xiong on 2022/6/9.
//

#include "core/utils/Reboot.hpp"
#include <sys/reboot.h>

//Reboot Msg ID
#define REBOOT_MSG_ID 0x00050000

using namespace Emx;

void Reboot::DoReboot(int delayS) {
    MsgTopicPublisherSync::Publish(REBOOT_MSG_ID, nullptr, 0);
    if (delayS) {
        sleep(delayS);
    }
    sync();
    sleep(1);
    reboot(RB_AUTOBOOT);
    while (true) {
        sleep(1);
        printf("waiting for reboot\n");
    }
}

void RebootMonitor::Create(EuvLoop &loop, char *buffer, int bufferSize, OnReboot cb) {
    m_cb = std::move(cb);
    MsgTopicSubscriberAsync::Config cfg = {};
    cfg.loop = &loop;
    cfg.sendByPack = true;
    cfg.maxSend = -1;
    cfg.topicArray.push_back(REBOOT_MSG_ID);
    cfg.buffer = buffer;
    cfg.size = bufferSize;
    cfg.OnRecvMsg = [this](uint32_t topic, const char *data, int32_t size) { m_cb(); };
    m_sub.CreateMsgTopicSubscriberAsync(cfg);
}

void RebootMonitor::Destroy() {
    m_sub.DestroyMsgTopicSubscriberAsync();
}
