//
// Created by xiong on 2022/6/29.
//
#include "BspBaseGpio.hpp"
#include "BspUtils.hpp"

using namespace Emx;

BspBaseGpio::BspBaseGpio(const char *name) : m_created(false) {
    auto inst = BspUtils::GetInst();
    inst->Lock();
    if (inst->GetCfg().isMember(name)) {
        auto &cfg = inst->GetCfg()[name];
        m_defaultChnIdx = cfg["defaultChnIdx"].asInt();
        for (auto &item:cfg["channels"]) {
            auto gpio = new Gpio(item["num"].asInt(),
                                 (Gpio::DirE) item["dir"].asInt(),
                                 (Gpio::ActiveE) item["active"].asInt(),
                                 (Gpio::ValueE) item["default"].asInt());
            m_channels.emplace_back(gpio);
        }
        m_created = true;
    } else {
        emxlogw("%s not exist in bsp\n", name);
    }
    inst->UnLock();
}

BspBaseGpio::~BspBaseGpio() {
    for (auto &gpio:m_channels)
        delete gpio;
    m_channels.clear();
}

ErrCodeE BspBaseGpio::Set(bool ena, int chn) {
    if (!m_created)
        return ErrCodeE::ResNotAvailable;
    if (chn < 0) chn = m_defaultChnIdx;
    if (chn >= (int) m_channels.size())
        return ErrCodeE::IllegalParam;
    auto inst = BspUtils::GetInst();
    inst->Lock();
    ErrCodeE e = m_channels[chn]->SetValue((Gpio::ValueE) ena);
    inst->UnLock();
    return e;
}

bool BspBaseGpio::Get(int chn) {
    if (!m_created)
        return false;
    if (chn < 0) chn = m_defaultChnIdx;
    if (chn >= (int) m_channels.size())
        return false;
    auto inst = BspUtils::GetInst();
    inst->Lock();
    bool val = m_channels[chn]->GetValue() == Gpio::ValueE::Enable;
    inst->UnLock();
    return val;
}
