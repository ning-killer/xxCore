#pragma once
#include "EventPoller.h"

class TcpSession :public std::enable_shared_from_this<TcpSession>
{
public:
	TcpSession(int fd, const EventPoller::Ptr poller);
	virtual ~TcpSession();

	std::string getID();

	void setErrorCb(std::function<void()>&& _cb)
	{
		_errCB = std::move(_cb);
	}
protected:

	virtual int handlePkt() = 0;
protected:
	int _fd = -1;
	EventPoller::Ptr _poller;

	std::string _buffer;
	std::function<void()> _errCB;
private:
	char* recvBuf = nullptr;
	unsigned short _capacity = 2048;
	void OnRecv();
};

