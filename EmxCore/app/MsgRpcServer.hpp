//
// Created by xiong on 2020/10/23.
//

#ifndef EMX_MSGRPCSERVER_HPP
#define EMX_MSGRPCSERVER_HPP

#include <map>
#include <string>
#include <list>
#include "core/EmxUV.hpp"
#include "core/msg/MsgRpc.hpp"

namespace Emx {

    class MsgRpcClient : public EuvPipeServerClient {
    public:
        MsgRpcClient(EuvStmServerHostBase *host) : EuvPipeServerClient(host) {
            waitSize = sizeof(MsgRpc::Header);
            waitHeader = true;
            clientId = 0;
        }

        const char *GetTypeName() { return type == MsgRpc::ClientTypeE::Req ? "REQ" : "RESP"; }

        bool waitHeader;
        uint32_t waitSize;
        MsgRpc::ClientTypeE type;
        uint32_t clientId;
        MsgRpc::Header header;
        std::string data;
    };

    class MsgRpcServer : public EuvPipeServerHost {
    public:
        static const int32_t BufferSize = 64 * 1024;

        MsgRpcServer() : EuvPipeServerHost() {}

        virtual ~MsgRpcServer() {};

        void CreateMsgRpcServer();

        void DestroyMsgRpcServer() {
            m_loop.StopAndDeInit();
            free(m_buffer);
        }

    private:
        EuvStmServerClientBase *OnAllocClient() override { return new MsgRpcClient(this); }

        void OnClientConnect(EuvStmServerClientBase *client) override;

        void OnClientDisConnect(EuvStmServerClientBase *client) override;

        void OnRecv(EuvStmServerClientBase *client, char *data, int32_t size) override;

        void SendRpc(MsgRpcClient *client);

        std::map<uint32_t, MsgRpcClient *> &GetMapByType(MsgRpc::ClientTypeE type) {
            return type == MsgRpc::ClientTypeE::Req ? m_reqMap : m_respMap;
        }

    private:
        EuvLoop m_loop;
        char *m_buffer;
        std::map<uint32_t, MsgRpcClient *> m_reqMap;
        std::map<uint32_t, MsgRpcClient *> m_respMap;
    };
}


#endif //EMX_MSGRPCSERVER_HPP
