/*
* @Author: xiong
* @Date: 2022/9/27
*/

#include "BspUpdateIndicator.hpp"
#include "BspUtils.hpp"

using namespace Emx;

BspUpdateIndicator::BspUpdateIndicator() : Bsp::UpdateIndicator() {
    m_started = false;
    m_idx = 0;
    auto inst = BspUtils::GetInst();
    inst->Lock();
    if (inst->GetCfg().isMember("updateIndicator")) {
        auto &cfg = inst->GetCfg()["updateIndicator"];
        m_invMs = cfg["invMs"].asInt();
        for (auto &item:cfg["allChannels"]) {
            auto gpio = new Gpio(item["num"].asInt(),
                                 (Gpio::DirE) item["dir"].asInt(),
                                 (Gpio::ActiveE) item["active"].asInt(),
                                 (Gpio::ValueE) item["default"].asInt());
            m_allChannels.emplace_back(gpio);
        }
        for (auto &item:cfg["channelsCycleIdx"]) {
            m_channelsCycleIdx.emplace_back(item.asInt());
        }
    } else {
        emxlogw("%s not exist in bsp\n", "updateIndicator");
    }
    inst->UnLock();
}

ErrCodeE BspUpdateIndicator::Start() {
    if (m_started)
        return ErrCodeE::Success;
    m_loop.Init("UpdateIndicator", [this]() { m_timer.Destroy(); });
    m_timer.Create(m_loop);
    m_timer.Start(0, m_invMs, [this]() {
        for (auto &gpio: m_allChannels)
            gpio->SetValue(Gpio::ValueE::Disable);
        if (m_idx < (int) m_channelsCycleIdx.size()) {
            if (m_channelsCycleIdx[m_idx] >= 0)
                m_allChannels[m_channelsCycleIdx[m_idx]]->SetValue(Gpio::ValueE::Enable);
            m_idx++;
            if (m_idx >= (int) m_channelsCycleIdx.size())
                m_idx = 0;
        }
    });
    m_loop.Start();
    m_started = true;
    return ErrCodeE::Success;
}

ErrCodeE BspUpdateIndicator::Stop() {
    if (!m_started)
        return ErrCodeE::Success;
    m_loop.StopAndDeInit();
    m_started = false;
    return ErrCodeE::Success;
}
