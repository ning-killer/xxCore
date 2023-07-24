#pragma once
#include <memory>
#include <mutex>
#include <list>

template<typename C>
class ResourcePool :public std::enable_shared_from_this<ResourcePool<C> >
{
public:
	ResourcePool()
	{}

	~ResourcePool()
	{
		std::lock_guard<std::mutex> lck(_objLock);
		for (auto item : _objs)
			delete item;
	}

	void setSize(unsigned short sz)
	{
		_poolsize = sz;
	}

	std::shared_ptr<C> obtain()
	{
		C* ptr;
		std::lock_guard<std::mutex> lck(_objLock);
		if (_objs.size() == 0)
			ptr = new C();
		else
		{
			ptr = _objs.front();
			_objs.pop_front();
		}
		
		std::weak_ptr<ResourcePool<C>> weakRef = this->shared_from_this();
		return std::shared_ptr<C>(ptr, [weakRef](C* obj)
		{
			auto v = weakRef.lock();
			if (v)
				v->recycle(obj);
			else
				delete obj;
		});
	}
private:
	void recycle(C* ptr)
	{
		std::lock_guard<std::mutex> lck(_objLock);
		if (_objs.size() >= _poolsize)
			delete ptr;
		else
			_objs.emplace_back(ptr);
	}
private:
	std::list<C*> _objs;
	std::mutex _objLock;
	unsigned short _poolsize = 8;
};

