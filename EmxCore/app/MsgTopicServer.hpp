//
// Created by xiong on 2020/10/23.
//

#ifndef EMX_MSGTOPICSERVER_HPP
#define EMX_MSGTOPICSERVER_HPP

#include <map>
#include <list>
#include <string>
#include "core/EmxUV.hpp"
#include "core/msg/MsgTopic.hpp"

namespace Emx {
    class MsgTopicClient : public EuvPipeServerClient {
    public:
        MsgTopicClient(EuvStmServerHostBase *host) : EuvPipeServerClient(host) {
            waitSize = sizeof(MsgTopic::Header);
            waitHeader = true;
            clientId = 0;
        }

        bool waitHeader;
        uint32_t waitSize;
        MsgTopic::Header header;
        std::string data;
        MsgTopic::ClientTypeE type;
        uint32_t clientId;
        std::list<uint32_t> topicList;
    };

    class MsgTopicServer : public EuvPipeServerHost {
    public:
        static const int32_t BufferSize = 64 * 1024;

        MsgTopicServer() : EuvPipeServerHost() {}

        virtual ~MsgTopicServer() {};

        void CreateMsgTopicServer();

        void DestroyMsgTopicServer() {
            m_loop.StopAndDeInit();
            free(m_buffer);
        }

    private:

        EuvStmServerClientBase *OnAllocClient() override {
            return new MsgTopicClient(this);
        }

        void OnClientConnect(EuvStmServerClientBase *client) override;

        void OnClientDisConnect(EuvStmServerClientBase *client) override;

        void OnRecv(EuvStmServerClientBase *client, char *data, int32_t size) override;

    private:
        EuvLoop m_loop;
        char *m_buffer;
        typedef std::list<MsgTopicClient *> ClientList;
        ClientList m_clientList;
        std::map<uint32_t, ClientList> m_clientAppend2Topic;
    };
}


#endif //EMX_MSGTOPICSERVER_HPP
