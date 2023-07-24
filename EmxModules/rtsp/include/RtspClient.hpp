/*
* @Author: xiong
* @Date: 2023/1/16
*/

#ifndef EMX_RtspClient_HPP
#define EMX_RtspClient_HPP

#include "Rtsp.hpp"

namespace Emx {

    class RtspClient : public Rtsp {
    public:
        ErrCodeE GetParam(Param &param) override;

        ErrCodeE SetParam(const Param &param) override;
    };

}

#endif //EMX_RtspClient_HPP
