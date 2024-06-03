//
// Created by xiong on 2020/11/7.
//

#ifndef EMX_CONFIGSERVICE_HPP
#define EMX_CONFIGSERVICE_HPP

#include <list>
#include "EmxProtoV1.hpp"
#include "EmxCore.hpp"
#include "UpdateClient.hpp"

namespace Emx {
    class Config {
    public:
        static const uint16_t SYNC = 0x5a0e;
        struct Header {
            uint32_t syncCmd;
            uint32_t size;
        };
    };

    class ConfigClient : public EuvTcpServerClient {
    public:
        ConfigClient(EuvStmServerHostBase *host, EuvLoop &loop) :
                EuvTcpServerClient(host) {
            waitSize = sizeof(Config::Header);
            waitHeader = true;
            update = nullptr;
        }

        virtual ~ConfigClient() {
            delete update;
        }

        bool waitHeader;
        uint32_t waitSize;
        Config::Header header;
        std::string data;
        char ip[16];
        UpdateClient *update;
    };

    class ConfigService : public EuvTcpServerHost {
    public:

        ConfigService() : EuvTcpServerHost() {}

        virtual ~ConfigService() {};

        void CreateConfigService(Json::Value &json);

        void DestroyConfigService();

    private:

        EuvStmServerClientBase *OnAllocClient() override;

        void OnClientConnect(EuvStmServerClientBase *client) override;

        void OnClientDisConnect(EuvStmServerClientBase *client) override;

        void OnRecv(EuvStmServerClientBase *client, char *data, int32_t size) override;

        void ProcCmd(ConfigClient *c);

    private:
        void UpdatePushResult(ConfigClient *c, int code, const char *desc);

    private:
        EuvLoop m_loop;
        bool m_started;
        bool m_ena;
        uint16_t m_port;
        char *m_buffer;
        int m_bufferSize;
        std::list<ConfigClient *> m_clientList;
        EmxProtoV1 m_protoV1;
    };
}


#endif //EMX_CONFIGSERVICE_HPP
