#include "PipeWrap.h"
#include "sockutil.h"
#include <iostream>

PipeWrap::PipeWrap()
{
//    int ret;
#if defined(_WIN32)
    _listenerFd = SockUtil::listen(0, "127.0.0.1");

    auto localPort = SockUtil::get_local_port(_listenerFd);

    //�󶨱��ض˿�
    _pipe_fd[1] = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    sockaddr_in servaddr = { AF_INET };
    servaddr.sin_addr.S_un.S_addr = INADDR_ANY;
    servaddr.sin_port = htons(0);
    ret = ::bind(_pipe_fd[1], (sockaddr*)&servaddr, sizeof(sockaddr_in));
    
    sockaddr_in sa = { AF_INET };
    sa.sin_port = htons(localPort);
    inet_pton(AF_INET, "127.0.0.1", &sa.sin_addr);
    ret = ::connect(_pipe_fd[1], (sockaddr*)&sa, sizeof(sockaddr_in));
    _pipe_fd[0] = accept(_listenerFd, nullptr, nullptr);
   
    SockUtil::setNoDelay(_pipe_fd[0]);
    SockUtil::setNoDelay(_pipe_fd[1]);
#else
    if (pipe(_pipe_fd) == -1) {
        //throw runtime_error(StrPrinter << "create posix pipe failed:" << get_uv_errmsg()); 
    }
#endif 
    SockUtil::setNoblock(_pipe_fd[0], true);
}

PipeWrap::~PipeWrap()
{
#if defined(_WIN32)
    closesocket(_listenerFd);
    closesocket(_pipe_fd[0]);
    closesocket(_pipe_fd[1]);
#else
    close(_pipe_fd[0]);
    close(_pipe_fd[1]);
#endif 
}

int PipeWrap::write(const void* buf, int n)
{
//    int ret;
#if defined(_WIN32)
//    ret = send(_pipe_fd[1], (char*)buf, n, 0);
    send(_pipe_fd[1], (char*)buf, n, 0);
#else
//    ret = ::write(_pipe_fd[1], buf, n);
    ::write(_pipe_fd[1], buf, n);
#endif
    return 1;
}

int PipeWrap::read(void* buf, int n)
{
    int ret;
#if defined(_WIN32)
    ret = recv(_pipe_fd[0], (char*)buf, n, 0);
#else
    ret = ::read(_pipe_fd[0], buf, n);
#endif 
    return ret;
}