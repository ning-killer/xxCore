#include "ThreadPool.h"
#include <iostream>

ThreadPool::ThreadPool()
{
	start();
}

ThreadPool::~ThreadPool()
{
	_sem.post(_thread_num);
	auto it = _thread_group.begin();
	while (it != _thread_group.end())
	{
		(*it)->join();
		++it;
	}
}

ThreadPool& ThreadPool::Instance()
{
	static std::shared_ptr<ThreadPool> g_instance(new ThreadPool);
	static ThreadPool& g_instance_ref = *g_instance;
	return g_instance_ref;
}
void ThreadPool::start()
{
	for (int i = 0; i < _thread_num; i++)
	{
		std::shared_ptr<std::thread> th(new std::thread(std::bind(&ThreadPool::run, this)));
		_thread_group.emplace_back(std::move(th));
	}
}

void ThreadPool::pushTask(std::function<int()>&& task)
{
	{
		std::lock_guard<std::mutex> lck(_taskLock);
		_tasks.emplace_back(task);
	}
	_sem.post();
}

bool ThreadPool::getTask(std::function<int()>& task)
{
	_sem.wait();
    std::lock_guard<std::mutex> lck(_taskLock);
	if (_tasks.empty())
		return false;
	task = std::move(_tasks.front());
	_tasks.pop_front();

	return true;
}

void ThreadPool::run()
{
	std::cout << "start thread " << std::this_thread::get_id() << std::endl;
	std::function<int()> t;
	while (true)
	{
		if (!getTask(t))
			break;
		t();
		t = nullptr;
	}
	std::cout << "stop thread " << std::this_thread::get_id() << std::endl;
}