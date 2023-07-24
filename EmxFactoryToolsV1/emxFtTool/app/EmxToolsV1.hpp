//
// Created by xiong on 2021/5/20.
//

#ifndef EMXFCTOOLSV1_EMXToolsV1_HPP
#define EMXFCTOOLSV1_EMXToolsV1_HPP

#include "EmxCore.hpp"
#include "ManageCfgFile.hpp"

#define EMX_TOOLS_BUFFER_SIZE (512)

namespace Emx {
    class DetectionService;

    class ConfigService;

    class StreamService;

    class EmxToolsV1 {
    public:
        EmxToolsV1();
        virtual ~EmxToolsV1();
        void Create(const char *path);

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
        ManageCfgFile *m_managecfg;
        char m_buffer[EMX_TOOLS_BUFFER_SIZE];
        bool m_isRunning;
        Json::Value m_json;
    };

}

#endif //EMXFCTOOLSV1_EMXToolsV1_HPP
