//
// Created by xiong on 2021/5/20.
//

#ifndef EMX_EmxToolsV1_HPP
#define EMX_EmxToolsV1_HPP

#include "EmxCore.hpp"


#define EMX_TOOLS_BUFFER_SIZE (512)

namespace Emx {
    class DetectionService;

    class ConfigService;

    class StreamService;

    class EmxToolsV1 {
    public:
        EmxToolsV1();

        virtual ~EmxToolsV1();

        void Create(bool newThread);

        void Destroy();

    private:
        ErrCodeE OnRecvMsg(uint32_t module, uint32_t method,
                           const char *data, int32_t size,
                           std::string &out);

    private:
        EuvLoop m_loop;
        MsgRpcRep m_resp;
        StreamService *m_stream;
        DetectionService *m_detection;
        ConfigService *m_config;
        char m_buffer[EMX_TOOLS_BUFFER_SIZE];
        bool m_isRunning;
        Json::Value m_json;
    };

}

#endif //EMX_EmxToolsV1_HPP
