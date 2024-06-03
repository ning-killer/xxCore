/*
* @Author: xiong
* @Date: 2022/9/30
*/

#include "AutoMaintain.hpp"
#include "OvdUtils.hpp"

using namespace Emx;

ErrCodeE AutoMaintain::Create(OvdCtx *ctx, CallBack cb) {
    if (m_created)
        return ErrCodeE::Success;
    m_ctx = ctx;
    m_cb = std::move(cb);
    m_timer.Create(m_ctx->loop);
    m_timer.Start(CalculateMaintainTime(), 0, [this]() { m_cb(); });
    m_created = true;
    emxlogi("AutoMaintain created\n");
    return ErrCodeE::Success;
}

void AutoMaintain::Destroy() {
    if (!m_created)
        return;
    m_timer.Destroy();
    m_created = false;
    emxlogi("AutoMaintain destroyed\n");
}

void AutoMaintain::UpdateMaintainTime() {
    if (m_created) {
        m_timer.Start(CalculateMaintainTime(), 0, [this]() { m_cb(); });
    }
}

/**
 * 获取当前天的秒数
 * @return
 */
static long long GetDaySecondOffsetByTime(time_t t) {
    tm now_time = {};
    localtime_r(&t, &now_time);
    return (long long) now_time.tm_hour * 60 * 60 + (long long) now_time.tm_min * 60 + now_time.tm_sec;
}

int64_t AutoMaintain::CalculateMaintainTime() {
    // 未来维护周期时刻的UTC时间
    long long futureCycleTime = m_ctx->env.cfg.auto_reboot_cycle + Time::GetS();
    // 未来维护周期时刻相对于当天0点的秒数偏移
    long long futureCycleTimeOffset = GetDaySecondOffsetByTime((time_t) futureCycleTime);
    // 起始时刻相对于当天0点的秒数偏移
    long long futureStartTime = OvdUtils::GetSecondByTimeStr(m_ctx->env.cfg.auto_reboot_start);
    // 结束时刻相对于当天0点的秒数偏移
    long long futureEndTime = OvdUtils::GetSecondByTimeStr(m_ctx->env.cfg.auto_reboot_end);
    if (futureStartTime == -1 || futureEndTime == -1)
        return INT64_MAX;
    if (futureStartTime > futureEndTime) {
        emxloge("illegal time start=%ll > end=%ll\n", futureStartTime, futureEndTime);
        return INT64_MAX;
    }
    long long randomS, randomE;
    if (futureCycleTimeOffset <= futureStartTime) {
        // C <= S < E
        randomS = futureStartTime;
        randomE = futureEndTime;
    } else if (futureEndTime <= futureCycleTimeOffset) {
        // S < E <= C ,第二天的对应时刻再执行
        randomS = futureStartTime;
        randomE = futureEndTime;
    } else {
        // S < C < E
        randomS = futureCycleTimeOffset;
        randomE = futureEndTime;
    }
    auto maintainTimeS = (long long) m_ctx->env.cfg.auto_reboot_cycle - futureCycleTimeOffset +
                         Misc::GetRandom((int) randomS, (int) randomE);
    emxlogi("maintain device after %lld second\n", maintainTimeS);
    if (maintainTimeS < 0)
        maintainTimeS = 0;
    return (int64_t) maintainTimeS * 1000;
}
