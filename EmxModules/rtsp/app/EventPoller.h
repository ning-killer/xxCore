#pragma once
#include <memory>
#include <list>
#include <map>
#include <thread>
#include <mutex>
#include <functional>
#include <vector>
#include "PipeWrap.h"

class EventPollerPool;

class EventPoller :public std::enable_shared_from_this<EventPoller>
{
public:
    typedef std::shared_ptr<EventPoller> Ptr;
    ~EventPoller();

    void runLoop();

    void addEvent(int fd, std::function<void()>&& _cb);

    void delEvent(int fd, std::function<void(bool)>&& _cb);

    void async(std::function<void()>&& task);

    void doDelayTask(uint64_t ms, std::function<uint64_t()>&& cb);
private:
    bool isCurrentThread();

    void OnPipeEvent();

    EventPoller();
    /*timer*/
    uint64_t getMinDelay();

    uint64_t flushDelayTask(uint64_t now_time);
private:
    friend EventPollerPool;

    std::map<int, std::function<void()>> _eventMap;
    bool _exitFlag = false;
    std::thread::id _threadID;
    PipeWrap pipe;
    //int pipeFd[2];
    std::mutex _taskLock;
    std::list<std::function<void()>> _taskList;

    std::multimap<uint64_t, std::function<uint64_t()>> _delayTask;
};

class EventPollerPool : public std::enable_shared_from_this<EventPollerPool>
{
public:
    typedef std::shared_ptr<EventPollerPool> Ptr;
    ~EventPollerPool() {};
    static EventPollerPool& Instance();

    EventPoller::Ptr getPoller();

    void for_each(std::function<void(EventPoller::Ptr&)>&& cb);
private:
    EventPollerPool();
private:
    std::vector<EventPoller::Ptr> m_pollers;
    int _index = 0;
};