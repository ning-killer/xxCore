#pragma once
#include <memory>
#include <map>
#include "EventPoller.h"

class RtspSession;

class RtspServer:public std::enable_shared_from_this<RtspServer>
{
public:
	RtspServer(const EventPoller::Ptr poller = nullptr);
	~RtspServer();

	int start(unsigned short port);
private:
	void OnAccept();
private:
	int _fd = -1;
	EventPoller::Ptr _poller;
	std::map<std::string, std::shared_ptr<RtspSession>> _sessionMap;
};

