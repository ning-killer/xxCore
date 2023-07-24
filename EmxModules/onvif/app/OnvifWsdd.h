#pragma once
#include <memory>
#include <thread>
#include <vector>
#include <string>
#include "EventPoller.h"

class SoapHeader;

class OnvifWsdd
{
public:
	OnvifWsdd(const EventPoller::Ptr poller = nullptr);
	~OnvifWsdd();

	int run();

private:
	void initNamespaces();

	int wsddServer();

	int parseSoap(size_t dataSize);

	int sendProbeMatches();

	int sendHello();
private:
	char* recvMsg = nullptr;
	char _peerAddr[32] = { 0 };
	char _broadCastAddr[32] = { 0 };
	int _fd = -1;
	std::string m_localIP;
private:
	std::vector<std::pair<std::string, std::string>> _namespaces;
	std::shared_ptr<SoapHeader> _header;
	std::string _addresID;

	EventPoller::Ptr _poller;
};

