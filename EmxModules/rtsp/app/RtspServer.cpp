#include "RtspServer.h"
#include "sockutil.h"
#include <iostream>
#include "RtspSession.h"

RtspServer::RtspServer(const EventPoller::Ptr poller) :_poller(poller)
{
	if (!_poller)
		_poller = EventPollerPool::Instance().getPoller();
}

RtspServer::~RtspServer()
{
	_poller->delEvent(_fd, [](bool) {});
#ifdef _WIN32
	closesocket(_fd);
#else
	close(_fd);
#endif 
}

int RtspServer::start(unsigned short port)
{
	_fd = SockUtil::listen(port);
	if (_fd == -1)
		return 1;
	SockUtil::setNoblock(_fd, true);
	std::cout << "tcp server start listen on[0.0.0.0:" << port << "]" << std::endl;

	_poller->addEvent(_fd, std::bind(&RtspServer::OnAccept, this));

	EventPollerPool::Instance().for_each([this](EventPoller::Ptr& poller)
	{
		if (this->_poller == poller)
			return;
		
		std::shared_ptr<RtspServer> ret(new RtspServer(poller));
		ret->_fd = this->_fd;
		poller->addEvent(ret->_fd, [ret]() 
		{
			ret->OnAccept();
		});
	});
	return 0;
}

void RtspServer::OnAccept()
{
	char ip[16];
	sockaddr_in addr;
	socklen_t addrLen = sizeof(sockaddr_in);
	int sk = ::accept(_fd, (sockaddr*)&addr, &addrLen);
	if (sk < 0)
	{
		if (SockUtil::getErrorCode() == EAGAIN)
			return;
		std::cout << "accpet failed" << SockUtil::getErrorCode() << std::endl;
		return;
	}
	inet_ntop(addr.sin_family, &addr.sin_addr, ip, 16);
	std::cout << "recv from[" << ip << ":" << ntohs(addr.sin_port) << "]" << std::endl;

	std::shared_ptr<RtspSession> c(new RtspSession(sk, _poller));
	auto id = c->getID();
	auto p = shared_from_this();
	c->setErrorCb([p, id]()
	{
		p->_sessionMap.erase(id);
	});

	_sessionMap.insert({id, c});
}
