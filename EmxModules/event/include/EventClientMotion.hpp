/*
* @Author: xiong
* @Date: 2023/2/16
*/

#ifndef EMX_EventClientMotion_HPP
#define EMX_EventClientMotion_HPP

#include "EventMotion.hpp"

namespace Emx {

    class EventClientMotion : public EventMotion {
    public:
        EventClientMotion(int chn) : EventMotion(chn) {}

        ErrCodeE GetParam(std::shared_ptr<Param> &param) override;

        ErrCodeE SetParam(const std::shared_ptr<Param> &param) override;
    };

}

#endif //EMX_EventClientMotion_HPP
