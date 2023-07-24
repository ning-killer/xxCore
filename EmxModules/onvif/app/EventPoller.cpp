#include "EventPoller.h"
#include "sockutil.h"
#include <iostream>
#include <sys/prctl.h>
#include <sys/syscall.h>

uint64_t nowTime()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

EventPoller::EventPoller()
{
    _threadID = std::this_thread::get_id();

    addEvent(pipe.readFD(), std::bind(&EventPoller::OnPipeEvent, this));
}

EventPoller::~EventPoller()
{}

void EventPoller::runLoop()
{
    _threadID = std::this_thread::get_id();

    {
        char thn[32] = { 0 };
        snprintf(thn, sizeof thn, "poller-%ld", syscall(SYS_gettid));
        prctl(PR_SET_NAME, thn);
    }

    int ret;
    fd_set rf;
    int maxFd = 1;
    timeval tv;
    std::list<std::function<void()>> list_callback;
    while (!_exitFlag)
    {
        auto x = getMinDelay();
        tv.tv_sec = x/1000;
        tv.tv_usec = x % 1000;

        FD_ZERO(&rf);

        for (auto& item : _eventMap)
        {
            FD_SET(item.first, &rf);
            if (maxFd < item.first)
                maxFd = item.first;
        }

        ret = ::select(maxFd + 1, &rf, nullptr, nullptr, &tv);
        if (ret > 0)
        {
            for (auto& item : _eventMap)
            {
                if (FD_ISSET(item.first, &rf))
                {
                    list_callback.emplace_back(item.second);
                }
            }
            for (auto& item : list_callback)
            {
                item();
            }
            list_callback.clear();
        }
    }
}

void EventPoller::addEvent(int fd, std::function<void()>&& _cb)
{
    if (isCurrentThread())
    {
        _eventMap.insert({fd, std::move(_cb)});
        return;
    }

    async([this, fd, _cb]()
          {
              addEvent(fd, std::move(const_cast<std::function<void()>&>(_cb)));
          });
}

void EventPoller::delEvent(int fd, std::function<void(bool)>&& _cb)
{
    if (isCurrentThread())
    {
        _cb(_eventMap.erase(fd));
        return;
    }

    async([this,fd,_cb]()
          {
              delEvent(fd, std::move(const_cast<std::function<void(bool)>&>(_cb)));
          });
}

void EventPoller::async(std::function<void()>&& task)
{
    if (isCurrentThread())
    {
        task();
        return;
    }
    {
        std::lock_guard<std::mutex> lck(_taskLock);
        _taskList.emplace_back(task);
    }

    pipe.write("", 1);
}

void EventPoller::doDelayTask(uint64_t ms, std::function<uint64_t()>&& cb)
{
    auto DeadLine = nowTime() + ms;
    async([DeadLine, cb, this]()
          {
              _delayTask.insert({DeadLine, cb});
          });
}

void EventPoller::OnPipeEvent()
{
    char buf[1024];
    int err = 0;
    do {
        if (pipe.read(buf, 1024) > 0)
            continue;

        err = SockUtil::getErrorCode();
#ifdef _WIN32
        } while (err != WSAEWOULDBLOCK);
#else
    }while (err != EAGAIN);
#endif

    decltype(_taskList) _list_swap;
    {
        std::lock_guard<std::mutex> lck(_taskLock);
        _list_swap.swap(_taskList);
    }
    for (auto& v : _list_swap)
    {
        v();
    }
}

uint64_t EventPoller::getMinDelay()
{
    auto it = _delayTask.begin();
    if (it == _delayTask.end())
        return 1000;

    auto now = nowTime();
    if (it->first > now) {
        //����������δ����
        return it->first - now;
    }
    //ִ���ѵ��ڵ�����ˢ��������ʱ
    return flushDelayTask(now);
}

uint64_t EventPoller::flushDelayTask(uint64_t now_time)
{
    std::multimap<uint64_t, std::function<uint64_t()>> taskCopy;
    taskCopy.swap(_delayTask);

    for (auto it = taskCopy.begin(); it != taskCopy.end() && it->first <= now_time; it = taskCopy.erase(it))
    {
        //expired task
        auto next_delay = (it->second)();
        if (next_delay) {
            //continue
            _delayTask.insert({next_delay + now_time, std::move(it->second)});
        }
    }

    taskCopy.insert(_delayTask.begin(), _delayTask.end());
    taskCopy.swap(_delayTask);

    auto it = _delayTask.begin();
    if (it == _delayTask.end()) {
        return 1000;
    }
    //the first
    return it->first - now_time;
}

bool EventPoller::isCurrentThread()
{
    return _threadID == std::this_thread::get_id();
}

EventPollerPool& EventPollerPool::Instance()
{
    static EventPollerPool::Ptr g_instance(new EventPollerPool);
    static EventPollerPool& g_instance_ref = *g_instance;
    return g_instance_ref;
}

EventPollerPool::EventPollerPool()
{
    auto poolsize = std::thread::hardware_concurrency();
    //auto poolsize = 1;
    std::cout << "cpu cores " << poolsize << std::endl;
    for (size_t i = 0; i < poolsize; i++)
    {
        EventPoller::Ptr item(new EventPoller);
        std::thread th(&EventPoller::runLoop, item.get());
        th.detach();
        m_pollers.emplace_back(item);
    }
}

EventPoller::Ptr EventPollerPool::getPoller()
{
    auto pollerSize = m_pollers.size();
    if (pollerSize == 0)
        return m_pollers[0];

    return m_pollers[(_index++ % pollerSize)];
}

void EventPollerPool::for_each(std::function<void(EventPoller::Ptr&)>&& cb)
{
    for (auto& item : m_pollers)
    {
        cb(item);
    }
}