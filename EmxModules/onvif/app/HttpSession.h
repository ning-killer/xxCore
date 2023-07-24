#pragma once
#include "TcpSession.h"
#include <string>

class SoapRequest;
class HttpRequest;

class HttpSession :public TcpSession
{
public:
	HttpSession(int fd, const EventPoller::Ptr poller);
	~HttpSession();
private:
	int handlePkt() override;

	int handleHTTP();

	int sendPostResponse(int sc, std::string b);

	int parseXmlBody();

	bool CheckAuth(const char* method, unsigned char& mode);

	int sendImageResponse(const std::string& data);

	int ResponseSC(int sc, const std::string& strJson);
private:
	int HandleOnvif();
private:/*ws*/
	/*int HandleSwitchingProtocols();

	int HandleWebSocketPkt();

	int webSocketSend(char* data, unsigned int len, int type);

	int sendWebSocketPong();*/
private:
	//bool _connectionUpgrade = false;
	std::shared_ptr<SoapRequest> _soapReq;

	HttpRequest* _req = nullptr;
	uint8_t _recvStatus = 0;
	size_t _contentLength = 0;
	std::string _body;
};

