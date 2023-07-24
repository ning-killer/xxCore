//
// Created by xiong on 2021/7/19.
//

#ifndef EMX_LogInner_HPP
#define EMX_LogInner_HPP

#include "EmxInner.hpp"
#include "core/EmxLog.hpp"
//AF_UNIX
#define EMX_LOG_DOMAIN_DIR EMX_DOMAIN_DIR"/log"
#define EMX_LOG_DOMAIN_CLIENT_PREFIX EMX_LOG_DOMAIN_DIR"/C"
#define EMX_LOG_DOMAIN_SERVER_CTRL     EMX_LOG_DOMAIN_DIR"/SCtrl"
#define EMX_LOG_DOMAIN_SERVER_NORMAL    EMX_LOG_DOMAIN_DIR"/SNormal"
#define EMX_LOG_DOMAIN_SERVER_FLASH     EMX_LOG_DOMAIN_DIR"/SFlash"
#define EMX_LOG_DOMAIN_SERVER_EXT     EMX_LOG_DOMAIN_DIR"/SExt"

//共享内存key
#define EMX_LOG_SHARED_MEM_KEY 1069876
namespace Emx {
    class LogInner {
    public:
        enum class CmdE : uint8_t {
            ConfigSet = 0x11,//set LogCom::Config
            ConfigGet = 0x12,//get LogCom::Config
            CopyNormal = 0x21,//copy normal log to the path specified
            CopyFlash = 0x22,//copy flash log to the path specified
            Start = 0x31,//start log write service
            Stop = 0x32,//stop log write service
        };

        static ErrCodeE ParseOpt(int argc, char *argv[], Log::Config &cfg);
    };
}

#endif //EMX_LogInner_HPP

