#pragma once
#include "Buffer.h"
#include <list>

class RTPFrame
{
public:
	typedef std::shared_ptr<RTPFrame> Ptr;
	RTPFrame()
	{}
	~RTPFrame()
	{}
	
	//void add(BufferRaw::Ptr& p)
	//{
	//	_pktCache.emplace_back(std::move(p));
	//}

	void reset()
	{
		_pktCache.clear();
	}
	unsigned char _interval = 0;
	bool _isKey = false;

	std::list<BufferRaw::Ptr> _pktCache;
};

