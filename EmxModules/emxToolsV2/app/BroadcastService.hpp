/*
* @Author: xiong
* @Date: 2023/2/9
*/

#ifndef EMX_BroadcastService_HPP
#define EMX_BroadcastService_HPP

#include "EmxCore.hpp"
#include "Resource.hpp"

namespace Emx {

    class BroadcastService {
    public:
        enum class CmdE : uint32_t {
            Detection = 60000,
        };
        struct Header {
            char magic[4]; //!<固定为'E','P','B','P'
            uint32_t size;
        };

        BroadcastService() {}

        void Create(Resource *res, Json::Value &json);

        void Destroy();

    private:
        void OnRecv(ErrCodeE e, const char *data, int32_t size,
                    const struct sockaddr *addr);

        void Detection(Json::Value &in, Json::Value &out);

    private:
        Resource *m_res;
        EuvUdp m_udp;
        uint16_t m_portHost;
        uint16_t m_portPeer;
    };

}

#endif //EMX_BroadcastService_HPP
