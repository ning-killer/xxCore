//
// Created by xiong on 2022/6/29.
//

#include "BspIrCut2Wire.hpp"
#include "BspUtils.hpp"

using namespace Emx;

BspIrCut2Wire::BspIrCut2Wire() : Bsp::IrCut(), m_created(false) {
    auto inst = BspUtils::GetInst();
    inst->Lock();
    if (inst->GetCfg().isMember("irCut2Wire")) {
        auto &e = inst->GetCfg()["irCut2Wire"];
        m_defaultChnIdx = e["defaultChnIdx"].asInt();
        for (auto &item : e["channels"]) {
            Channel chn = {};
            chn.delayMs = item["delayMs"].asInt();
            auto &wire0 = item["wire0"];
            chn.wire0 = new Gpio(wire0["num"].asInt(),
                                 (Gpio::DirE) wire0["dir"].asInt(),
                                 (Gpio::ActiveE) wire0["active"].asInt(),
                                 (Gpio::ValueE) wire0["default"].asInt());
            auto &wire1 = item["wire1"];
            chn.wire1 = new Gpio(wire1["num"].asInt(),
                                 (Gpio::DirE) wire1["dir"].asInt(),
                                 (Gpio::ActiveE) wire1["active"].asInt(),
                                 (Gpio::ValueE) wire1["default"].asInt());
            m_channels.emplace_back(chn);
        }
        m_created = true;
    }
    inst->UnLock();
}

BspIrCut2Wire::~BspIrCut2Wire() {
    for (auto &chn:m_channels) {
        delete chn.wire0;
        delete chn.wire1;
    }
    m_channels.clear();
}

ErrCodeE BspIrCut2Wire::SwitchToNight(bool ena, int chn) {
    if (!m_created)
        return ErrCodeE::ResNotAvailable;
    if (chn >= (int) m_channels.size())
        return ErrCodeE::IllegalParam;
    auto inst = BspUtils::GetInst();
    int e = 0;
    if (chn < 0) {
        //默认操作所有通道

        inst->Lock();
        for (auto &channel:m_channels) {
            auto wire = ena ? channel.wire0 : channel.wire1;
            e &= (int) wire->SetValue(Gpio::ValueE::Enable);
        }
        inst->UnLock();

        int maxDelayMs = 0;
        for (auto &channel:m_channels)
            maxDelayMs = channel.delayMs > maxDelayMs ? channel.delayMs : maxDelayMs;
        usleep(maxDelayMs * 1000);

        inst->Lock();
        for (auto &channel:m_channels) {
            auto wire = ena ? channel.wire0 : channel.wire1;
            e &= (int) wire->SetValue(Gpio::ValueE::Disable);
        }
        inst->UnLock();

    } else {
        auto wire = ena ? m_channels[chn].wire0 : m_channels[chn].wire1;
        inst->Lock();
        e &= (int) wire->SetValue(Gpio::ValueE::Enable);
        inst->UnLock();

        usleep(m_channels[chn].delayMs * 1000);

        inst->Lock();
        e &= (int) wire->SetValue(Gpio::ValueE::Disable);
        inst->UnLock();
    }
    return e == 0 ? ErrCodeE::Success : ErrCodeE::Failure;
}
