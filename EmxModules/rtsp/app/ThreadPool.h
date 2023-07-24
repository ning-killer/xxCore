#pragma once
#include <deque>
#include <thread>
#include <functional>
#include "semaphore0.h"

class ThreadPool :public  std::enable_shared_from_this<ThreadPool>
{
public:
	
	~ThreadPool();

	static ThreadPool& Instance();

	void pushTask(std::function<int()>&& task);
private:
	ThreadPool();

	void start();

	bool getTask(std::function<int()>& task);

	void run();
private:
	int _thread_num = 4;
	std::deque<std::shared_ptr<std::thread>> _thread_group;
	std::deque<std::function<int()>> _tasks;
	std::mutex _taskLock;
	semaphore _sem;
};

