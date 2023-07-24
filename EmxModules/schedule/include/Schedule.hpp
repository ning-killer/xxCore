/*
* @Author: xiong
* @Date: 2023/2/7
*/

#ifndef EMX_Schedule_HPP
#define EMX_Schedule_HPP

#include "EmxCore.hpp"

namespace Emx {

    class Schedule {
    public:
        const static int DayInWeek = 7;

        struct Param {
            ErrCodeE Serialize(Json::Value &json);

            void Serialize(std::string &data);

            ErrCodeE Parse(const Json::Value &json);

            int Parse(const char *data, int size);


        public:
            //!记录每一个时间段的起止时间
            struct Section {
                int sSec;//!起始时间相对于当天0点的秒数
                int eSec;//!结束时间相对于当天0点的秒数
            };
            std::vector<Section> section[DayInWeek];
        };

        Schedule() : m_created(false) {}

        using CallBack = std::function<void(const char *name, bool ena)>;

        ErrCodeE Create(EuvLoop &loop, const char *name, const Param &param, CallBack cb);

        void Destroy();

        void Update(const Param &param);

    private:

        struct Point {
            int64_t utc;
            bool ena;
        };

        Point GetNextPoint(int64_t nowUtc);

        void DoSchedule();

        static int64_t GetCurrentWeekMon0TimeUtc(int64_t utc);

    private:
        bool m_created;
        EuvLoop *m_loop;
        std::string m_name;
        Param m_param;
        EuvTimer m_timer;
        CallBack m_cb;
        enum class StatusE : uint8_t {//状态
            None = 0,
            Enable,
            Disable
        } m_currentStatus;

    };
}

#endif //EMX_Schedule_HPP
