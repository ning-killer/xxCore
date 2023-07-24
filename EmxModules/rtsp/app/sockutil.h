#pragma once
#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#endif
#include <stdint.h>
#include <string>

class SockUtil
{
public:
	static int connect(const char* host, uint16_t port);
	static int listen(const uint16_t port, const char* localIp = "0.0.0.0", int backLog = 1024);
	static int bindUdpSock(unsigned short port, const char* localIp);
	//static int bindSock(int sockFd, const char* localIp, uint16_t port);
	static int setNoDelay(int sockFd, bool on = true);
	static bool getDomainIP(const char* host, sockaddr& addr);
	static void setNoblock(int fd, bool flag);
	static uint16_t get_local_port(int sockFD);
	static int getErrorCode();
	
	static int setRecvBuf(int sock, int size);
	static int setSendBuf(int sock, int size);

	static std::string get_local_ip(int fd);
	static std::string get_peer_ip(int fd);

	static std::string guess_local_ipv4();

	static int send_file(char* fn, int fd, long sz);
};

int sendBinaryData(int fd, const char* data, int len, uint8_t flag = 0);

