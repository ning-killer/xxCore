//
// Created by xiong on 2020/11/7.
//

#ifndef EMX_DETECTIONSERVICE_HPP
#define EMX_DETECTIONSERVICE_HPP

#include "EmxCore.hpp"

namespace Emx {
    class DetectionService {
    public:
        enum class CmdE : uint32_t {
            Req = 0x5a0eff00,
            Resp = 0x5a0eff01,
        };
        struct Header {
            CmdE cmd;
            uint32_t size;
        };

        DetectionService() : m_started(false), m_ena(false), m_buffer(nullptr) {}

        void Create(Json::Value &json);

        void Destroy();

    private:
        void OnRecv(ErrCodeE e, const char *data, int32_t size,
                    const struct sockaddr *addr);

        void OnReq(Header *header);

    private:
        bool m_started;
        EuvUdp m_udp;
        EuvLoop m_loop;
        bool m_ena;
        uint16_t m_port;
        char *m_buffer;
        int m_bufferSize;
    };
}


#endif //EMX_DETECTIONSERVICE_HPP
