/*
* @Author: xiong
* @Date: 2022/9/30
*/

#include "Schedule.hpp"
#include "OvdUtils.hpp"

using namespace Emx;

ErrCodeE Schedule::Create(const char *name, OvdCtx *ctx, EnvSchedule *schedule, CallBack cb) {
    if (m_created)
        return ErrCodeE::Success;
    m_name = name;
    m_ctx = ctx;
    m_cb = std::move(cb);
    m_schedule = schedule;
    m_timer.Create(m_ctx->loop);
    m_created = true;
    UpdateSchedule();
    emxlogi("Schedule %s created\n", name);
    return ErrCodeE::Success;
}

void Schedule::Destroy() {
    if (!m_created)
        return;
    m_timer.Destroy();
    m_created = false;
    emxlogi("Schedule %s destroyed\n", m_name);
}

void Schedule::UpdateSchedule() {
    if (m_created) {
        m_currentStatus = EnvSchedule::Status::None;
        DoSchedule();
    }
}

//1920-00-00 19:00:00
static int64_t GetUtcByString(const char *str) {
    tm tmp_time = {};
    strptime(str, "%Y-%m-%d %H:%M:%S", &tmp_time);
    return mktime(&tmp_time);
}

void Schedule::DoSchedule() {
    int64_t nowUtc = Time::GetS();//当前UTC时间
    int64_t manualUtc = GetUtcByString(m_schedule->manualTime);
    int64_t nextPoint = -1;
    EnvSchedule::Status newStatus = EnvSchedule::Status::None;

    //首先确认当前所处的时间段模式
    if (m_schedule->once) {
        //当前处于一次性计划生效中
        //这里会同时确认出当前状态与计算出下一个开启或关闭的时间点
        int64_t utcS = GetUtcByString(m_schedule->onceStartTime);//一次性时间段的起始utc时间
        int64_t utcE = GetUtcByString(m_schedule->onceEndTime);//一次性时间段的结束utc时间

        emxlogi("newStarts: %d,nowUtc: %lld,utcS: %lld,utcE: %lld,manualUtc: %lld\n",newStatus,nowUtc,utcE,utcS,manualUtc);
        //检测手动生效时间与当前时间是否在同一区间
        if ((nowUtc < utcS && manualUtc < utcS) ||
            (nowUtc >= utcE && manualUtc >= utcE) ||
            (nowUtc >= utcS && nowUtc < utcE && manualUtc >= utcS && manualUtc < utcE && nowUtc == manualUtc)) {
            //在同一区间则优先使用手动模式
            newStatus = m_schedule->manual ? EnvSchedule::Status::On : EnvSchedule::Status::Off;
            emxlogi("one test1,newStarts: %d\n",newStatus);
            if (nowUtc < utcS)
                nextPoint = utcS;
            else if (nowUtc >= utcE)
                nextPoint = -1;//后面没有待执行的计划了
            else
                nextPoint = utcE;
        } else {
            //不在同一区间
            //如果当前不在一次性区间中，则依然使用手动模式配置
            //如果当前在一次性区间中，则状态是开
            emxlogi("not   !!! newStarts: %d,nowUtc: %lld,utcS: %lld,utcE: %lld,manualUtc: %lld\n",newStatus,nowUtc,utcE,utcS,manualUtc);
            if (nowUtc < utcS) {
                nextPoint = utcS;
                newStatus = m_schedule->manual ? EnvSchedule::Status::On : EnvSchedule::Status::Off;
                emxlogi("one test2,newStarts: %d\n",newStatus);
            } else if (nowUtc >= utcE) {
                nextPoint = -1;//后面没有待执行的计划了
                newStatus = m_schedule->manual ? EnvSchedule::Status::On : EnvSchedule::Status::Off;
                emxlogi("one test3,newStarts: %d\n",newStatus);
            } else {
                nextPoint = utcE;
                newStatus = EnvSchedule::Status::On;
                emxlogi("one test4,newStarts: %d\n",newStatus);
            }
        }
    } else {
        //当前处于常规计划生效中
        Point sp = {}, ep = {};


        //获取到当前所处时间段的左右端点
        GetCurrentTimeSegmentByNormal(nowUtc, sp, ep);
        printf("%s:%d:nowUtc=%u sp.utc=%u ep.utc=%u\n", __FUNCTION__, __LINE__,
               (uint32_t) nowUtc, (uint32_t) sp.utc, (uint32_t) ep.utc);
        if (sp.utc == -1 || ep.utc == -1) {
            printf("test1\n");
            //如果没有获取到区间，则使用手动模式
            newStatus = m_schedule->manual ? EnvSchedule::Status::On : EnvSchedule::Status::Off;
            nextPoint = -1;//后面没有待执行的计划了
        } else {
                emxlogi("newStarts = %d manualUtc = %lld\n",newStatus,manualUtc);
                //如果获取到区间，则先判断手动模式是否处于这个区间

                /*
                if (manualUtc >= sp.utc && manualUtc < ep.utc) {
                            
                    //如果手动模式也处于这个区间内，则以手动模式优先
                    newStatus = m_schedule->manual ? EnvSchedule::Status::On : EnvSchedule::Status::Off;
                    emxlogi("test3,newStarts: %d\n",newStatus);
                } else {
                    //否则，判断当前区间的实际状态
                        
                    newStatus = sp.active ? EnvSchedule::Status::On : EnvSchedule::Status::Off;
                    emxlogi("test4,newStarts: %d\n",newStatus);
                }
                */

                if(manualUtc == nowUtc){
                    newStatus = m_schedule->manual ? EnvSchedule::Status::On : EnvSchedule::Status::Off;
                    emxlogi("test2,newStarts: %d\n",newStatus);
                }else if((sp.utc < manualUtc) && (manualUtc < ep.utc) && (sp.utc < nowUtc) && (nowUtc < ep.utc)){
                    newStatus = EnvSchedule::Status::Off;
                    emxlogi("test3,newStarts: %d\n",newStatus);
                    if((ep.utc - sp.utc) < 24 * 60 *60){
                        newStatus = EnvSchedule::Status::On;
                        emxlogi("test4,newStarts: %d\n",newStatus);
                    }
                }else if((sp.utc < nowUtc) && (nowUtc < ep.utc) && (manualUtc == -1)){
                    newStatus = EnvSchedule::Status::On;
                    emxlogi("test5,newStarts: %d\n",newStatus);
                }else{
                    newStatus = sp.active ? EnvSchedule::Status::On : EnvSchedule::Status::Off;
                    emxlogi("test7,newStarts: %d\n",newStatus);
                }

                nextPoint = ep.utc;
                
        }
    }

    //根据前面确定的最新状态与下个时间点，执行回调和延时计划
    if (newStatus != m_currentStatus) {
        //如果当前状态并没有发生变化则不执行回调
        //(考虑到有可能延时提前几秒结束，则并没有真正到达下个时间点，这时会再次延时一段时间)
        emxlogi("%s switch from %d to %d\n", m_name, m_currentStatus, newStatus);
        m_currentStatus = newStatus;
        m_cb(newStatus == EnvSchedule::Status::On);
    }
    if (nextPoint > 0) {
        //后续还有计划
        int64_t delay = nextPoint - nowUtc;
        //这里做一下容错处理(并且防止接近时间点时不停的延时0s)
        if (delay <= 0) {
            delay = 1;
        }
        emxlogi("%s next point utc %lld delay %llds\n", m_name, nextPoint, delay);
        m_timer.Start(delay * 1000, 0, [this]() { DoSchedule(); });
    } else {
        //后续无计划
        emxlogi("%s no time point plan to proc\n", m_name);
        m_timer.Stop();
    }
}

//获取上周一0点的UTC时间
static int64_t GetLastMonDay0TimeUtc(int64_t utc) {
    auto t = (time_t) utc;
    tm tm = {};
    localtime_r(&t, &tm);
    int wday = tm.tm_wday;
    tm.tm_sec = 0;
    tm.tm_min = 0;
    tm.tm_hour = 0;
    //当天0点的UTC时间
    auto cur0TimeUtc = mktime(&tm);
    cur0TimeUtc -= 7 * 60 * 60 * 24;//首先减去一周的时间
    cur0TimeUtc -= (wday == 0 ? 6 : wday - 1) * 60 * 60 * 24;//再减去当周的偏移时间
    return cur0TimeUtc;
}

ErrCodeE Schedule::GetCurrentTimeSegmentByNormal(int64_t nowUtc, Schedule::Point &sp, Schedule::Point &ep) {
    //扩展的week，扩展了上一周和下一周，便于后续处理隔天或隔周point问题
    bool extWeek[7 * 3] = {};
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 7; j++)
            extWeek[i * 7 + j] = m_schedule->repeat[j];

    //计算出上周一0点的UTC时间
    int64_t lastMonDay0TimeUtc = GetLastMonDay0TimeUtc(nowUtc);
    int64_t startTimeDaySecond = OvdUtils::GetSecondByTimeStr(m_schedule->startTime);
    int64_t endTimeDaySecond = OvdUtils::GetSecondByTimeStr(m_schedule->endTime);
    //构造起始和结束时间点UTC列表
    std::vector<Point> pointArray;
    for (int i = 0; i < (int) (sizeof(extWeek) / sizeof(bool)); i++) {
        Point pointS = {0, true}, pointE = {0, false};
        int64_t offUTC = i * 60 * 60 * 24;
        if (extWeek[i]) {
            if (startTimeDaySecond < endTimeDaySecond) {
                pointS.utc = lastMonDay0TimeUtc + offUTC + startTimeDaySecond;
                pointE.utc = lastMonDay0TimeUtc + offUTC + endTimeDaySecond;
            } else {
                pointS.utc = lastMonDay0TimeUtc + offUTC + startTimeDaySecond;
                pointE.utc = lastMonDay0TimeUtc + offUTC + (60 * 60 * 24) + endTimeDaySecond;
            }
            pointArray.push_back(pointS);
            pointArray.push_back(pointE);
        }
    }
    //找到小于nowUtc最大的时间点，以及大于nowUtc的最小时间点
    Point pointS = {-1, false}, pointE = {INT64_MAX, false};
    for (auto &point : pointArray) {
        if (nowUtc >= point.utc && pointS.utc < point.utc) {
            pointS = point;
        } else if (nowUtc < point.utc && pointE.utc > point.utc) {
            pointE = point;
        }
    }
    sp = pointS;
    ep = pointE;
    return ErrCodeE::Success;
}
