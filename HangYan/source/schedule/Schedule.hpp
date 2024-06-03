/*
* @Author: xiong
* @Date: 2022/9/30
*/

#ifndef EAPIL_Schedule_HPP
#define EAPIL_Schedule_HPP

#include "EmxCore.hpp"
#include "OvdCtx.hpp"

namespace Emx {

    class Schedule {
    public:

        Schedule() : m_created(false) {}

        using CallBack = std::function<void(bool ena)>;

        ErrCodeE Create(const char *name, OvdCtx *ctx, EnvSchedule *schedule, CallBack cb);

        void Destroy();

        void UpdateSchedule();

    private:

        struct Point {
            int64_t utc;
            bool active;
        };

        ErrCodeE GetCurrentTimeSegmentByNormal(int64_t nowUtc, Point &sp, Point &ep);

        void DoSchedule();

    private:
        const char *m_name;
        bool m_created;
        OvdCtx *m_ctx;
        EnvSchedule *m_schedule;
        EuvTimer m_timer;
        CallBack m_cb;
        EnvSchedule::Status m_currentStatus;
    };
}

#endif //EAPIL_Schedule_HPP
