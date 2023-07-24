#include "VideoHandler.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include "EmxCore.hpp"
#include <map>

int VideoHandler::ConnectServer()
{
	if (_iTcp)
		_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	else
		_fd = socket(AF_INET, SOCK_DGRAM, 0);

	if (_fd == -1)
		return 1;

	sockaddr_in remoteAddr;
	remoteAddr.sin_family = AF_INET;
	inet_pton(AF_INET, _remoteIp.c_str(), &remoteAddr.sin_addr);
	remoteAddr.sin_port = htons(_remotePort);

	if (connect(_fd, (sockaddr*)&remoteAddr, sizeof(sockaddr_in)) < 0)
	{
		emxlogi("connect %s:%d failed %d\n", _remoteIp.c_str(), _remotePort, errno);
		return 1;
	}
	if (_iTcp)
	{
		int sndBufSize = 200 * 1024;
		setsockopt(_fd, SOL_SOCKET, SO_SNDBUF, (char*)&sndBufSize, sizeof(int));

		int opt = 0;
		setsockopt(_fd, IPPROTO_TCP, TCP_NODELAY, (char*)&opt, sizeof(int));

		u_long ul = 1;
#ifdef _WIN32
		ioctlsocket(_fd, FIONBIO, &ul);
#else
		ioctl(_fd, FIONBIO, &ul);
#endif
	}

	return 0;
}

int VideoHandler::InputPSFrame(char* psData, uint32_t psSize)
{
	m_latestTime = time(NULL);

	uint32_t offset = 0;
	while (psSize > 1400) {
		auto rtpSz = packHeader(rtpPacket, 0, _rtpTimestamp);
		memcpy(rtpPacket + rtpSz, psData + offset, 1400);
		rtpSz += 1400;

		if (SendRTPPacket(rtpPacket, rtpSz))
			return 1;

		offset += 1400;
		psSize -= 1400;
	}

	if (psSize > 0)
	{
		auto rtpSz = packHeader(rtpPacket, 1, _rtpTimestamp);
		memcpy(rtpPacket + rtpSz, psData + offset, psSize);
		rtpSz += psSize;
		if (SendRTPPacket(rtpPacket, rtpSz))
			return 1;
	}
	

	_rtpTimestamp += 3600;
	return 0;
}

int VideoHandler::SendRTPPacket(char* rtp, uint32_t rtp_size)
{
	if (_iTcp == 1)
	{
		char temp[2];
		temp[0] = rtp_size >> 8;
		temp[1] = rtp_size;
		SendRawData(temp, 2, 0);

		return SendRawData(rtp, rtp_size);
	}

	return SendRawData(rtp, rtp_size, 1);
}

int VideoHandler::packHeader(char* in, bool mark, uint32_t timeStamp)
{
	in[0] = 0x80;//V_P_X
	in[1] = mark ? (1 << 7) : 0;
	in[1] |= 96;/*M_PT*/
	in[2] = _seq >> 8;
	in[3] = (_seq & 0xff);/*seq*/
	_seq++;

	in[8] = _ssrc >> 24;
	in[9] = _ssrc >> 16;
	in[10] = _ssrc >> 8;
	in[11] = (_ssrc & 0xff);

	in[4] = timeStamp >> 24;
	in[5] = timeStamp >> 16;
	in[6] = timeStamp >> 8;
	in[7] = (timeStamp & 0xff);
	return 12;
}

int VideoHandler::SendRawData(const char* data, int len, uint8_t flag)
{
	int offset = 0;
	int ret = 0;
//	int i = 0;
	while (offset < len) {
		if (flag)
			ret = ::send(_fd, data + offset, len - offset, MSG_NOSIGNAL);
		else
			ret = ::send(_fd, data + offset, len - offset, MSG_NOSIGNAL | MSG_MORE);

		if (ret <= 0) {
			int err = errno;
			if (err == EAGAIN) {
				//i++;
				//if (i > 20)
				//    return 1;
				//Sleep(1);
				continue;
			}
			else {
				printf("send error:%d\n", err);
				return 1;
			}
		}
//		i = 0;
		offset += ret;
	}
	return 0;
}

std::map<std::string, std::shared_ptr<VideoHandler>> g_hMap;
std::mutex g_hLock;

int AddHandler(std::shared_ptr<VideoHandler> h, const std::string& id)
{
	std::lock_guard<std::mutex> lck(g_hLock);
	g_hMap.insert({ id,std::move(h) });
	return 0;
}

void RunHandler(const std::string& id)
{
	std::lock_guard<std::mutex> lck(g_hLock);
	auto it = g_hMap.find(id);
	if(it != g_hMap.end())
		it->second->Start();
}

void RemoveHandler(const std::string& id)
{
	std::lock_guard<std::mutex> lck(g_hLock);
	auto it = g_hMap.find(id);
	if (it != g_hMap.end()) {
		it->second->Stop();
		g_hMap.erase(it);
	}
}

void OperationHandler(const std::string& id, int type, float value)
{
	std::lock_guard<std::mutex> lck(g_hLock);
	auto it = g_hMap.find(id);
	if (it != g_hMap.end()) {
		if (type == 0)
			it->second->Play(value);
		else if (type == 1)
			it->second->Pause(value);
		else if (type == 2)
			it->second->SeekPos(value);
		else if (type == 3)
			it->second->Teardown();
	}
}

void CheckHandlerTimeout()
{
	auto now = time(NULL);
	std::lock_guard<std::mutex> lck(g_hLock);
	auto it = g_hMap.begin();
	while (it != g_hMap.end()) {
		if (difftime(now, it->second->GetLatestTime()) >= 6)
		{
			it = g_hMap.erase(it);
			continue;
		}

		++it;
	}
}

