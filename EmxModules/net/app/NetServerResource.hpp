//
// Created by xiong on 2022/5/26.
//

#ifndef EMX_NetServerResource_HPP
#define EMX_NetServerResource_HPP

#include "EmxCore.hpp"

namespace Emx {
    struct NetServerResource {
        char netConfigDir[EMX_MAX_PATH_SIZE];
        using SaveParamCallback = std::function<ErrCodeE()>;
        Json::Value param;
        EuvLoop loop;
        char *buffer;
        int bufferSize;
        MsgTopicPublisherAsync pub;
        SaveParamCallback saveParam;
    };
}
#endif //EMX_NetServerResource_HPP

