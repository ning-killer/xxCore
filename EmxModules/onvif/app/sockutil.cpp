#include "sockutil.h"
#ifdef _WIN32
#else
#include <sys/ioctl.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
//#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#endif 
#include <iostream>
#include <errno.h>

int SockUtil::connect( const char* host, uint16_t port)
{
	sockaddr addr;
	if (!getDomainIP(host, addr))
		return -1;
	((sockaddr_in*)&addr)->sin_port = htons(port);

	int sockFd = socket(addr.sa_family, SOCK_STREAM, IPPROTO_TCP);
	
	if (::connect(sockFd, (sockaddr*)&addr, sizeof(sockaddr)) < 0)
	{
		std::cout << "connect failed:" << getErrorCode() << std::endl;
#ifdef _WIN32
		closesocket(sockFd);
#else
		close(sockFd);
#endif 
		return -1;
	}

	return sockFd;
}

int SockUtil::listen(const uint16_t port, const char* localIp, int backLog)
{
	int sockfd = -1;
	sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sockfd < 0)
		return -1;

	int opt = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));

	sockaddr_in sa;
	sa.sin_family = AF_INET;
	inet_pton(AF_INET, localIp, &sa.sin_addr);
	sa.sin_port = htons(port);

	if (bind(sockfd, (sockaddr*)&sa, sizeof(sockaddr_in)) < 0)
	{
#ifdef _WIN32
		closesocket(sockfd);
#else
		close(sockfd);
#endif // _WIN32
		std::cout << "bind failed" << std::endl;
		return -1;
	}
	::listen(sockfd, backLog);
	return sockfd;
}

int SockUtil::bindUdpSock(unsigned short port, const char* localIp)
{
	int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (fd == -1)
		return -1;
	sockaddr_in sa;
	sa.sin_family = AF_INET;
	inet_pton(AF_INET, localIp, &sa.sin_addr);
	sa.sin_port = htons(0);
	if (bind(fd, (sockaddr*)&sa, sizeof(sockaddr_in)) < 0)
	{
		std::cout << "bind udp sock failed" << getErrorCode() << std::endl;
#ifdef _WIN32
		closesocket(fd);
#else
		close(fd);
#endif 
		return -1;
	}
	return fd;
}

bool SockUtil::getDomainIP(const char* host, sockaddr& addr)
{
	addrinfo* res = nullptr;
	getaddrinfo(host, NULL, NULL, &res);
	if (!res)
		return false;

	addr = *(res->ai_addr);
	freeaddrinfo(res);
	return true;
}

int SockUtil::setNoDelay(int sockFd, bool on)
{
	int opt = on ? 1 : 0;
	int ret = setsockopt(sockFd, IPPROTO_TCP, TCP_NODELAY, (char*)&opt, sizeof(opt));
	if (ret == -1) {
		std::cout << "���� NoDelay ʧ��!" << std::endl;
	}
	return ret;
}

void SockUtil::setNoblock(int fd, bool flag)
{
	u_long ul = flag ? 1 : 0;
#ifdef _WIN32
	ioctlsocket(fd, FIONBIO, &ul);
#else
	ioctl(fd, FIONBIO, &ul);
#endif
}

uint16_t SockUtil::get_local_port(int sockFD)
{
	socklen_t addrLen = sizeof(sockaddr_in);
	sockaddr_in localAddr = { 0 };
	if (getsockname(sockFD, (sockaddr*)&localAddr, &addrLen) == 0)
		return  ntohs(localAddr.sin_port);
	return 0;
}

int SockUtil::getErrorCode()
{
#ifdef _WIN32
	return WSAGetLastError();
#else
	return errno;
#endif // _WIN32
}

int SockUtil::setRecvBuf(int sock, int size)
{
	int ret = setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char*)&size, sizeof(size));
	if (ret == -1)
		std::cout << "���ý��ջ�����ʧ��!" << std::endl;
	
	return ret;
}

int SockUtil::setSendBuf(int sock, int size)
{
	int ret = setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char*)&size, sizeof(size));
	if (ret == -1) 
		std::cout << "���÷��ͻ�����ʧ��!" << std::endl;

	return ret;
}

std::string SockUtil::get_peer_ip(int fd)
{
	sockaddr addr;
	sockaddr_in* addr_v4;
	socklen_t addr_len = sizeof(addr);
	//��ȡremote ip and port
	if (0 == getpeername(fd, &addr, &addr_len)) {
		if (addr.sa_family == AF_INET) {
			addr_v4 = (sockaddr_in*)&addr;
			char buf[20];
			inet_ntop(addr_v4->sin_family, &addr_v4->sin_addr, buf, 20);
			return buf;
		}
	}
	return "";
}

std::string SockUtil::get_local_ip(int fd)
{
	sockaddr addr;
	sockaddr_in* addr_v4;
	socklen_t addr_len = sizeof(addr);
	if (0 == getsockname(fd, &addr, &addr_len))
	{
		if (addr.sa_family == AF_INET)
		{
			addr_v4 = (sockaddr_in*)&addr;
			char buf[20];
			inet_ntop(addr_v4->sin_family, &addr_v4->sin_addr, buf, 20);
			return buf;
		}
	}
	return "";
}

std::string SockUtil::guess_local_ipv4()
{
	std::string localIP = "127.0.0.1";
	int ks = -1;
	do
	{
		sockaddr_in iface_out;
		sockaddr_in remote;
		remote.sin_family = AF_INET;
		inet_pton(AF_INET, "217.12.3.11", &remote.sin_addr);
		remote.sin_port = htons(11111);
		int type;
		type = SOCK_DGRAM;
#if defined(SOCK_CLOEXEC)
		type = SOCK_CLOEXEC | SOCK_DGRAM;
#endif
		ks = socket(AF_INET, type, 0);
		int on = 1;
		if (setsockopt(ks, SOL_SOCKET, SO_BROADCAST, (char*)&on, sizeof(on)))
			break;

		if (::connect(ks, (struct sockaddr*)&remote, sizeof(struct sockaddr_in)) < 0)
			break;

		socklen_t len = sizeof(iface_out);
		if (getsockname(ks, (sockaddr*)&iface_out, &len) < 0)
			break;

		char ip[INET_ADDRSTRLEN] = { 0 };
		inet_ntop(iface_out.sin_family, &iface_out.sin_addr, ip, INET_ADDRSTRLEN);
		localIP = ip;
	} while (0);

	if (ks != -1)
	{
#ifdef _WIN32
		closesocket(ks);
#else
		close(ks);
#endif 
	}
	return localIP;
}


int SockUtil::send_file(char* fn, int fd, long sz)
{
#ifdef _WIN32
	HANDLE hf = CreateFileA(fn, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (hf == INVALID_HANDLE_VALUE)
		return -1;
	TransmitFile(fd, hf, 0, 0, NULL, NULL, TF_USE_DEFAULT_WORKER);
	CloseHandle(hf);
	return 0;
#else
	auto hf = open(fn, O_RDONLY);
	if (hf == -1)
		return -1;
	auto x = sendfile(fd, hf, NULL, sz);
	if (x <= 0)
		std::cout << "sendfile error " << errno << std::endl;
	close(hf);
	return 0;
#endif
}

int sendBinaryData(int fd,const char* data, int len, uint8_t flag)
{
    int offset = 0;
    int ret = 0;
    int i = 0;
    while (offset < len) {
        if (flag)
            ret = ::send(fd, data + offset, len - offset, MSG_NOSIGNAL);
        else
            ret = ::send(fd, data + offset, len - offset, MSG_NOSIGNAL | MSG_MORE);

        if (ret <= 0) {
            int err = SockUtil::getErrorCode();
            if (err == EAGAIN) {
                i++;
                if (i > 300)
                {
                   std::cout <<"send too many times" <<std::endl;
                    return 1;
                }
                usleep(1000);
                continue;
            } else {
                //printf("send error:%d\n", err);
                return 1;
            }
        }
        i = 0;
        offset += ret;
    }
    return 0;
}