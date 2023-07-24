#pragma once

#include <memory>
#include <map>
#include <functional>
#include "EventPoller.h"

class OnvifWsdd;

class HttpSession;

namespace Emx {
    class EmxOnvifService;
}

class OnvifService : public std::enable_shared_from_this<OnvifService> {
public:
    OnvifService(Emx::EmxOnvifService *emxOnvif, const EventPoller::Ptr poller = nullptr);

    ~OnvifService();

    int start();

    void OpenDeviceDiscovery();

    void CloseDeviceDiscovery();

private:
    void OnAccept();

private:
    int _fd = -1;
    bool _isClone = false;
    EventPoller::Ptr _poller;
    std::shared_ptr<OnvifWsdd> _wsdd;
    std::map<std::string, std::shared_ptr<HttpSession>> _sessionMap;
    Emx::EmxOnvifService *m_emxOnvif;
};

