#pragma once
#include "RTPFrame.h"
#include <functional>
#include <deque>
/*
* ���̴߳���
*/

class RtspDispatcher
{
public:
	RtspDispatcher(std::deque<RTPFrame::Ptr>& _cache) :_gopCache(_cache)
	{}
	typedef std::shared_ptr<RtspDispatcher> Ptr;
	void write(const RTPFrame::Ptr& r)
	{
		if (r->_isKey)
			_gopCache.clear();
		//_gopCache.emplace_back(r);

		auto it = _readers.begin();
		while (it != _readers.end())
		{
			if ((*it)(r))
			{
				it = _readers.erase(it);
				continue;
			}
			++it;
		}
	}

	void addReader(std::function<int(const RTPFrame::Ptr&)>&& _cb)
	{
	/* BEGIN: Modified by wenyi for wifi web , 2021/6/2 */
//		for (auto& ptr : _gopCache)	
//			_cb(ptr->_pktCache);

		_readers.emplace_back(std::move(_cb));
	}
private:
	std::deque<RTPFrame::Ptr> _gopCache;
	
	std::list<std::function<int(const RTPFrame::Ptr &)>> _readers;
};

