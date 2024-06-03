//
// Created by xiong on 2020/11/7.
//

#ifndef EMX_CONFIGSERVICE_HPP
#define EMX_CONFIGSERVICE_HPP

#include <list>
#include "EmxProtoV2.hpp"
#include "EmxCore.hpp"
#include "Resource.hpp"

namespace Emx {
    struct ConfigHeader {
        static const uint8_t HeaderSize = 8;
        char magic[4]; //!<固定为'E','P','T','P'
        uint32_t size;
    };

    class ConfigClient : public EuvTcpServerClient {
    public:
        ConfigClient(EuvStmServerHostBase *host, EuvLoop &loop) :
                EuvTcpServerClient(host) {
            waitSize = ConfigHeader::HeaderSize;
            waitHeader = true;
        }

        virtual ~ConfigClient() {}

        bool waitHeader;
        uint32_t waitSize;
        ConfigHeader header;
        std::string data;
        char ip[16];
    };

    class ConfigService : public EuvTcpServerHost {
    public:

        ConfigService() : EuvTcpServerHost() {}

        virtual ~ConfigService() {};

        void CreateConfigService(Resource *res, Json::Value &json);

        void DestroyConfigService();

    private:

        EuvStmServerClientBase *OnAllocClient() override;

        void OnClientConnect(EuvStmServerClientBase *client) override;

        void OnClientDisConnect(EuvStmServerClientBase *client) override;

        void OnRecv(EuvStmServerClientBase *client, char *data, int32_t size) override;

        void ProcCmd(ConfigClient *c);

    private:
        Resource *m_res;
        uint16_t m_port;
        std::list<ConfigClient *> m_clientList;
        EmxProtoV2 m_proto;
    };
}


#endif //EMX_CONFIGSERVICE_HPP
