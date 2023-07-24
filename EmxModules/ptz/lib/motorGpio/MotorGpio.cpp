/*
* @Author: xiong
* @Date: 2023/4/17
*/

#include "MotorGpio.hpp"
#include <sys/ioctl.h>

using namespace Emx;

ErrCodeE MotorGpio::Init() {
    if (m_fd != -1) {
        emxlogw("already inited\n");
        return ErrCodeE::Success;
    }
    char path[EMX_MAX_PATH_SIZE] = {};
    snprintf(path, sizeof(path), "%s/ptz/motorGpio.json", Misc::GetAppConfigsDir());
    Json::Value root;
    EasyJson::Load(path, root);
    // 加载电机驱动初始化配置参数
    m_arg.timeDivision = (int) root["timeDivision"].asBool();
    auto &chnArray = root["chn"];
    m_arg.chnNum = (int) chnArray.size();
    for (int i = 0; i < m_arg.chnNum; i++) {
        auto &chnJson = chnArray[i];
        auto &chnArg = m_arg.arg[i];

        chnArg.unlimited = (int) chnJson["unlimited"].asBool();
        for (auto &location : chnArg.location) {
            location.pinNum = -1;
        }
        for (int j = 0; j < (int) chnJson["locationSignal"].size(); j++) {
            chnArg.location[j].pinNum = chnJson["locationSignal"][j]["pinNum"].asInt();
            chnArg.location[j].active = chnJson["locationSignal"][j]["active"].asInt();
            chnArg.location[j].step = chnJson["locationSignal"][j]["step"].asInt();
        }
        chnArg.pinEnaNum = chnJson["pinEna"]["num"].asInt();
        chnArg.pinEnaActive = chnJson["pinEna"]["active"].asInt();
        chnArg.stepMax = chnJson["stepMax"].asInt();
        chnArg.stepDef = chnJson["stepDef"].asInt();
        chnArg.speedDef = chnJson["speedDef"].asInt();
        for (int j = 0; j < MOTOR_GPIO_PIN_NUM; j++)
            chnArg.pinNum[j] = chnJson["pinNum"][j].asInt();
        chnArg.stepOrderNum = chnJson["stepOrder"].size();
        for (int j = 0; j < chnArg.stepOrderNum; j++)
            for (int k = 0; k < MOTOR_GPIO_PIN_NUM; k++)
                chnArg.stepOrder[j][k] = chnJson["stepOrder"][j][k].asInt();
    }
    // 各个类型对应的电机通道
    for (auto &m : root["map"]) {
        Config cfg = {};
        cfg.type = (Ptz::TypeE) m["type"].asInt();
        cfg.chn = m["chn"].asInt();
        cfg.position.min = m["position"]["min"].asInt();
        cfg.position.max = m["position"]["max"].asInt();
        cfg.speed.min = m["speed"]["min"].asInt();
        cfg.speed.max = m["speed"]["max"].asInt();
        m_map[cfg.type] = cfg;
    }

    m_fd = open("/dev/motor_gpio", O_RDWR);
    if (m_fd <= 0) {
        emxloge("open /dev/motor_gpio failed\n");
        return ErrCodeE::OpenFailed;
    }
    // 初始化驱动
    auto ret = ioctl(m_fd, MOTOR_GPIO_IOCTL_INIT, &m_arg);
    return ret == 0 ? ErrCodeE::Success : ErrCodeE::Failure;
}

void MotorGpio::DeInit() {
    if (m_fd != -1) {
        close(m_fd);
    }
}

ErrCodeE MotorGpio::MoveStep(Ptz::TypeE type, bool block, int step, int speed) {
    if (m_map.count(type) == 0) {
        emxloge("type %d not found\n", (int) type);
        return ErrCodeE::ResNotExist;
    }
    auto &cfg = m_map[type];
    auto &arg = m_arg.arg[cfg.chn];
    MotorArgSet set = {};
    set.chn = cfg.chn;
    set.block = block;
    set.offset = step;
    set.speed = speed == 0 ? arg.speedDef : speed;
    auto ret = ioctl(m_fd, MOTOR_GPIO_IOCTL_CHN_SET, &set);
    return ret == 0 ? ErrCodeE::Success : ErrCodeE::Failure;
}

ErrCodeE MotorGpio::GetStat(Ptz::TypeE type, bool block, Ptz::Status &status) {
    if (m_map.count(type) == 0) {
        emxloge("type %d not found\n", (int) type);
        return ErrCodeE::ResNotExist;
    }
    auto &cfg = m_map[type];
    auto &arg = m_arg.arg[cfg.chn];
    MotorArgGet get = {};
    get.chn = cfg.chn;
    get.block = block;
    auto ret = ioctl(m_fd, MOTOR_GPIO_IOCTL_CHN_GET, &get);
    if (ret != 0) {
        emxloge("get ptz stat failed\n");
        return ErrCodeE::Failure;
    }
    status.step.min = 0;
    status.step.max = arg.stepMax;
    status.step.current = get.step;
    status.speed.min = cfg.speed.min;
    status.speed.max = cfg.speed.max;
    status.speed.current = get.speed;
    return ErrCodeE::Success;
}

ErrCodeE MotorGpio::Reset() {
    auto ret = ioctl(m_fd, MOTOR_GPIO_IOCTL_RESET);
    if (ret != 0) {
        emxloge("reset ptz failed\n");
        return ErrCodeE::Failure;
    }
    return ErrCodeE::Success;
}

bool MotorGpio::IsBusy(Ptz::TypeE type) {
    if (m_map.count(type) == 0) {
        emxloge("type %d not found\n", (int) type);
        return false;
    }
    return ioctl(m_fd, MOTOR_GPIO_IOCTL_CHN_IS_BUSY, m_map[type].chn) == 1;
}
