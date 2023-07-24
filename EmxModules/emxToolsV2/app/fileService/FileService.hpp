/*
* @Author: xiong
* @Date: 2023/2/14
*/

#ifndef EMX_FileService_HPP
#define EMX_FileService_HPP

#include "EmxCore.hpp"
#include "Resource.hpp"

namespace Emx {
    class FileClient;

    class FileService : public EuvTcpServerHost {
    public:

        FileService() : EuvTcpServerHost() {}

        virtual ~FileService() {};

        void CreateFileService(Resource *res, Json::Value &json);

        void DestroyFileService();

    private:

        EuvStmServerClientBase *OnAllocClient() override;

        void OnClientConnect(EuvStmServerClientBase *client) override;

        void OnClientDisConnect(EuvStmServerClientBase *client) override;

        void OnRecv(EuvStmServerClientBase *client, char *data, int32_t size) override;

    private:
        Resource *m_res;
        uint16_t m_port;
        std::list<FileClient *> m_clientList;
    };
}

#endif //EMX_FileService_HPP
