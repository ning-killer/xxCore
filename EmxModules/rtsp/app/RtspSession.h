#pragma once
#include "TcpSession.h"
#include <string>
#ifdef _WIN32
#include <WinSock2.h>
#else
#include <netinet/in.h>
#endif

class MediaSource;
class HttpRequest;

class RtspSession :public TcpSession
{
public:
	RtspSession(int fd, const EventPoller::Ptr poller);
	~RtspSession();

private:
	int handlePkt() override;
private:
	void handleCmd_OPTIONS(const std::string& cseq);
	void handleCmd_DESCRIBE(std::shared_ptr<HttpRequest>& hd);
	void handleCmd_SETUP(std::shared_ptr<HttpRequest>& hd);
	void handleCmd_PLAY(std::shared_ptr<HttpRequest>& hd);
	void handleCmd_PAUSE(std::shared_ptr<HttpRequest>& hd);
	void handleCmd_TEARDOWN(std::shared_ptr<HttpRequest>& hd);
	void handleCmd_bad(const std::string& cseq);
	void handleCmd_notFound(const std::string& cseq);
	void handleCmd_sessionNotFound(const std::string& cseq);

	int handleRtsp();

	int handleRtp();

	int handleReplay(std::shared_ptr<HttpRequest>& hd);
private:
	/*
	* udp mode
	*/
	void OnRecvUdpData(int fd, int interval);

	bool CheckAuth(std::shared_ptr<HttpRequest>& hd);
private:
	char sessionID[32] = { 0 };
	std::shared_ptr<MediaSource> _src;

	time_t m_curTime;
	std::string m_baseUrl;

	std::string _streamMode;
	//udp
	std::pair<int, int> trackSock[2] = { {-1,-1},{-1,-1} };
	std::pair<sockaddr_in, sockaddr_in> trackAddr[2];
	int _index = 0;
	char* udpBuf = nullptr;

	char* responseBuf = nullptr;
	uint8_t _recvStatus = 0;
	size_t _contentLength = 0;
	std::string _body;
	std::shared_ptr<HttpRequest> _req;
	uint8_t _enableMedia = 0x00;
private:/*for replay*/
	float _scale = 1.f;
	bool _isReplay = false;

	bool _sendKeyFrame = false;
};

