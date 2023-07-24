/*
* @Author: xiong
* @Date: 2023/2/7
*/

#include "Schedule.hpp"

using namespace Emx;


//"schedule": {
//"Mon": [["00:00", "00:01"], ["00:02", "02:01"]],
//"Tue": [],
//"Wed": [["00:00", "00:01"], ["00:02", "02:01"]],
//"Thu": [["00:00", "00:01"], ["00:02", "02:01"]],
//"Fri": [["00:00", "00:01"], ["00:02", "02:01"]],
//"Sat": [["00:00", "00:01"], ["00:02", "02:01"]],
//"Sun": [["00:00", "00:01"], ["00:02", "02:01"]]
//}

ErrCodeE Schedule::Param::Parse(const Json::Value &json) {
    try {
        for (auto s : section)
            s.clear();
        const char *week[DayInWeek] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
        for (int i = 0; i < DayInWeek; i++) {
            auto &day = json[week[i]];
            for (auto &item : day) {
                Section s = {};
                int hour, min;
                sscanf(item[0].asCString(), "%02d:%02d", &hour, &min);
                s.sSec = hour * 60 * 60 + min * 60;
                sscanf(item[1].asCString(), "%02d:%02d", &hour, &min);
                s.eSec = hour * 60 * 60 + min * 60;
                section[i].emplace_back(s);
            }
        }
        return ErrCodeE::Success;
    } catch (std::exception &e) {
        emxlogc("parse failed\n");
        return ErrCodeE::ParseFailed;
    }
}

int Schedule::Param::Parse(const char *data, int size) {
    for (auto s : section)
        s.clear();
    int off = 0;
    for (auto &day : section) {
        int num;
        memcpy(&num, data + off, sizeof(num));
        off += sizeof(num);
        day.clear();
        for (int i = 0; i < num; i++) {
            Section sec = {};
            memcpy(&sec, data + off, sizeof(sec));
            off += sizeof(sec);
            day.emplace_back(sec);
        }
    }
    return off;
}

ErrCodeE Schedule::Param::Serialize(Json::Value &json) {
    try {
        const char *week[DayInWeek] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
        for (int i = 0; i < DayInWeek; i++) {
            auto &day = json[week[i]];
            day.resize(0);
            for (auto &item : section[i]) {
                char startEndStr[8];
                int hour, min;
                Json::Value se;
                hour = item.sSec / 60 / 60;
                min = (item.eSec - (hour * 60 * 60)) / 60;
                snprintf(startEndStr, sizeof(startEndStr), "%02d:%02d", hour, min);
                se.append(startEndStr);
                hour = item.sSec / 60 / 60;
                min = (item.eSec - (hour * 60 * 60)) / 60;
                snprintf(startEndStr, sizeof(startEndStr), "%02d:%02d", hour, min);
                se.append(startEndStr);
                day.append(se);
            }
        }
        return ErrCodeE::Success;
    } catch (std::exception &e) {
        emxlogc("serialize failed\n");
        return ErrCodeE::Failure;
    }
}

void Schedule::Param::Serialize(std::string &data) {
    for (auto &day : section) {
        int num = (int) day.size();
        data.append((char *) &num, sizeof(num));
        for (auto &sec:day) {
            data.append((char *) &sec, sizeof(sec));
        }
    }
}


ErrCodeE Schedule::Create(EuvLoop &loop, const char *name, const Schedule::Param &param, Schedule::CallBack cb) {
    if (m_created)
        return ErrCodeE::Success;
    m_name = name;
    m_cb = std::move(cb);
    m_loop = &loop;
    m_timer.Create(*m_loop);
    m_created = true;
    Update(param);
    emxlogd("Schedule %s created\n", name);
    return ErrCodeE::Success;
}

void Schedule::Destroy() {
    if (!m_created)
        return;
    m_timer.Destroy();
    m_created = false;
    emxlogd("Schedule %s destroyed\n", m_name.c_str());
}

void Schedule::Update(const Schedule::Param &param) {
    if (m_created) {
        m_currentStatus = StatusE::None;
        m_param = param;
        DoSchedule();
    }
}

void Schedule::DoSchedule() {
    int64_t nowUtc = Time::GetS();//当前UTC时间
    StatusE newStatus = StatusE::None;

    //获取下一个时间点
    Point nextPoint = GetNextPoint(nowUtc);
    emxlogd("%s now=%u next=%u\n", m_name.c_str(), (uint32_t) nowUtc, (uint32_t) nextPoint.utc);
    if (nextPoint.utc == -1) {
        //如果没有获取到下一个时间点，后面没有待执行的计划了
        newStatus = StatusE::Disable;
    } else {
        //获取到下一个时间点，当前状态与下一个时间点的状态相反
        newStatus = nextPoint.ena ? StatusE::Disable : StatusE::Enable;
    }

    //根据前面确定的最新状态与下个时间点，执行回调和延时计划
    if (newStatus != m_currentStatus) {
        //如果当前状态并没有发生变化则不执行回调
        //(考虑到有可能延时提前几秒结束，则并没有真正到达下个时间点，这时会再次延时一段时间)
        emxlogi("%s switch from %d to %d\n", m_name.c_str(), m_currentStatus, newStatus);
        m_currentStatus = newStatus;
        m_cb(m_name.c_str(), m_currentStatus == StatusE::Enable);
    }
    if (nextPoint.utc > 0) {
        //后续还有计划
        int64_t delay = nextPoint.utc - nowUtc;
        //这里做一下容错处理(并且防止接近时间点时不停的延时0s)
        if (delay <= 0) {
            delay = 1;
        }
        emxlogi("%s next point utc %lld delay %llds\n", m_name.c_str(), nextPoint.utc, delay);
        m_timer.Start(delay * 1000, 0, [this]() { DoSchedule(); });
    } else {
        //后续无计划
        emxlogi("%s no time point plan to proc\n", m_name.c_str());
        m_timer.Stop();
    }
}

//获取本周一0点的UTC时间
int64_t Schedule::GetCurrentWeekMon0TimeUtc(int64_t utc) {
    auto t = (time_t) utc;
    tm tm = {};
    localtime_r(&t, &tm);
    int wday = tm.tm_wday;
    tm.tm_sec = 0;
    tm.tm_min = 0;
    tm.tm_hour = 0;
    //当天0点的UTC时间
    auto cur0TimeUtc = mktime(&tm);
    cur0TimeUtc -= (wday == 0 ? 6 : wday - 1) * 60 * 60 * 24;//减去当周的偏移时间
    return cur0TimeUtc;
}

Schedule::Point Schedule::GetNextPoint(int64_t nowUtc) {
    //计算出本周一0点的UTC时间
    int64_t preWeekMon0TimeUtc = GetCurrentWeekMon0TimeUtc(nowUtc);
    //构造起始和结束时间点UTC列表
    std::vector<Point> pointArray;
    Point pointS = {0, true}, lastPointE = {-1, false};
    //扩展的week，扩展了下一周，便于后续处理周日的下一个point问题
    for (int i = 0; i < DayInWeek * 2; i++) {
        int64_t offUTC = i * 60 * 60 * 24;
        for (auto &section : m_param.section[i % DayInWeek]) {
            pointS.utc = preWeekMon0TimeUtc + offUTC + section.sSec;
            //判断如果当前时间段的起始时间与前一个结束时间是相同的，也就是时间段连续
            //则合并这个时间段，做法是只更新lastPointE
            if (lastPointE.utc != pointS.utc) {
                if (lastPointE.utc != -1)
                    pointArray.emplace_back(lastPointE);
                pointArray.emplace_back(pointS);
            }
            lastPointE.utc = preWeekMon0TimeUtc + offUTC + section.eSec;
        }
    }
    //如果存在，则填入最后一个PointE
    if (lastPointE.utc != -1)
        pointArray.emplace_back(lastPointE);

    //找到大于nowUtc的最小时间点
    Point point = {-1, false};
    for (int i = 0; i < (int) pointArray.size() - 1; i++) {
        if (nowUtc >= pointArray[i].utc && nowUtc < pointArray[i + 1].utc) {
            point = pointArray[i + 1];
        }
    }
    return point;
}