#include "TcpSession.h"
#include "sockutil.h"
#include <string>
#include <stdio.h>

TcpSession::TcpSession(int fd, const EventPoller::Ptr poller) :_fd(fd), _poller(poller)
{
	recvBuf = new char[_capacity];
	_errCB = []() {};

	_poller->addEvent(_fd, std::bind(&TcpSession::OnRecv, this));
}

TcpSession::~TcpSession()
{
	delete[] recvBuf;
#ifdef _WIN32
	closesocket(_fd);
#else
	close(_fd);
#endif
}

void TcpSession::OnRecv()
{
	auto readBytes = ::recv(_fd, recvBuf, _capacity, MSG_NOSIGNAL);
	if (readBytes <= 0)
	{
		_poller->delEvent(_fd, [](bool) {});
		_errCB();
		return;
	}
	_buffer.append(recvBuf, readBytes);

	if (handlePkt())
	{
		_poller->delEvent(_fd, [](bool) {});
		_errCB();
	}
}

std::string TcpSession::getID()
{
	char ab[32];
	snprintf(ab,sizeof(ab),"%lu",reinterpret_cast<unsigned long int>(this));
	return std::string(ab);
}