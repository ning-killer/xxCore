//
// Created by xiong on 2022/6/24.
//

#include "Env.hpp"

using namespace Emx;

ErrCodeE Env::LoadAll() {
    ErrCodeE e;
    if ((e = devInfo.Load()) != ErrCodeE::Success) {
        emxloge("load devInfo failed\n");
        return e;
    }
    if ((e = sn.Load()) != ErrCodeE::Success) {
        emxloge("load sn failed\n");
        return e;
    }
    if ((e = mac.Load()) != ErrCodeE::Success) {
        emxloge("load mac failed\n");
        return e;
    }
    if ((e = timeZone.Load()) != ErrCodeE::Success) {
        emxloge("load timeZone failed\n");
        return e;
    }
    if ((e = burn.Load()) != ErrCodeE::Success) {
        emxloge("load burn failed\n");
        return e;
    }
    if ((e = modelId.Load()) != ErrCodeE::Success) {
        emxloge("load modelId failed\n");
        return e;
    }
    if ((e = cap.Load()) != ErrCodeE::Success) {
        emxloge("load cap failed\n");
        return e;
    }
    if ((e = cfg.Load()) != ErrCodeE::Success) {
        emxloge("load cfg failed\n");
        return e;
    }
    if ((e = client.Load()) != ErrCodeE::Success) {
        emxloge("load client failed\n");
        return e;
    }
    if ((e = wifi.Load()) != ErrCodeE::Success) {
        emxloge("load wifi failed\n");
        return e;
    }
    if ((e = face.Load()) != ErrCodeE::Success) {
        emxloge("load face failed\n");
        return e;
    }
    if ((e = person.Load()) != ErrCodeE::Success) {
        emxloge("load person failed\n");
        return e;
    }
    if ((e = motion.Load()) != ErrCodeE::Success) {
        emxloge("load motion failed\n");
        return e;
    }
    if ((e = voice.Load()) != ErrCodeE::Success) {
        emxloge("load voice failed\n");
        return e;
    }
    if ((e = sleep.Load()) != ErrCodeE::Success) {
        emxloge("load sleep failed\n");
        return e;
    }
    if ((e = invasion.Load()) != ErrCodeE::Success) {
        emxloge("load invasion failed\n");
        return e;
    }
    if ((e = manufacturers.Load()) != ErrCodeE::Success) {
        emxloge("load manufacturers failed\n");
        return e;
    }
    if ((e = voip.Load()) != ErrCodeE::Success) {
        emxloge("load voip failed\n");
        return e;
    }
    if ((e = batteryCar.Load()) != ErrCodeE::Success) {
        emxloge("load batteryCar failed\n");
        return e;
    }
    if ((e = licensePlate.Load()) != ErrCodeE::Success) {
        emxloge("load licensePlate failed\n");
        return e;
    }
    if ((e = alertarea.Load()) != ErrCodeE::Success) {
        emxloge("load alertarea failed\n");
        return e;
    }
    if ((e = transgression.Load()) != ErrCodeE::Success) {
        emxloge("load transgression failed\n");
        return e;
    }
    if ((e = initMsg.Load()) != ErrCodeE::Success) {
        emxloge("load initMsg failed\n");
        return e;
    }
    if ((e = laneLine.Load()) != ErrCodeE::Success) {
        emxloge("load laneLine failed\n");
        return e;
    }
    if ((e = passengerFlow.Load()) != ErrCodeE::Success) {
        emxloge("load passengerFlow failed\n");
        return e;
    }
    if ((e = regionalPeople.Load()) != ErrCodeE::Success) {
        emxloge("load regionalPeople failed\n");
        return e;
    }
    if ((e = offDuty.Load()) != ErrCodeE::Success) {
        emxloge("load offDuty failed\n");
        return e;
    }
    emxlogi("env load done\n");
    return ErrCodeE::Success;
}
