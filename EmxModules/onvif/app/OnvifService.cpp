#include "OnvifService.h"
#include "sockutil.h"
#include <iostream>
#include <string.h>
#include "OnvifWsdd.h"
#include "HttpSession.h"
#include "DigestAuth.h"
#include "WSAuth.h"
#include "soapdefine.h"
#include "SoapDispatcher.h"
#include "EmxOnvifService.hpp"

OnvifService::OnvifService(Emx::EmxOnvifService *emxOnvif, const EventPoller::Ptr polle) : m_emxOnvif(
        emxOnvif) {
    if (!_poller)
        _poller = EventPollerPool::Instance().getPoller();
}

OnvifService::~OnvifService() {
    _poller->delEvent(_fd, [](bool) {});
    if (!_isClone) {
#ifdef _WIN32
        closesocket(_fd);
#else
        close(_fd);
#endif
    }
}

int OnvifService::start() {
    Emx::Onvif::Param param = {};
    if (m_emxOnvif->LoadParam(param) != Emx::ErrCodeE::Success) {
        emxloge("get onvif param fialed\n");
        return -1;
    }
    _fd = SockUtil::listen(param.port);
    if (_fd == -1)
        return 1;
    SockUtil::setNoblock(_fd, true);
    std::cout << "tcp server start listen on[0.0.0.0:" << param.port << "]" << std::endl;

    _poller->addEvent(_fd, std::bind(&OnvifService::OnAccept, this));

    EventPollerPool::Instance().for_each([this](EventPoller::Ptr &poller) {
        if (this->_poller == poller)
            return;

        std::shared_ptr<OnvifService> ret(new OnvifService(m_emxOnvif, poller));
        ret->_fd = this->_fd;
        ret->_isClone = true;
        poller->addEvent(ret->_fd, [ret]() {
            ret->OnAccept();
        });
    });

    //if(te_env_get()->onvif.status == 1){
    _wsdd = std::make_shared<OnvifWsdd>();
    _wsdd->run();
    //}

    //todo: only one user is supported now
    std::string name = "admin";
    std::string password = "admin";
    if (!param.userArray.empty()) {
        name = param.userArray[0].name;
        password = param.userArray[0].password;
    }
    DigestAuth::Instance().setUserPassword(name, password);
    WSAuth::Instance().setPassword(password);

    EPSoap::Instance().Init();
    SoapDispatcher::Instance().Init();
    return 0;
}

void OnvifService::OnAccept() {
    char ip[16];
    sockaddr_in addr;
    socklen_t addrLen = sizeof(sockaddr_in);
    int sk = ::accept(_fd, (sockaddr *) &addr, &addrLen);
    if (sk < 0) {
        if (SockUtil::getErrorCode() == EAGAIN)
            return;
        std::cout << "accpet failed" << SockUtil::getErrorCode() << std::endl;
        return;
    }
    inet_ntop(addr.sin_family, &addr.sin_addr, ip, 16);
    std::cout << "recv from[" << ip << ":" << ntohs(addr.sin_port) << "]" << std::endl;

    std::shared_ptr<HttpSession> c(new HttpSession(sk, _poller));
    auto id = c->getID();
    auto p = shared_from_this();
    c->setErrorCb([p, id]() {
        p->_sessionMap.erase(id);
    });

    _sessionMap.insert({id, c});
}

void OnvifService::OpenDeviceDiscovery() {
    if (_wsdd)
        return;

    _wsdd = std::make_shared<OnvifWsdd>();
    _wsdd->run();
}

void OnvifService::CloseDeviceDiscovery() {
    if (_wsdd)
        _wsdd.reset();
}
