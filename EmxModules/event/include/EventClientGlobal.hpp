/*
* @Author: xiong
* @Date: 2023/2/16
*/

#ifndef EMX_EventClientGlobal_HPP
#define EMX_EventClientGlobal_HPP

#include "EventGlobal.hpp"

namespace Emx {

    class EventClientGlobal : public EventGlobal {
    public:
        ErrCodeE GetSupportedEventChannel(std::vector<Event::Channel> &channels) override;

        ErrCodeE GetParam(std::shared_ptr<Param> &param) override;

        ErrCodeE SetParam(const std::shared_ptr<Param> &param) override;
    };

}

#endif //EMX_EventClientGlobal_HPP
